/********************************************************************
Purpose: Utility Macros
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <mc9s12c32.h>
#include <hidef.h> 

// Macro to grab only the lower byte of an argument
#define LOW( value )    ((value) & 0xFF)

// Macro to set specified bits
#define SET_BITS( port, bits )            ((port) |= (bits))

// Macro to clear specified bits
#define CLR_BITS( port, bits )            ((port) &= LOW(~(bits)))

//Macro to flip specified bits
#define FLIP_BITS( port, bits )           ((port) ^= (bits))

// Macro to force the bits identified by mask to take on the values of the matching bits in value
#define FORCE_BITS( port, mask, value)    (port) = ((port) & LOW(~(mask)) | ((value) & (mask)))
#define FORCE_WORD( port, mask, value)    (port) = ((port) & ((~(mask)) & 0xFFFF) | ((value) & (mask)))


#define PORT_B PORTB
#define B_DDR DDRB
#define POWER_MASK PORTAB_BIT6_MASK
#define DDRB_POWER_INIT SET_BITS(B_DDR,POWER_MASK)
#define ANALOG_ON       SET_BITS(PORT_B,POWER_MASK)

//Cases
#define ERROR 2U
#endif