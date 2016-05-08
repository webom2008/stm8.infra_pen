/**
  ******************************************************************************
  * @file    queue.c
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

#include "queue.h"

QUEUE_TYPE *InitQueue(u32 ItemNum, u32 ItemSize)
{
//    QUEUE_TYPE *pQueue = (QUEUE_TYPE *)malloc(sizeof(QUEUE_TYPE));
//    if (pQueue != NULL)
//    {
//        pQueue->QueueBuffer = (u8 *)malloc(sizeof(u8) * (ItemNum * ItemSize));
//        pQueue->QueueBufferSize = ItemNum * ItemSize;
//        pQueue->QueueCounter = 0;
//        pQueue->QueueHead = 0;
//        pQueue->QueueTail = ItemNum * ItemSize;
//        pQueue->QueueItemNum = ItemNum;
//        pQueue->QueueItemSize = ItemSize;
//        if (pQueue->QueueBuffer)
//        {
//            memset(pQueue->QueueBuffer, 0, pQueue->QueueBufferSize);
//        }
//    }
    
 //   return pQueue;
}

u8 ClearQueue(QUEUE_TYPE *pQueue)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    pQueue->QueueBufferSize = 64 * 4;
    pQueue->QueueCounter = 0;
    pQueue->QueueHead = 0;
    pQueue->QueueTail = 64 * 4;
    pQueue->QueueItemNum = 64;
    pQueue->QueueItemSize = 4;
    
    return Q_SUCCESS;
}

u8 DestroyQueue(QUEUE_TYPE *pQueue)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    free(pQueue->QueueBuffer);
    free(pQueue);
    pQueue = NULL;
    
    return Q_SUCCESS;
}

u32 GetItemNum(QUEUE_TYPE *pQueue)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    return pQueue->QueueItemNum;
}

u32 GetItemSize(QUEUE_TYPE *pQueue)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    return pQueue->QueueItemSize;
}

u32 GetCounter(QUEUE_TYPE *pQueue)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    return pQueue->QueueCounter;
}

u8 GetFront(QUEUE_TYPE *pQueue, u8 *buf)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    if (pQueue->QueueCounter == 0)
    {
        return Q_EMPTY;
    }

    memcpy(buf, pQueue->QueueBuffer+pQueue->QueueHead, pQueue->QueueItemSize);
    
    return Q_SUCCESS;
}

u8 GetRear(QUEUE_TYPE *pQueue, u8 *buf)
{
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    if (pQueue->QueueCounter == 0)
    {
        return Q_EMPTY;
    }

    memcpy(buf, pQueue->QueueBuffer+pQueue->QueueTail, pQueue->QueueItemSize);
    
    
    return Q_SUCCESS;
}

u8 EnQueue(QUEUE_TYPE *pQueue, u8 *buf)
{
    int count = 0;
    u8 *temp = NULL;
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    if (pQueue->QueueCounter >= pQueue->QueueItemNum)
    {
        return Q_FULL;
    }

    pQueue->QueueTail += pQueue->QueueItemSize;
    if (pQueue->QueueTail >= pQueue->QueueBufferSize)
    {
        pQueue->QueueTail = 0;
    }

    //memcpy(pQueue->QueueBuffer+pQueue->QueueTail, buf, pQueue->QueueItemSize);
    count = pQueue->QueueItemSize;
    temp = pQueue->QueueBuffer+pQueue->QueueTail;
    if(temp != NULL && buf != NULL && count > 0)
    {
      while(count--)
        *temp++ = *buf++;
    }
    pQueue->QueueCounter++;
    
    return Q_SUCCESS;
}

u8 DeQueue(QUEUE_TYPE *pQueue, u8 *buf)
{
    int count = 0;
    u8 *temp = NULL;
    if (pQueue == NULL)
    {
        return Q_NULL;
    }
    
    if (pQueue->QueueCounter == 0)
    {
        return Q_EMPTY;
    }

    //memcpy(buf, pQueue->QueueBuffer+pQueue->QueueHead, pQueue->QueueItemSize);
    count = pQueue->QueueItemSize;
    temp = pQueue->QueueBuffer+pQueue->QueueHead;
    if(temp != NULL && buf != NULL && count > 0)
    {
      while(count--)
        *buf++ = *temp++;
    }
    
    pQueue->QueueCounter--;
    pQueue->QueueHead += pQueue->QueueItemSize;
    if (pQueue->QueueHead >= pQueue->QueueBufferSize)
    {
        pQueue->QueueHead = 0;
    }
    
    return Q_SUCCESS;
}


/************************ (C) COPYRIGHT CVTE ************************/

