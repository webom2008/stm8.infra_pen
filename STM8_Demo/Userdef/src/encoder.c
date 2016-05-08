/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : Ecoder.c
* Function  : Collect status of Ecoder
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/07
*********************************************************************************************************/
#define _ENCODER_C_

#include "stm8s.h"
#include "encoder.h"
#include "key.h"
#include "timer.h"

#define ENCODER_PORT        (GPIOD)
#define ENCODER_PRESS_PORT  (GPIOC)
#define ENCODER_KEY_PIN     (GPIO_PIN_7)
#define ENCODER_B_PIN       (GPIO_PIN_2)
#define ENCODER_A_PIN       (GPIO_PIN_3)

#define ENCODER_A_DATA      (GPIO_ReadInputData(ENCODER_PORT)&ENCODER_A_PIN)
#define ENCODER_B_DATA      (GPIO_ReadInputData(ENCODER_PORT)&ENCODER_B_PIN)
#define ENCODER_KEY_DATA    (GPIO_ReadInputData(ENCODER_PRESS_PORT)&ENCODER_KEY_PIN)

#define ECDKEY_DOWN_FILTER_CNT 2    /* �������������˲�Ϊ10ms����ʱ��ÿ�ж�һ��Ϊ5ms  2*/ 
#define ECDKEY_UP_FILTER_CNT   20   /* �������������˲�Ϊ100ms,Encoder�����Ķ����ϴ���ʱӦ��һ�� 20*/
#define ECDKEY_LONGPRESS_CNT   400  /* ���������˲�Ϊ2s  400*/

EncoderPhaseLevel EcdPhaselLevel = ENCODER_PHASE_MIN; /*The level at A&B Phase of Encoder.*/
EncoderPhaseLevel PreEcdPhaseStat = ENCODER_PHASE_MIN;/*The level at A&B Phase of Encoder on the last time*/
EncoderPhaseLevel EcdBirdge = ENCODER_PHASE_MIN;      /*The level of intermediate state.*/



static bool IsDownEncoderKey(void)  {if(ENCODER_KEY_DATA == 0x00) return TRUE; return FALSE;}

KEY_INFO EcdKeyInfo = {
                       IsDownEncoderKey, 0, ECDKEY_DOWN_FILTER_CNT, 
                       ECDKEY_UP_FILTER_CNT, ECDKEY_UP_FILTER_CNT,
                       0, ECDKEY_LONGPRESS_CNT, 0, KEY_UP, 
                       KEY_LONG_PRESS_NONE
                       };



