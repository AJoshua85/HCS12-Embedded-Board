#include "DCMotor.h"
#include "TimerModule.h"
#include <mc9s12c32.h>
#include <stdarg.h>

const unsigned char DArray[] = {FWD1,BKD1,STOPH1,FWD2,BKD2,STOPH2,(FWD1|FWD2),(BKD1|BKD2),(STOPH1|STOPH2), (FWD1|BKD2), (BKD1|FWD2)};
unsigned char EncorderEvent0;
unsigned char EncorderEvent1;
unsigned char overflow0 =0;
unsigned char overflow1 =0;
unsigned long period0 = 0;
unsigned long period1 =0;

static unsigned int setPoint0;
static unsigned int setPoint1;
static signed int feedBack0;
static signed int feedBack1;
static signed long Error0;
static signed long Error1;
unsigned char PIDSpeed0;
unsigned char PIDSpeed1;
volatile signed long driveValue0;
volatile signed long driveValue1;
static signed int ErrorIntergral0;
static signed int ErrorIntergral1;
static unsigned char P_GAIN =1;
static unsigned char I_GAIN =1;
static unsigned char GAIN_DIVISOR =100;

static unsigned int travelTime= 0;
static unsigned char travelMode;

unsigned char speedMotor1;
unsigned char speedMotor2;


volatile unsigned int Msec;




 

/********************initDCMotor()*******************************************
Purpose:Initialize the DC Motors
Input:None
**************************************************************************/ 
void initDCMotor() 
{
  
  //DC motor Configs
  INIT_MOT_DDR;
    
  PWMCTL = MODE_8BIT;
  PWMPRCLK = NO_PSCALE;
  
  CH5_HIGH_POL;
  CH4_HIGH_POL;
  CH4_CENTRE_ALIGN;
  CH5_CENTRE_ALIGN;
  
  //Skip SA 
  CH5_COUNT_CLK_DIRECTLY;
  CH4_COUNT_CLK_DIRECTLY;
  
  //Period
  PWMPER4 = _22VKhz;
  PWMPER5 = _22VKhz;
  
  //Duty cycle
  PWMDTY4 =153U;
  PWMDTY5 =153U;
}
/********************enableChannel()*******************************************
Purpose:Enables the PWM Channel
Input: char value - PWM bit mask value
**************************************************************************/ 
void enableChannel(unsigned char channel) 
{
   SET_BITS(PWME,channel);
}
/********************disableChannel()*******************************************
Purpose:Disables the PWM Channel
Input: char value - PWM bit mask value
**************************************************************************/ 

void disableChannel(unsigned char channel) 
{
  CLR_BITS(PWME,channel);
}

/********************setMotor()*******************************************
Purpose:Set the motor speed and direction
Input: char value - motor speed ie:0-100%,direction:bit pattern,motor:0-3U
**************************************************************************/ 
//Note: Motor doesnt turn until speed value of 55 
void setMotor(unsigned char DCspeed, unsigned char DCdirection, unsigned char motor) 
{
 unsigned char maskedDirection = DArray[DCdirection];
 //Calculate speed in terms of pwm duty cycle
 unsigned char pwm = (unsigned char)((231U*(DCspeed)+2369U+50)/100U); //+50 to round up the answer
 
 switch(motor)
  {
    case MOT1:
    MOT1_BUS(maskedDirection);
    PWMDTY4 = pwm;
    break;
  
    case MOT2:
    MOT2_BUS(maskedDirection);
    PWMDTY5 = pwm;
    break;
    
    case MOT12:
    MOT12_BUS(maskedDirection);
    /*
    PWMDTY4 = pwm;
    PWMDTY5 = pwm;*/
    PIDSpeed0 = 20;
    PIDSpeed1 = 20;
    break;
    
    default:
    break;
  }
}


void setMotorAlternate(unsigned directionM1,unsigned directionM2) 
{
  MOT1_BUS(directionM1);
  MOT2_BUS(directionM2);
}


/********************setSpeed()************************
Purpose:Set the motor speed of each motor
Input: char value - motor speed ie:0-100%
****************************************************/ 

