#include "screen_layout.h"
#include "lcd_driver.h"
#include "fonts.h"

#if (defined(LAY_800x480))

extern "C" constexpr STRUCT_LAYOUT_THEME LAYOUT_THEMES[1] =
	{
		{
			.TOPBUTTONS_X1 = 0,
			.TOPBUTTONS_X2 = (LCD_WIDTH - 1),
			.TOPBUTTONS_Y1 = 95,
			.TOPBUTTONS_COUNT = 10,
			.TOPBUTTONS_WIDTH = (uint16_t)(LCD_WIDTH / LAYOUT_THEMES[0].TOPBUTTONS_COUNT),
			.TOPBUTTONS_HEIGHT = 50,
			.TOPBUTTONS_PRE_X = LAYOUT_THEMES[0].TOPBUTTONS_X1,
			.TOPBUTTONS_PRE_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_ATT_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_PRE_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_ATT_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_PGA_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_ATT_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_PGA_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_DRV_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_PGA_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_DRV_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_AGC_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_DRV_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_AGC_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_DNR_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_AGC_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_DNR_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_NB_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_DNR_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_NB_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_NOTCH_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_NB_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_NOTCH_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_FAST_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_NOTCH_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_FAST_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			.TOPBUTTONS_MUTE_X = (uint16_t)(LAYOUT_THEMES[0].TOPBUTTONS_FAST_X + LAYOUT_THEMES[0].TOPBUTTONS_WIDTH),
			.TOPBUTTONS_MUTE_Y = LAYOUT_THEMES[0].TOPBUTTONS_Y1,
			//clock
			.CLOCK_POS_Y = 17,
			.CLOCK_POS_HRS_X = (LCD_WIDTH - 75),
			.CLOCK_POS_MIN_X = (uint16_t)(LAYOUT_THEMES[0].CLOCK_POS_HRS_X + 25),
			.CLOCK_POS_SEC_X = (uint16_t)(LAYOUT_THEMES[0].CLOCK_POS_MIN_X + 25),
			.CLOCK_FONT = &FreeSans9pt7b,
			//WIFI
			.STATUS_WIFI_ICON_X = (LCD_WIDTH - 98),
			.STATUS_WIFI_ICON_Y = 3,
			// frequency output VFO-A
			.FREQ_X_OFFSET_100 = 37,
			.FREQ_X_OFFSET_10 = 73,
			.FREQ_X_OFFSET_1 = 113,
			.FREQ_X_OFFSET_KHZ = 170,
			.FREQ_X_OFFSET_HZ = 307,
			.FREQ_HEIGHT = 51,
			.FREQ_WIDTH = 370,
			.FREQ_TOP_OFFSET = 4,
			.FREQ_LEFT_MARGIN = 37,
			.FREQ_RIGHT_MARGIN = (uint16_t)(LCD_WIDTH - LAYOUT_THEMES[0].FREQ_LEFT_MARGIN - LAYOUT_THEMES[0].FREQ_WIDTH),
			.FREQ_BOTTOM_OFFSET = 8,
			.FREQ_BLOCK_HEIGHT = (uint16_t)(LAYOUT_THEMES[0].FREQ_HEIGHT + LAYOUT_THEMES[0].FREQ_TOP_OFFSET + LAYOUT_THEMES[0].FREQ_BOTTOM_OFFSET),
			.FREQ_Y_TOP = 0,
			.FREQ_Y_BASELINE = (uint16_t)(LAYOUT_THEMES[0].FREQ_Y_TOP + LAYOUT_THEMES[0].FREQ_HEIGHT + LAYOUT_THEMES[0].FREQ_TOP_OFFSET),
			.FREQ_Y_BASELINE_SMALL = (uint16_t)(LAYOUT_THEMES[0].FREQ_Y_BASELINE - 2),
			.FREQ_FONT = &FreeSans36pt7b,
			.FREQ_SMALL_FONT = &Quito32pt7b,
			.FREQ_DELIMITER_Y_OFFSET = 0,
			.FREQ_DELIMITER_X1_OFFSET = 151,
			.FREQ_DELIMITER_X2_OFFSET = 287,
			// frequency output VFO-B
			.FREQ_B_LEFT = (LCD_WIDTH - 375),
			.FREQ_B_X_OFFSET_100 = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 37),
			.FREQ_B_X_OFFSET_10 = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 73),
			.FREQ_B_X_OFFSET_1 = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 105),
			.FREQ_B_X_OFFSET_KHZ = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 153),
			.FREQ_B_X_OFFSET_HZ = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 268),
			.FREQ_B_HEIGHT = 51,
			.FREQ_B_WIDTH = 288,
			.FREQ_B_TOP_OFFSET = 4,
			.FREQ_B_LEFT_MARGIN = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 37),
			.FREQ_B_RIGHT_MARGIN = (uint16_t)(LCD_WIDTH - LAYOUT_THEMES[0].FREQ_B_LEFT_MARGIN - LAYOUT_THEMES[0].FREQ_B_WIDTH),
			.FREQ_B_BOTTOM_OFFSET = 8,
			.FREQ_B_BLOCK_HEIGHT = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_HEIGHT + LAYOUT_THEMES[0].FREQ_B_TOP_OFFSET + LAYOUT_THEMES[0].FREQ_B_BOTTOM_OFFSET),
			.FREQ_B_Y_TOP = 0,
			.FREQ_B_Y_BASELINE = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_Y_TOP + LAYOUT_THEMES[0].FREQ_B_HEIGHT + LAYOUT_THEMES[0].FREQ_B_TOP_OFFSET),
			.FREQ_B_Y_BASELINE_SMALL = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_Y_BASELINE - 2),
			.FREQ_B_FONT = &FreeSans32pt7b,
			.FREQ_B_SMALL_FONT = &FreeSans18pt7b,
			.FREQ_B_DELIMITER_Y_OFFSET = 0,
			.FREQ_B_DELIMITER_X1_OFFSET = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 142),
			.FREQ_B_DELIMITER_X2_OFFSET = (uint16_t)(LAYOUT_THEMES[0].FREQ_B_LEFT + 256),
			// display statuses under frequency
			.STATUS_Y_OFFSET = (uint16_t)(LAYOUT_THEMES[0].FREQ_Y_TOP + LAYOUT_THEMES[0].FREQ_BLOCK_HEIGHT + 1),
			.STATUS_HEIGHT = 30,
			.STATUS_BAR_X_OFFSET = 60,
			.STATUS_BAR_Y_OFFSET = 16,
			.STATUS_BAR_HEIGHT = 10,
			.STATUS_TXRX_X_OFFSET = 3,
			.STATUS_TXRX_Y_OFFSET = -50,
			.STATUS_TXRX_FONT = &FreeSans9pt7b,
			.STATUS_VFO_X_OFFSET = 0,
			.STATUS_VFO_Y_OFFSET = -43,
			.STATUS_VFO_BLOCK_WIDTH = 37,
			.STATUS_VFO_BLOCK_HEIGHT = 22,
			.STATUS_ANT_X_OFFSET = 0,
			.STATUS_ANT_Y_OFFSET = -23,
			.STATUS_ANT_BLOCK_WIDTH = 37,
			.STATUS_ANT_BLOCK_HEIGHT = 22,
			.STATUS_TX_LABELS_OFFSET_X = 5,
			.STATUS_TX_LABELS_MARGIN_X = 55,
			.STATUS_SMETER_WIDTH = 282,
			.STATUS_SMETER_MARKER_HEIGHT = 25,
			.STATUS_PMETER_WIDTH = 200,
			.STATUS_AMETER_WIDTH = 70,
			.STATUS_ALC_BAR_X_OFFSET = 10,
			.STATUS_LABELS_OFFSET_Y = 0,
			.STATUS_LABELS_FONT_SIZE = 1,
			.STATUS_LABEL_S_VAL_X_OFFSET = 10,
			.STATUS_LABEL_S_VAL_Y_OFFSET = 10,
			.STATUS_LABEL_S_VAL_FONT = &FreeSans7pt7b,
			.STATUS_LABEL_DBM_X_OFFSET = 5,
			.STATUS_LABEL_DBM_Y_OFFSET = 20,
			.STATUS_LABEL_BW_X_OFFSET = 457,
			.STATUS_LABEL_BW_Y_OFFSET = 20,
			.STATUS_LABEL_RIT_X_OFFSET = 457,
			.STATUS_LABEL_RIT_Y_OFFSET = 5,
			.STATUS_LABEL_THERM_X_OFFSET = 532,
			.STATUS_LABEL_THERM_Y_OFFSET = 20,
			.STATUS_LABEL_NOTCH_X_OFFSET = 650,
			.STATUS_LABEL_NOTCH_Y_OFFSET = 20,
			.STATUS_LABEL_FFT_BW_X_OFFSET = 740,
			.STATUS_LABEL_FFT_BW_Y_OFFSET = 20,
			.STATUS_LABEL_CPU_X_OFFSET = 532,
			.STATUS_LABEL_CPU_Y_OFFSET = 5,
			.STATUS_LABEL_AUTOGAIN_X_OFFSET = 650,
			.STATUS_LABEL_AUTOGAIN_Y_OFFSET = 5,
			.STATUS_LABEL_LOCK_X_OFFSET = 740,
			.STATUS_LABEL_LOCK_Y_OFFSET = 5,
			.STATUS_SMETER_PEAK_HOLDTIME = 1000,
			.STATUS_SMETER_TXLABELS_MARGIN = 55,
			.STATUS_SMETER_TXLABELS_PADDING = 23,
			.STATUS_TX_LABELS_VAL_WIDTH = 25,
			.STATUS_TX_LABELS_VAL_HEIGHT = 8,
			.STATUS_TX_ALC_X_OFFSET = 40,
			.STATUS_MODE_X_OFFSET = (uint16_t)(LCD_WIDTH - LAYOUT_THEMES[0].FREQ_RIGHT_MARGIN + 5),
			.STATUS_MODE_Y_OFFSET = -42,
			.STATUS_MODE_BLOCK_WIDTH = 48,
			.STATUS_MODE_BLOCK_HEIGHT = 22,
			.STATUS_MODE_B_X_OFFSET = (uint16_t)(LCD_WIDTH - (LCD_WIDTH - LAYOUT_THEMES[0].FREQ_B_LEFT_MARGIN - LAYOUT_THEMES[0].FREQ_B_WIDTH) + 4),
			.STATUS_MODE_B_Y_OFFSET = -30,
			.STATUS_ERR_OFFSET_X = (LCD_WIDTH - 50),
			.STATUS_ERR_OFFSET_Y = 25,
			.STATUS_ERR_WIDTH = 50,
			.STATUS_ERR_HEIGHT = 8,
			//text bar under wtf
			.TEXTBAR_FONT = 2,
			//bottom buttons
			.BOTTOM_BUTTONS_BLOCK_HEIGHT = 30,
			.BOTTOM_BUTTONS_BLOCK_TOP = (uint16_t)(LCD_HEIGHT - LAYOUT_THEMES[0].BOTTOM_BUTTONS_BLOCK_HEIGHT),
			.BOTTOM_BUTTONS_COUNT = 8,
			.BOTTOM_BUTTONS_ONE_WIDTH = (uint16_t)(LCD_WIDTH / LAYOUT_THEMES[0].BOTTOM_BUTTONS_COUNT),
			// FFT and waterfall
			.FFT_HEIGHT_STYLE1 = 100,
			.WTF_HEIGHT_STYLE1 = 200,
			.FFT_HEIGHT_STYLE2 = 150,
			.WTF_HEIGHT_STYLE2 = 150,
			.FFT_HEIGHT_STYLE3 = 170,
			.WTF_HEIGHT_STYLE3 = 130,
			.FFT_PRINT_SIZE = LCD_WIDTH,
			.FFT_CWDECODER_OFFSET = 17,
			.FFT_FFTWTF_POS_Y = (uint16_t)(LCD_HEIGHT - LAYOUT_THEMES[0].FFT_HEIGHT_STYLE1 - LAYOUT_THEMES[0].WTF_HEIGHT_STYLE1 - LAYOUT_THEMES[0].BOTTOM_BUTTONS_BLOCK_HEIGHT),
			.FFT_FFTWTF_BOTTOM = (uint16_t)(LAYOUT_THEMES[0].FFT_FFTWTF_POS_Y + LAYOUT_THEMES[0].FFT_HEIGHT_STYLE1 + LAYOUT_THEMES[0].WTF_HEIGHT_STYLE1),
			// system menu
			.SYSMENU_X1 = 5,
			.SYSMENU_X2 = 400,
			.SYSMENU_X2_BIGINT = 350,
			.SYSMENU_X2R_BIGINT = 400,
			.SYSMENU_W = 458,
			.SYSMENU_ITEM_HEIGHT = 18,
			.SYSMENU_MAX_ITEMS_ON_PAGE = (uint16_t)(LCD_HEIGHT / LAYOUT_THEMES[0].SYSMENU_ITEM_HEIGHT),
			//Tooltip
			.TOOLTIP_TIMEOUT = 1000,
			.TOOLTIP_MARGIN = 5,
			.TOOLTIP_POS_X = (LCD_WIDTH / 4),
			.TOOLTIP_POS_Y = 15,
			//BW Trapezoid
			.BW_TRAPEZ_POS_X = 352,
			.BW_TRAPEZ_POS_Y = 65,
			.BW_TRAPEZ_HEIGHT = 25,
			.BW_TRAPEZ_WIDTH = 96,
			//Touch buttons layout
			.BUTTON_PADDING = 1,
			.BUTTON_LIGHTER_WIDTH = 0.4f,
			.BUTTON_LIGHTER_HEIGHT = 4,
			//Windows
			.WINDOWS_BUTTON_WIDTH = 80,
			.WINDOWS_BUTTON_HEIGHT = 50,
			.WINDOWS_BUTTON_MARGIN = 10,
		},
};

#endif
