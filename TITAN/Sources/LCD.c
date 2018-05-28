#include "LCD.h"
#include <stdio.h>
#include "TimerModule.h"


/****************LCDinit()***************
Purpose: Initalize the LCD
*****************************************/
void initLCD(void) 
{
   
   INIT_LCD_DDR_WR;//Set up the DDR for LCD to write to the port
   
   LCD_E_HI;      // wake up display & sync
   LCD_BUS(0x03);
   LCD_E_LO;
   //readBusyFlag();
   msDELAY(5);
   
   
   LCD_E_HI;      // wake up display & sync
   LCD_BUS(0x03);
   LCD_E_LO;
   msDELAY(1);
   //readBusyFlag();
   
   
   LCD_E_HI;      // wake up display & sync
   LCD_BUS(0x03);
   LCD_E_LO;
   
   
   LCD_E_HI;       // wake up display & sync - go to 4bit mode
   LCD_BUS(0x02);
   LCD_E_LO;
   msDELAY(2);
   //readBusyFlag();
   
   
   LCDcmd(LCD_CMD_FUNCTION|LCD_FUNCTION_4BIT|LCD_FUNCTION_2LINES|LCD_FUNCTION_5X8FONT);
   LCDcmd(LCD_DISPLAY|LCD_DISPLAY_OFF);
   LCDcmd(LCD_CLEAR_MASK);
   LCDcmd(LCD_ENTRY|LCD_MOVE_CURSOR|LCD_INC);
   LCDcmd(LCD_DISPLAY|LCD_DISPLAY_ON|LCD_NOCURSOR|LCD_NOBLINK);
}

/*************LCDprintf()*********************
Purpose:Emulates printf() for the LCD screen
Input: Arguments are the same as printf()
********************************************/

void LCDprintf(char *format, ...) 
{
  va_list myArgs;
  char Buffer[MAX_CHARACTERS];
  va_start(myArgs, format);
  (void)vsprintf(Buffer,format,myArgs);
  va_end(myArgs);
  LCDputs(Buffer);  
}
/************LCDdata()**********************
Purpose: Write data to the data register
Input: Data to display in the display
*******************************************/
static void LCDdata(unsigned char value) 
{
  
  LCD_E_LO;
  LCD_DR;
  LCD_RW_WRITE;
  LCD_E_HI;
  LCD_BUS(HI_NYBBLE(value));
  LCD_E_LO;
  LCD_E_HI;
  LCD_BUS(LO_NYBBLE(value));
  LCD_E_LO;
  //msDELAY(2);
  readBusyFlag();
  
}
/********************LCDcmd()************************
Purpose:Write data to the instruction register
Input: char value -Commands to configure the lcd to specific mode
****************************************************/
static void LCDcmd(unsigned char value) 
{
  
  LCD_E_LO;
  LCD_IR;
  LCD_RW_WRITE;
  LCD_E_HI;
  LCD_BUS(HI_NYBBLE(value));
  LCD_E_LO;
  LCD_E_HI;
  LCD_BUS(LO_NYBBLE(value));
  LCD_E_LO;
  //msDELAY(2);
  readBusyFlag();
  
}

/****************LCDputc()*********************
Purpose:Writes one character to lcd display
Input: char cx - character to display
***********************************************/ 
void LCDputc(char cx) 
{
  switch(cx) 
  {
    case 13:
      LCDcmd(LCD_HOME_MASK);
      break;
    case 10:
      LCDcmd(LCD_NEW_LINE);
      break;
    default:
      LCDdata(cx);
  }
}
/****************LCDputs()*********************
Purpose:Writes a string to lcd display
Input: char sx - string to display to the lcd
***********************************************/ 
void LCDputs(char *sx) 
{
  int count = 0;
  
  while(*sx) 
  {
    if(count == 16)
    LCDcmd(LCD_NEW_LINE);
    if(count == 32)
    break;
    LCDputc(*sx);
    sx++;
    count++;
  }
}
/*************LCDclear()**************************
Purpose: Clear the LCD screen
**************************************************/
void LCDclear(void) 
{
  LCDcmd(LCD_CLEAR_MASK);
  //msDELAY(16);
  readBusyFlag();
 }

/*********readBusyFLag()**************************
Purpose:Reads the status of busy flag of the LCD
**************************************************/

static void readBusyFlag(void) 
{
   volatile unsigned char checkBusy;
   INIT_LCD_DDR_BUSY_FLAG;

   LCD_E_LO;
   LCD_IR;
   LCD_RW_READ;
   
   do 
   {
    
    LCD_E_HI;
    checkBusy = PORTA;
    LCD_E_LO;
    
    LCD_E_HI;
    (void)PORTA;
    LCD_E_LO;
    
   }while(checkBusy & LCD_BUSY_FLAG);
  RESTORE_LCD_DDR;   
}
  
  


   
  
  
    
    
  








  
  


  



   
   
   
  
  