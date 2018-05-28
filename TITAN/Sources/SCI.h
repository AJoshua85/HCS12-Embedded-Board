/********************************************************************
Purpose: Macros for the SCI
Author: Avinash Joshua
Date: October 25 2017
Version:00
*********************************************************************/


#include "utils.h"

#ifndef _SCIMOD_H
#define _SCIMOD_H

#define TURN_ON_TRANS SET(SCICR2,SCICR2_TE_MASK)
#define TURN_ON_RECIV SET(SCICR2,SCICR2_RE_MASK)
#define TURN_OFF_TRANS CLR(SCICR2,SCICR2_TE_MASK)
#define TURN_OFF_RECIV CLR(SCICR2,SCICR2_RE_MASK)
#define FULLSCI 16U

#define rxBufSize 16

void InitSCI(void);
void putcSCI(char cx);
void putsSCI(char *str);
void getcSCI(char cx);
void get_cmdbuf(uchar *buf0,uchar buf1[],uchar buf2[], uchar buf3[], uchar buf4[]);
void copyString(uchar *dest,uchar *src,unsigned char num);
void transferBuf(uchar buf1, uchar buf2);

#endif
 