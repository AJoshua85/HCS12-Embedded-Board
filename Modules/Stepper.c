#include <stdio.h>
#include <stdlib.h>
#include "Stepper.h"

unsigned char const stepPattern [] = { 0x80,0xA0,0x20,0x60,0x40,0x50,0x10,0x90};

static unsigned char stepType;
static unsigned char mode;
int MaxStep= 0;
int numStep = 0;
unsigned int position;
unsigned char index = 0;
volatile unsigned char switchStatus;
volatile unsigned char direction;

/********************initRTI()*******************************************
Purpose:Initialize the Real Time interrupt
Input:None
**************************************************************************/ 
void initRTI(void) 
{
 
  direction = STRIGHT;
  
  DDRA_SWR_INIT;
  SET_SWR_INPUT_EN;//Extra intialization because of PORTA
  DDRT_STEPPER_INIT;
  COPCTL |= COPCTL_RSBCK_MASK;  // freeze RTI during BDM active
  CRGFLG = CRGFLG_RTIF_MASK;    // clear any possibly pending RTI interrupts
}

/********************setMode()*******************************************
Purpose:Initialize the Real Time interrupt
Input:variable arg int:(3,RELAT,HALF,80),(3,RELAT,HALF,80)
**************************************************************************/ 
int setMode(int num,...)
{
   va_list modeList;
   va_start (modeList,num);
   
   if (num == 1)//set mode
   {
       mode = (unsigned char)va_arg(modeList,int);
       
   } 
   else if (num == 2)//set mode and position 
   {
      mode =(unsigned char) va_arg(modeList,int);
      position = va_arg(modeList,int);
      
   }
   else if (num == 3)//set mode, step type, position 
   {
      mode = (unsigned char) va_arg(modeList,int);
      stepType = (unsigned char)va_arg(modeList,int);
      position = va_arg(modeList,int);
      
   }
    
   else 
   {
     va_end(modeList);
     return ERROR;
   }
   
   va_end(modeList);
   CRGINT |= CRGINT_RTIE_MASK; 
   return 1;
 }
  
