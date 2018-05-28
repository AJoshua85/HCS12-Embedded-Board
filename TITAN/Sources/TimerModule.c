#include "derivative.h"      
#include "TimerModule.h"
#include "utils.h"


/**************msDELAY()*******************************************************
Purpose: ms Delay
Input:   int k - number of milliseconds
**Currently Uses a prescaler of 8**
******************************************************************************/
void msDELAY(int k) 
{
  int ix;
  
  TC7 = TCNT + msDELAY_TICKS;  // Preset TC7 for first OC event
  MAKE_CHNL_OC( 7 );  // Set channel as OC
  
  for(ix = 0; ix < k; ix++)
  {
    while(!(TFLG1 & TFLG1_C7F_MASK));
      TC7 += msDELAY_TICKS;  
  }
  
  TIOS &= LOW(~TIOS_IOS7_MASK );  // Turn off OC
}
