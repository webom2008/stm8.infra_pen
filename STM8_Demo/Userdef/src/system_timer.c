/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : system_timer.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/9
  Last Modified :
  Description   : system_timer.c header file
  Function List :
  History       :
  1.Date        : 2016/5/9
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
static volatile u32 u32SystemTick_ms;
static volatile u16 u32SystemTick_us;
/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define SYSTEMTIMER_TICK_US         10 /* used for timer count increase*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

void SysTick_Init(void)
{
#ifdef TIME3_FOR_SYSTEM_TIMER
/*
       ===================================================================      
              TIM3 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM3 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE) function.
        
       2. Call TIM3_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM3_ITConfig(TIM3_IT_Update) 
          
       5. Call the TIM3_Cmd(ENABLE) function to enable the TIM3 counter.
       
*/
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
    TIM3_DeInit();
    TIM3_TimeBaseInit(TIM3_Prescaler_8, TIM3_CounterMode_Up, 999); /* 8MHz peripheral Clock / 8 = 1MHz, 1000 = 1ms*/
    TIM3_PrescalerConfig(TIM3_Prescaler_8, TIM3_PSCReloadMode_Immediate);
    TIM3_ARRPreloadConfig(ENABLE);
    TIM3_ITConfig(TIM3_IT_Update, ENABLE);
#endif

#ifdef TIME4_FOR_SYSTEM_TIMER
/*

       ===================================================================      
              TIM4 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM4 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE) function.
        
       2. Call TIM4_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM4_ITConfig(TIM4_IT_Update) 
          
       5. Call the TIM4_Cmd(ENABLE) function to enable the TIM4 counter.
       
       System Clock = 8MHz
       timer 4 freq. = 100KHz, 10us
       ==============Detail for TIMER4=============
         SCLK = 8MHz, TIM4CLK = 8MHz / 8 = 1MHz
         freq. = 1MHz / (9+1) = 100KHz
         period = 1/20KHz = 0.05ms = 50us
       ============================================
*/
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_Prescaler_8, 9);
    TIM4_PrescalerConfig(TIM4_Prescaler_8, TIM4_PSCReloadMode_Immediate);
    TIM4_ARRPreloadConfig(ENABLE);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
#endif
}

/*****************************************************************************
 Prototype    : SysTick_Start
 Description  : c
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void SysTick_Start(void)
{
    u32SystemTick_ms = 0;
    u32SystemTick_us = 0;
#ifdef TIME3_FOR_SYSTEM_TIMER
    TIM3_Cmd(ENABLE);
#endif
#ifdef TIME4_FOR_SYSTEM_TIMER
    TIM4_Cmd(ENABLE);
#endif
}

/*****************************************************************************
 Prototype    : SysTick_Incremental
 Description  : 
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void SysTick_Incremental(void)
{
    u32SystemTick_us += SYSTEMTIMER_TICK_US;
    if (u32SystemTick_us >= 1000)
    {
        u32SystemTick_us = 0;
        u32SystemTick_ms++;
    }
}

/*****************************************************************************
 Prototype    : SysTick_Get
 Description  : get stystick
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
u32 SysTick_Get(void)
{
    return u32SystemTick_ms;
}

/*****************************************************************************
 Prototype    : SysTick_Get_us
 Description  : get system tick us
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
u32 SysTick_Get_us(void)
{
    return u32SystemTick_us;
}

/*****************************************************************************
 Prototype    : SysTick_Compare
 Description  : compare
 Input        : u32 u32Tick1  
                u32 u32Tick2  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
s32 SysTick_Compare(u32 u32Tick1, u32 u32Tick2) // Interval must less than 0x80000000
{
    if(u32Tick1 == u32Tick2)
    {
        return 0;       // ==
    }
    else if(u32Tick1 < u32Tick2)
    {
        if((u32Tick2 - u32Tick1) < 0x80000000)
        {
            return -1;  // <
        }
        else            // Tick Out
        {
            return 1;   // >
        }
    }
    else    // u32Tick1 > u32Tick2
    {
        if((u32Tick1 - u32Tick2) < 0x80000000)
        {
            return 1;   // >
        }
        else            // Tick Out
        {
            return -1;  // <
        }
    }
}

/*****************************************************************************
 Prototype    : IsOnTime
 Description  : 
 Input        : const u32 u32Target  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
u32 IsOnTime(const u32 u32Target)
{
    if(SysTick_Compare(SysTick_Get(), u32Target) == -1) 
    {
        return 0;
    }

    return 1;
}

/*****************************************************************************
 Prototype    : IsOverTime
 Description  : 
 Input        : const u32 u32Base      
                const u32 u32Duration  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
u32 IsOverTime(const u32 u32Base, const u32 u32Duration)    
{
    u32 u32Target;

    u32Target = u32Base + u32Duration;

    if(SysTick_Compare(SysTick_Get(), u32Target) == 1)
    {
        return 1;
    }

    return 0;
}

