#include "utils.h"

/********************************************************************
Purpose: Macros for the LCD
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/

#ifndef _SERVOMOD_H
#define _SERVOMOD_H


#define _20MS 20000U
#define SVRIGHT 0U
#define SVLEFT 1U
#define IDLE 0U
#define DELAY 1U
#define FIXED  2U


void servoInit(void);
static unsigned int calculatePulse(void);
static int sweepServo(void);
static void fixedServo(void);
void setModeServo(uchar tMode, uchar tDegree);

#endif
