#include "SCI.h"
#include <mc9s12c32.h>
#include <string.h>
#include "utils.h"



uchar rxBuffer[rxBufSize];
uchar mainBuf[17];
volatile unsigned char rxIndex = 0;
volatile uchar msgCount = 0;
uchar StartFlag;
uchar START = 1;
uchar STOP = 0;
void transferBuf(uchar buf1, uchar buf2);

/*************InitSCI()*******************************************************
Purpose: Initalize the SCI module
Input:   int baudRate - a divisor to get the desired baud rate
         int SCIData  - configures word length, parity
         int config    - configures interrupts and enables Receiver and Trasmitter
******************************************************************************/
void InitSCI(void) 
{
  SCIBD = 52;
  SCICR1 = 0;
  SCICR2 = (SCICR2_RE_MASK|SCICR2_TE_MASK|SCICR2_RIE_MASK);
 
}
/************putcSCI()*********************************************************
Purpose: Put a character to the Serial Port
Input:   char cx - character to be printed
******************************************************************************/
void putcSCI(char cx) 
{
  //Wait until transmit data register empty
  while(!(SCISR1&SCISR1_TDRE_MASK));
  SCIDRL = cx;
}
/************putsSCI()*********************************************************
Purpose: Put a string to the Serial Port
Input:   char *str - pointer to the printed string
******************************************************************************/
void putsSCI(char *str) 
{
  while(*str)
    putcSCI(*str++);
}

/************SCIHandler()*****************************************************
Purpose: Grab a character from the serial port on every interupt
******************************************************************************/


interrupt VectorNumber_Vsci void SCIHandler( void )
{
  // Wait until data is available on port
  if(SCISR1 & SCISR1_RDRF_MASK)
  {
    getcSCI(SCIDRL);
    // Clear indicator stating that there is data on the port
    CLR_BITS(SCISR1, SCISR1_RDRF_MASK);
  }
}

/************getcSCI()*********************************************************
Purpose: gets a character from the Serial Port
Input:   char cx - character read from the port
******************************************************************************/
void getcSCI(char cx) 
{
  if(cx == '>') 
  {
    msgCount++;
    StartFlag= STOP;
  } else if (cx == '<') 
  {
    StartFlag = START;
  } else if (StartFlag == START) 
  {
    rxBuffer[rxIndex]= cx;
    rxIndex++;
    rxIndex %= rxBufSize;
  }
}

/*

transferBuf(rxBuffer, mainBuf);


void transferBuf(uchar buf1, uchar buf2){
   
   if(msgCount = TRUE){
    
   
       strncpy(&buf1, buf2, 16);
       msgCount = 0
   }
}


*/


void get_cmdbuf(uchar *buf0,uchar buf1[],uchar buf2[], uchar buf3[], uchar buf4[])
{
 
 DisableInterrupts;
  *(buf0) = *(rxBuffer);
 copyString(&buf1[0],(&rxBuffer[1]),3);
 copyString(&buf2[0],(&rxBuffer[4]),3);
 copyString(&buf3[0],(&rxBuffer[7]),3);
 copyString(&buf4[0],(&rxBuffer[10]),3);
 (void)memset(rxBuffer,0,rxBufSize);
 rxIndex = 0;
 EnableInterrupts;
   
}
  

/************ copyString()*********************************************************
Purpose: gets a array into another array
Input:   char *dest - array to be copied to 
         char *src array to copy from,
         int num- number of characters to copy not including null 
NOTE:Do not pass in null character, as it automatically adds it.
******************************************************************************/  
void copyString(uchar *dest,uchar *src,unsigned char num) 
{
   int i=0;
   for(i;i<num;i++) 
   {
    dest[i]=src[i];
   }
   dest[i]='\0';
}
    

  
  
