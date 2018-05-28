#include "ADC.h"
#include "TimerModule.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

int val1;
int val2;
int val3;

unsigned char adcFlag;

char ADC[36];
int intADC[9];
static unsigned char j = 0;







/********************initADC()***********************************************
Purpose:initialize ADC Registers (no interrupts,1 channel,one conversion)
Input: None
*****************************************************************************/ 
void initADC(void) 
{
  //ATDCTL2 Config
  SET_BITS(ATDCTL2,ADC_POWER|ADC_FAST_CLR|HALT_WAI);// Turn on ADC,stop conversation on wait mode and Fast clear flag
  asm("nop"); 
 
  //ATDCTL3 Config
  ADC_NUM_CONV(0x01);//One conversion
  ADC_CONV_SETTINGS(0x02);//finish current conversion and halt
  FIFO_OFF;
   
  //ATCTL4 Config
  SET_PRESCALER(0x01);// prescaler of 2 (20Mhz-5Mhz)
  SAMPLE_SEL(0x02);// 4 A/D conversation clock period and 10bit mode
}

/********************initADCV2()************************************************
Purpose:initialize ADC Registers (uses interrupts,2 channel,continous conversion)
Input: unsigned char - channel 0-7U
********************************************************************************/ 
void initADCV2(unsigned char inputChannel) 
{
  ADC[0]='\0';
  //ATDCTL2
  SET_BITS(ATDCTL2,ADC_POWER|HALT_WAI|ADC_INTERRUPT); // Turn on ADC,stop conversation on wait mode,No Fast clear flag and conversation completed interrupt on
  asm ("nop");
  
  //ATDCTL3 Config
 
  ADC_NUM_CONV(0x03);//Three conversion
  ADC_CONV_SETTINGS(0x02);//finish current conversion and halt
  FIFO_OFF;
  
  //ATCTL4 Config
  SET_PRESCALER(0x01);// prescaler of 2 (20Mhz-5Mhz)
  SAMPLE_SEL(0x03);// 16 A/D conversation clock period
  CLR_BITS(ATDCTL4,ATDCTL4_SRES8_MASK);//10bit mode
  
  //ATCTL5 Config
  ATDCTL5 = ATDCTL5INIT3;//Right justified data unsigned,continous converstion, multi channel
  
  MAKE_CHNL_OC(6);//Create channel 6 for time stamps for ADC
  TIMER_CHNL(6)= TCNT + 50000U;
  ENABLE_CHNL_INTERRUPT(6);
  SELECT_CHNL(inputChannel);

}

/********************readChannel()************************************************
Purpose:Set which Channel to begin reading (Use only for with intterupts configured)
Input:unsigned char - channel:0-7U
********************************************************************************/ 
int readChannel(unsigned char channel) 
{
   static unsigned int result; 
    
   ATDCTL5 = ATDCTL5INIT;//Result right justified,unsigned result,single conversion sequence, sample one channel.
   SELECT_CHNL(channel);//Read channel 4
    
   DisableInterrupts;
   while (!(ATDSTAT0 & SCF)); //Atomic Instruction
   EnableInterrupts;
   return (result = ATDDR0);
}

/********************ATDhandler()************************************************
Purpose:Read ATD value once ready to read
Input:None
********************************************************************************/ 
interrupt VectorNumber_Vatd0 void ATDhandler (void) 
{
  
  val1= ATDDR0;
  val2 = ATDDR1;
  val3 = ATDDR2;
  
  if (adcFlag == TRUE) 
  {
    intADC[j++] = val1;
    intADC[j++] = val2;
    intADC[j++] = val3;
    adcFlag = FALSE;
  }
  
  if (j== 9) 
  {
     CLR_BITS(ATDCTL2,ADC_POWER|ADC_INTERRUPT);//Turn off ADC
     DISABLE_CHNL_INTERRUPT(6);
  }
  
  CLR_SCF;   
}

interrupt VectorNumber_Vtimch6 void TimerCh6Handler (void) 
{
  
  static unsigned char k = 0;
  k++;
  if(k == 20)/*1 min has gone take ADC value*/ 
  {
   k = 0;
   adcFlag = TRUE;
   
  }
  TIMER_CHNL(6)= TCNT + 50000U;
  
}

void processADC(void) 
{
  
  unsigned char k =0;
  char temp[4];
  int i = 0;
  temp[0] = '\0';
 
  for (i;i< 9; i++)
	{
	  sprintf(temp,"%d",intADC[i]);
		strcat(ADC,temp);
		if (i == 8)
		strcat(ADC,"D");
		else
		strcat(ADC,"@");
	}
	
	putsSCI(ADC);

}
  
  




  
  


  
  
  
  