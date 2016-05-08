/**
  ******************************************************************************
  * @file    ir_driver.c
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

#include "ir_driver.h"


extern uint32_t g_Timer_Count;
extern uint32_t g_Send_Count;

#define NEW_NEC_IR_DECODE


IrDev IR;

#ifdef NEW_NEC_IR_DECODE
vu32 IrPulseTime = 0;
vu32 PulseFallingTime = 0;
vu8 LogicIndex = 0;
vu8 DecodeIndex = 0;
IrNecState IrState = IR_NEC_NONE;
DecodeNecState NecState = DECODE_NEC_NONE;
IrDev NecDev;

#define IR_ITEM_NUM                     64
#define IR_ITEM_SIZE                    4
QUEUE_TYPE g_IrQueue;
QUEUE_TYPE *pIrQueue = &g_IrQueue;

//extern void TIM1_Config(void);
extern void TIM2_Config(void);

static void Delay(__IO uint16_t nCount);
static void IrSendHigh(void);
static void IrSendLow(void);
static void IrSendStart(void);
static void IrSendStop(void);

/* IrQueue */
u8 Ir_EnQueue(u32 data)
{
    u8 buf[IR_ITEM_SIZE];
//    if (pIrQueue == NULL)
//    {
//        pIrQueue = InitQueue(IR_ITEM_NUM, IR_ITEM_SIZE);
//    }
    buf[0] = (u8)(data>>24);
    buf[1] = (u8)(data>>16);
    buf[2] = (u8)(data>>8);
    buf[3] = (u8)(data);

    return EnQueue(pIrQueue, buf);
}

u8 Ir_DeQueue(u32 *data)
{
    u8 buf[IR_ITEM_SIZE];
    u8 result = 0;
//    if (pIrQueue == NULL)
//    {
//        pIrQueue = InitQueue(IR_ITEM_NUM, IR_ITEM_SIZE);
//    }

    result = DeQueue(pIrQueue, buf);
    if (result == Q_SUCCESS)
    {
        u8 i = 0;
        *data = 0;
        for(i=0; i<4; i++)
        {
            *data <<= 8;
            *data |= buf[i];
        }
    }

    return result;
}

void NecIrDetect(u32 Pluse)
{
    static vu8 IrFlag = 0;
    
    if (IF_LOGIC0(Pluse) || IF_LOGIC1(Pluse))
    {
        if (IrFlag != 0)
        {
            IrFlag = 0;
            Ir_EnQueue(HEADER_MID);  
        }

        Ir_EnQueue(Pluse);
    }
    else if (IF_HEADER(Pluse))
    {
        IrFlag = 1;
    }
    else if (IF_REPEAT(Pluse))
    {
        IrFlag = 0;
        Ir_EnQueue(Pluse);
    }
    else
    {
        IrFlag = 0;
    }

}


