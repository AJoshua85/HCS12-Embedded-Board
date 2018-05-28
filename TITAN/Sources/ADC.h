/********************************************************************
Purpose: Macros for the ATD
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/

#include "utils.h"


#ifndef _ATDModule_H
#define _ATDModule_H

// ATD Control Register 2 Defines
#define ADC_INTERRUPT           ATDCTL2_ASCIE_MASK
#define ADC_INTERRUPT_STATUS    ATDCTL2_ASCIF_MASK 
#define ADC_POWER               ATDCTL2_ADPU_MASK 
#define ADC_FAST_CLR            ATDCTL2_AFFC_MASK
#define HALT_WAI                ATDCTL2_AWAI_MASK

//ATD Control Register 3 Defines and Macros
#define CONV_MASK               0x78
#define CONV_SETTING            0x03
#define FIFO_MASK               ATDCTL3_FIFO_MASK
#define ADC_NUM_CONV(data)      FORCE_BITS(ATDCTL3,CONV_MASK,(data)<<3)
#define ADC_CONV_SETTINGS(data) FORCE_BITS(ATDCTL3,CONV_SETTING,data)
#define FIFO_OFF                CLR_BITS(ATDCTL3,FIFO_MASK)
#define FIFO_ON                 SET_BITS(ATDCTL3,FIFO_MASK)


//ATD Control Registers 4 Defines and Macros
#define SAMPLE_MASK             ATDCTL4_SMP1_MASK|ATDCTL4_SMP0_MASK
#define PRESCALER_MASK          0x1F
#define _10BIT                  0x00
#define _8BIT                   ATDCTL4_SRES8_MASK
#define SAMPLE_SEL(data)        FORCE_BITS(ATDCTL4,SAMPLE_MASK,(data)<<5)
#define SET_PRESCALER(data)     FORCE_BITS(ATDCTL4,PRESCALER_MASK,data)
  
//ATD Control Register 5 Defines
#define CHNL_MASK               0x07
#define ORIEN_RIGHT             ATDCTL5_DJM_MASK
#define SIGNED                  ATDCTL5_DSGN_MASK
#define MULTI_CHNL              ATDCTL5_MULT_MASK
#define ATDCTL5INIT             0x80 //Right justified data,unsigned, single conversion,one channel
#define ATDCTL5INIT2            0xA0 //Right justified data,unsigned, continous conversion,one channel          
#define ATDCTL5INIT3            0xB0 //Right justified data unsigned,continous converstion, multi channel
#define SELECT_CHNL(chnl)       FORCE_BITS(ATDCTL5,CHNL_MASK,(chnl))
#define SLOPE 1428U
#define OFFSET  485700UL

//ATD Status Register 0 defines
#define SCF ATDSTAT0_SCF_MASK 
#define CLR_SCF SET_BITS(ATDSTAT0,ATDSTAT0_SCF_MASK)

//prototypes
void initADC(void);//Busy waits
void initADCV2(unsigned char inputChannel);//Uses interrupts 
int readChannel(unsigned char channel);
void processADC(void); 
#endif






 


