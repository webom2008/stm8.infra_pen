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
static volatile u32 u32SystemTick;

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

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

void SysTick_Init(void)
{
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
//    enableInterrupts();
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
    u32SystemTick = 0;
    TIM3_Cmd(ENABLE);
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
    u32SystemTick++;
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
    return u32SystemTick;
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