u8 IrNecDecode(IrDev *Dev)
{
    u32 Pluse = 0;
    u8 result = IRERR_NONE;
    
    if (Ir_DeQueue(&Pluse) != Q_SUCCESS)
    {
        return result;
    }
    
    switch (NecState)
    {
        case DECODE_NEC_CUSTOM:
            {
                if (IF_LOGIC0(Pluse))
                {
                    NecDev.customecode &= ~(BIT0<<DecodeIndex);
                }
                else if (IF_LOGIC1(Pluse))
                {
                    NecDev.customecode |= (BIT0<<DecodeIndex);
                }
                else
                {
                    NecState = DECODE_NEC_NONE;
                    result = IRERR_ERROR;
                }
                DecodeIndex++;
                if (DecodeIndex>=16)
                {
                    NecState = DECODE_NEC_KEY;
                    DecodeIndex = 0;
                }
            }
            break;
        case DECODE_NEC_KEY:
            {
                if (IF_LOGIC0(Pluse))
                {
                    NecDev.keycode &= ~(BIT0<<DecodeIndex);
                }
                else if (IF_LOGIC1(Pluse))
                {
                    NecDev.keycode |= (BIT0<<DecodeIndex);
                }
                else
                {
                    NecState = DECODE_NEC_NONE;
                    result = IRERR_ERROR;
                }
                
                DecodeIndex++;
                if (DecodeIndex>=8)
                {
                    NecState = DECODE_NEC_KEYN;
                    DecodeIndex = 0;
                }
            }           
            break;
        case DECODE_NEC_KEYN:
            {
                if (IF_LOGIC0(Pluse))
                {
                    NecDev.keycodeN &= ~(BIT0<<DecodeIndex);
                }
                else if (IF_LOGIC1(Pluse))
                {
                    NecDev.keycodeN |= (BIT0<<DecodeIndex);
                }
                else
                {
                    NecState = DECODE_NEC_NONE;
                    result = IRERR_ERROR;
                }
                
                DecodeIndex++;
                if (DecodeIndex>=8)
                {
                    if ((NecDev.keycode^NecDev.keycodeN) == 0xFF)
                    {
                        Dev->customecode = ((NecDev.customecode >> 8) & 0x00FF) | ((NecDev.customecode << 8) & 0xFF00);
                        Dev->keycode = NecDev.keycode;
                        Dev->keycodeN = NecDev.keycodeN;
                        result = IRERR_SUCCESS;
                    }
                    NecState = DECODE_NEC_REPEAT;
                    DecodeIndex = 0;
                }
            }           
            break;
        case DECODE_NEC_REPEAT:
            {
                if (IF_HEADER(Pluse))
                {
                    NecState = DECODE_NEC_CUSTOM;
                    DecodeIndex = 0;
                    NecDev.customecode = 0;
                    NecDev.keycode = 0;
                    NecDev.keycodeN = 0;
                    NecDev.repeat = 0;
                }
                else if (IF_REPEAT(Pluse))
                {
                    NecDev.repeat++;
                    Dev->customecode = (NecDev.customecode<<8)|(NecDev.customecode>>8);
                    Dev->keycode = NecDev.keycode;
                    Dev->keycodeN = NecDev.keycodeN;
                    Dev->repeat = NecDev.repeat;
                    result = IRERR_REPEAT;
                }
                else
                {
                    NecState = DECODE_NEC_NONE;
                }
            }
            break;
        case DECODE_NEC_NONE:
            {
                if (IF_HEADER(Pluse))
                {
                    NecState = DECODE_NEC_CUSTOM;
                    DecodeIndex = 0;
                    NecDev.customecode = 0;
                    NecDev.keycode = 0xFF;
                    NecDev.keycodeN = 0;
                    NecDev.repeat = 0;
                }
            }
            break;
        default:
            break;
    }

    return result;
}

void IrNecIsr(void)
{  
    NecIrDetect(g_Timer_Count-PulseFallingTime);
    PulseFallingTime = g_Timer_Count;
}

//560us+1.68ms
static void IrSendHigh(void)
{
    uint32_t end_count = 0;
    
    IR_OUT_PWM_ENABLE;
    end_count = g_Timer_Count + 9;
    while(g_Timer_Count < end_count);
    IR_OUT_PWM_DISABLE;
    
//    IR_SEND_LOW;
    end_count = g_Timer_Count + 30;
    while(g_Timer_Count < end_count);
}

//560us+560ms
static void IrSendLow(void)
{
    uint32_t end_count = 0;
    
    IR_OUT_PWM_ENABLE;
    end_count = g_Timer_Count + 9;
    while(g_Timer_Count < end_count);
    IR_OUT_PWM_DISABLE;
    
//    IR_SEND_LOW;
    end_count = g_Timer_Count + 9;
    while(g_Timer_Count < end_count);
}

static void IrSendStart(void)
{
    uint32_t end_count = 0;
    
    IR_OUT_PWM_ENABLE;
    end_count = g_Timer_Count + 180;  //346*26us = 9ms
    while(g_Timer_Count < end_count);
    IR_OUT_PWM_DISABLE;
    
//    IR_SEND_LOW;
    end_count = g_Timer_Count + 90;   //173*26us = 4.5ms
    while(g_Timer_Count < end_count);
}

static void IrSendStop(void)
{
    uint32_t end_count = 0;
    
    IR_OUT_PWM_ENABLE;
    end_count = g_Timer_Count + 9;
    while(g_Timer_Count < end_count);
    IR_OUT_PWM_DISABLE;
}

