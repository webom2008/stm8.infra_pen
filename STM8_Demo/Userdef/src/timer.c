/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : timer.c
* Function  : timing function
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/08
*********************************************************************************************************/
#include "stm8s.h"
#include "encoder.h"
#include "key.h"
#include "timer.h"
#include "uart.h"

#define _TIMER_C_


#define TIMER_2MS_CNT      2    /* 1ms触发一次 */
#define TIMER_5MS_CNT      5    /* 硬件定时器触发一次中断为1ms，软件定时器为 5*1 = 5ms */
#define TIMER_10MS_CNT     10    /* 10ms触发一次 */
#define TIMER_100MS_CNT    100  /* 软件定时器100ms一次 */
#define TIMER_500MS_CNT    500  /* 软件定时器500ms一次 */
#define TIMER_1000MS_CNT   1000 /*  软件定时器1000ms一次 */

TIMER timer[TIMER_NUM];
u8 TimerTickBeat = 0;         /* 硬件定时器触发标志 */
TIMER *TimerKeyScan;
TIMER *TimerSendData;
TIMER *TimerAnalysia;
TIMER *TimerLongSend;
TIMER *TimerSelfCheck;
TIMER *TimerFeedWtg;

/********************************************************************************
 * 函数名: void Enable_GlobalInterrupt(void)
 * 功  能: 打开全局中断
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Enable_GlobalInterrupt(void)
{
    enableInterrupts();
}
/********************************************************************************
 * 函数名: void Timer_Init(void)
 * 功  能: 硬件和软件定时器初始化
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Timer_Init(void)
{
    Timer_Config();
    CustomerTimer_Init();
}
/********************************************************************************
 * 函数名: void Timer_Config(void)
 * 功  能: 硬件定时器初始化
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Timer_Config(void)
{
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_128,61);//153 =2.5MS   61 = 1ms
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_GenerateEvent(TIM4_EVENTSOURCE_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE); 
    
    TIM4_Cmd(ENABLE);
    
    Enable_GlobalInterrupt();
}
/********************************************************************************
 * 函数名: void Wtg_Init(void)
 * 功  能: 看门狗初始化
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Wtg_Init(void)
{
    /* Enable the IWDG*/
    IWDG_Enable();
    /* Enable the access to the IWDG registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* Fixe IWDG Reset period */
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0xFF);
    /* Refresh IWDG */
    IWDG_ReloadCounter();
}
/********************************************************************************
 * 函数名: void Feed_Wtg(void)
 * 功  能: 喂狗功能
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Feed_Wtg(void)
{
    IWDG_WriteAccessCmd(0xAA);
}
/********************************************************************************
 * 函数名: void CustomerTimer_Init(void)
 * 功  能: 自定义软件定时器初始化
 * 参  数: 无
 * 返  回: 无 
 *******************************************************************************/
void CustomerTimer_Init(void)
{
    unsigned char i;

    for (i=0; i<TIMER_NUM; i++)
    {
        timer[i].flag = 0;          
        timer[i].stat = 0;        
        timer[i].time_cur = 0;    
        timer[i].time_lit = 0;    
        timer[i].cb_fun = 0;    
    }
}

/********************************************************************************
 * 函数名: TIMER *CustomerTimer_Create(unsigned int time, void (*fun)())
 * 功  能: 创建一个定时器
 * 参  数: time-->需要定时的时间，fun-->定时时间到的回调函数
 * 返  回: 定时器指针
 *******************************************************************************/
TIMER *CustomerTimer_Create(unsigned int time, void (*fun)())
{
    unsigned char i;

    for (i=0; i<TIMER_NUM; i++)
    {
        if (!timer[i].flag)
        {
            timer[i].flag = 1;
            timer[i].stat = 0;
            timer[i].time_cur = 0;
            timer[i].time_lit = time;
            timer[i].cb_fun = fun;

            return (timer + i);
        }
    }

    return 0;
}

