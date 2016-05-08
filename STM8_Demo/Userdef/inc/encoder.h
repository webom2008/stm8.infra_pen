/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : Ecoder.h
* Function  : Collect status of Ecoder
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/07
*********************************************************************************************************/
#ifndef __ENCODER_H_
#define __ENCODER_H_
#include "key.h"

typedef enum _ENCODER_DERECTION
{
    DIRECTION_NONE,
    DIRECTION_CW,
    DIRECTION_CCW,    
}EncoderDirect;

typedef enum _ENCODER_PHASE_LEVEL
{
    ENCODER_PHASE_MIN,
    ENCODER_AB_L,
    ENCODER_AB_H,
    ENCODER_A_H_B_L,
    ENCODER_A_L_B_H,
}EncoderPhaseLevel;

#ifdef _ENCODER_C_
#define INTERFACE
#else
#define INTERFACE extern 
#endif

INTERFACE KEY_INFO EcdKeyInfo;

INTERFACE void Encoder_Init(void);
INTERFACE EncoderDirect Get_EncoderDirection(void);
INTERFACE EncoderPhaseLevel GetEcdPhaseLevel(void);
INTERFACE u8 Get_EncoderCurrentData(void);

#undef INTERFACE

#endif