void setSpeed(unsigned char speed,unsigned char motor) 
{
  unsigned char pwm = (unsigned char)((231U*(speed)+2369U+50)/100U); //+50 to round up the answer
 
  switch(motor)
  {
    case MOT1:
    PWMDTY4 = pwm;
    break;
  
    case MOT2:
    PWMDTY5 = pwm;
    break;
    
    case MOT12:
    PWMDTY4 = pwm;
    PWMDTY5 = pwm;
    break;
  }
  
}


void initEncorder(void) 
{
  EncorderEvent0 = INIT_RISING;
  EncorderEvent1 = INIT_RISING;
  TSCR2|= TSCR2_TOI_MASK;/*Enable TCNT overflow interrupt*/
  MAKE_CHNL_IC(0);
  MAKE_CHNL_IC(1);
  MAKE_CHNL_OC(3);
  MAKE_CHNL_OC(4);
  MAKE_CHNL_OC(5);
  
  SET_IC_EDGE(0,IC_EDGE_RISING);
  SET_IC_EDGE(1,IC_EDGE_RISING);
  (void)TC0;/*Clear pending interrupts for channel 0*/
  (void)TC1;/*Clear pending interrupts for channel 1*/
  SET_BITS(TIE,0x03);/*Enable both timer channels 0 and 1*/
  SET_BITS(TIE,0x18);/*Enable both watchdog timers 2 and 3*/
  
 
  TIMER_CHNL(3) = TCNT + WATCHDOG;
  TIMER_CHNL(4) = TCNT + WATCHDOG;
  
 }


interrupt VectorNumber_Vtimch0 void TimerCh0Handler (void) 
{
   static volatile unsigned int edge0;
   static volatile unsigned int edge1;
   switch(EncorderEvent0) 
  {
    case INIT_RISING:
    EncorderEvent0 = RISING;
    edge0 = TC0;
    break;
    
    case RISING:
    edge1 = TC0;
    period0 = (overflow0*65536UL+edge1)-edge0;
    edge0= edge1;
    overflow0 =0;
    break;
  }
  
}

interrupt VectorNumber_Vtimch1 void TimerCh1Handler (void) 
{
   static volatile unsigned int edge0;
   static volatile unsigned int edge1;
   switch(EncorderEvent1) 
  {
    case INIT_RISING:
    EncorderEvent1 = RISING;
    edge0 = TC1;
    break;
    
    case RISING:
    edge1 = TC1;
    period1 = (overflow1*65536UL+edge1)-edge0;
    edge0= edge1;
    overflow1 =0;
    break;
  }
  
}

interrupt VectorNumber_Vtimovf void overFlowHandler(void) 
{

  overflow0++;
  overflow1++;
  (void)TCNT;//Clear interrupt by reading TCNT becaues of fast clear TFLG2 Reg
}

interrupt VectorNumber_Vtimch3 void TimerCh3Handler (void) 
{
  setPoint0= PIDSpeed0*SENSOR_GAIN;
  feedBack0 = (signed int)(FEEDBACK_SCALE_FACTOR/period0);
 
 
 /*Calculate error term*/
  Error0 = (signed)setPoint0 -(signed)feedBack0;
 
 /*Calculate the control law*/
  if((Error0 < STUPID_SPEED_ERROR) && (Error0 > NSTUPID_SPEED_ERROR))
  {
    /*Check if we are at PWM limits, if we are do not do PI intergral*/
    if( ((PWMDTY4==MIN_DRIVE_VALUE)&&(Error0<0)) || ((PWMDTY4 == MAX_DRIVE_VALUE) && (Error0>0)) ) 
    {
   
    }
    else 
    {
     ErrorIntergral0+= (int)Error0;
    }
   
      /*calculate control law*/
    driveValue0 =((Error0*P_GAIN)+(ErrorIntergral0*I_GAIN))/GAIN_DIVISOR;
    if(driveValue0 > MAX_DRIVE_VALUE) 
    {
      driveValue0 = MAX_DRIVE_VALUE;
    }
    else if (driveValue0 < MIN_DRIVE_VALUE)
    {
      driveValue0 = MIN_DRIVE_VALUE;
    }
 
    PWMDTY4 = (unsigned char)driveValue0;
  }
  TIMER_CHNL(3) = TCNT + WATCHDOG;
}


