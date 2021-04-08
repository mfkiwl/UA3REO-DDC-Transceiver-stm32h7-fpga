#ifndef SPEC_ANALYZER_H
#define SPEC_ANALYZER_H

#include "stm32h7xx.h"
#include "main.h"
#include "stdbool.h"

#define SPEC_Resolution 1000 //resolution, 1khz
#define SPEC_StepDelay 1     //scan delay, msec
#define SPEC_VParts 6        //vertical signatures

//Public variabled
extern bool SYSMENU_spectrum_opened;

//Public methods
extern void SPEC_Start(void);                 //analyzer launch
extern void SPEC_Stop(void);                  //stop session
extern void SPEC_Draw(void);                  //drawing analyzer
extern void SPEC_EncRotate(int8_t direction); //analyzer events per encoder

#endif
