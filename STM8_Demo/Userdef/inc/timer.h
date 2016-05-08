/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : timer.h
* Function  : timing function
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/08
*********************************************************************************************************/
#ifndef __TIMER_H_
#define __TIMER_H_

#define TIMER_NUM   6

typedef struct  
{
    unsigned char flag;      //1: timer exsit, 0: timer not exsit
    unsigned char stat;      //1:run, 0:pause
    unsigned int  time_cur;  //Currnet time
    unsigned int  time_lit;  //Preset time
    void (*cb_fun)();        //Response Function
}TIMER;


#ifdef _TIMER_C_
#define INTERFACE
#else
#define INTERFACE extern 
#endif


INTERFACE TIMER timer[TIMER_NUM];
INTERFACE u8 TimerTickBeat;
INTERFACE TIMER *TimerKeyScan;
INTERFACE TIMER *TimerSendData;
INTERFACE TIMER  *TimerAnalysia;
INTERFACE TIMER *TimerLongSend;


INTERFACE void Timer_Config(void);
INTERFACE void Timer_Init(void);
INTERFACE void Wtg_Init(void);
INTERFACE void CustomerTimer_Init(void);
INTERFACE void CustomerTimer_Run(TIMER *timer);
INTERFACE void CustomerTimer_Stop(TIMER *timer);
INTERFACE bool GetCustomerTimer_Status(TIMER *timer);
INTERFACE void CustomerTimer_Restart(TIMER *timer);
INTERFACE void CustomerTimer_Reset(TIMER *timer);
INTERFACE void CustomerTimer_TimeChange(TIMER *timer, unsigned int time);
INTERFACE void CustomerTimer_Manage();
INTERFACE void CustomerTimer_Delete(TIMER *timer);
INTERFACE void Create_AllCustomerTimers(void);
INTERFACE void Run_AllCustomerTimers(void);



#endif