interrupt VectorNumber_Vtimch4 void TimerCh4Handler (void) 
{
  setPoint1= PIDSpeed1*SENSOR_GAIN;
  feedBack1 = (signed int)(FEEDBACK_SCALE_FACTOR/period1);
 
 
 /*Calculate error term*/
  Error1 = (signed)setPoint1 -(signed)feedBack1;
 
 /*Calculate the control law*/
  if((Error1 < STUPID_SPEED_ERROR) && (Error1 > NSTUPID_SPEED_ERROR))
  {
    /*Check if we are at PWM limits, if we are do not do PI intergral*/
    if( ((PWMDTY5==MIN_DRIVE_VALUE)&&(Error1<0)) || ((PWMDTY5 == MAX_DRIVE_VALUE) && (Error1>0)) ) 
    {
   
    }
    else 
    {
     ErrorIntergral1+= (int)Error1;
    }
   
      /*calculate control law*/
    driveValue1 =((Error1*P_GAIN)+(ErrorIntergral1*I_GAIN))/GAIN_DIVISOR;
    if(driveValue1 > MAX_DRIVE_VALUE) 
    {
      driveValue1 = MAX_DRIVE_VALUE;
    }
    else if (driveValue1 < MIN_DRIVE_VALUE)
    {
      driveValue1 = MIN_DRIVE_VALUE;
    }
 
    PWMDTY5 = (unsigned char)driveValue1;
  }
  TIMER_CHNL(4) = TCNT + WATCHDOG;
}


void dcMotor0Speed(void) 
{
  speedMotor1 = (feedBack0/SENSOR_GAIN);
}


void dcMotor1Speed(void) 
{
  speedMotor2 = (feedBack1/SENSOR_GAIN); 
}



interrupt VectorNumber_Vtimch5 void TimerCh5Handler (void) 
{
  
  Msec+=50;
  TIMER_CHNL(5)= TCNT + TIMER;
  
  
  if((travelMode == STRAIGHTM)&&(travelTime == Msec)) 
  {
   
    Msec = 0;
    travelMode = TURN90;
    PIDSpeed0=10;
    PIDSpeed1=10;
    MOT12_BUS(DArray[8]);//Stop motors
    MOT12_BUS(DArray[3]);//Run one motor
    travelTime =4900;
    
  } 
  else if ((travelMode == TURN90) && (travelTime == Msec)) 
  {
    
    Msec = 0;
    travelMode = STRAIGHTS;
    PIDSpeed0=11;
    PIDSpeed1=11;
    MOT12_BUS(DArray[8]);//Stop motors 
    MOT12_BUS(DArray[6]);//Run both motors
    travelTime =9700;
    
  } 
  else if ((travelMode == STRAIGHTS)&&(travelTime == Msec)) 
  {
     
     Msec = 0;
     travelMode = TURN60;
     MOT12_BUS(DArray[8]);//Stop motors
     MOT12_BUS(DArray[3]);//Run one motor
     travelTime=6000;
  } 
  else if ((travelMode == TURN60) && (travelTime == Msec)) 
  {
  
    Msec = 0;
    PIDSpeed0 = 10;
    PIDSpeed1 = 10;
    travelMode=STRAIGHTL;
    MOT12_BUS(DArray[8]);//Stop motors
    MOT12_BUS(DArray[6]);//Run both motors
    travelTime =21400;
    
  } 
  else if((travelMode == STRAIGHTL)&&(travelTime == Msec)) 
  {
   
    Msec=0;
    PIDSpeed0 = 11;
    PIDSpeed1 = 11;
    travelMode = TURN30;
    MOT12_BUS(DArray[8]);//Stop motors
    MOT12_BUS(DArray[3]);//Run both motors
    travelTime= 7100;
    //CLR_BITS(TIE,0x18);//Stop both watchdog timers
    //DISABLE_CHNL_INTERRUPT(5);
  } 
  else if ((travelMode == TURN30)&&(travelTime == Msec)) 
  {
   MOT12_BUS(DArray[8]);//Stop motors
   CLR_BITS(TIE,0x18);//Stop both watchdog timers
   DISABLE_CHNL_INTERRUPT(5);
  }
}


void runDistance (unsigned char motorSpeed,unsigned int time) 
{
  
  
  travelTime = time;
  travelMode = STRAIGHTM;
  PIDSpeed0 = PIDSpeed1= motorSpeed;
  ENABLE_CHNL_INTERRUPT(5);
  TIMER_CHNL(5)= TCNT + TIMER;
}


  
  






  
  
  
  
  
  
  


        
    
    






  





