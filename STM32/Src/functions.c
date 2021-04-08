#include "functions.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "arm_math.h"
#include "fpga.h"
#include "trx_manager.h"
#include "usbd_debug_if.h"
#include "usbd_cat_if.h"
#include "lcd.h"

CPULOAD_t CPU_LOAD = {0};
volatile bool SPI_busy = false;
volatile bool SPI_process = false;
volatile bool SPI_TXRX_ready = false;

void readFromCircleBuffer32(uint32_t *source, uint32_t *dest, uint32_t index, uint32_t length, uint32_t words_to_read)
{
	if (index >= words_to_read)
	{
		dma_memcpy32(&dest[0], &source[index - words_to_read], words_to_read);
	}
	else
	{
		uint_fast16_t prev_part = words_to_read - index;
		dma_memcpy32(&dest[0], &source[length - prev_part], prev_part);
		dma_memcpy32(&dest[prev_part], &source[0], (words_to_read - prev_part));
	}
}

void readHalfFromCircleUSBBuffer24Bit(uint8_t *source, int32_t *dest, uint32_t index, uint32_t length)
{
	uint_fast16_t halflen = length / 2;
	uint_fast16_t readed_index = 0;
	if (index >= halflen)
	{
		for (uint_fast16_t i = (index - halflen); i < index; i++)
		{
			dest[readed_index] = (source[i * 3 + 0] << 8) | (source[i * 3 + 1] << 16) | (source[i * 3 + 2] << 24);
			readed_index++;
		}
	}
	else
	{
		uint_fast16_t prev_part = halflen - index;
		for (uint_fast16_t i = (length - prev_part); i < length; i++)
		{
			dest[readed_index] = (source[i * 3 + 0] << 8) | (source[i * 3 + 1] << 16) | (source[i * 3 + 2] << 24);
			readed_index++;
		}
		for (uint_fast16_t i = 0; i < (halflen - prev_part); i++)
		{
			dest[readed_index] = (source[i * 3 + 0] << 8) | (source[i * 3 + 1] << 16) | (source[i * 3 + 2] << 24);
			readed_index++;
		}
	}
}

static uint16_t dbg_lcd_y = 0;
static uint16_t dbg_lcd_x = 0;
void print_chr_LCDOnly(char chr)
{
	if (LCD_DEBUG_ENABLED)
	{
		if (chr == '\r')
			return;

		if (chr == '\n')
		{
			dbg_lcd_y += 9;
			dbg_lcd_x = 0;
			if (dbg_lcd_y >= LCD_HEIGHT)
			{
				dbg_lcd_y = 0;
				LCDDriver_Fill(BG_COLOR);
			}
			return;
		}

		LCDDriver_drawChar(dbg_lcd_x, dbg_lcd_y, chr, COLOR_RED, BG_COLOR, 1);
		dbg_lcd_x += 6;
	}
}

void print_flush(void)
{
	uint_fast16_t tryes = 0;
	while (DEBUG_Transmit_FIFO_Events() == USBD_BUSY && tryes < 512)
		tryes++;
}

void print_hex(uint8_t data, bool _inline)
{
	char tmp[50] = ""; //-V808
	if (_inline)
		sprintf(tmp, "%02X", data);
	else
		sprintf(tmp, "%02X\n", data);
	print(tmp);
}

