#ifndef _TimerModule_H
#define _TimerModule_H

// Macro to enable the PWM Channel
#define ENABLE_PWM_CHNL( chnl )		SET_BITS( PWME, 1 << ( chnl ) )

// Macro to disable the PWM Channel
#define DISABLE_PWM_CHNL( chnl )	CLR_BITS( PWME, 1 << ( chnl ) )

// Macro to configure specified timer channel as input capture
#define MAKE_CHNL_IC( chnl )	CLR_BITS( TIOS, 1 << ( chnl ) )

// Macro to configure a specified timer channel as output compare
#define MAKE_CHNL_OC( x )	( SET_BITS( TIOS,( 1 << ( x ) ) ) )
       

// Combine edges used for input and output capture
#define TCTL_1_2 ( * ( volatile word *const) & TCTL1 )
#define TCTL_3_4 ( * ( volatile unsigned short int *const) & TCTL3 )

// Macro to define what edge the input capture will interrupt on
#define SET_IC_EDGE( chnl, edgetype )	FORCE_BITS( TCTL_3_4, 0x03 << ( ( chnl ) * 2 ), ( edgetype ) << ( ( chnl ) * 2 ) )

// Edge types for input capture
#define IC_EDGE_NONE    0x00
#define IC_EDGE_RISING  0x01
#define IC_EDGE_FALLING 0x02
#define IC_EDGE_EITHER  0x03

// Macro to configure specified timer channel to perform specified action
#define SET_OC_ACTION( chnl, action )     FORCE_WORD( TCTL_1_2, ( 0x03 << ( ( chnl ) * 2 ) ), ( ( action ) << ( ( chnl ) * 2 ) ) )

// Macro to force output capture function on next clock event
#define FORCE_OC_ACTION_NOW( chnl, action )	SET_OC_ACTION( ( chnl ), ( action ) ); SET_BITS( CFORC, 1 << ( chnl ) )

// Valid action values for the output capture
#define OC_OFF    0x00
#define OC_TOGGLE 0x01
#define OC_GO_LOW  0x02
#define OC_GO_HI  0x03


// Macro to allow the specified channel to generate interrupts
#define ENABLE_CHNL_INTERRUPT( chnl )     SET_BITS( TIE, 1 << (chnl) )
#define DISABLE_CHNL_INTERRUPT( chnl)     CLR_BITS( TIE, 1 << (chnl) )

// Macro to calculate the vector # given the vector address
// - Taken From Bill Stefanuk
// - take 1's complement of address to get a positive value
// - mask off lower byte since that's the only meaningful data - rest should be 0
// - right shift by 1 (divide by 2) to get the vector number since each vector address is 2 bytes wide
// - (The LSB will be 1 before shift.  This will get shifted off on the divide by 2.  e.g. 3/2 = 1, 5/2= 2)
#define VECTOR_NUM( vector_addr ) (((~(vector_addr)) & 0xFF) >> 1)

// Macro to build name of the timer channel vector from its channel number (Vector names defined in mc9s12c32.h)
#define TIMER_CHNL_VECTOR( chnl )  Vtimch ## chnl

// Macro to build a mask for the timer channel flag in TFLG1
#define TIMER_CHNL_FLAG( chnl )		(1 << ( chnl ) )

// Macro to clear flag associated with specified timer channel
#define CLR_TIMER_CHNL_FLAG( x )	( TFLG |= ( 1 << ( x ) ) )

// Macro to form a valid timer channel register name from a channel number
#define TIMER_CHNL( chnl )                TC ## chnl// *** Macros specific to the HCS12 Timer Module

// Macro and manifests to set the timer prescale value to one of 8 valid values
#define SET_TCNT_PRESCALE( value )  FORCE_BITS( TSCR2, TSCR2_PR_MASK, ( value ) )
#define TCNT_PRESCALE_1       0x00
#define TCNT_PRESCALE_2       0x01
#define TCNT_PRESCALE_4       0x02
#define TCNT_PRESCALE_8       0x03
#define TCNT_PRESCALE_16      0x04
#define TCNT_PRESCALE_32      0x05
#define TCNT_PRESCALE_64      0x06
#define TCNT_PRESCALE_128     0x07

//Pulse Width State
#define LOWSTATE 0U
#define HIGHSTATE 1U

// Init value for TSCR1 to enable timer, freeze the timer while debugging, and use the fast-clear function
#define TSCR1_INIT            (TSCR1_TEN_MASK | TSCR1_TFFCA_MASK| TSCR1_TSFRZ_MASK)

// Macro to specify period length for msDELAY
#define msDELAY_TICKS 1000

// TimerModule.c Protypes
void msDELAY(int k);

#endif
