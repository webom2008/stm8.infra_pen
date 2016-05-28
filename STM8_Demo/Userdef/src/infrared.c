/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : infrared.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/28
  Last Modified :
  Description   : driver for infrared
  Function List :
  History       :
  1.Date        : 2016/5/28
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

extern uint32_t g_Timer_Count;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static vu32 u16PulseFallingTime = 0;


typedef enum _DECODE_STATE_TYPEDEF
{
    DECODE_HEAD         = 0,
    DECODE_DATA_BIT0    = 1,
    DECODE_DATA_BIT1    = 2,
    DECODE_ODD_EVEN     = 3,
    
    DECODE_MAX_BIT_LEN  = 4,
} DecodeState_TypeDef;

static DecodeState_TypeDef gDecodeState = DECODE_HEAD;

typedef enum _ENCODE_STATE_TYPEDEF
{
    ENCODE_HEAD         = 0,
    ENCODE_DATA_BIT0    = 1,
    ENCODE_DATA_BIT1    = 2,
    ENCODE_DATA_BIT2    = 3,
    ENCODE_DATA_BIT3    = 4,
    ENCODE_DATA_BIT4    = 5,
    ENCODE_ODD_EVEN     = 6,
    
    ENCODE_MAX_BIT_LEN  = 7,
} EncodeState_TypeDef;
#define ENCODE_DATA_BITS        5



#define BUF_OK 0
#define BUF_FULL 1
#define BUF_EMPTY 2
#define BUFFER_SIZE         32
typedef u16 BUFFER_TYPE;
typedef struct _CIRCULARBUFFER
{
    BUFFER_TYPE *rx_buf;
    u8 rx_max_length;
    u8 rx_head;
    u8 rx_tail;
}CIRCULARBUFFER;

static BUFFER_TYPE infrared_rx_buf[BUFFER_SIZE];
static CIRCULARBUFFER gCirCularBuffer;
static CIRCULARBUFFER *gpCirCularBuffer = &gCirCularBuffer;

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define TIM2_PERIOD                     104
#define TIM2_PULSE                      52

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static int BufferInit(CIRCULARBUFFER *pDev)
{
    pDev->rx_buf = infrared_rx_buf;
    pDev->rx_head = 0;
    pDev->rx_tail = 0;
    pDev->rx_max_length = BUFFER_SIZE;
    return 0;
}

/*******************************************************************************
** Function name:           isRxQueueFull
** Descriptions:            check whether the receive buffer is full
** input parameters:        pDev: device pointer
** output parameters:       none
** Returned value:          BUF_OK:success
*******************************************************************************/
static int isBufferFull(CIRCULARBUFFER *pDev)
{
    if((pDev->rx_head+1) % pDev->rx_max_length == pDev->rx_tail)
    {
        return BUF_FULL;
    }
    
    return BUF_OK;
}

/*******************************************************************************
** Function name:           isRxQueueEmpty
** Descriptions:            check whether the receive buffer is empty
** input parameters:        pDev: device pointer
** output parameters:       none
** Returned value:          BUF_EMPTY:success
*******************************************************************************/
static int isBufferEmpty(CIRCULARBUFFER *pDev)
{
    if(pDev->rx_head != pDev->rx_tail)
    {
        return BUF_OK;
    }
    return BUF_EMPTY;
}

/*******************************************************************************
** Function name:           BufferEnqueue
** Descriptions:            Insert one element to buffer
** input parameters:        pDev:adc device
**                          data:data to be insert
** output parameters:       none
** Returned value:          none
*******************************************************************************/
static int BufferEnqueue(CIRCULARBUFFER *pDev,BUFFER_TYPE data)
{
    if(BUF_FULL == isBufferFull(pDev))
    {
        return BUF_FULL;
    }
    
    pDev->rx_buf[pDev->rx_head] = data;
    pDev->rx_head = (pDev->rx_head+1)%pDev->rx_max_length;

    return BUF_OK;
}

/*******************************************************************************
** Function name:           adc_rx_buf_dequeue
** Descriptions:            read one byte from adc receive buffer
** input parameters:        pDev:adc device
**                          buf:buf pointer to store data
**                          num:data number read form spi receive buffer
** output parameters:       none
** Returned value:          none
*******************************************************************************/
static int BufferDequeue(CIRCULARBUFFER *pDev,BUFFER_TYPE *pBuf)
{
    if(BUF_EMPTY == isBufferEmpty(pDev))
    {
        return BUF_EMPTY;
    }


    *pBuf = pDev->rx_buf[pDev->rx_tail];
    pDev->rx_tail = (pDev->rx_tail+1) % pDev->rx_max_length;
        
    return BUF_OK;
}

static int BufferGetLength(CIRCULARBUFFER *pDev)
{
    int nLen = pDev->rx_head - pDev->rx_tail;
    if (nLen < 0)
    {
        nLen += pDev->rx_max_length;
    }
    return nLen;
}


//====================== Data API Start =========================
static int DataInit(void)
{
    return BufferInit(gpCirCularBuffer);
}

static int DataEnqueue(BUFFER_TYPE data)
{
    return BufferEnqueue(gpCirCularBuffer,data);
}

