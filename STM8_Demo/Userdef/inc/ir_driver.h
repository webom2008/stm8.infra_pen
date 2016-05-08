/**
  ******************************************************************************
  * @file    ir_driver.h
  * @author  Origin
  * @version V1.1.0
  * @date    05-January-2015
  * @brief   IR Driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 CVTE</center></h2>
  *
  * You may send Email to the anthor: yangqiyuan@cvte.cn
  *
  *
  ******************************************************************************
  */

#ifndef __IR_DRIVER_H
#define __IR_DRIVER_H

#include "stm8l15x.h"
#include "queue.h"

typedef __IO uint32_t  vu32;
typedef __IO uint16_t  vu16;
typedef __IO uint8_t   vu8;


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

/* NEC 50us*/
#define HEADER_MIN          180//900//1300
#define HEADER_MAX          350//1750//1400
#define HEADER_MID          270//1350

#define LOGIC0_MIN          16//80//91
#define LOGIC0_MAX          28//140//132
#define LOGIC0_MID          23//115

#define LOGIC1_MIN          32//160//200
#define LOGIC1_MAX          58//290//245
#define LOGIC1_MID          45//225


#define TAIL_MIN            760//3800
#define TAIL_MAX            860//4300
#define TAIL_MID            810//4050

#define STOP_MIN            213//1065
#define STOP_MAX            240//1200
#define STOP_MID            224//1120

#define REPEAT_MIN          1600//8000
#define REPEAT_MAX          2200//11000
#define REPEAT_MID          1920//9600

#define IRERR_NONE          0
#define IRERR_SUCCESS       1
#define IRERR_REPEAT        2
#define IRERR_ERROR         3   


#define IF_HEADER(x)        (((x)>=HEADER_MIN)&&((x)<=HEADER_MAX))
#define IF_LOGIC0(x)        (((x)>=LOGIC0_MIN)&&((x)<=LOGIC0_MAX))
#define IF_LOGIC1(x)        (((x)>=LOGIC1_MIN)&&((x)<=LOGIC1_MAX))
#define IF_TAIL(x)          (((x)>=TAIL_MIN)&&((x)<=TAIL_MAX))
#define IF_STOP(x)          (((x)>=STOP_MIN)&&((x)<=STOP_MAX))
#define IF_REPEAT(x)        (((x)>=REPEAT_MIN)&&((x)<=REPEAT_MAX))

#define IF_ABOVE_LOGIC(x)   ((x)>LOGIC1_MAX)
#define IF_BELOWE_LOGIC(x)  ((x)<LOGIC0_MIN)

#define IF_ABOVE_TAIL(x)    ((x)>TAIL_MAX)
#define IF_BELOW_TAIL(x)    ((x)<TAIL_MIN)

#define IF_ABOVE_STOP(x)    ((x)>STOP_MAX)
#define IF_BELOW_STOP(x)    ((x)<STOP_MIN)

#define IF_ABOVE_REPEAT(x)  ((x)>REPEAT_MAX)
#define IF_BELOW_REPEAT(x)  ((x)<REPEAT_MIN)

/* RC5 */
#define RC5_CUSTOMER                0x00

#define RC5_HEAD_PLUSE4_MIN         334
#define RC5_HEAD_PLUSE4_MAX         345
#define RC5_HEAD_PLUSE3_MIN         249
#define RC5_HEAD_PLUSE3_MAX         261
#define RC5_HEAD_PLUSE2_MIN         164
#define RC5_HEAD_PLUSE2_MAX         177 

#define RC5_DECODE_OUT_TIME         25
#define RC5_DATA_OUT_TIME           10
#define RC5_DATA_MIN_LEN            10
#define RC5_DATA_SUM_MAX            2000
#define RC5_DATA_BUF_LEN            20


#define RC5_PLUSE4_MIN              334
#define RC5_PLUSE4_MID              340
#define RC5_PLUSE4_MAX              345
#define RC5_PLUSE3_MIN              249
#define RC5_PLUSE3_MID              255
#define RC5_PLUSE3_MAX              261
#define RC5_PLUSE2_MIN              164
#define RC5_PLUSE2_MID              170
#define RC5_PLUSE2_MAX              177 
#define RC5_PLUSE_LOW               111