void IrNECSend(uint16_t customecode, uint8_t keycode)
{
    uint8_t Data[4] = {0};
    uint8_t i = 0, j = 0;
    
    TIM2_Config();
    
    Data[0] = (customecode >> 8) & 0xFF;
    Data[1] = customecode & 0x00FF;
    Data[2] = keycode;
    Data[3] = ~keycode;
    IrSendStart();
    
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 8; j++)
        {
            if(((Data[i] >> j)&0x01) == 0x01)
            {
                IrSendHigh();
            }
            else
            {
                IrSendLow();
            }
        }
    }
    IrSendStop();  //must have stop signal.
    
    TIM2_Cmd(DISABLE);
    TIM2_ForcedOC2Config(TIM2_ForcedAction_Inactive);
    TIM2_DeInit();
    IR_SEND_LOW;
}

void IrNecTest(void)
{
    u8 flag = 0;
    IrDev TestNecDev;
    flag = IrNecDecode(&TestNecDev);
    if (flag == IRERR_SUCCESS || flag == IRERR_REPEAT)
    {
        IrNECSend(TestNecDev.customecode, TestNecDev.keycode);
        //GPIO_ToggleBits(GPIOE, GPIO_Pin_7);
    }
}
#endif


#ifdef USE_IR_RC5
//#define RC5_LOG
vu32 Rc5DecodeOutTime = 0;
vu32 Rc5PulseTime = 0;
vu16 Rc5DecodeLogic = 0;
vu8 Rc5DecodeIndex = 0;
DecodeRC5State Rc5DecodeState = DECODE_RC5_NONE;
vu32 Rc5PulseFallingTime = 0;
u32 Rc5DataBuf[RC5_DATA_BUF_LEN] = {0};
u32 Rc5DecodeBuf[RC5_DATA_BUF_LEN] = {0};
vu8 Rc5DataIndex = 0;
vu32 Rc5DataOutTime = 0;

static u32 IrRc5DataDiff(u32 Data1, u32 Data2);
static u8 IrRc5SortData(u32 *Buf, u8 Len);
static u8 IrRc5DecodeKey(IrDev *Dev);
static u8 IrRc5GetKey(IrDev *Dev, u32 Pluse);


void IrRc5Isr(void)
{       
    Rc5IrDetect(msAPI_Timer3_GetTime()-Rc5PulseFallingTime);
    Rc5PulseFallingTime = msAPI_Timer3_GetTime();
}

void Rc5IrDetect(u32 Pluse)
{
    if (IF_RC5_LEGAL(Pluse))
    {
        Rc5DataIndex++;
        if (Rc5DataIndex >= RC5_DATA_BUF_LEN)
        {
            Rc5DataIndex = RC5_DATA_BUF_LEN - 1;
        }
        Rc5DataBuf[Rc5DataIndex] = Pluse;    
        Rc5DataOutTime = msAPI_Timer_GetTime() + RC5_DATA_OUT_TIME;
    }
}

u8 DefifoData(u32 *Buf)
{
    if (msAPI_Timer_GetTime() > Rc5DataOutTime)
    {
        u8 i = 0;
        u8 TempIndex = Rc5DataIndex;
        
        for(i=0; i<Rc5DataIndex;i++)
        {
            Buf[i] = Rc5DataBuf[i+1];
            Rc5DataBuf[i+1] = 0;
        }
        Rc5DataOutTime = U32_MAX;
        Rc5DataIndex = 0;
        return TempIndex;
    }
    else
    {
        return 0;
    } 
}

u8 IrRc5Decode(IrDev *Dev)
{
    u8 Result = IRERR_NONE;
    u8 DataLen = DefifoData(Rc5DecodeBuf);
    
    if (DataLen < RC5_DATA_MIN_LEN)
    {
        return Result;
    }
    
    #ifdef RC5_LOG
    {
        u8 i = 0;
        u32 test = 0;
        USBPrintf("\r\nYuan:"); 
        for (i=0; i<DataLen; i++)
        {
            test += Rc5DecodeBuf[i];
            USBPrintf("%d, ",Rc5DecodeBuf[i]); 
        }
        USBPrintf("Sum=%d\r\n",test);
    }
    #endif

    IrRc5SortData(Rc5DecodeBuf,DataLen);
    
    Result = IrRc5DecodeKey(Dev);
    return Result;
}

u32 IrRc5DataDiff(u32 Data1, u32 Data2)
{
    if (Data1>Data2)
    {
        return (Data1-Data2);
    }
    else
    {
        return (Data2-Data1);
    }
}

