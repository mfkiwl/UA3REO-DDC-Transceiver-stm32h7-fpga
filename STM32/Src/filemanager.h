#ifndef FILEMANAGER_h
#define FILEMANAGER_h

#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include "system_menu.h"

#define FILEMANAGER_LISTING_MAX_FILES 26
#define FILEMANAGER_LISTING_MAX_FILELEN 40
#define FILEMANAGER_LISTING_ITEMS_ON_PAGE (LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE - 4)

extern char FILEMANAGER_CurrentPath[128];
extern char FILEMANAGER_LISTING[FILEMANAGER_LISTING_MAX_FILES][FILEMANAGER_LISTING_MAX_FILELEN + 1];
extern uint16_t FILEMANAGER_files_startindex;
extern uint16_t FILEMANAGER_files_count;

extern void FILEMANAGER_Draw(bool redraw);
extern void FILEMANAGER_EventRotate(int8_t direction);
extern void FILEMANAGER_Closing(void);
extern void FILEMANAGER_EventSecondaryRotate(int8_t direction);

#endif
