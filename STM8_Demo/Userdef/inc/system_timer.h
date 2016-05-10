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

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void SysTick_Init(void);
extern void SysTick_Start(void);
extern void SysTick_Incremental(void);  
extern u32 SysTick_Get(void);
extern s32 SysTick_Compare(u32 u32Tick1, u32 u32Tick2);
extern u32 IsOnTime(const u32 u32Target);
extern u32 IsOverTime(const u32 u32Base, const u32 u32Duration);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SYSTEM_TIMER_H__ */