static int DataDequeue(BUFFER_TYPE *buf)
{    
    return BufferDequeue(gpCirCularBuffer,buf);
}

static int DataGetLength(void)
{
    return  BufferGetLength(gpCirCularBuffer);
}

//====================== Data API End =========================


/*****************************************************************************
 Prototype    : TIM2_Config
 Description  : timer2 configure
                System Clock = 8MHz
                timer 4 freq. = 38KHz
                ==============Detail for TIMER4=============
                  SCLK = 8MHz, TIM4CLK = 8MHz / 2 = 4MHz
                  freq. = 4MHz / 105 = 38095 Hz = 38 KHz
                ============================================
                
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/19
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void TIM2_Config(void)
{
    /* Enable TIM2 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    
    TIM2_TimeBaseInit(TIM2_Prescaler_2, TIM2_CounterMode_Up, TIM2_PERIOD);

    /* Channel 2 configuration in PWM1 mode */
    /* TIM2 channel Duty cycle is 100 * TIM2_PULSE / (TIM2_PERIOD + 1) = 100 * 4/8 = 50 % */
    TIM2_OC2Init(   TIM2_OCMode_PWM1,
                    TIM2_OutputState_Enable,
                    TIM2_PULSE,
                    TIM2_OCPolarity_Low,
                    TIM2_OCIdleState_Set);

    /* TIM2 Main Output Enable */
    TIM2_CtrlPWMOutputs(ENABLE);

    /* TIM2 counter disable */
    TIM2_Cmd(DISABLE);
}

/*****************************************************************************
 Prototype    : Infrared_InitGPIO
 Description  : infrared gpio config
 Input        : void  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void Infrared_InitGPIO(void)
{
    //IR send pin init
    GPIO_Init(IR_GPIO_SEND_PORT, IR_GPIO_SEND_PIN, GPIO_Mode_Out_PP_Low_Fast);

    //IR receive pin init
    GPIO_Init(IR_GPIO_REV_PORT, IR_GPIO_REV_PIN, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(IR_REV_EXTI_PIN, EXTI_Trigger_Falling);
}

/*****************************************************************************
 Prototype    : Infrared_Init
 Description  : infrared init
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void Infrared_Init(void)
{
    TIM2_Config();
    Infrared_InitGPIO();
    DataInit();
}

/*****************************************************************************
 Prototype    : calc_odd_parity
 Description  : 奇校验：就是让原有数据序列中（包括你要加上的一位）1的个数为
                奇数
 Input        : u8 value  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static u8 calc_odd_parity(u8 value)
{
    u8 count = 0;
    u8 i = 0;

    for (i=0; i < 8; i++)
    {
        if (0x01 == ((value>>i) & 0x01)) // 检测数据中含有1的数目
        {
            count++;
        }
    }
    
    if (0x01 == (count & 0x01)) // 1的数目为奇数
    {
        return 0;
    }
    else // 1的数目为偶数
    {
        return 1;
    }
}

/*****************************************************************************
 Prototype    : calc_even_parity
 Description  : 偶校验：就是让原有数据序列中（包括你要加上的一位）1的个数为
                偶数
 Input        : u8 value  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static u8 calc_even_parity(u8 value)
{
    u8 count = 0;
    u8 i = 0;

    for (i=0; i < 8; i++)
    {
        if (0x01 == ((value>>i) & 0x01)) // 检测数据中含有1的数目
        {
            count++;
        }
    }
    
    if (0x01 == (count & 0x01)) // 1的数目为奇数
    {
        return 1;
    }
    else // 1的数目为偶数
    {
        return 0;
    }
}

/*****************************************************************************
 Prototype    : Infrared_Decode
 Description  : infrared decode
 Input        : u8 *pCode  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static u8 Infrared_Decode(u8 *pCode)
{
    u16 Pluse = 0;
    u8 get_odd_even_bit = 0;
    u8 result = IRERR_ERROR;

    *pCode = 0;
    
    if (DataGetLength() < DECODE_MAX_BIT_LEN) // 未到达完整一帧的长度,不处理
    {
        return IRERR_NONE;
    }

    while (1)
    {
        if (BUF_OK != DataDequeue(&Pluse))
        {
            break;
        }

        switch (gDecodeState)
        {
            case DECODE_HEAD:
            {
                if (IF_HEADER(Pluse))
                {
                    gDecodeState = DECODE_DATA_BIT0;
                }
            }
                break;
            case DECODE_DATA_BIT0:
            {
                if (IF_LOGIC0(Pluse))
                {
                    *pCode &= ~BIT0;
                    gDecodeState = DECODE_DATA_BIT1;
                }
                else if (IF_LOGIC1(Pluse))
                {
                    *pCode |= BIT0;
                    gDecodeState = DECODE_DATA_BIT1;
                }
                else
                {
                    gDecodeState = DECODE_HEAD;
                }
            }           
                break;
            case DECODE_DATA_BIT1:
            {
                if (IF_LOGIC0(Pluse))
                {
                    *pCode &= ~BIT1;
                    gDecodeState = DECODE_ODD_EVEN;
                }
                else if (IF_LOGIC1(Pluse))
                {
                    *pCode |= BIT1;
                    gDecodeState = DECODE_ODD_EVEN;
                }
                else
                {
                    gDecodeState = DECODE_HEAD;
                }
            }           
                break;
            case DECODE_ODD_EVEN:
                {
                    if (IF_LOGIC0(Pluse))
                    {
                        get_odd_even_bit = 0;
                        result = IRERR_SUCCESS;
                    }
                    else if (IF_LOGIC1(Pluse))
                    {
                        get_odd_even_bit = 1;
                        result = IRERR_SUCCESS;
                    }
                    else
                    {
                        result = IRERR_ERROR;
                    }
                    gDecodeState = DECODE_HEAD;
                    if (IRERR_SUCCESS == result)
                    {
                        if (calc_even_parity(*pCode) == get_odd_even_bit)
                        {
                            return IRERR_SUCCESS;
                        }
                        else
                        {
                            //odd_even bit error
                            return IRERR_ODD_EVEN_ERR;
                        }
                    }
                }           
                break;
            default:
                break;
        }
    }
    return IRERR_ERROR;
}

/*****************************************************************************
 Prototype    : Infrared_GetCurrentTime
 Description  : get current time
 Input        : void  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static u16 Infrared_GetCurrentTime(void)
{
    /*
     * 保留当前系统时间的10ms以下精度，例如 9.999ms
     */
    u16 ms = SysTick_Get() % 10;
    u16 us = SysTick_Get_us();
    u16 res = ms * 1000 + us;
    return res;
}