void EncoderPin_Init(void)
{
    GPIO_Init(ENCODER_PORT, (ENCODER_A_PIN|ENCODER_B_PIN), GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(ENCODER_PRESS_PORT,  ENCODER_KEY_PIN, GPIO_MODE_IN_PU_NO_IT);
}
/********************************************************************************
 * ������: void EncoderData_Init(void)
 * ��  ��: ��ʼ�����������������
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void EncoderData_Init(void)
{
   PreEcdPhaseStat = GetEcdPhaseLevel(); 
}
/********************************************************************************
 * ������: EncoderPhaseLevel GetEcdPhaseLevel(void)
 * ��  ��: ��ȡ������A��B����ĵ�ƽ״̬
 * ��  ��: ��
 * ��  ��: A��B����ĵ�ƽ״̬
 *******************************************************************************/
EncoderPhaseLevel GetEcdPhaseLevel(void)
{ 
    EncoderPhaseLevel EcdPhaselLevel;
	u8 EncordData_A = ENCODER_A_DATA;
	u8 EncordData_B = ENCODER_B_DATA;

    if((EncordData_A == 0x00)&&(EncordData_B == 0x00))
    {
        EcdPhaselLevel = ENCODER_AB_L;   
    }
    else if((EncordData_A == ENCODER_A_PIN)&&(EncordData_B == ENCODER_B_PIN))
    {
        EcdPhaselLevel = ENCODER_AB_H;
    }

    else if((EncordData_A == ENCODER_A_PIN)&&(EncordData_B == 0x00))
    {
         EcdPhaselLevel = ENCODER_A_H_B_L;
         EcdBirdge = EcdPhaselLevel; 
    }
    else if((EncordData_A == 0x00)&&(EncordData_B == ENCODER_B_PIN))
    {
        EcdPhaselLevel = ENCODER_A_L_B_H;
        EcdBirdge = EcdPhaselLevel;
    } 
    
    return EcdPhaselLevel;
}
/********************************************************************************
 * ������: EncoderDirect Get_EncoderDirection(void)
 * ��  ��: ��ȡ����������ת����
 * ��  ��: ��
 * ��  ��: ��ת����
 *******************************************************************************/
EncoderDirect Get_EncoderDirection(void)
{
    EncoderDirect EcdDirect = DIRECTION_NONE;
    EncoderPhaseLevel CurEcdPhaseStat = ENCODER_PHASE_MIN;

	CurEcdPhaseStat = GetEcdPhaseLevel();
    if(PreEcdPhaseStat != CurEcdPhaseStat)
    {
        if((PreEcdPhaseStat == ENCODER_AB_L)&&((CurEcdPhaseStat == ENCODER_AB_H)))
        {
            if(EcdBirdge == ENCODER_A_H_B_L)
            {
                PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CW; 
		        EcdBirdge = ENCODER_PHASE_MIN;                //��ֹ�����ж���ת����ʧ��
            }
            else if(EcdBirdge == ENCODER_A_L_B_H)
            {
                PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CCW;
				EcdBirdge = ENCODER_PHASE_MIN;
                return EcdDirect;
            }
        }
        else if((PreEcdPhaseStat == ENCODER_AB_H)&&((CurEcdPhaseStat == ENCODER_AB_L)))
        {
            if(EcdBirdge == ENCODER_A_H_B_L)
            {
                PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CCW;
	            EcdBirdge = ENCODER_PHASE_MIN;          //��ֹ�����ж���ת����ʧ��
            }
            else if(EcdBirdge == ENCODER_A_L_B_H)
            {
                PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CW;
		        EcdBirdge = ENCODER_PHASE_MIN;
            }
        }
		else
		{
			if(PreEcdPhaseStat == ENCODER_A_H_B_L && CurEcdPhaseStat == ENCODER_AB_H)
			{
			    PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CW;
		        EcdBirdge = ENCODER_PHASE_MIN;
			}
			else if(PreEcdPhaseStat == ENCODER_A_H_B_L && CurEcdPhaseStat == ENCODER_AB_L)
			{
				PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CCW;
		        EcdBirdge = ENCODER_PHASE_MIN;
			}
			else if(PreEcdPhaseStat == ENCODER_A_L_B_H && CurEcdPhaseStat == ENCODER_AB_H)
			{
			    PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CCW;
		        EcdBirdge = ENCODER_PHASE_MIN;
			}
			else if(PreEcdPhaseStat == ENCODER_A_L_B_H && CurEcdPhaseStat == ENCODER_AB_L)
			{
				PreEcdPhaseStat = CurEcdPhaseStat; 
                EcdDirect = DIRECTION_CW;
		        EcdBirdge = ENCODER_PHASE_MIN;
			}

		}
    } 
    
    return EcdDirect;
}
/********************************************************************************
 * ������: u8 Get_EncoderCurrentData(void)
 * ��  ��: ��ȡ��������ǰ��״̬����
 * ��  ��: ��
 * ��  ��: ��ǰ״̬����
 *******************************************************************************/
u8 Get_EncoderCurrentData(void)
{
    u8 NewEcdData = 0;

    switch(Get_EncoderDirection())
    {
        case DIRECTION_NONE:
            NewEcdData |= 0x00;
            if(EcdKeyInfo.IsKeyDown == KEY_DOWN)
                NewEcdData |= 0x04;
            else
                NewEcdData &= ~(0x04);
                
            if(EcdKeyInfo.IsKeyLong == KEY_LONG_PRESS)
                NewEcdData |= 0x80;
            else
                NewEcdData &= ~(0x80); 
            break;
        case DIRECTION_CW:
            NewEcdData |= 0x01;
            if(EcdKeyInfo.IsKeyDown == KEY_DOWN)
                NewEcdData |= 0x04;
            else
                NewEcdData &= ~(0x04);
                
            if(EcdKeyInfo.IsKeyLong == KEY_LONG_PRESS)
                NewEcdData |= 0x80;
            else
                NewEcdData &= ~(0x80); 
            break;
        case DIRECTION_CCW:
            NewEcdData |= 0x02;
            if(EcdKeyInfo.IsKeyDown == KEY_DOWN)
                NewEcdData |= 0x04;
            else
                NewEcdData &= ~(0x04);
                
            if(EcdKeyInfo.IsKeyLong == KEY_LONG_PRESS)
                NewEcdData |= 0x80;
            else
                NewEcdData &= ~(0x80); 
            break;
        default:
            NewEcdData &= 0x00;
            break;
   }
   
   return NewEcdData;
}
/********************************************************************************
 * ������: void Encoder_Init(void)
 * ��  ��: ��ʼ�����������Ӳ��������
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Encoder_Init(void)
{
    EncoderPin_Init();
    EncoderData_Init();
}

#undef _ENCODER_C_

