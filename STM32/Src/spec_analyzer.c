#include "spec_analyzer.h"
#include "main.h"
#include "lcd_driver.h"
#include "trx_manager.h"
#include "functions.h"
#include "fpga.h"
#include "lcd.h"

//Private variables
static const uint16_t graph_start_x = 25;
static const uint16_t graph_width = LCD_WIDTH - 30;
static const uint16_t graph_start_y = 5;
static const uint16_t graph_height = LCD_HEIGHT - 25;
static float32_t now_freq;
static float32_t freq_step;
static uint16_t graph_sweep_x = 0;
static uint32_t tick_start_time = 0;
static int16_t graph_selected_x = (LCD_WIDTH - 30) / 2;
static int_fast16_t data[LCD_WIDTH - 30] = {0};

//Saved variables
static uint32_t Lastfreq = 0;
static uint_fast8_t Lastmode = 0;
static bool LastAutoGain = false;
static bool LastBandMapEnabled = false;
static bool LastRF_Filters = false;
static bool LastManualNotch = false;
static bool LastAutoNotch = false;
static uint8_t LastDNR = false;
static bool LastShift = false;
static bool LastNB = false;
static bool LastMute = false;

//Public variables
bool SYSMENU_spectrum_opened = false;

//Prototypes
static void SPEC_DrawBottomGUI(void);				   // display status at the bottom of the screen
static void SPEC_DrawGraphCol(uint16_t x, bool clear); // display the data column
static uint16_t SPEC_getYfromX(uint16_t x);			   // get height from data id

// prepare the spectrum analyzer
void SPEC_Start(void)
{
	LCD_busy = true;

	//save settings
	Lastfreq = CurrentVFO()->Freq;
	Lastmode = CurrentVFO()->Mode;
	LastAutoGain = TRX.AutoGain;
	LastBandMapEnabled = TRX.BandMapEnabled;
	LastRF_Filters = TRX.RF_Filters;
	LastManualNotch = CurrentVFO()->ManualNotchFilter;
	LastAutoNotch = CurrentVFO()->AutoNotchFilter;
	LastDNR = CurrentVFO()->DNR_Type;
	LastShift = TRX.ShiftEnabled;
	LastNB = TRX.NOISE_BLANKER;
	LastMute = TRX_Mute;

	// draw the GUI
	LCDDriver_Fill(COLOR_BLACK);
	LCDDriver_drawFastVLine(graph_start_x, graph_start_y, graph_height, COLOR_WHITE);
	LCDDriver_drawFastHLine(graph_start_x, graph_start_y + graph_height, graph_width, COLOR_WHITE);

	// horizontal labels
	static IRAM2 char ctmp[64] = {0};
	sprintf(ctmp, "%u", TRX.SPEC_Begin);
	LCDDriver_printText(ctmp, graph_start_x + 2, graph_start_y + graph_height + 3, COLOR_GREEN, COLOR_BLACK, 1);
	sprintf(ctmp, "%u", TRX.SPEC_End);
	LCDDriver_printText(ctmp, graph_start_x + graph_width - 36, graph_start_y + graph_height + 3, COLOR_GREEN, COLOR_BLACK, 1);
	SPEC_DrawBottomGUI();

	// vertical labels
	int16_t vres = (TRX.SPEC_BottomDBM - TRX.SPEC_TopDBM);
	int16_t partsize = vres / (SPEC_VParts - 1);
	for (uint8_t n = 0; n < SPEC_VParts; n++)
	{
		int32_t y = graph_start_y + (partsize * n * graph_height / vres);
		sprintf(ctmp, "%d", TRX.SPEC_TopDBM + partsize * n);
		LCDDriver_printText(ctmp, 0, (uint16_t)y, COLOR_GREEN, COLOR_BLACK, 1);
		LCDDriver_drawFastHLine(graph_start_x, (uint16_t)y, graph_width, COLOR_DGRAY);
	}

	// start scanning
	TRX.BandMapEnabled = false;
	TRX.AutoGain = false;
	TRX.RF_Filters = false;
	TRX.ShiftEnabled = false;
	TRX.NOISE_BLANKER = false;
	TRX_setFrequency(TRX.SPEC_Begin * SPEC_Resolution, CurrentVFO());
	TRX_setMode(TRX_MODE_CW_U, CurrentVFO());
	CurrentVFO()->ManualNotchFilter = false;
	CurrentVFO()->AutoNotchFilter = false;
	CurrentVFO()->DNR_Type = false;
	TRX_Mute = true;
	FPGA_NeedSendParams = true;
	now_freq = TRX.SPEC_Begin * SPEC_Resolution;
	freq_step = (TRX.SPEC_End * SPEC_Resolution - TRX.SPEC_Begin * SPEC_Resolution) / graph_width;
	graph_sweep_x = 0;
	tick_start_time = HAL_GetTick();

	LCD_busy = false;

	LCD_UpdateQuery.SystemMenu = true;
}