u8 IrRc5SortData(u32 *Buf, u8 Len)
{
    u8  i = 0;
    u8  j = 0;
    u32 nextData = 0;
    u32 data = 0;
    vs32 diff = 0; 
    u32 tempBuf[RC5_DATA_BUF_LEN];
    
    
    for (i=0; i<Len; i++)
    {
        data = data + Buf[Len-i-1] + diff;

        if (i != Len-1)
        {
            nextData = Buf[Len-i-2];
        }
        else
        {
            nextData = 0;
            if (!IF_RC5_PLUSE2(data)
            && !IF_RC5_PLUSE3(data)
            && !IF_RC5_PLUSE4(data))
            {
                break;
            }
        }
        
        if (IF_RC5_PLUSE2(data)
        || IF_RC5_PLUSE3(data)
        || IF_RC5_PLUSE4(data))
        {
            tempBuf[j] = data;
            j++;
            diff = 0;
            data = 0;
        }
        else if (IF_RC5_LOW(data))
        {

        }
        else if (IF_RC5_PLUSE3(data+nextData))
        {
            
        }
        else
        {
            u16 pluse2Diff = IrRc5DataDiff(data,RC5_PLUSE2_MID);
            u16 pluse3Diff = IrRc5DataDiff(data,RC5_PLUSE3_MID);
            u16 pluse4Diff = IrRc5DataDiff(data,RC5_PLUSE4_MID);

            if (pluse2Diff<=pluse3Diff && pluse2Diff<=pluse4Diff)
            {
                diff = data - RC5_PLUSE2_MID;
                if (IF_RC5_PLUSE2(diff+nextData)
                 || IF_RC5_PLUSE3(diff+nextData)
                 || IF_RC5_PLUSE4(diff+nextData))
                {
                    tempBuf[j] = RC5_PLUSE2_MID; 
                    j++;
                }
                else
                {
                    diff = 0;
                }
            }
            else if (pluse3Diff<=pluse2Diff && pluse3Diff<=pluse4Diff)
            {
                diff = data - RC5_PLUSE3_MID;
                if (IF_RC5_PLUSE2(diff+nextData)
                 || IF_RC5_PLUSE3(diff+nextData)
                 || IF_RC5_PLUSE4(diff+nextData))
                {
                    tempBuf[j] = RC5_PLUSE3_MID; 
                    j++;
                }
                else
                {
                    diff = 0;
                }
            }
            else if (pluse4Diff<=pluse2Diff && pluse4Diff<=pluse3Diff)
            {
                diff = data - RC5_PLUSE4_MID;
                if (IF_RC5_PLUSE2(diff+nextData)
                 || IF_RC5_PLUSE3(diff+nextData)
                 || IF_RC5_PLUSE4(diff+nextData))
                {
                    tempBuf[j] = RC5_PLUSE4_MID; 
                    j++;
                }
                else
                {
                    diff = 0;
                }
            }
            data = 0;
        }
    }
    
    #ifdef RC5_LOG
    USBPrintf("\r\nSort:");
    #endif
    for (i=0; i<j; i++)
    {
        Ir_EnQueue(tempBuf[j-i-1]);
        #ifdef RC5_LOG
        {
            USBPrintf("%d, ",tempBuf[j-i-1]); 
        }
        #endif
    }
    #ifdef RC5_LOG
    USBPrintf("\r\n");
    #endif
    
    return j;
}

u8 IrRc5DecodeKey(IrDev *Dev)
{
    u32 Pluse = 0;
    u8 result = IRERR_NONE;
    
    while (Ir_DeQueue(&Pluse) == ERR_SUCCESS)
    {
        result = IrRc5GetKey(Dev, Pluse);
        #ifdef RC5_LOG
        USBPrintf("Index=%d, Pluse=%d \r\n", Rc5DecodeIndex, Pluse);
        #endif
    }

    if (Rc5DecodeState == DECODE_RC5_STOP)
    {
            Rc5DecodeLogic <<= 1;
            Dev->customecode = (Rc5DecodeLogic>>6)&0x1F;
            Dev->keycode = (((~Rc5DecodeLogic)&BIT12)>>6) | (Rc5DecodeLogic&0x3F);
            Dev->keycodeN = ~Dev->keycode;
            Dev->protocols = RC5_PROTOCOL;
            result = IRERR_SUCCESS;     
    } 
    
    Rc5DecodeState = DECODE_RC5_NONE;
    
    return result;
}

