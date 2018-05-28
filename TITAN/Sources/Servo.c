#include <stdio.h>
#include <stdlib.h>
#include "Servo.h"
#include "TimerModule.h"


static unsigned char mode;
static unsigned long msDelay;
static unsigned char degree;
static unsigned int currentPosition;

static unsigned char PrevState = LOWSTATE;

/********************servoInit()*******************************************
Purpose:Initialize the servo
Input:None
**************************************************************************/ 
void servoInit(void) 
{
  currentPosition = 750U;
  TSCR1=TSCR1_INIT;// enable TCNT, fast timer flag clear, freeze during debbugging
  SET_TCNT_PRESCALE(TCNT_PRESCALE_8);// set prescale to 8
  MAKE_CHNL_OC(2);//Config channel 1
  SET_OC_ACTION(2,OC_GO_HI);//Set the state of pin to change when the intterupt goes off
  TIMER_CHNL(2) = TCNT + (_20MS - currentPosition);//Set the intterrupt to go off in the future
  ENABLE_CHNL_INTERRUPT(2);//enable TC2 channel interrupts
  mode = IDLE;
}

/********************TimerCh2Handler()*************************************
Purpose:Moves the servo depending on the mode selected 
Input:None
**************************************************************************/                                 
interrupt VectorNumber_Vtimch2 void TimerCh2Handler(void) 
{
  
  
  switch(mode) 
  {
    case DELAY:
    sweepServo();
    break;
    case FIXED:
    fixedServo();
    break;
    case IDLE:
    break;
  }
  
  switch(PrevState) 
  {
    case HIGHSTATE:
    TIMER_CHNL(2) += _20MS - currentPosition;//Duration of the low state
    SET_OC_ACTION(2,OC_GO_HI);
    PrevState = LOWSTATE;
    break;
    
    
    case LOWSTATE:
    TIMER_CHNL(2) += currentPosition;//Duration of the high state
    SET_OC_ACTION(2, OC_GO_LOW);
    PrevState = HIGHSTATE;
    break;
  }
}

/********************calculatePulse()*************************************
Purpose:Calculates the pulse width for given degree
Input:None
**************************************************************************/ 
static unsigned int calculatePulse(void)
{
  static unsigned int result;
  result =(unsigned int) ((93086L*(degree)+5750000L)/10000U);
  return result;
}


/********************sweepServo()*************************************
Purpose:Moves the servo to specific a location with a delay
Input:None
**************************************************************************/
static int sweepServo(void) 
{
  static unsigned char calcFlag = FALSE;
  static unsigned char direction;
  static unsigned long scaler;//ratio
  static unsigned int newPosition; 
  static unsigned int distance;//This removes statment
  static unsigned char state;

  if (calcFlag == FALSE)
  {
      
    newPosition = calculatePulse();
    if( newPosition > currentPosition) 
    {
      direction = SVRIGHT;
    } 
    else if (newPosition < currentPosition) 
    {
     direction = SVLEFT;
    } 
    else 
    {
      return ERROR; 
    }
       
    scaler = (10000L*20L)/msDelay;
    distance = abs(newPosition - currentPosition);
    distance = (unsigned int)(((unsigned long)(scaler*(distance)))/10000U);
    calcFlag = TRUE;
    state = PrevState;
  }
  
  
  if (PrevState == state)//This makes sure the calculation happens once a full cylce has completed 
  {
    
  
    if (direction == SVRIGHT) 
    {
      if((currentPosition + distance) <= newPosition)//Need to travel more 
      {                                                                                                                                                       
       currentPosition+= distance;
      } 
      else if (currentPosition == newPosition)//We are at our desired point Hooray!! 
      {
        mode = IDLE;
        calcFlag = FALSE;
      }
      
      else if( (currentPosition + distance) > newPosition)////The offset will cause overshoot our desired position, so take the differance  
      {
        currentPosition += abs(currentPosition-newPosition);
      } 
    
      
    } 
    else if (direction == SVLEFT) 
    {
      if((currentPosition - distance)>= newPosition)//Need to travel more 
      {
        currentPosition-= distance;
      } 
      else if(currentPosition == newPosition)//We are at our desired point Hooray!! 
      {
        mode = IDLE;
        calcFlag = FALSE;
      }
      
      else if((currentPosition - distance)<newPosition)//The offset will cause overshoot our desired position, so take the differance  
      {
        currentPosition -= abs(currentPosition-newPosition);
      } 
   
    }
  }
}
/********************fixedServo()*************************************
Purpose:Moves the servo to specific a location no delay
Input:None
**************************************************************************/
static void fixedServo(void) 
{
  currentPosition = calculatePulse();
  mode = IDLE;
}
/********************setModeServoV()*************************************
Purpose:Set the mode which the servo responds too
Input:unsigned char- mode:1-2U,tDegree:0-202U,tDelay:20MS>
**************************************************************************/
void setModeServo(uchar tMode, uchar tDegree) 
{
 
  if(tMode == FIXED) 
  {
    mode = tMode;
    degree =tDegree;
  } 
  else 
  {
    mode = tMode;
    degree =tDegree;
    msDelay = 50000;  //microseconds to delay the stepper
  
  }
}

    
    
  









































  



 
  
  