void SPEC_Stop(void)
{
	TRX_setFrequency(Lastfreq, CurrentVFO());
	TRX_setMode(Lastmode, CurrentVFO());
	TRX.AutoGain = LastAutoGain;
	TRX.BandMapEnabled = LastBandMapEnabled;
	TRX.RF_Filters = LastRF_Filters;
	CurrentVFO()->ManualNotchFilter = LastManualNotch;
	CurrentVFO()->AutoNotchFilter = LastAutoNotch;
	CurrentVFO()->DNR_Type = LastDNR;
	TRX.ShiftEnabled = LastShift;
	TRX.NOISE_BLANKER = LastNB;
	TRX_Mute = LastMute;
}

// draw the spectrum analyzer
void SPEC_Draw(void)
{
	if (LCD_busy)
		return;

	// Wait while data is being typed
	if ((HAL_GetTick() - tick_start_time) < SPEC_StepDelay)
	{
		LCD_UpdateQuery.SystemMenu = true;
		return;
	}
	if (Processor_RX_Power_value == 0) //-V550
		return;
	tick_start_time = HAL_GetTick();

	LCD_busy = true;
	// Read the signal strength
	TRX_DBMCalculate();

	// Draw
	data[graph_sweep_x] = TRX_RX_dBm;
	SPEC_DrawGraphCol(graph_sweep_x, true);
	// draw a marker
	if (graph_sweep_x == graph_selected_x)
		SPEC_DrawBottomGUI();

	// Move on to calculating the next step
	graph_sweep_x++;
	if (now_freq > (TRX.SPEC_End * SPEC_Resolution))
	{
		graph_sweep_x = 0;
		now_freq = TRX.SPEC_Begin * SPEC_Resolution;
	}
	now_freq += freq_step;
	TRX_setFrequency((uint32_t)now_freq, CurrentVFO());
	FPGA_NeedSendParams = true;
	LCD_busy = false;
}

// get height from data id
static uint16_t SPEC_getYfromX(uint16_t x)
{
	int32_t y = graph_start_y + ((data[x] - TRX.SPEC_TopDBM) * (graph_height) / (TRX.SPEC_BottomDBM - TRX.SPEC_TopDBM));
	if (y < graph_start_y)
		y = graph_start_y;
	if (y > ((graph_start_y + graph_height) - 1))
		y = (graph_start_y + graph_height) - 1;
	return (uint16_t)y;
}

// display the data column
static void SPEC_DrawGraphCol(uint16_t x, bool clear)
{
	if (x >= graph_width)
		return;

	if (clear)
	{
		// clear
		LCDDriver_drawFastVLine((graph_start_x + x + 1), graph_start_y, graph_height, COLOR_BLACK);
		// draw stripes behind the chart
		int16_t vres = (TRX.SPEC_BottomDBM - TRX.SPEC_TopDBM);
		for (uint8_t n = 0; n < (SPEC_VParts - 1); n++)
			LCDDriver_drawPixel((graph_start_x + x + 1), (uint16_t)(graph_start_y + ((vres / (SPEC_VParts - 1)) * n * graph_height / vres)), COLOR_DGRAY);
	}
	// draw the graph
	if (x > 0)
		LCDDriver_drawLine((graph_start_x + x), SPEC_getYfromX(x - 1), (graph_start_x + x + 1), SPEC_getYfromX(x), COLOR_RED);
	else
		LCDDriver_drawPixel((graph_start_x + x + 1), SPEC_getYfromX(x), COLOR_RED);
}

// display status at the bottom of the screen
static void SPEC_DrawBottomGUI(void)
{
	static IRAM2 char ctmp[64] = {0};
	int32_t freq = (int32_t)TRX.SPEC_Begin + (graph_selected_x * (int32_t)(TRX.SPEC_End - TRX.SPEC_Begin) / (graph_width - 1));
	sprintf(ctmp, "Freq=%dkHz DBM=%d", freq, data[graph_selected_x]);
	LCDDriver_Fill_RectWH(170, graph_start_y + graph_height + 3, 200, 6, COLOR_BLACK);
	LCDDriver_printText(ctmp, 170, graph_start_y + graph_height + 3, COLOR_GREEN, COLOR_BLACK, 1);
	LCDDriver_drawFastVLine(graph_start_x + (uint16_t)graph_selected_x + 1, graph_start_y, graph_height, COLOR_GREEN);
}

// analyzer events to the encoder
void SPEC_EncRotate(int8_t direction)
{
	if (LCD_busy)
		return;
	LCD_busy = true;
	// erase the old marker
	SPEC_DrawGraphCol((uint16_t)graph_selected_x, true);
	if (direction < 0)
		SPEC_DrawGraphCol((uint16_t)graph_selected_x + 1, false);
	// draw a new one
	graph_selected_x += direction;
	if (graph_selected_x < 0)
		graph_selected_x = 0;
	if (graph_selected_x > (graph_width - 1))
		graph_selected_x = graph_width - 1;
	SPEC_DrawBottomGUI();
	LCD_busy = false;
}