/********************seekSwitch()*******************************************
Purpose:Initialize the stepper motor so it know where the limits are
Input:None
**************************************************************************/ 
void seekSwitch(void) 
{
    
    if (direction == STLEFT )
    {
      switchStatus =  CHECK_LEFT_SWITCH_ON;
      MaxStep++;
      //Case for the left limit is triggered
      if (switchStatus ==  L_SWR_ON_MASK) 
      { 
        direction= STRIGHT;
        mode = HOME;
        //CLR_BITS(CRGINT,CRGINT_RTIE_MASK);//Disable Interrupt
      } 
      else if (switchStatus != L_SWR_ON_MASK) 
      {
        index--;
        index&=STEPPER_MOD_7;
        
      }
         
    } 
    else if (direction == STRIGHT) 
    {
       switchStatus =  CHECK_RIGHT_SWITCH_ON;
       
       //Case for the right limit is triggered
       if(switchStatus == R_SWR_ON_MASK) 
       {
          direction = STLEFT;
          index--;
          index&=STEPPER_MOD_7;
          MaxStep++;
          
       } 
       else if (switchStatus != R_SWR_ON_MASK) 
       {
          index++;
          index&=STEPPER_MOD_7; 
       }
        
     }
    
    STEPPER_BUS(stepPattern[index]);
}
/********************home()**********************************************
Purpose:Move the stepper motor to middle point
Input:None
**************************************************************************/ 
static void home(void) 
{
   
   if(numStep == MaxStep/2) 
   {
     CLR_BITS(CRGINT,CRGINT_RTIE_MASK);//Disable Interrupt
     mode = READY;
   }
   else
   {
      numStep++;
      index++;
      index&=STEPPER_MOD_7;
      STEPPER_BUS(stepPattern[index]);
   }
   
}
/********************calculatePosition()***********************************
Purpose:Calculate the stepper motor position given an angle input
Input:None
**************************************************************************/ 
int calculatePosition(int angle) 
{
  static int calcPosition;
  calcPosition = (int)( ( (21444L)*(angle) + 10000L )/10000L);
  return calcPosition;
}
/********************continuous()*****************************************
Purpose:Move the stepper motor to fixed an angle
Input:None
**************************************************************************/ 
static void continuous (void) 
{
   static unsigned char calcFlag = FALSE;
   
   if (calcFlag != TRUE) 
   {
      MaxStep = numStep - (calculatePosition (position));
      calcFlag = TRUE;
      if ( MaxStep > 0)  
      {
        direction = STRIGHT;
        MaxStep = MaxStep/stepType;
      } 
      else if (MaxStep < 0) 
      {
         direction = STLEFT;
         MaxStep = abs(MaxStep)/stepType;
      }
   }
   
   
   if (direction == STRIGHT && 0!= MaxStep) 
   {
    
     index+=stepType;
     MaxStep--;
     numStep-=stepType;
     index &=STEPPER_MOD_7; 
   } 
   else if (direction == STLEFT && 0!= MaxStep) 
   {
      index -=stepType;
      MaxStep--;
      numStep+=stepType;
      index&=STEPPER_MOD_7; 
   }
   else 
   {
    calcFlag = FALSE;// Ready for the next calculations 
    CLR_BITS(CRGINT,CRGINT_RTIE_MASK);//Disable Interrupt
   }
   STEPPER_BUS(stepPattern[index]);
}
/********************relative()*****************************************
Purpose:Move the stepper motor to a relative angle given the current position
Input:None
**************************************************************************/ 
static void relative(void) 
{ 
   static unsigned char calcFlag = FALSE;
   
   if (calcFlag != TRUE) 
   {
      MaxStep =  (calculatePosition (position));
      calcFlag = TRUE;
      if ( MaxStep > 0)  
      {
        direction = STLEFT;
        MaxStep = MaxStep/stepType;
      } 
      else if (MaxStep < 0) 
      {
         direction = STRIGHT;
         MaxStep = abs(MaxStep)/stepType;
      }
   }
   
   if (direction == STRIGHT && 0!= MaxStep) 
   {
     
     index+=stepType;
     MaxStep--;
     numStep-=stepType;
     index &=STEPPER_MOD_7; 
      
   } 
   else if (direction == STLEFT && 0!= MaxStep) 
   {
      
      index -=stepType;
      MaxStep--;
      numStep+=stepType;
      index&=STEPPER_MOD_7;  
   }
   else 
   {
    calcFlag = FALSE;// Ready for the next calculations 
    CLR_BITS(CRGINT,CRGINT_RTIE_MASK);//Disable Interrupt
   }
   STEPPER_BUS(stepPattern[index]);
}

/********************initStepper()*****************************************
Purpose:Initialize Stepper motor
Input:None
**************************************************************************/  
void initStepper(void) 
{
  while(mode == SEEK || mode == HOME);
}

/********************RTIhandler()*****************************************
Purpose:moves the stepper motor depending on the mode 
Input:None
**************************************************************************/
interrupt VectorNumber_Vrti void RTIhandler( void )
{
   
   CRGFLG = CRGFLG_RTIF_MASK;
   switch(mode) 
   {
    case SEEK:
    seekSwitch();
    break;
    case HOME:
    home();
    break;
    case STPFIXED:
    continuous();
    break;
    case RELAT:
    relative();
    break;
   }
}
/********************Status()*****************************************
Purpose:returns the mode of stepper motor 
Input:None
**************************************************************************/
unsigned int Status(void)
{
  return mode;
}
/********************speed()*****************************************
Purpose:set the speed of stepper motor 
Input:None
**************************************************************************/
void speed(unsigned char speed) 
{
  RTICTL = speed;// set RTI period
}
  
  

  


  
  