/*****************************************************************************
 Prototype    : Infrared_SubtractTime
 Description  : get pluse time
 Input        : const u16 new_time  
                const u16 old_time  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static u16 Infrared_SubtractTime(const u16 new_time, const u16 old_time)
{
    s16 time = new_time - old_time;
    if (time < 0)
    {
        time += 10000;  /*new_time溢出, 需要补偿10ms = 10000us */
    }
    return (u16)time;
}

/*****************************************************************************
 Prototype    : Infrared_Delay_us
 Description  : delay us
 Input        : const u16 us  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void Infrared_Delay_us(const u16 us)
{
    u32 set_ms = SysTick_Get() + us / 1000;
    u16 set_us = SysTick_Get_us() + us % 1000;
    
    while (1)
    {
        if (SysTick_Get() >= set_ms)
        {
            if (SysTick_Get_us() >= set_us)
            {
                return;
            }
        }
    }
}

/*****************************************************************************
 Prototype    : Infrared_InterrurptHandler
 Description  : interrupt handler
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void Infrared_InterrurptHandler(void)
{  
    static vu8 IrFlag = 0;
    u16 cur_time = Infrared_GetCurrentTime();
    u16 pulse = Infrared_SubtractTime(cur_time, u16PulseFallingTime);
    
    if (IF_LOGIC0(pulse) || IF_LOGIC1(pulse))
    {
        if (IrFlag != 0) // 没有检测到Head情况下来了正常的逻辑0或1,则补充Head信息
        {
            IrFlag = 0;
            DataEnqueue(HEADER_MID_US);
        }
        DataEnqueue(pulse);
    }
    else if (IF_HEADER(pulse))
    {
        IrFlag = 1;
    }
    else
    {
        IrFlag = 0;
    }
    u16PulseFallingTime = cur_time;
}

/*****************************************************************************
 Prototype    : IrSendOneBit
 Description  : send one bit
 Input        : u16 delay  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void IrSendOneBit(u16 delay)
{
//    IR_SEND_LOW;
    Infrared_Delay_us(delay/2);

    IR_OUT_PWM_ENABLE;
    Infrared_Delay_us(delay/2);
    IR_OUT_PWM_DISABLE;
    
}

/*****************************************************************************
 Prototype    : Infrared_Send
 Description  : infrared send data api
 Input        : u8 keycode  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void Infrared_Send(u8 keycode)
{
    int i = 0;
    u8 send_bit_number = ENCODE_DATA_BITS + 1;  // 5bit + 1bit odd_even
    
    TIM2_Config();

    if (calc_even_parity(keycode))
    {
        keycode |= BIT5;
    }
    
    IrSendOneBit(HEADER_MID_US);
    
    for(i = 0; i < send_bit_number; i++)
    {
        if(((keycode >> i)&0x01) == 0x01)
        {
            IrSendOneBit(LOGIC1_MID_US);
        }
        else
        {
            IrSendOneBit(LOGIC0_MID_US);
        }
    }
    
    TIM2_ForcedOC2Config(TIM2_ForcedAction_Inactive);
    TIM2_DeInit();
    IR_SEND_LOW;
}

/*****************************************************************************
 Prototype    : Infrared_server
 Description  : infrared server
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/28
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void Infrared_server(void)
{
    u8 flag = 0;
    u8 code = 0;
    u8 send_data = 0x15; //0001 0101
    flag = Infrared_Decode(&code);
    if (flag == IRERR_SUCCESS)
    {
        Infrared_Send(send_data);
    }
}

