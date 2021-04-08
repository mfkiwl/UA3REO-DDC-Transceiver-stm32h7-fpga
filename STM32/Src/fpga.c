#include "fpga.h"
#include "main.h"
#include "trx_manager.h"
#include "lcd.h"

#if FPGA_FLASH_IN_HEX
#include "fpga_flash.h"
#endif

// Public variables
volatile uint32_t FPGA_samples = 0;			 // counter of the number of samples when exchanging with FPGA
volatile bool FPGA_NeedSendParams = false;	 // flag of the need to send parameters to FPGA
volatile bool FPGA_NeedGetParams = false;	 // flag of the need to get parameters from FPGA
volatile bool FPGA_NeedRestart = true;		 // flag of necessity to restart FPGA modules
volatile bool FPGA_Buffer_underrun = false;	 // flag of lack of data from FPGA
uint_fast16_t FPGA_Audio_RXBuffer_Index = 0; // current index in FPGA buffers
uint_fast16_t FPGA_Audio_TXBuffer_Index = 0; // current index in FPGA buffers
bool FPGA_Audio_Buffer_State = true;		 // buffer state, half or full full true - compleate; false - half
bool FPGA_RX_Buffer_Current = true;			 // buffer state, false - fill B, work A
bool FPGA_RX_buffer_ready = true;
volatile float32_t FPGA_Audio_Buffer_RX1_Q_A[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0}; // FPGA buffers
volatile float32_t FPGA_Audio_Buffer_RX1_I_A[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_Buffer_RX1_Q_B[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0}; // FPGA buffers
volatile float32_t FPGA_Audio_Buffer_RX1_I_B[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_Buffer_RX2_Q_A[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_Buffer_RX2_I_A[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_Buffer_RX2_Q_B[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_Buffer_RX2_I_B[FPGA_RX_IQ_BUFFER_HALF_SIZE] = {0};
volatile float32_t FPGA_Audio_SendBuffer_Q[FPGA_TX_IQ_BUFFER_SIZE] = {0};
volatile float32_t FPGA_Audio_SendBuffer_I[FPGA_TX_IQ_BUFFER_SIZE] = {0};
uint16_t FPGA_FW_Version[3] = {0};

// Private variables
static GPIO_InitTypeDef FPGA_GPIO_InitStruct; // structure of GPIO ports
bool FPGA_bus_stop = true;					  // suspend the FPGA bus

// Prototypes
static inline void FPGA_clockFall(void);			// remove CLK signal
static inline void FPGA_clockRise(void);			// raise the CLK signal
static inline void FPGA_syncAndClockRiseFall(void); // raise CLK and SYNC signals, then release
static void FPGA_fpgadata_sendiq(void);				// send IQ data
static void FPGA_fpgadata_getiq(void);				// get IQ data
static void FPGA_fpgadata_getparam(void);			// get parameters
static void FPGA_fpgadata_sendparam(void);			// send parameters
static void FPGA_setBusInput(void);					// switch the bus to input
static void FPGA_setBusOutput(void);				// switch bus to pin
#if FPGA_FLASH_IN_HEX
static bool FPGA_is_present(void);			  // check that the FPGA has firmware
static bool FPGA_spi_flash_verify(bool full); // read the contents of the FPGA SPI memory
static void FPGA_spi_flash_write(void);		  // write new contents of FPGA SPI memory
static void FPGA_spi_flash_erase(void);		  // clear flash memory
#endif

// initialize exchange with FPGA
void FPGA_Init(bool bus_test, bool firmware_test)
{
	FPGA_bus_stop = true;

	FPGA_GPIO_InitStruct.Pin = FPGA_BUS_D0_Pin | FPGA_BUS_D1_Pin | FPGA_BUS_D2_Pin | FPGA_BUS_D3_Pin | FPGA_BUS_D4_Pin | FPGA_BUS_D5_Pin | FPGA_BUS_D6_Pin | FPGA_BUS_D7_Pin;
	FPGA_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	FPGA_GPIO_InitStruct.Pull = GPIO_PULLUP;
	FPGA_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(FPGA_BUS_D0_GPIO_Port, &FPGA_GPIO_InitStruct);

	FPGA_GPIO_InitStruct.Pin = FPGA_CLK_Pin | FPGA_SYNC_Pin;
	FPGA_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	FPGA_GPIO_InitStruct.Pull = GPIO_PULLUP;
	FPGA_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(FPGA_CLK_GPIO_Port, &FPGA_GPIO_InitStruct);

	//BUS TEST
	for (uint16_t i = 0; i < 256; i++)
	{
		FPGA_setBusOutput();
		FPGA_writePacket(0);
		FPGA_syncAndClockRiseFall();

		FPGA_writePacket(i);
		FPGA_clockRise();
		FPGA_clockFall();

		FPGA_setBusInput();
		FPGA_clockRise();
		uint8_t ret = FPGA_readPacket;
		FPGA_clockFall();

		if (ret != i)
		{
			char buff[64];
			sprintf(buff, "BUS Error: %d -> %d", i, ret);
			LCD_showError(buff, false);
			HAL_Delay(1000);
		}
	}

	//GET FW VERSION

	FPGA_setBusOutput();
	FPGA_writePacket(8);
	FPGA_syncAndClockRiseFall();

	FPGA_setBusInput();
	FPGA_clockRise();
	FPGA_FW_Version[2] = FPGA_readPacket;
	FPGA_clockFall();
	FPGA_clockRise();
	FPGA_FW_Version[1] = FPGA_readPacket;
	FPGA_clockFall();
	FPGA_clockRise();
	FPGA_FW_Version[0] = FPGA_readPacket;
	FPGA_clockFall();

	if (bus_test) //BUS STRESS TEST MODE
	{
		LCD_showError("Check FPGA BUS...", false);
		HAL_Delay(1000);

		while (bus_test)
		{
			for (uint16_t i = 0; i < 256; i++)
			{
				FPGA_setBusOutput();
				FPGA_writePacket(0);
				FPGA_syncAndClockRiseFall();

				FPGA_writePacket(i);
				FPGA_clockRise();
				FPGA_clockFall();

				FPGA_setBusInput();
				FPGA_clockRise();
				uint8_t ret = FPGA_readPacket;
				FPGA_clockFall();

				if (ret != i)
				{
					char buff[64];
					sprintf(buff, "BUS Error: %d -> %d", i, ret);
					LCD_showError(buff, false);
					HAL_Delay(1000);
				}
			}
			LCD_showError("Check compleate!", false);
		}
	}

	if (firmware_test) //FIRMWARE VERIFICATION MODE
	{
		LCD_showError("Check FPGA FIRMWARE...", false);
		HAL_Delay(1000);
#if FPGA_FLASH_IN_HEX
		if (FPGA_spi_flash_verify(true))
		{
			LCD_showError("FPGA flash verification complete!", false);
			HAL_Delay(1000);
		}
#endif
	}

#if FPGA_FLASH_IN_HEX
	if (FPGA_is_present())
	{
		if (!FPGA_spi_flash_verify(false)) // check the first 2048 bytes of FPGA firmware
		{
			while (!FPGA_spi_flash_verify(true))
			{
				FPGA_spi_flash_erase();
				FPGA_spi_flash_write();
				if (FPGA_spi_flash_verify(true))
				{
					LCD_showError("Flash update complete!", false);
					HAL_Delay(1000);
					HAL_GPIO_WritePin(PWR_HOLD_GPIO_Port, PWR_HOLD_Pin, GPIO_PIN_RESET);
					while (true)
					{
					};
				}
			}
		}
	}
#endif

	//pre-reset FPGA to sync IQ data
	FPGA_setBusOutput();
	FPGA_writePacket(5); // RESET ON
	FPGA_syncAndClockRiseFall();
	HAL_Delay(100);
	FPGA_writePacket(6); // RESET OFF
	FPGA_syncAndClockRiseFall();

	//star FPGA bus
	FPGA_bus_stop = false;
}

// restart FPGA modules
void FPGA_restart(void) // restart FPGA modules
{
	static bool FPGA_restart_state = false;
	if (!FPGA_restart_state)
	{
		FPGA_setBusOutput();
		FPGA_writePacket(5); // RESET ON
		FPGA_syncAndClockRiseFall();
	}
	else
	{
		FPGA_writePacket(6); // RESET OFF
		FPGA_syncAndClockRiseFall();
		FPGA_NeedRestart = false;
	}
	FPGA_restart_state = !FPGA_restart_state;
}

// exchange parameters with FPGA
void FPGA_fpgadata_stuffclock(void)
{
	if (!FPGA_NeedSendParams && !FPGA_NeedGetParams && !FPGA_NeedRestart)
		return;
	if (FPGA_bus_stop)
		return;
	uint8_t FPGA_fpgadata_out_tmp8 = 0;
	//data exchange

	//STAGE 1
	//out
	if (FPGA_NeedSendParams) //send params
		FPGA_fpgadata_out_tmp8 = 1;
	else //get params
		FPGA_fpgadata_out_tmp8 = 2;

	FPGA_setBusOutput();
	FPGA_writePacket(FPGA_fpgadata_out_tmp8);
	FPGA_syncAndClockRiseFall();

	if (FPGA_NeedSendParams)
	{
		FPGA_fpgadata_sendparam();
		FPGA_NeedSendParams = false;
	}
	else if (FPGA_NeedRestart)
	{
		FPGA_restart();
	}
	else if (FPGA_NeedGetParams)
	{
		FPGA_fpgadata_getparam();
		FPGA_NeedGetParams = false;
	}
}

// exchange IQ data with FPGA
void FPGA_fpgadata_iqclock(void)
{
	if (FPGA_bus_stop)
		return;
	VFO *current_vfo = CurrentVFO();
	if (current_vfo->Mode == TRX_MODE_LOOPBACK)
		return;
	//data exchange

	//STAGE 1
	//out
	FPGA_setBusOutput();
	if (TRX_on_TX())
	{
		FPGA_writePacket(3); //TX SEND IQ
		FPGA_syncAndClockRiseFall();
		FPGA_fpgadata_sendiq();
	}
	else
	{
		FPGA_writePacket(4); //RX GET IQ
		FPGA_syncAndClockRiseFall();

		//blocks by 48k
		switch (TRX_GetRXSampleRateENUM)
		{
		case TRX_SAMPLERATE_K48:
			FPGA_fpgadata_getiq();
			break;
		case TRX_SAMPLERATE_K96:
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			break;
		case TRX_SAMPLERATE_K192:
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			break;
		case TRX_SAMPLERATE_K384:
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();

			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			FPGA_fpgadata_getiq();
			break;
		default:
			break;
		}
	}
}

// send parameters
static inline void FPGA_fpgadata_sendparam(void)
{
	uint8_t FPGA_fpgadata_out_tmp8 = 0;
	VFO *current_vfo = CurrentVFO();

	//STAGE 2
	//out PTT+PREAMP
	bitWrite(FPGA_fpgadata_out_tmp8, 0, (!TRX.ADC_SHDN && !TRX_on_TX() && current_vfo->Mode != TRX_MODE_LOOPBACK));				   //RX1
	bitWrite(FPGA_fpgadata_out_tmp8, 1, (!TRX.ADC_SHDN && TRX.Dual_RX && !TRX_on_TX() && current_vfo->Mode != TRX_MODE_LOOPBACK)); //RX2
	bitWrite(FPGA_fpgadata_out_tmp8, 2, (TRX_on_TX() && current_vfo->Mode != TRX_MODE_LOOPBACK));								   //TX
	bitWrite(FPGA_fpgadata_out_tmp8, 3, TRX.ADC_DITH);
	bitWrite(FPGA_fpgadata_out_tmp8, 4, TRX.ADC_SHDN);
	if (TRX_on_TX())
		bitWrite(FPGA_fpgadata_out_tmp8, 4, true); //shutdown ADC on TX
	bitWrite(FPGA_fpgadata_out_tmp8, 5, TRX.ADC_RAND);
	bitWrite(FPGA_fpgadata_out_tmp8, 6, TRX.ADC_PGA);
	if (!TRX_on_TX())
		bitWrite(FPGA_fpgadata_out_tmp8, 7, TRX.ADC_Driver);
	FPGA_writePacket(FPGA_fpgadata_out_tmp8);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 3
	//out RX1-FREQ
	FPGA_writePacket(((TRX_freq_phrase & (0xFFU << 24)) >> 24));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 4
	//out RX1-FREQ
	FPGA_writePacket(((TRX_freq_phrase & (0XFFU << 16)) >> 16));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 5
	//OUT RX1-FREQ
	FPGA_writePacket(((TRX_freq_phrase & (0XFFU << 8)) >> 8));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 6
	//OUT RX1-FREQ
	FPGA_writePacket(TRX_freq_phrase & 0XFFU);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 7
	//out RX2-FREQ
	FPGA_writePacket(((TRX_freq_phrase2 & (0XFFU << 24)) >> 24));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 8
	//out RX2-FREQ
	FPGA_writePacket(((TRX_freq_phrase2 & (0XFFU << 16)) >> 16));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 9
	//OUT RX2-FREQ
	FPGA_writePacket(((TRX_freq_phrase2 & (0XFFU << 8)) >> 8));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 10
	//OUT RX2-FREQ
	FPGA_writePacket(TRX_freq_phrase2 & 0XFF);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 11
	//OUT CICCOMP-GAIN
	switch (TRX_GetRXSampleRateENUM)
	{
	case TRX_SAMPLERATE_K48:
		FPGA_writePacket(CALIBRATE.CICFIR_GAINER_48K_val);
		break;
	case TRX_SAMPLERATE_K96:
		FPGA_writePacket(CALIBRATE.CICFIR_GAINER_96K_val);
		break;
	case TRX_SAMPLERATE_K192:
		FPGA_writePacket(CALIBRATE.CICFIR_GAINER_192K_val);
		break;
	case TRX_SAMPLERATE_K384:
		FPGA_writePacket(CALIBRATE.CICFIR_GAINER_384K_val);
		break;
	default:
		break;
	}
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 12
	//OUT TX-CICCOMP-GAIN
	FPGA_writePacket(CALIBRATE.TXCICFIR_GAINER_val);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 13
	//OUT DAC-GAIN
	FPGA_writePacket(CALIBRATE.DAC_GAINER_val);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 14
	//OUT ADC OFFSET
	FPGA_writePacket(((CALIBRATE.adc_offset & (0XFFU << 8)) >> 8));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 15
	//OUT ADC OFFSET
	FPGA_writePacket(CALIBRATE.adc_offset & 0XFFU);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 16
	//OUT VCXO OFFSET
	FPGA_writePacket(CALIBRATE.VCXO_correction);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 17
	//OUT DAC/DCDC SETTINGS
	FPGA_fpgadata_out_tmp8 = 0;
	bitWrite(FPGA_fpgadata_out_tmp8, 0, TRX_DAC_DIV0);
	bitWrite(FPGA_fpgadata_out_tmp8, 1, TRX_DAC_DIV1);
	bitWrite(FPGA_fpgadata_out_tmp8, 2, TRX_DAC_HP1);
	bitWrite(FPGA_fpgadata_out_tmp8, 3, TRX_DAC_HP2);
	bitWrite(FPGA_fpgadata_out_tmp8, 4, TRX_DAC_X4);
	bitWrite(FPGA_fpgadata_out_tmp8, 5, TRX_DCDC_Freq);
	//11 - 48khz 01 - 96khz 10 - 192khz 00 - 384khz IQ speed
	switch (TRX_GetRXSampleRateENUM)
	{
	case TRX_SAMPLERATE_K48:
		bitWrite(FPGA_fpgadata_out_tmp8, 6, 1);
		bitWrite(FPGA_fpgadata_out_tmp8, 7, 1);
		break;
	case TRX_SAMPLERATE_K96:
		bitWrite(FPGA_fpgadata_out_tmp8, 6, 0);
		bitWrite(FPGA_fpgadata_out_tmp8, 7, 1);
		break;
	case TRX_SAMPLERATE_K192:
		bitWrite(FPGA_fpgadata_out_tmp8, 6, 1);
		bitWrite(FPGA_fpgadata_out_tmp8, 7, 0);
		break;
	case TRX_SAMPLERATE_K384:
		bitWrite(FPGA_fpgadata_out_tmp8, 6, 0);
		bitWrite(FPGA_fpgadata_out_tmp8, 7, 0);
		break;
	default:
		break;
	}
	FPGA_writePacket(FPGA_fpgadata_out_tmp8);
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 18
	//out TX-FREQ
	FPGA_writePacket(((TRX_freq_phrase_tx & (0XFFU << 24)) >> 24));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 19
	//out TX-FREQ
	FPGA_writePacket(((TRX_freq_phrase_tx & (0XFFU << 16)) >> 16));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 20
	//OUT TX-FREQ
	FPGA_writePacket(((TRX_freq_phrase_tx & (0XFFU << 8)) >> 8));
	FPGA_clockRise();
	FPGA_clockFall();

	//STAGE 21
	//OUT TX-FREQ
	FPGA_writePacket(TRX_freq_phrase_tx & 0XFFU);
	FPGA_clockRise();
	FPGA_clockFall();
}

// get parameters
static inline void FPGA_fpgadata_getparam(void)
{
	register uint8_t FPGA_fpgadata_in_tmp8 = 0;
	register int32_t FPGA_fpgadata_in_tmp32 = 0;
	FPGA_setBusInput();

	//STAGE 2
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp8 = FPGA_readPacket;
	TRX_ADC_OTR = bitRead(FPGA_fpgadata_in_tmp8, 0);
	TRX_DAC_OTR = bitRead(FPGA_fpgadata_in_tmp8, 1);
	/*bool IQ_overrun = bitRead(FPGA_fpgadata_in_tmp8, 2);
	if(IQ_overrun)
		println("iq overrun");*/
	FPGA_clockFall();

	//STAGE 3
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp8 = FPGA_readPacket;
	FPGA_clockFall();
	//STAGE 4
	FPGA_clockRise();
	TRX_ADC_MINAMPLITUDE = (int16_t)(((FPGA_fpgadata_in_tmp8 << 8) & 0xFF00) | FPGA_readPacket);
	FPGA_clockFall();

	//STAGE 5
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp8 = FPGA_readPacket;
	FPGA_clockFall();
	//STAGE 6
	FPGA_clockRise();
	TRX_ADC_MAXAMPLITUDE = (int16_t)(((FPGA_fpgadata_in_tmp8 << 8) & 0xFF00) | FPGA_readPacket);
	FPGA_clockFall();

	//STAGE 7 - TCXO ERROR
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp8 = FPGA_readPacket;
	FPGA_fpgadata_in_tmp32 = 0;
	if (bitRead(FPGA_fpgadata_in_tmp8, 7) == 1)
		FPGA_fpgadata_in_tmp32 = 0xFF000000;
	FPGA_fpgadata_in_tmp32 |= (FPGA_fpgadata_in_tmp8 << 16);
	FPGA_clockFall();
	//STAGE 8
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 8);
	FPGA_clockFall();
	//STAGE 9
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket);
	TRX_VCXO_ERROR = FPGA_fpgadata_in_tmp32;
	FPGA_clockFall();
}

// get IQ data
static float32_t *FFTInput_I_current = (float32_t *)&FFTInput_I_A;
static float32_t *FFTInput_Q_current = (float32_t *)&FFTInput_Q_A;
static float32_t *FPGA_Audio_Buffer_RX1_I_current = (float32_t *)&FPGA_Audio_Buffer_RX1_I_A;
static float32_t *FPGA_Audio_Buffer_RX1_Q_current = (float32_t *)&FPGA_Audio_Buffer_RX1_Q_A;
static float32_t *FPGA_Audio_Buffer_RX2_I_current = (float32_t *)&FPGA_Audio_Buffer_RX2_I_A;
static float32_t *FPGA_Audio_Buffer_RX2_Q_current = (float32_t *)&FPGA_Audio_Buffer_RX2_Q_A;
static inline void FPGA_fpgadata_getiq(void)
{
	register int_fast32_t FPGA_fpgadata_in_tmp32 = 0;

	float32_t FPGA_fpgadata_in_float32_i = 0;
	float32_t FPGA_fpgadata_in_float32_q = 0;
	FPGA_samples++;
	FPGA_setBusInput();

	//STAGE 2 in Q RX1
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 = (FPGA_readPacket << 24);
	FPGA_clockFall();

	//STAGE 3
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 16);
	FPGA_clockFall();

	//STAGE 4
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 8);
	FPGA_clockFall();

	//STAGE 5
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket);
	FPGA_clockFall();

	FPGA_fpgadata_in_float32_q = (float32_t)FPGA_fpgadata_in_tmp32;

	//STAGE 6 in I RX1
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 = (FPGA_readPacket << 24);
	FPGA_clockFall();

	//STAGE 7
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 16);
	FPGA_clockFall();

	//STAGE 8
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 8);
	FPGA_clockFall();

	//STAGE 9
	FPGA_clockRise();
	FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket);
	FPGA_clockFall();

	FPGA_fpgadata_in_float32_i = (float32_t)FPGA_fpgadata_in_tmp32;

	FPGA_fpgadata_in_float32_q = FPGA_fpgadata_in_float32_q / 2147483648.0f;
	FPGA_fpgadata_in_float32_i = FPGA_fpgadata_in_float32_i / 2147483648.0f;
	if (TRX_RX1_IQ_swap)
	{
		FFTInput_I_current[FFT_buff_index] = FPGA_fpgadata_in_float32_q;
		FPGA_Audio_Buffer_RX1_I_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_q;
		FFTInput_Q_current[FFT_buff_index] = FPGA_fpgadata_in_float32_i;
		FPGA_Audio_Buffer_RX1_Q_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_i;
	}
	else
	{
		FFTInput_Q_current[FFT_buff_index] = FPGA_fpgadata_in_float32_q;
		FPGA_Audio_Buffer_RX1_Q_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_q;
		FFTInput_I_current[FFT_buff_index] = FPGA_fpgadata_in_float32_i;
		FPGA_Audio_Buffer_RX1_I_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_i;
	}

	if (TRX.Dual_RX)
	{
		//STAGE 10 in Q RX2
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 = (FPGA_readPacket << 24);
		FPGA_clockFall();

		//STAGE 11
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 16);
		FPGA_clockFall();

		//STAGE 12
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 8);
		FPGA_clockFall();

		//STAGE 13
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket);
		FPGA_clockFall();

		FPGA_fpgadata_in_float32_q = (float32_t)FPGA_fpgadata_in_tmp32;

		//STAGE 14 in I RX2
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 = (FPGA_readPacket << 24);
		FPGA_clockFall();

		//STAGE 15
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 16);
		FPGA_clockFall();

		//STAGE 16
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket << 8);
		FPGA_clockFall();

		//STAGE 17
		FPGA_clockRise();
		FPGA_fpgadata_in_tmp32 |= (FPGA_readPacket);
		FPGA_clockFall();

		FPGA_fpgadata_in_float32_i = (float32_t)FPGA_fpgadata_in_tmp32;

		FPGA_fpgadata_in_float32_q = FPGA_fpgadata_in_float32_q / 2147483648.0f;
		FPGA_fpgadata_in_float32_i = FPGA_fpgadata_in_float32_i / 2147483648.0f;
		if (TRX_RX2_IQ_swap)
		{
			FPGA_Audio_Buffer_RX2_I_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_q;
			FPGA_Audio_Buffer_RX2_Q_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_i;
		}
		else
		{
			FPGA_Audio_Buffer_RX2_Q_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_q;
			FPGA_Audio_Buffer_RX2_I_current[FPGA_Audio_RXBuffer_Index] = FPGA_fpgadata_in_float32_i;
		}
	}

	FPGA_Audio_RXBuffer_Index++;
	if (FPGA_Audio_RXBuffer_Index == FPGA_RX_IQ_BUFFER_HALF_SIZE)
	{
		FPGA_Audio_RXBuffer_Index = 0;
		if (FPGA_RX_buffer_ready)
		{
			FPGA_Buffer_underrun = true;
			//println("fpga overrun");
		}
		else
		{
			FPGA_RX_buffer_ready = true;
			FPGA_RX_Buffer_Current = !FPGA_RX_Buffer_Current;
			FPGA_Audio_Buffer_RX1_I_current = FPGA_RX_Buffer_Current ? (float32_t *)&FPGA_Audio_Buffer_RX1_I_A : (float32_t *)&FPGA_Audio_Buffer_RX1_I_B;
			FPGA_Audio_Buffer_RX1_Q_current = FPGA_RX_Buffer_Current ? (float32_t *)&FPGA_Audio_Buffer_RX1_Q_A : (float32_t *)&FPGA_Audio_Buffer_RX1_Q_B;
			FPGA_Audio_Buffer_RX2_I_current = FPGA_RX_Buffer_Current ? (float32_t *)&FPGA_Audio_Buffer_RX2_I_A : (float32_t *)&FPGA_Audio_Buffer_RX2_I_B;
			FPGA_Audio_Buffer_RX2_Q_current = FPGA_RX_Buffer_Current ? (float32_t *)&FPGA_Audio_Buffer_RX2_Q_A : (float32_t *)&FPGA_Audio_Buffer_RX2_Q_B;
		}
	}

	FFT_buff_index++;
	if (FFT_buff_index >= FFT_HALF_SIZE)
	{
		FFT_buff_index = 0;
		if (FFT_new_buffer_ready)
		{
			//println("fft overrun");
		}
		else
		{
			FFT_new_buffer_ready = true;
			FFT_buff_current = !FFT_buff_current;
			FFTInput_I_current = FFT_buff_current ? (float32_t *)&FFTInput_I_A : (float32_t *)&FFTInput_I_B;
			FFTInput_Q_current = FFT_buff_current ? (float32_t *)&FFTInput_Q_A : (float32_t *)&FFTInput_Q_B;
		}
	}
}

