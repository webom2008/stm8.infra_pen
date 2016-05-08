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


#define TIMER_2MS_CNT      2    /* 1ms����һ�� */
#define TIMER_5MS_CNT      5    /* Ӳ����ʱ������һ���ж�Ϊ1ms�������ʱ��Ϊ 5*1 = 5ms */
#define TIMER_10MS_CNT     10    /* 10ms����һ�� */
#define TIMER_100MS_CNT    100  /* �����ʱ��100msһ�� */
#define TIMER_500MS_CNT    500  /* �����ʱ��500msһ�� */
#define TIMER_1000MS_CNT   1000 /*  �����ʱ��1000msһ�� */

TIMER timer[TIMER_NUM];
u8 TimerTickBeat = 0;         /* Ӳ����ʱ��������־ */
TIMER *TimerKeyScan;
TIMER *TimerSendData;
TIMER *TimerAnalysia;
TIMER *TimerLongSend;
TIMER *TimerSelfCheck;
TIMER *TimerFeedWtg;

/********************************************************************************
 * ������: void Enable_GlobalInterrupt(void)
 * ��  ��: ��ȫ���ж�
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Enable_GlobalInterrupt(void)
{
    enableInterrupts();
}
/********************************************************************************
 * ������: void Timer_Init(void)
 * ��  ��: Ӳ���������ʱ����ʼ��
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Timer_Init(void)
{
    Timer_Config();
    CustomerTimer_Init();
}
/********************************************************************************
 * ������: void Timer_Config(void)
 * ��  ��: Ӳ����ʱ����ʼ��
 * ��  ��: ��
 * ��  ��: ��
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
 * ������: void Wtg_Init(void)
 * ��  ��: ���Ź���ʼ��
 * ��  ��: ��
 * ��  ��: ��
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
 * ������: void Feed_Wtg(void)
 * ��  ��: ι������
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Feed_Wtg(void)
{
    IWDG_WriteAccessCmd(0xAA);
}
/********************************************************************************
 * ������: void CustomerTimer_Init(void)
 * ��  ��: �Զ��������ʱ����ʼ��
 * ��  ��: ��
 * ��  ��: �� 
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
 * ������: TIMER *CustomerTimer_Create(unsigned int time, void (*fun)())
 * ��  ��: ����һ����ʱ��
 * ��  ��: time-->��Ҫ��ʱ��ʱ�䣬fun-->��ʱʱ�䵽�Ļص�����
 * ��  ��: ��ʱ��ָ��
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
 * ������: void CustomerTimer_Run(TIMER *timer)
 * ��  ��: ����ָ���Ķ�ʱ��
 * ��  ��: timer->��Ҫ�����Ķ�ʱ��
 * ��  ��: ��
 *******************************************************************************/
void CustomerTimer_Run(TIMER *timer)
{
    timer->stat = 1;
}

/********************************************************************************
 * ������: void CustomerTimer_Stop(TIMER *timer)
 * ��  ��: ָֹͣ���Ķ�ʱ��
 * ��  ��: timer->��Ҫ�����Ķ�ʱ��
 * ��  ��: ��
 *******************************************************************************/
void CustomerTimer_Stop(TIMER *timer)
{
    timer->stat = 0;
}

/********************************************************************************
 * ������: bool GetCustomerTimer_Status(TIMER *timer)
 * ��  ��: ��ȡָ����ʱ��״̬
 * ��  ��: timer->��Ҫ�����Ķ�ʱ��
 * ��  ��: ��ʱ��״̬��TRUE ->��ʱ�����ڣ�FALSE ->��ʱ�������ڡ�
 *******************************************************************************/
bool GetCustomerTimer_Status(TIMER *timer)
{
    if(timer->stat == 1)
        return TRUE;
    else
        return FALSE;
}

/********************************************************************************
 * ������: void CustomerTimer_Restart(TIMER *timer)
 * ��  ��: ����ָ����ʱ��
 * ��  ��: timer->��Ҫ�����Ķ�ʱ��
 * ��  ��: ��
 *******************************************************************************/
void CustomerTimer_Restart(TIMER *timer)
{
    timer->time_cur = 0;
    timer->stat = 1;
}

/********************************************************************************
 * ������: void CustomerTimer_Reset(TIMER *timer)
 * ��  ��: ��λָ����ʱ��
 * ��  ��: timer->��Ҫ�����Ķ�ʱ��
 * ��  ��: ��
 *******************************************************************************/
void CustomerTimer_Reset(TIMER *timer)
{
    timer->time_cur = 0;
    timer->stat = 0;
}

/********************************************************************************
 * ������: void CustomerTimer_TimeChange(TIMER *timer, unsigned int time)
 * ��  ��: �������ö�ʱ���Ķ�ʱʱ��
 * ��  ��: timer->��Ҫ�޸ĵĶ�ʱ����time->����Ķ�ʱʱ��
 * ��  ��: ��
 *******************************************************************************/
void CustomerTimer_TimeChange(TIMER *timer, unsigned int time)
{
    timer->time_lit = time;
}

/********************************************************************************
 * ������: void CustomerTimer_Manage(void)
 * ��  ��: �������ж�ʱ�������ж�ʱ��ʱ����ִ���䴦����
 * ��  ��: ��
 * ��  ��: ��
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
 * ������: void CustomerTimer_Delete(TIMER *timer)
 * ��  ��: ɾ��ָ����ʱ��
 * ��  ��: timer->��Ҫɾ���Ķ�ʱ��
 * ��  ��: ��
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
 * ������: void Create_AllCustomerTimers(void)
 * ��  ��: �������ж�ʱ��
 * ��  ��: ��
 * ��  ��: ��
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
 * ������: void Run_AllCustomerTimers(void)
 * ��  ��: �������ж�ʱ��
 * ��  ��: ��
 * ��  ��: ��
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

