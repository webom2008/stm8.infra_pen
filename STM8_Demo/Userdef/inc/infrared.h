/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : infrared.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/28
  Last Modified :
  Description   : infrared.c header file
  Function List :
  History       :
  1.Date        : 2016/5/28
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __INFRARED_H__
#define __INFRARED_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080
#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

#define INFRARED_DATA_PRECISION     5
#define HEADER_MID_US   ((u16)1000)
#define HEADER_MIN_US   ((u16)(HEADER_MID_US * (100 - INFRARED_DATA_PRECISION) / 100))
#define HEADER_MAX_US   ((u16)(HEADER_MID_US * (100 + INFRARED_DATA_PRECISION) / 100))

#define LOGIC0_MID_US   ((u16)790)
#define LOGIC0_MIN_US   ((u16)(LOGIC0_MID_US * (100 - INFRARED_DATA_PRECISION) / 100))
#define LOGIC0_MAX_US   ((u16)(LOGIC0_MID_US * (100 + INFRARED_DATA_PRECISION) / 100))

#define LOGIC1_MID_US   ((u16)890)
#define LOGIC1_MIN_US   ((u16)(LOGIC1_MID_US * (100 - INFRARED_DATA_PRECISION) / 100))      
#define LOGIC1_MAX_US   ((u16)(LOGIC1_MID_US * (100 + INFRARED_DATA_PRECISION) / 100))


#define IRERR_NONE          0
#define IRERR_SUCCESS       1
#define IRERR_ODD_EVEN_ERR  2
#define IRERR_ERROR         3   


#define IF_HEADER(x)        (((x)>=HEADER_MIN_US)&&((x)<=HEADER_MAX_US))
#define IF_LOGIC0(x)        (((x)>=LOGIC0_MIN_US)&&((x)<=LOGIC0_MAX_US))
#define IF_LOGIC1(x)        (((x)>=LOGIC1_MIN_US)&&((x)<=LOGIC1_MAX_US))

#define IR_SEND_Toggle      IR_GPIO_SEND_PORT->ODR ^= IR_GPIO_SEND_PIN;
#define IR_SEND_HIGH        IR_GPIO_SEND_PORT->ODR |= IR_GPIO_SEND_PIN;
#define IR_SEND_LOW         IR_GPIO_SEND_PORT->ODR &= (uint8_t)(~IR_GPIO_SEND_PIN);
#define IR_OUT_PWM_ENABLE   TIM2->CR1 |= TIM_CR1_CEN;
#define IR_OUT_PWM_DISABLE  TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN)


#define IR_GPIO_SEND_PORT   GPIOB
#define IR_GPIO_SEND_PIN    GPIO_Pin_2
#define IR_GPIO_REV_PORT    GPIOB
#define IR_GPIO_REV_PIN     GPIO_Pin_0
#define IR_REV_EXTI_PIN     EXTI_Pin_0

extern void Infrared_InterrurptHandler(void);
extern void Infrared_Init(void);
extern void Infrared_server(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __INFRARED_H__ */