u8 IrRc5GetKey(IrDev *Dev, u32 Pluse)
{   
    u8 result = IRERR_NONE;
    switch (Rc5DecodeState)
    {
        case DECODE_RC5_NONE:
        {
            Rc5DecodeLogic = BIT0;
            Rc5DecodeIndex = 1;

            Rc5DecodeState = DECODE_RC5_START;
            if (IF_RC5_PLUSE4(Pluse))
            {
                Rc5DecodeIndex += 2;
                Rc5DecodeLogic = (Rc5DecodeLogic<<2) | BIT0;            
            }
            else if (IF_RC5_PLUSE3(Pluse))
            {
                Rc5DecodeIndex += 1;
                Rc5DecodeLogic = (Rc5DecodeLogic<<1) | ((~Rc5DecodeLogic)&BIT0);
            }
            else if (IF_RC5_PLUSE2(Pluse))
            {
                Rc5DecodeIndex += 1;
                Rc5DecodeLogic = (Rc5DecodeLogic<<1) | (Rc5DecodeLogic&BIT0);
            }
            else
            {
                Rc5DecodeState = DECODE_RC5_NONE;
            }
        }
        break;
        case DECODE_RC5_START:
        {
            if (IF_RC5_PLUSE4(Pluse))
            {
                Rc5DecodeIndex += 2;
                Rc5DecodeLogic = (Rc5DecodeLogic<<2) | BIT0;
            }
            else if (IF_RC5_PLUSE3(Pluse))
            {
                if (Rc5DecodeLogic&BIT0)
                {
                    Rc5DecodeIndex += 1;
                    Rc5DecodeLogic = (Rc5DecodeLogic<<1) | ((~Rc5DecodeLogic)&BIT0);
                }
                else
                {
                    Rc5DecodeIndex += 2;
                    Rc5DecodeLogic = (Rc5DecodeLogic<<2) | ((~Rc5DecodeLogic)&BIT0);
                }
            }
            else if (IF_RC5_PLUSE2(Pluse))
            {
                Rc5DecodeIndex += 1;
                Rc5DecodeLogic = (Rc5DecodeLogic<<1) | (Rc5DecodeLogic&BIT0);
            }
            else
            {
                Rc5DecodeState = DECODE_RC5_NONE;
            }

            if (Rc5DecodeIndex >= 14)
            {
                Dev->customecode = (Rc5DecodeLogic>>6)&0x1F;
                Dev->keycode = (((~Rc5DecodeLogic)&BIT12)>>6) | (Rc5DecodeLogic&0x3F);
                Dev->keycodeN = ~Dev->keycode;
                Dev->protocols = RC5_PROTOCOL;
                result = IRERR_SUCCESS;
                Rc5DecodeState = DECODE_RC5_NONE;
            }
            else if (Rc5DecodeIndex == 13)
            {
                Rc5DecodeState = DECODE_RC5_STOP;
            }
        }
        break;
        case DECODE_RC5_STOP:
        {
            if (IF_RC5_PLUSE2(Pluse))
            {
                Rc5DecodeIndex += 1;
                Rc5DecodeLogic = (Rc5DecodeLogic<<1) | (Rc5DecodeLogic&BIT0);
            }
            else 
            {
                Rc5DecodeIndex += 1;
                Rc5DecodeLogic = (Rc5DecodeLogic<<1) | ((~Rc5DecodeLogic)&BIT0);
            }

            if (Rc5DecodeIndex >= 14)
            {
                Dev->customecode = (Rc5DecodeLogic>>6)&0x1F;
                Dev->keycode = (((~Rc5DecodeLogic)&BIT12)>>6) | (Rc5DecodeLogic&0x3F);
                Dev->keycodeN = ~Dev->keycode;
                Dev->protocols = RC5_PROTOCOL;
                result = IRERR_SUCCESS;  
            }
            Rc5DecodeState = DECODE_RC5_NONE;
        }
        break;
        default:
        {
            Rc5DecodeState = DECODE_RC5_NONE;
        }
        break;
    }
    return result;
}

void IrRc5Reinit(void)
{
    Rc5DataOutTime = U32_MAX;
    Rc5DataIndex = 0;
}

#endif


/************************ (C) COPYRIGHT CVTE ************************/

