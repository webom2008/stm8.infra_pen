/**
  ******************************************************************************
  * @file    queue.h
  * @author  Origin
  * @version V1.0.0
  * @date    11-December-2014
  * @brief   Queue Functions
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

#ifndef __QUEUE_H
#define __QUEUE_H

#include "stm8l15x.h"
#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL            0
#endif

#define Q_NULL          0
#define Q_SUCCESS       1
#define Q_FULL          2
#define Q_EMPTY         3


typedef struct QueueType
{
    u8  QueueBuffer[64 * 4];
    u32 QueueHead;
    u32 QueueTail;
    u32 QueueCounter;
    u32 QueueItemNum;
    u32 QueueItemSize;
    u32 QueueBufferSize;
}
QUEUE_TYPE, *QUEUE_POINTER;

extern QUEUE_TYPE *InitQueue(u32 ItemNum, u32 ItemSize);
extern u8 ClearQueue(QUEUE_TYPE *pQueue);
extern u8 DestroyQueue(QUEUE_TYPE *pQueue);
extern u32 GetItemNum(QUEUE_TYPE *pQueue);
extern u32 GetItemSize(QUEUE_TYPE *pQueue);
extern u32 GetCounter(QUEUE_TYPE *pQueue);
extern u8 GetFront(QUEUE_TYPE *pQueue, u8 *buf);
extern u8 GetRear(QUEUE_TYPE *pQueue, u8 *buf);
extern u8 EnQueue(QUEUE_TYPE *pQueue, u8 *buf);
extern u8 DeQueue(QUEUE_TYPE *pQueue, u8 *buf);

#endif
/************************ (C) COPYRIGHT CVTE ************************/