/********************************************************************************
 * 函数名: void CustomerTimer_Run(TIMER *timer)
 * 功  能: 运行指定的定时器
 * 参  数: timer->需要启动的定时器
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Run(TIMER *timer)
{
    timer->stat = 1;
}

/********************************************************************************
 * 函数名: void CustomerTimer_Stop(TIMER *timer)
 * 功  能: 停止指定的定时器
 * 参  数: timer->需要启动的定时器
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Stop(TIMER *timer)
{
    timer->stat = 0;
}

/********************************************************************************
 * 函数名: bool GetCustomerTimer_Status(TIMER *timer)
 * 功  能: 获取指定定时器状态
 * 参  数: timer->需要启动的定时器
 * 返  回: 定时器状态，TRUE ->定时器存在，FALSE ->定时器不存在。
 *******************************************************************************/
bool GetCustomerTimer_Status(TIMER *timer)
{
    if(timer->stat == 1)
        return TRUE;
    else
        return FALSE;
}

/********************************************************************************
 * 函数名: void CustomerTimer_Restart(TIMER *timer)
 * 功  能: 重启指定定时器
 * 参  数: timer->需要启动的定时器
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Restart(TIMER *timer)
{
    timer->time_cur = 0;
    timer->stat = 1;
}

/********************************************************************************
 * 函数名: void CustomerTimer_Reset(TIMER *timer)
 * 功  能: 复位指定定时器
 * 参  数: timer->需要启动的定时器
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Reset(TIMER *timer)
{
    timer->time_cur = 0;
    timer->stat = 0;
}

/********************************************************************************
 * 函数名: void CustomerTimer_TimeChange(TIMER *timer, unsigned int time)
 * 功  能: 重新设置定时器的定时时间
 * 参  数: timer->需要修改的定时器，time->重设的定时时间
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_TimeChange(TIMER *timer, unsigned int time)
{
    timer->time_lit = time;
}

/********************************************************************************
 * 函数名: void CustomerTimer_Manage(void)
 * 功  能: 管理所有定时器，如有定时超时，则执行其处理函数
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Manage(void)
{
    unsigned char i;

    for (i=0; i<TIMER_NUM; i++)
    {
        if (timer[i].flag)
        {
            if (timer[i].stat)
            {
                if (++timer[i].time_cur >= timer[i].time_lit)
                {
                    timer[i].time_cur = 0;
                    (*timer[i].cb_fun)();
                }
            }
        }
    }
}

/********************************************************************************
 * 函数名: void CustomerTimer_Delete(TIMER *timer)
 * 功  能: 删除指定定时器
 * 参  数: timer->需要删除的定时器
 * 返  回: 无
 *******************************************************************************/
void CustomerTimer_Delete(TIMER *timer)
{
    timer->flag = 0;
    timer->stat = 0;
    timer->time_cur = 0;
    timer->time_lit = 0;
    timer->cb_fun = 0;	
}

extern void Transmit_DataPackToMPU(void);
extern void LongPress_TransmitData(void);
/********************************************************************************
 * 函数名: void Create_AllCustomerTimers(void)
 * 功  能: 创建所有定时器
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Create_AllCustomerTimers(void)
{
    TimerKeyScan  = CustomerTimer_Create(TIMER_5MS_CNT, Keys_Scan);
    TimerSendData = CustomerTimer_Create(TIMER_2MS_CNT, Transmit_DataPackToMPU);
    TimerAnalysia = CustomerTimer_Create(TIMER_10MS_CNT, UartData_Analysia);
    TimerLongSend = CustomerTimer_Create(TIMER_100MS_CNT, LongPress_TransmitData);
    TimerSelfCheck= CustomerTimer_Create(TIMER_1000MS_CNT, SelfCheck_TransmitData);
    TimerFeedWtg  = CustomerTimer_Create(TIMER_500MS_CNT, Feed_Wtg); 
}
/********************************************************************************
 * 函数名: void Run_AllCustomerTimers(void)
 * 功  能: 运行所有定时器
 * 参  数: 无
 * 返  回: 无
 *******************************************************************************/
void Run_AllCustomerTimers(void)
{
    CustomerTimer_Run(TimerKeyScan);
    CustomerTimer_Run(TimerSendData);
	CustomerTimer_Run(TimerAnalysia);
    CustomerTimer_Run(TimerLongSend);
	CustomerTimer_Run(TimerSelfCheck);
    CustomerTimer_Run(TimerFeedWtg);
}


#undef _TIMER_C_