#define IF_RC5_PLUSE4(x)            (((x)>=RC5_PLUSE4_MIN)&&((x)<=RC5_PLUSE4_MAX))
#define IF_RC5_PLUSE3(x)            (((x)>=RC5_PLUSE3_MIN)&&((x)<=RC5_PLUSE3_MAX))
#define IF_RC5_PLUSE2(x)            (((x)>=RC5_PLUSE2_MIN)&&((x)<=RC5_PLUSE2_MAX))
#define IF_RC5_LEGAL(x)             ((x)<=RC5_PLUSE4_MAX)
#define IF_RC5_MAX(x)               ((x)>RC5_PLUSE4_MAX)
#define IF_RC5_LOW(x)               ((x)<RC5_PLUSE_LOW)

#define IF_RC5_HEAD_PLUSE4(x)       (((x)>=RC5_HEAD_PLUSE4_MIN)&&((x)<=RC5_HEAD_PLUSE4_MAX))
#define IF_RC5_HEAD_PLUSE3(x)       (((x)>=RC5_HEAD_PLUSE3_MIN)&&((x)<=RC5_HEAD_PLUSE3_MAX))
#define IF_RC5_HEAD_PLUSE2(x)       (((x)>=RC5_HEAD_PLUSE2_MIN)&&((x)<=RC5_HEAD_PLUSE2_MAX))

#define IR_GPIO_SEND_PORT   GPIOB
#define IR_GPIO_SEND_PIN    GPIO_Pin_2
#define IR_GPIO_REV_PORT    GPIOB
#define IR_GPIO_REV_PIN     GPIO_Pin_0
#define IR_REV_EXTI_PIN     EXTI_Pin_0

#define IR_SEND_Toggle              IR_GPIO_SEND_PORT->ODR ^= IR_GPIO_SEND_PIN;
#define IR_SEND_HIGH                IR_GPIO_SEND_PORT->ODR |= IR_GPIO_SEND_PIN;
#define IR_SEND_LOW                 IR_GPIO_SEND_PORT->ODR &= (uint8_t)(~IR_GPIO_SEND_PIN);
#define IR_OUT_PWM_ENABLE           TIM2->CR1 |= TIM_CR1_CEN; //TIM1->BKR |= TIM1_BKR_MOE
#define IR_OUT_PWM_DISABLE          TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN) //TIM1->BKR &= (uint8_t)(~TIM1_BKR_MOE)

#define CUSTOME_CODE                0x20DF
#define VOLUME_PLUS_CODE            0x03
#define VOLUME_MINUS_CODE           0x41

typedef enum _ir_nec_state
{
    IR_NEC_NONE,
    IR_NEC_HEAD,
    IR_NEC_LOGIC,
    IR_NEC_TAIL,
    IR_NEC_STOP,
    IR_NEC_REPEAT,
}IrNecState;

typedef enum _decode_nec_state
{
    DECODE_NEC_NONE = 0,
    DECODE_NEC_CUSTOM = 1,
    DECODE_NEC_KEY = 2,
    DECODE_NEC_KEYN = 3,
    DECODE_NEC_TAIL = 4,
    DECODE_NEC_STOP = 5,
    DECODE_NEC_REPEAT = 6,
}DecodeNecState;

typedef enum _decode_rc5_state
{
    DECODE_RC5_NONE     = 0,
    DECODE_RC5_START    = 1,
    DECODE_RC5_LOGIC    = 2,
    DECODE_RC5_STOP     = 3,
}DecodeRC5State;

typedef enum _ir_protocol
{
    NEC_PROTOCOL,
    RC5_PROTOCOL,
}IrProtocol;


typedef struct _ir_dev
{
    IrProtocol protocols;
    uint16_t customecode;
    u8 keycode;
    u8 keycodeN;
    u16 repeat;
}IrDev;
extern IrDev IR;
extern QUEUE_TYPE *pIrQueue;

extern void NecIrDetect(u32 Pluse);
extern u8 IrNecDecode(IrDev *Dev);
extern void IrNecTest(void);
extern void IrNecIsr(void);

extern void IrRc5Isr(void);
extern void Rc5IrDetect(u32 Pluse);
extern u8 IrRc5Decode(IrDev *Dev);
extern void IrRc5Reinit(void);
extern u8 Ir_EnQueue(u32 data);
extern u8 Ir_DeQueue(u32 *data);


extern void IrNECSend(uint16_t customecode, uint8_t keycode);
#endif
/************************ (C) COPYRIGHT CVTE ************************/