void print_bin8(uint8_t data, bool _inline)
{
	char tmp[50] = ""; //-V808
	if (_inline)
		sprintf(tmp, "%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(data));
	else
		sprintf(tmp, "%c%c%c%c%c%c%c%c\n", BYTE_TO_BINARY(data));
	print(tmp);
}

void print_bin16(uint16_t data, bool _inline)
{
	char tmp[50] = ""; //-V808
	if (_inline)
		sprintf(tmp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", BIT16_TO_BINARY(data));
	else
		sprintf(tmp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", BIT16_TO_BINARY(data));
	print(tmp);
}

uint32_t getRXPhraseFromFrequency(int32_t freq, uint8_t rx_num) // calculate the frequency from the phrase for FPGA (RX1 / RX2)
{
	if (freq < 0)
		return 0;
	bool inverted = false;
	int32_t _freq = freq;
	if (_freq > ADC_CLOCK / 2) //Go Nyquist
	{
		while (_freq > (ADC_CLOCK / 2))
		{
			_freq -= (ADC_CLOCK / 2);
			inverted = !inverted;
		}
		if (inverted)
		{
			_freq = (ADC_CLOCK / 2) - _freq;
		}
	}
	if (rx_num == 1)
		TRX_RX1_IQ_swap = inverted;
	if (rx_num == 2)
		TRX_RX2_IQ_swap = inverted;
	float64_t res = round(((float64_t)_freq / (float64_t)ADC_CLOCK) * (float64_t)4294967296); //freq in hz/oscil in hz*2^bits (32 now);
	return (uint32_t)res;
}

uint32_t getTXPhraseFromFrequency(float64_t freq) // calculate the frequency from the phrase for FPGA (TX)
{
	if (freq < 0)
		return 0;
	bool inverted = false;
	int32_t _freq = (int32_t)freq;

	uint8_t nyquist = _freq / (DAC_CLOCK / 2);
	if (nyquist == 0) // <99.84mhz (good 0mhz - 79.872mhz) 0-0.4 dac freq
	{
		TRX_DAC_HP2 = false; //low-pass
	}
	if (nyquist == 1) // 99.84-199.68mhz (good 119.808mhz - 159.744mhz) dac freq - (0.2-0.4 dac freq)
	{
		TRX_DAC_HP2 = true; //high-pass
	}

	if (_freq > (DAC_CLOCK / 2)) //Go Nyquist
	{
		while (_freq > (DAC_CLOCK / 2))
		{
			_freq -= (DAC_CLOCK / 2);
			inverted = !inverted;
		}
		if (inverted)
		{
			_freq = (DAC_CLOCK / 2) - _freq;
		}
	}
	TRX_TX_IQ_swap = inverted;

	float64_t res = round((float64_t)_freq / (float64_t)DAC_CLOCK * (float64_t)4294967296); //freq in hz/oscil in hz*2^NCO phase bits (32 now)
	return (uint32_t)res;
}

void addSymbols(char *dest, char *str, uint_fast8_t length, char *symbol, bool toEnd) // add zeroes
{
	char res[50] = "";
	strcpy(res, str);
	while (strlen(res) < length)
	{
		if (toEnd)
			strcat(res, symbol);
		else
		{
			char tmp[50] = "";
			strcat(tmp, symbol);
			strcat(tmp, res);
			strcpy(res, tmp);
		}
	}
	strcpy(dest, res);
}

float32_t log10f_fast(float32_t X)
{
	float32_t Y, F;
	int32_t E;
	F = frexpf(fabsf(X), &E);
	Y = 1.23149591368684f;
	Y *= F;
	Y += -4.11852516267426f;
	Y *= F;
	Y += 6.02197014179219f;
	Y *= F;
	Y += -3.13396450166353f;
	Y += E;
	return (Y * 0.3010299956639812f);
}

float32_t db2rateV(float32_t i) // from decibels to times (for voltage)
{
	return powf(10.0f, (i / 20.0f));
}

float32_t db2rateP(float32_t i) // from decibels to times (for power)
{
	return powf(10.0f, (i / 10.0f));
}

float32_t rate2dbV(float32_t i) // times to decibels (for voltage)
{
	return 20 * log10f_fast(i);
}

float32_t rate2dbP(float32_t i) // from times to decibels (for power)
{
	return 10 * log10f_fast(i);
}

#define VOLUME_LOW_DB (-40.0f)
#define VOLUME_EPSILON powf(10.0f, (VOLUME_LOW_DB / 20.0f))
float32_t volume2rate(float32_t i) // from the position of the volume knob to the gain
{
	if (i < (15.0f / 1024.f)) //mute zone
		return 0.0f;
	return powf(VOLUME_EPSILON, (1.0f - i));
}

void shiftTextLeft(char *string, uint_fast16_t shiftLength)
{
	uint_fast16_t size = strlen(string);
	if (shiftLength >= size)
	{
		dma_memset(string, '\0', size);
		return;
	}
	for (uint_fast16_t i = 0; i < size - shiftLength; i++)
	{
		string[i] = string[i + shiftLength];
		string[i + shiftLength] = '\0';
	}
}

float32_t getMaxTXAmplitudeOnFreq(uint32_t freq)
{
	if (freq > MAX_TX_FREQ_HZ)
		return 0.0f;

	if (freq < 1.0 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_2200m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 2.5 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_160m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 5.3 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_80m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 8.5 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_40m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 12.0 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_30m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 16.0 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_20m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 19.5 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_17m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 22.5 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_15m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 26.5 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_12m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 40.0 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_10m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
	if (freq < 80.0 * 1000000)
		return (float32_t)CALIBRATE.rf_out_power_6m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;

	return (float32_t)CALIBRATE.rf_out_power_2m / 100.0f * (float32_t)MAX_TX_AMPLITUDE;
}

float32_t generateSin(float32_t amplitude, float32_t *index, uint32_t samplerate, uint32_t freq)
{
	float32_t ret = amplitude * arm_sin_f32(*index * (2.0f * F_PI));
	*index += ((float32_t)freq / (float32_t)samplerate);
	while (*index >= 1.0f)
		*index -= 1.0f;
	return ret;
}

static uint32_t CPULOAD_startWorkTime = 0;
static uint32_t CPULOAD_startSleepTime = 0;
static uint32_t CPULOAD_WorkingTime = 0;
static uint32_t CPULOAD_SleepingTime = 0;
static uint32_t CPULOAD_SleepCounter = 0;
static bool CPULOAD_status = true; // true - wake up ; false - sleep

void CPULOAD_Init(void)
{
	DBGMCU->CR |= (DBGMCU_CR_DBG_SLEEPD1_Msk | DBGMCU_CR_DBG_STOPD1_Msk | DBGMCU_CR_DBG_STANDBYD1_Msk);
	// allow using the counter
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// start the counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	// zero the value of the counting register
	DWT->CYCCNT = 0;
	CPULOAD_status = true;
}

void CPULOAD_GoToSleepMode(void)
{
	// Add to working time
	CPULOAD_WorkingTime += DWT->CYCCNT - CPULOAD_startWorkTime;
	// Save count cycle time
	CPULOAD_SleepCounter++;
	CPULOAD_startSleepTime = DWT->CYCCNT;
	CPULOAD_status = false;
	// Go to sleep mode Wait for wake up interrupt
	__WFI();
}

void CPULOAD_WakeUp(void)
{
	if (CPULOAD_status)
		return;
	CPULOAD_status = true;
	// Increase number of sleeping time in CPU cycles
	CPULOAD_SleepingTime += DWT->CYCCNT - CPULOAD_startSleepTime;
	// Save current time to get number of working CPU cycles
	CPULOAD_startWorkTime = DWT->CYCCNT;
}

void CPULOAD_Calc(void)
{
	// Save values
	CPU_LOAD.SCNT = CPULOAD_SleepingTime;
	CPU_LOAD.WCNT = CPULOAD_WorkingTime;
	CPU_LOAD.SINC = CPULOAD_SleepCounter;
	CPU_LOAD.Load = ((float)CPULOAD_WorkingTime / (float)(CPULOAD_SleepingTime + CPULOAD_WorkingTime) * 100);
	if (CPU_LOAD.SCNT == 0)
	{
		CPU_LOAD.Load = 100;
	}
	if (CPU_LOAD.SCNT == 0 && CPU_LOAD.WCNT == 0)
	{
		CPU_LOAD.Load = 255;
		CPULOAD_Init();
	}
	// Reset time
	CPULOAD_SleepingTime = 0;
	CPULOAD_WorkingTime = 0;
	CPULOAD_SleepCounter = 0;
}

inline int32_t convertToSPIBigEndian(int32_t in)
{
	return (int32_t)(0xFFFF0000 & (uint32_t)(in << 16)) | (int32_t)(0x0000FFFF & (uint32_t)(in >> 16));
}

inline uint8_t rev8(uint8_t data)
{
	uint32_t tmp = data;
	return (uint8_t)(__RBIT(tmp) >> 24);
}

IRAM2 uint8_t SPI_tmp_buff[8] = {0};
bool SPI_Transmit(uint8_t *out_data, uint8_t *in_data, uint16_t count, GPIO_TypeDef *CS_PORT, uint16_t CS_PIN, bool hold_cs, uint32_t prescaler, bool dma)
{
	if (SPI_busy)
	{
		println("SPI Busy");
		return false;
	}

	//SPI speed
	if (hspi2.Init.BaudRatePrescaler != prescaler)
	{
		hspi2.Init.BaudRatePrescaler = prescaler;
		HAL_SPI_Init(&hspi2);
	}

	const int32_t timeout = 0x200; //HAL_MAX_DELAY
	SPI_busy = true;
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	HAL_StatusTypeDef res = 0;

	if (dma)
	{
		Aligned_CleanDCache_by_Addr((uint32_t)out_data, count);
		Aligned_CleanDCache_by_Addr((uint32_t)in_data, count);
		uint32_t starttime = HAL_GetTick();
		SPI_TXRX_ready = false;
		if (in_data == NULL)
		{
			if (hdma_spi2_rx.Init.MemInc != DMA_MINC_DISABLE)
			{
				hdma_spi2_rx.Init.MemInc = DMA_MINC_DISABLE;
				HAL_DMA_Init(&hdma_spi2_rx);
			}
			if (hdma_spi2_tx.Init.MemInc != DMA_MINC_ENABLE)
			{
				hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
				HAL_DMA_Init(&hdma_spi2_tx);
			}
			res = HAL_SPI_TransmitReceive_DMA(&hspi2, out_data, SPI_tmp_buff, count);
		}
		else if (out_data == NULL)
		{
			if (hdma_spi2_rx.Init.MemInc != DMA_MINC_ENABLE)
			{
				hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
				HAL_DMA_Init(&hdma_spi2_rx);
			}
			if (hdma_spi2_tx.Init.MemInc != DMA_MINC_DISABLE)
			{
				hdma_spi2_tx.Init.MemInc = DMA_MINC_DISABLE;
				HAL_DMA_Init(&hdma_spi2_tx);
			}
			res = HAL_SPI_TransmitReceive_DMA(&hspi2, SPI_tmp_buff, in_data, count);
		}
		else
		{
			if (hdma_spi2_rx.Init.MemInc != DMA_MINC_ENABLE)
			{
				hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
				HAL_DMA_Init(&hdma_spi2_rx);
			}
			if (hdma_spi2_tx.Init.MemInc != DMA_MINC_ENABLE)
			{
				hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
				HAL_DMA_Init(&hdma_spi2_tx);
			}
			res = HAL_SPI_TransmitReceive_DMA(&hspi2, out_data, in_data, count);
		}
		while (!SPI_TXRX_ready && ((HAL_GetTick() - starttime) < 1000))
			CPULOAD_GoToSleepMode();
		Aligned_CleanInvalidateDCache_by_Addr((uint32_t)in_data, count);
	}
	else
	{
		__SPI2_CLK_ENABLE();
		if (in_data == NULL)
		{
			res = HAL_SPI_Transmit_IT(&hspi2, out_data, count);
		}
		else if (out_data == NULL)
		{
			dma_memset(in_data, 0x00, count);
			res = HAL_SPI_Receive_IT(&hspi2, in_data, count);
		}
		else
		{
			dma_memset(in_data, 0x00, count);
			res = HAL_SPI_TransmitReceive_IT(&hspi2, out_data, in_data, count);
		}
		uint32_t startTime = HAL_GetTick();
		while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY && (HAL_GetTick() - startTime) < timeout) {}
	}

	if (!hold_cs)
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	SPI_busy = false;
	if (res == HAL_TIMEOUT)
	{
		println("[ERR] SPI timeout");
		return false;
	}
	if (res == HAL_ERROR)
	{
		println("[ERR] SPI error, code: ", hspi2.ErrorCode);
		return false;
	}

	return true;
}

/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */
float32_t quick_median_select(float32_t *arr, int n)
{
	int low, high;
	int median;
	int middle, ll, hh;

	low = 0;
	high = n - 1;
	median = (low + high) / 2;
	for (;;)
	{
		if (high <= low) /* One element only */
			return arr[median];

		if (high == low + 1)
		{ /* Two elements only */
			if (arr[low] > arr[high])
				ELEM_SWAP_F32(arr[low], arr[high]);
			return arr[median];
		}

		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;
		if (arr[middle] > arr[high])
			ELEM_SWAP_F32(arr[middle], arr[high]);
		if (arr[low] > arr[high])
			ELEM_SWAP_F32(arr[low], arr[high]);
		if (arr[middle] > arr[low])
			ELEM_SWAP_F32(arr[middle], arr[low]);

		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP_F32(arr[middle], arr[low + 1]);

		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;)
		{
			do
				ll++;
			while (arr[low] > arr[ll]);
			do
				hh--;
			while (arr[hh] > arr[low]);

			if (hh < ll)
				break;

			ELEM_SWAP_F32(arr[ll], arr[hh]);
		}

		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP_F32(arr[low], arr[hh]);

		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}

static uint32_t dma_memset32_reg = 0;
static bool dma_memset32_busy = false;
void dma_memset32(void *dest, uint32_t val, uint32_t size)
{
	if (size == 0)
		return;

	if (dma_memset32_busy) //for async calls
	{
		memset(dest, val, size * 4);
		return;
	}

	dma_memset32_busy = true;
	dma_memset32_reg = val;
	Aligned_CleanDCache_by_Addr(&dma_memset32_reg, sizeof(dma_memset32_reg));
	Aligned_CleanDCache_by_Addr(dest, size * 4);

	HAL_MDMA_Start(&hmdma_mdma_channel44_sw_0, (uint32_t)&dma_memset32_reg, (uint32_t)dest, 4 * size, 1);
	SLEEPING_MDMA_PollForTransfer(&hmdma_mdma_channel44_sw_0);

	Aligned_CleanInvalidateDCache_by_Addr(dest, size * 4);
	dma_memset32_busy = false;

	/*uint32_t *pDst = (uint32_t *)dest;
	for(uint32_t i = 0; i < size; i++)
		if(pDst[i] != val)
			println(i);*/
}

void dma_memset(void *dest, uint8_t val, uint32_t size)
{
	if (dma_memset32_busy || size < 128) //for async and fast calls
	{
		memset(dest, val, size);
		return;
	}

	//left align
	char *pDst = (char *)dest;
	while ((uint32_t)pDst != (((uint32_t)pDst) & ~(uint32_t)0x3) && size > 0)
	{
		*pDst++ = val;
		size--;
	}

	if (size > 0)
	{
		//center fills in 32bit
		uint32_t val32 = (val << 24) | (val << 16) | (val << 8) | (val << 0);
		uint32_t block32 = size / 4;
		uint32_t block8 = size % 4;
		uint32_t max_block = DMA_MAX_BLOCK / 4;
		while (block32 > max_block)
		{
			dma_memset32(pDst, val32, max_block);
			block32 -= max_block;
			pDst += max_block * 4;
		}
		dma_memset32(pDst, val32, block32);

		//right align
		if (block8 > 0)
		{
			pDst += block32 * 4;
			while (block8--)
				*pDst++ = val;
		}
	}
}

static bool dma_memcpy32_busy = false;
void dma_memcpy32(void *dest, void *src, uint32_t size)
{
	if (size == 0)
		return;

	if (dma_memcpy32_busy) //for async calls
	{
		memcpy(dest, src, size * 4);
		return;
	}

	dma_memcpy32_busy = true;
	Aligned_CleanDCache_by_Addr(src, size * 4);
	Aligned_CleanDCache_by_Addr(dest, size * 4);

	uint8_t res = HAL_MDMA_Start(&hmdma_mdma_channel40_sw_0, (uint32_t)src, (uint32_t)dest, size * 4, 1);
	SLEEPING_MDMA_PollForTransfer(&hmdma_mdma_channel40_sw_0);

	Aligned_CleanInvalidateDCache_by_Addr(dest, size * 4);
	dma_memcpy32_busy = false;

	/*char *pSrc = (char *)src;
	char *pDst = (char *)dest;
	for(uint32_t i = 0; i < size * 4; i++)
		if(pSrc[i] != pDst[i])
			println(size * 4, " ", i);*/
}

void dma_memcpy(void *dest, void *src, uint32_t size)
{
	if (dma_memcpy32_busy || size < 1024) //for async and fast calls
	{
		memcpy(dest, src, size);
		return;
	}

	//left align
	char *pSrc = (char *)src;
	char *pDst = (char *)dest;
	while (((uint32_t)pSrc != (((uint32_t)pSrc) & ~(uint32_t)0x3) || (uint32_t)pDst != (((uint32_t)pDst) & ~(uint32_t)0x3)) && size > 0)
	{
		*pDst++ = *pSrc++;
		size--;
	}

	if (size > 0)
	{
		//center copy in 32bit
		uint32_t block32 = size / 4;
		uint32_t block8 = size % 4;
		uint32_t max_block = DMA_MAX_BLOCK / 4;
		while (block32 > max_block)
		{
			dma_memcpy32(pDst, pSrc, max_block);
			block32 -= max_block;
			pDst += max_block * 4;
			pSrc += max_block * 4;
		}
		dma_memcpy32(pDst, pSrc, block32);

		//right align
		if (block8 > 0)
		{
			pDst += block32 * 4;
			pSrc += block32 * 4;
			while (block8--)
				*pDst++ = *pSrc++;
		}
	}
}

void SLEEPING_MDMA_PollForTransfer(MDMA_HandleTypeDef *hmdma)
{
#define Timeout 100
	uint32_t tickstart;

	if (HAL_MDMA_STATE_BUSY != hmdma->State)
		return;

	/* Get timeout */
	tickstart = HAL_GetTick();

	while (__HAL_MDMA_GET_FLAG(hmdma, MDMA_FLAG_CTC) == 0U)
	{
		if ((__HAL_MDMA_GET_FLAG(hmdma, MDMA_FLAG_TE) != 0U))
		{
			(void)HAL_MDMA_Abort(hmdma); /* if error then abort the current transfer */
			return;
		}

		/* Check for the Timeout */
		if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
		{
			(void)HAL_MDMA_Abort(hmdma); /* if timeout then abort the current transfer */
			return;
		}

		//go sleep
		CPULOAD_GoToSleepMode();
	}

	/* Clear the transfer level flag */
	__HAL_MDMA_CLEAR_FLAG(hmdma, (MDMA_FLAG_BRT | MDMA_FLAG_BT | MDMA_FLAG_BFTC | MDMA_FLAG_CTC));

	/* Process unlocked */
	__HAL_UNLOCK(hmdma);

	hmdma->State = HAL_MDMA_STATE_READY;
}