// send IQ data
static inline void FPGA_fpgadata_sendiq(void)
{
	q31_t FPGA_fpgadata_out_q_tmp32 = 0;
	q31_t FPGA_fpgadata_out_i_tmp32 = 0;
	q31_t FPGA_fpgadata_out_tmp_tmp32 = 0;
	arm_float_to_q31((float32_t *)&FPGA_Audio_SendBuffer_Q[FPGA_Audio_TXBuffer_Index], &FPGA_fpgadata_out_q_tmp32, 1);
	arm_float_to_q31((float32_t *)&FPGA_Audio_SendBuffer_I[FPGA_Audio_TXBuffer_Index], &FPGA_fpgadata_out_i_tmp32, 1);
	FPGA_samples++;

	if (TRX_TX_IQ_swap)
	{
		FPGA_fpgadata_out_tmp_tmp32 = FPGA_fpgadata_out_i_tmp32;
		FPGA_fpgadata_out_i_tmp32 = FPGA_fpgadata_out_q_tmp32;
		FPGA_fpgadata_out_q_tmp32 = FPGA_fpgadata_out_tmp_tmp32;
	}

	//STAGE 2 out Q
	FPGA_writePacket((FPGA_fpgadata_out_q_tmp32 >> 24) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 3
	FPGA_writePacket((FPGA_fpgadata_out_q_tmp32 >> 16) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 4
	FPGA_writePacket((FPGA_fpgadata_out_q_tmp32 >> 8) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 5
	FPGA_writePacket((FPGA_fpgadata_out_q_tmp32 >> 0) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 6 out I
	FPGA_writePacket((FPGA_fpgadata_out_i_tmp32 >> 24) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 7
	FPGA_writePacket((FPGA_fpgadata_out_i_tmp32 >> 16) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 8
	FPGA_writePacket((FPGA_fpgadata_out_i_tmp32 >> 8) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	//STAGE 9
	FPGA_writePacket((FPGA_fpgadata_out_i_tmp32 >> 0) & 0xFF);
	//clock
	FPGA_clockRise();
	//clock
	FPGA_clockFall();

	FPGA_Audio_TXBuffer_Index++;
	if (FPGA_Audio_TXBuffer_Index == FPGA_TX_IQ_BUFFER_SIZE)
	{
		if (Processor_NeedTXBuffer)
		{
			FPGA_Buffer_underrun = true;
			FPGA_Audio_TXBuffer_Index--;
		}
		else
		{
			FPGA_Audio_TXBuffer_Index = 0;
			FPGA_Audio_Buffer_State = true;
			Processor_NeedTXBuffer = true;
		}
	}
	else if (FPGA_Audio_TXBuffer_Index == FPGA_TX_IQ_BUFFER_HALF_SIZE)
	{
		if (Processor_NeedTXBuffer)
		{
			FPGA_Buffer_underrun = true;
			FPGA_Audio_TXBuffer_Index--;
		}
		else
		{
			FPGA_Audio_Buffer_State = false;
			Processor_NeedTXBuffer = true;
		}
	}
}

// switch the bus to input
static inline void FPGA_setBusInput(void)
{
	// Configure IO Direction mode (Input)
	/*register uint32_t temp = GPIOA->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (0 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (0 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (1 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (1 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (2 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (2 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (3 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (3 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (4 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (4 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (5 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (5 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (6 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (6 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (7 * 2U));
	temp |= ((GPIO_MODE_INPUT & 0x00000003U) << (7 * 2U));
	//sendToDebug_uint32(temp,false);
	GPIOA->MODER = temp;*/

	GPIOA->MODER = -1431764992;
}

// switch bus to pin
static inline void FPGA_setBusOutput(void)
{
	// Configure IO Direction mode (Output)
	/*uint32_t temp = GPIOA->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (0 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (0 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (1 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (1 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (2 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (2 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (3 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (3 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (4 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (4 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (5 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (5 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (6 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (6 * 2U));
	temp &= ~(GPIO_MODER_MODE0 << (7 * 2U));
	temp |= ((GPIO_MODE_OUTPUT_PP & 0x00000003U) << (7 * 2U));
	//sendToDebug_uint32(temp,false);
	GPIOA->MODER = temp;*/

	GPIOA->MODER = -1431743147;
}

// raise the CLK signal
static inline void FPGA_clockRise(void)
{
	FPGA_CLK_GPIO_Port->BSRR = FPGA_CLK_Pin;
}

// remove CLK signal
static inline void FPGA_clockFall(void)
{
	FPGA_CLK_GPIO_Port->BSRR = (FPGA_CLK_Pin << 16U);
}

// raise CLK and SYNC signal, then lower
static inline void FPGA_syncAndClockRiseFall(void)
{
	FPGA_CLK_GPIO_Port->BSRR = FPGA_SYNC_Pin;
	FPGA_CLK_GPIO_Port->BSRR = FPGA_CLK_Pin;
	FPGA_CLK_GPIO_Port->BSRR = (FPGA_SYNC_Pin << 16U) | (FPGA_CLK_Pin << 16U);
}

#if FPGA_FLASH_IN_HEX
static uint8_t FPGA_spi_start_command(uint8_t command) // execute command to SPI flash
{
	//STAGE 1
	FPGA_setBusOutput();
	FPGA_writePacket(7); //FPGA FLASH READ command
	FPGA_syncAndClockRiseFall();
	FPGA_FLASH_COMMAND_DELAY

	//STAGE 2 WRITE (F700)
	FPGA_writePacket(command);
	FPGA_clockRise();
	FPGA_clockFall();
	FPGA_FLASH_WRITE_DELAY

	//STAGE 3 READ ANSWER (F701)
	FPGA_setBusInput();
	FPGA_clockRise();
	uint8_t data = FPGA_readPacket;
	FPGA_clockFall();
	FPGA_FLASH_READ_DELAY

	return data;
}

static void FPGA_spi_stop_command(void) // shutdown with SPI flash
{
	// STAGE 1
	FPGA_setBusOutput();
	FPGA_writePacket(7); // FPGA FLASH READ command
	FPGA_syncAndClockRiseFall();
	FPGA_FLASH_COMMAND_DELAY
}

static uint8_t FPGA_spi_continue_command(uint8_t writedata) // Continue reading and writing SPI flash
{
	//STAGE 2 WRITE (F700)
	FPGA_setBusOutput();
	FPGA_writePacket(writedata);
	FPGA_clockRise();
	FPGA_clockFall();
	FPGA_FLASH_WRITE_DELAY

	//STAGE 3 READ ANSWER (F701)
	FPGA_setBusInput();
	FPGA_clockRise();
	uint8_t data = FPGA_readPacket;
	FPGA_clockFall();
	FPGA_FLASH_READ_DELAY

	return data;
}

static void FPGA_spi_flash_wait_WIP(void) // We are waiting for the end of writing to the flash (resetting the WIP register)
{
	uint8_t status = 1;
	while (bitRead(status, 0) == 1)
	{
		FPGA_spi_start_command(M25P80_READ_STATUS_REGISTER);
		status = FPGA_spi_continue_command(M25P80_READ_STATUS_REGISTER);
		FPGA_spi_stop_command();
	}
}

static bool FPGA_is_present(void) // check that the FPGA has firmware
{
	HAL_Delay(1);
	uint8_t data = 0;
	FPGA_spi_start_command(M25P80_RELEASE_from_DEEP_POWER_DOWN); //Wake-Up
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP();
	FPGA_spi_start_command(M25P80_READ_DATA_BYTES); //READ DATA BYTES
	FPGA_spi_continue_command(0x00);				//addr 1
	FPGA_spi_continue_command(0x00);				//addr 2
	FPGA_spi_continue_command(0x00);				//addr 3
	data = FPGA_spi_continue_command(0xFF);
	FPGA_spi_stop_command();
	FPGA_spi_start_command(M25P80_DEEP_POWER_DOWN); //Go sleep
	FPGA_spi_stop_command();
	if (data != 0xFF)
	{
		LCD_showError("FPGA not found", true);
		println("[ERR] FPGA not found");
		return false;
	}
	else
		return true;
}

static bool FPGA_spi_flash_verify(bool full) // check flash memory
{
	HAL_Delay(1);
	if (full)
		LCD_showError("FPGA Flash Verification...", false);
	uint8_t data = 0;
	FPGA_spi_start_command(M25P80_RELEASE_from_DEEP_POWER_DOWN); //Wake-Up
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP();
	FPGA_spi_start_command(M25P80_READ_DATA_BYTES); //READ DATA BYTES
	FPGA_spi_continue_command(0x00);				//addr 1
	FPGA_spi_continue_command(0x00);				//addr 2
	FPGA_spi_continue_command(0x00);				//addr 3
	data = FPGA_spi_continue_command(0xFF);
	uint8_t progress_prev = 0;
	uint8_t progress = 0;

	//Decompress RLE and verify
	uint32_t errors = 0;
	uint32_t file_pos = 0;
	uint32_t flash_pos = 0;
	int32_t decoded = 0;
	while (file_pos < sizeof(FILES_WOLF_JIC))
	{
		if ((int8_t)FILES_WOLF_JIC[file_pos] < 0) // no repeats
		{
			uint8_t count = (-(int8_t)FILES_WOLF_JIC[file_pos]);
			file_pos++;
			for (uint8_t p = 0; p < count; p++)
			{
				if ((decoded - FPGA_flash_file_offset) >= 0)
				{
					if (file_pos < sizeof(FILES_WOLF_JIC) && rev8((uint8_t)data) != FILES_WOLF_JIC[file_pos] && ((decoded - FPGA_flash_file_offset) < FPGA_flash_size))
					{
						errors++;
						print(flash_pos, ": FPGA: ");
						print_hex(rev8((uint8_t)data), true);
						println(" HEX: ", FILES_WOLF_JIC[file_pos]);
						print_flush();

						/*if(full)
						{
							char ctmp[50];
							sprintf(ctmp, "Error in pos: %d", flash_pos);
							LCD_showError(ctmp, true);
						}*/
					}
					data = FPGA_spi_continue_command(0xFF);
					flash_pos++;
				}
				decoded++;
				file_pos++;
			}
		}
		else // repeats
		{
			uint8_t count = ((int8_t)FILES_WOLF_JIC[file_pos]);
			file_pos++;
			for (uint8_t p = 0; p < count; p++)
			{
				if ((decoded - FPGA_flash_file_offset) >= 0)
				{
					if (file_pos < sizeof(FILES_WOLF_JIC) && rev8((uint8_t)data) != FILES_WOLF_JIC[file_pos] && ((decoded - FPGA_flash_file_offset) < FPGA_flash_size))
					{
						errors++;
						print(flash_pos, ": FPGA: ");
						print_hex(rev8((uint8_t)data), true);
						println(" HEX: ", FILES_WOLF_JIC[file_pos]);
						print_flush();

						/*if(full)
						{
							char ctmp[50];
							sprintf(ctmp, "Error in pos: %d", flash_pos);
							LCD_showError(ctmp, true);
						}*/
					}
					data = FPGA_spi_continue_command(0xFF);
					flash_pos++;
				}
				decoded++;
			}
			file_pos++;
		}
		progress = (uint8_t)((float32_t)decoded / (float32_t)(FPGA_flash_size + FPGA_flash_file_offset) * 100.0f);
		if (progress_prev != progress && full && ((progress - progress_prev) >= 5))
		{
			char ctmp[50];
			sprintf(ctmp, "FPGA Flash Verification... %d%%", progress);
			LCD_showError(ctmp, false);
			progress_prev = progress;
		}
		if (!full && decoded > (FPGA_flash_file_offset + 2048))
			break;
		if (decoded >= (FPGA_flash_size + FPGA_flash_file_offset))
			break;
		if (errors > 10)
			break;
	}
	FPGA_spi_stop_command();
	FPGA_spi_start_command(M25P80_DEEP_POWER_DOWN); //Go sleep
	FPGA_spi_stop_command();
	//
	if (errors > 0)
	{
		println("[ERR] FPGA Flash verification failed");
		LCD_showError("FPGA Flash verification failed", true);
		return false;
	}
	else
	{
		println("[OK] FPGA Flash verification compleated");
		return true;
	}
}

static void FPGA_spi_flash_erase(void) // clear flash memory
{
	HAL_Delay(1);
	LCD_showError("FPGA Flash Erasing...", false);
	FPGA_spi_start_command(M25P80_RELEASE_from_DEEP_POWER_DOWN); //Wake-Up
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP(); //wait write in progress

	FPGA_spi_start_command(M25P80_WRITE_ENABLE); //Write Enable
	FPGA_spi_stop_command();
	FPGA_spi_start_command(M25P80_BULK_ERASE); //BULK FULL CHIP ERASE
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP(); //wait write in progress

	println("[OK] FPGA Flash erased");
}

static void FPGA_spi_flash_write(void) // write new contents of FPGA SPI memory
{
	HAL_Delay(1);
	LCD_showError("FPGA Flash Programming...", false);
	uint32_t flash_pos = 0;
	uint16_t page_pos = 0;
	FPGA_spi_start_command(M25P80_RELEASE_from_DEEP_POWER_DOWN); //Wake-Up
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP();					 //wait write in progress
	FPGA_spi_start_command(M25P80_WRITE_ENABLE); //Write Enable
	FPGA_spi_stop_command();
	FPGA_spi_start_command(M25P80_PAGE_PROGRAM);		 //Page programm
	FPGA_spi_continue_command((flash_pos >> 16) & 0xFF); //addr 1
	FPGA_spi_continue_command((flash_pos >> 8) & 0xFF);	 //addr 2
	FPGA_spi_continue_command(flash_pos & 0xFF);		 //addr 3
	uint8_t progress_prev = 0;
	uint8_t progress = 0;

	//Decompress RLE and write
	uint32_t file_pos = 0;
	int32_t decoded = 0;
	while (file_pos < sizeof(FILES_WOLF_JIC))
	{
		if ((int8_t)FILES_WOLF_JIC[file_pos] < 0) //no repeats
		{
			uint8_t count = (-(int8_t)FILES_WOLF_JIC[file_pos]);
			file_pos++;
			for (uint8_t p = 0; p < count; p++)
			{
				if ((decoded - FPGA_flash_file_offset) >= 0 && ((decoded - FPGA_flash_file_offset) < FPGA_flash_size))
				{
					FPGA_spi_continue_command(rev8((uint8_t)FILES_WOLF_JIC[file_pos]));
					flash_pos++;
					page_pos++;
					if (page_pos >= FPGA_page_size)
					{
						FPGA_spi_stop_command();
						FPGA_spi_flash_wait_WIP();					 //wait write in progress
						FPGA_spi_start_command(M25P80_WRITE_ENABLE); //Write Enable
						FPGA_spi_stop_command();
						FPGA_spi_start_command(M25P80_PAGE_PROGRAM);		 //Page programm
						FPGA_spi_continue_command((flash_pos >> 16) & 0xFF); //addr 1
						FPGA_spi_continue_command((flash_pos >> 8) & 0xFF);	 //addr 2
						FPGA_spi_continue_command(flash_pos & 0xFF);		 //addr 3
						page_pos = 0;
					}
				}
				decoded++;
				file_pos++;
			}
		}
		else //repeats
		{
			uint8_t count = ((int8_t)FILES_WOLF_JIC[file_pos]);
			file_pos++;
			for (uint8_t p = 0; p < count; p++)
			{
				if ((decoded - FPGA_flash_file_offset) >= 0 && ((decoded - FPGA_flash_file_offset) < FPGA_flash_size))
				{
					FPGA_spi_continue_command(rev8((uint8_t)FILES_WOLF_JIC[file_pos]));
					flash_pos++;
					page_pos++;
					if (page_pos >= FPGA_page_size)
					{
						FPGA_spi_stop_command();
						FPGA_spi_flash_wait_WIP();					 //wait write in progress
						FPGA_spi_start_command(M25P80_WRITE_ENABLE); //Write Enable
						FPGA_spi_stop_command();
						FPGA_spi_start_command(M25P80_PAGE_PROGRAM);		 //Page programm
						FPGA_spi_continue_command((flash_pos >> 16) & 0xFF); //addr 1
						FPGA_spi_continue_command((flash_pos >> 8) & 0xFF);	 //addr 2
						FPGA_spi_continue_command(flash_pos & 0xFF);		 //addr 3
						page_pos = 0;
					}
				}
				decoded++;
			}
			file_pos++;
		}
		progress = (uint8_t)((float32_t)decoded / (float32_t)(FPGA_flash_size + FPGA_flash_file_offset) * 100.0f);
		if (progress_prev != progress && ((progress - progress_prev) >= 5))
		{
			char ctmp[50];
			sprintf(ctmp, "FPGA Flash Programming... %d%%", progress);
			LCD_showError(ctmp, false);
			progress_prev = progress;
		}
		if (decoded >= (FPGA_flash_size + FPGA_flash_file_offset))
			break;
	}
	FPGA_spi_stop_command();
	FPGA_spi_flash_wait_WIP();					  //wait write in progress
	FPGA_spi_start_command(M25P80_WRITE_DISABLE); //Write Disable
	FPGA_spi_stop_command();
	println("[OK] FPGA Flash programming compleated");
}

#endif
