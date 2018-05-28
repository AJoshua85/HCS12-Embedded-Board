#include <hidef.h>      /* common defines and macros */
//#include "derivative.h"      /* derivative-specific definitions */
#include "ADC.h"
#include "LCD.h"
#include "SCI.h"
#include "Servo.h"
#include "DCMotor.h"
#include "Stepper.h"
#include "TimerModule.h"
#include <string.h>
#include <stdio.h>     
#include <stdlib.h>



uchar argv0;
uchar argv1[4];
uchar argv2[4];
uchar argv3[4];
uchar argv4[2];




uchar speedV;
uchar directionV;
uchar motorV;
extern unsigned char speedMotor1;
extern unsigned char speedMotor2;


 




extern volatile uchar msgCount ; // rs232 flag, set by SCI to notify mainline that a msg has been recieved. 
//void Clearcmdbuffs(uchar a,uchar b,uchar c,uchar d,uchar e);


void main(void) 
{
  /* put your own code here */
  

  
  /****INITIALIZE servo init before dc motor init because servo init sets timer prescalers*/
  
  
    
  
  
  
  
  
  DDRB_POWER_INIT;
  ANALOG_ON;
  CLR_BITS(DDRB,DDRB_BIT4|DDRB_BIT5); 
  
  servoInit();
  
  initLCD();
  InitSCI();
  initDCMotor();
  initEncorder();
  enableChannel(CH5|CH4);/***THIS LINE WAS MISSING*****/
  
  initRTI();
  speed(0x3F);
  
  
  

  
  EnableInterrupts;
  
  
  setMode(1,SEEK);
  initStepper();
  
  

  
  for(;;) 
  {
    if ((PORTB&PORTB_BIT4_MASK)== 0) 
    {
      /*Used for testing*/ 
      setMotor(100,6,MOT12);
      runDistance(11,17100);
      initADCV2(3);
      asm("nop");
    }
    
    if ((PORTB&PORTB_BIT5_MASK)== 0) 
    { /*Used for testing*/
      processADC(); 
    }
      
    
  
    if (msgCount == 1)
    {
      // critical region when copying rs232_buf to local cmd_buf
      get_cmdbuf(&argv0,argv1,argv2,argv3,argv4);
      asm("nop");
    }
        
    if(argv0 =='D') {
      
       setMotor((uchar)(atoi(argv1)),(uchar)(atoi(argv2)),(uchar)(atoi(argv3)));
      msgCount = 0;
     
      argv0 = '0';
      (void)memset(argv1,0,4);
      (void)memset(argv2,0,4);
      (void)memset(argv3,0,4);
    }
     
    else if(argv0 == 'R')
    {
      setModeServo((uchar)(atoi(argv1)),(uchar)(atoi(argv2)));
      msgCount = 0;
      argv0 = '0';
      (void)memset(argv1,0,4);
      (void)memset(argv2,0,4);
    } 
    else if (argv0 == 'S')
    {
      setMode(3,STPFIXED,HALF,atoi(argv2));
      msgCount = 0;
      argv0 = '0';
      (void)memset(argv1,0,4);
      (void)memset(argv2,0,4); 
    }
    else if (argv0 == 'C') 
    {
      LCDclear();
      LCDprintf("Camera Capture");
      msgCount = 0;
      argv0 = '0';
      (void)memset(argv1,0,4);
      (void)memset(argv2,0,4);
      (void)memset(argv3,0,4);
    }
    else if (argv0 == 'P') 
    {
     if (atoi(argv1) == 3) 
     {
      LCDclear();
      LCDprintf("Triangle");
     } 
     else 
     {
      LCDclear();
      LCDprintf("Square");
     }
     msgCount = 0;
     argv0 = '0';
     (void)memset(argv1,0,4);
     (void)memset(argv2,0,4); 
    }
    else if (argv0 == 'A') 
    {
     LCDclear();
     LCDprintf("Initiating ADC");
     initADCV2(3);
     msgCount = 0;
     argv0 = '0';
     (void)memset(argv1,0,4);
     (void)memset(argv2,0,4);  
    } 
    else if (argv0 == 'B') 
    {
     LCDclear();
     LCDprintf("Gathering ADC values");
     processADC();
     msgCount = 0;
     argv0 = '0';
     (void)memset(argv1,0,4);
     (void)memset(argv2,0,4); 
    }
      
   }  
 
}
