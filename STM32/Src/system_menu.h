#ifndef SYSTEM_MENU_H
#define SYSTEM_MENU_H

#include "stm32h7xx.h"
#include <stdbool.h>
#include <stdint.h>

#define ENUM_MAX_COUNT 8
#define ENUM_MAX_LENGTH 10

typedef enum
{
	SYSMENU_BOOLEAN,
	SYSMENU_RUN,
	SYSMENU_UINT8,
	SYSMENU_UINT16,
	SYSMENU_UINT32,
	SYSMENU_UINT32R,
	SYSMENU_INT8,
	SYSMENU_INT16,
	SYSMENU_INT32,
	SYSMENU_FLOAT32,
	SYSMENU_MENU,
	SYSMENU_HIDDEN_MENU,
	SYSMENU_INFOLINE,
	SYSMENU_FUNCBUTTON,
	SYSMENU_ENUM,
	SYSMENU_ENUMR,
} SystemMenuType;

struct sysmenu_item_handler
{
	char *title;
	SystemMenuType type;
	uint32_t *value;
	void (*menuHandler)(int8_t direction);
	char enumerate[ENUM_MAX_COUNT][ENUM_MAX_LENGTH];
};

extern void SYSMENU_drawSystemMenu(bool draw_background);
extern void SYSMENU_redrawCurrentItem(void);
extern void SYSMENU_eventRotateSystemMenu(int8_t direction);
extern void SYSMENU_eventSecEncoderClickSystemMenu(void);
extern void SYSMENU_eventSecRotateSystemMenu(int8_t direction);
extern void SYSMENU_eventCloseSystemMenu(void);
extern void SYSMENU_eventCloseAllSystemMenu(void);
extern bool SYSMENU_spectrum_opened;
extern bool SYSMENU_hiddenmenu_enabled;
extern void SYSMENU_TRX_RFPOWER_HOTKEY(void);
extern void SYSMENU_TRX_STEP_HOTKEY(void);
extern void SYSMENU_CW_WPM_HOTKEY(void);
extern void SYSMENU_CW_KEYER_HOTKEY(void);
extern void SYSMENU_AUDIO_BW_SSB_HOTKEY(void);
extern void SYSMENU_AUDIO_BW_CW_HOTKEY(void);
extern void SYSMENU_AUDIO_BW_AM_HOTKEY(void);
extern void SYSMENU_AUDIO_BW_FM_HOTKEY(void);
extern void SYSMENU_AUDIO_HPF_SSB_HOTKEY(void);
extern void SYSMENU_AUDIO_HPF_CW_HOTKEY(void);
extern void SYSMENU_AUDIO_SQUELCH_HOTKEY(void);
extern void SYSMENU_AUDIO_AGC_HOTKEY(void);
extern void SYSMENU_HANDL_SERVICESMENU(int8_t direction);

#endif
