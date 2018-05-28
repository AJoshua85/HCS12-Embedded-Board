/********************************************************************
Purpose: Macros for the Stepper
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/


#include "utils.h"
#ifndef _STEPPER_H
#define _STEPPER_H

//Switch Initialization
#define DDRA_SWR_INIT CLR_BITS(DDRAD,DDRAD_DDRAD7_MASK|DDRAD_DDRAD6_MASK)
#define SET_SWR_INPUT_EN SET_BITS(ATDDIEN,ATDDIEN_IEN7_MASK |ATDDIEN_IEN6_MASK)

//Switch states
#define L_SWR_ON_MASK PORTA_BIT6_MASK
#define R_SWR_ON_MASK PORTA_BIT7_MASK
#define CHECK_LEFT_SWITCH_ON (PTAD & L_SWR_ON_MASK)
#define CHECK_RIGHT_SWITCH_ON (PTAD & R_SWR_ON_MASK)


//Stepper Modes
#define SEEK     0U
#define HOME     1U
#define RELAT    2U
#define STPFIXED 3U
#define READY    4U


//Stepper Direction
#define STLEFT  0U
#define STRIGHT 1U

//Stepper Step Type
#define HALF 1U
#define FULL 2U


//Stepper Initialization and Write
#define STEPPER_MASK  0xF0
#define STEPPER_MOD_7 0x07
#define DDRT_STEPPER_INIT SET_BITS(DDRT,STEPPER_MASK)
#define STEPPER_BUS(data)  FORCE_BITS(PTT,STEPPER_MASK,data)



void seekSwitch(void);
void initRTI(void);
static void home(void);
static void continuous (void);
static void relative(void);
int calculatePosition(int angle);
void setMode(int num,...);
void initStepper(void);
unsigned int Status(void);
void speed(unsigned char speed); 
 
#endif


  




 
 
