/********************************************************************
Purpose: Macros for the LCD
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/

#include "utils.h"

#ifndef _LCDMOD_H
#define _LCDMOD_H

//define masks for E,RS,RW  
#define LCD_RW_MASK  PORTA_BIT6_MASK
#define LCD_RS_MASK  PORTA_BIT5_MASK
#define LCD_E_MASK   PORTA_BIT4_MASK

//define masks for busy flag,data bus,port and ddr 
#define LCD_BUSY_FLAG PORTA_BIT3_MASK
#define LCD_BUS_MASK 0x0F
#define LCD_PORT PORTA
#define LCD_DDR DDRA

 //Macros to evaluate high/low nybble
#define LO_NYBBLE(value)   ((value) & 0x0F)               
#define HI_NYBBLE(value)   (LO_NYBBLE((value)>>4))

//Macros to set or clear E
#define LCD_E_LO CLR_BITS (LCD_PORT,LCD_E_MASK)
#define LCD_E_HI SET_BITS (LCD_PORT,LCD_E_MASK)

//Macros to select either Data reg or Instruction reg
#define LCD_IR  CLR_BITS(LCD_PORT,LCD_RS_MASK)
#define LCD_DR  SET_BITS(LCD_PORT,LCD_RS_MASK)

//Macros to read or write the register
#define LCD_RW_WRITE CLR_BITS(LCD_PORT,LCD_RW_MASK)
#define LCD_RW_READ  SET_BITS(LCD_PORT,LCD_RW_MASK)

//Macro to write to the lcd bus  
#define LCD_BUS(value) FORCE_BITS(LCD_PORT,LCD_BUS_MASK,(value))

//Macro to config the LCD DDR to write
#define INIT_LCD_DDR_WR   SET_BITS(LCD_DDR,0x7F)

#define INIT_LCD_DDR_BUSY_FLAG CLR_BITS(LCD_DDR,LCD_BUS_MASK)
#define RESTORE_LCD_DDR        SET_BITS(LCD_DDR,LCD_BUS_MASK)


 
// Macro for clearing LCD
#define LCD_CLEAR_MASK 0x01

// Macro to return cursor to home position
#define LCD_HOME_MASK 0x02


// Macro to move cursor to new line
#define LCD_NEW_LINE 0xC0

// Macro to initialize the LCD
#define LCD_CMD_FUNCTION 0x20
#define LCD_FUNCTION_4BIT 0x00
#define LCD_FUNCTION_2LINES 0x08
#define LCD_FUNCTION_5X8FONT 0x00
#define LCD_DISPLAY 0x08
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_NOCURSOR 0x00
#define LCD_NOBLINK 0x00
#define LCD_ENTRY 0x04
#define LCD_INC 0x02
#define LCD_MOVE_CURSOR 0x00


#define MAX_CHARACTERS 32


void initLCD(void);
void LCDprintf(char *format, ...);
void LCDputs(char *sx);
void LCDputc(char cx);
void LCDclear(void);
static void LCDdata(unsigned char value);
static void LCDcmd(unsigned char value);
static void readBusyFlag(void);

#endif
