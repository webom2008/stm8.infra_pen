/***********************************************************************************************\
* Freescale MMA865xQ Driver
*
* Filename: iic.c
*
*
* (c) Copyright 2011, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/

#include "iic.h"

//#pragma MESSAGE DISABLE C4002 /* Warning C4002: Result not used */

/***********************************************************************************************\
* Private macros
\***********************************************************************************************/

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

void IIC_Start(void);
void IIC_Stop(void);
void IIC_RepeatStart(void);
void IIC_Delay(void);
void IIC_CycleWrite(uint8_t bout);
uint8_t IIC_CycleRead(uint8_t byteLeft);
uint8_t IIC_StopRead(void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

//#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

static uint8_t error;
static uint32_t timeout;

//#pragma DATA_SEG DEFAULT

#define BUFFER_OUT_SIZE       8

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* IIC Write Register
\*********************************************************/
void IIC_RegWrite(uint8_t address, uint8_t reg,uint8_t val)
{
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg);                          // Send Register
  IIC_CycleWrite(val);                          // Send Value
  IIC_Stop();                                   // Send Stop
}


/*********************************************************\
* IIC Read Register
\*********************************************************/
uint8_t IIC_RegRead(uint8_t address, uint8_t reg)
{
  uint8_t b;
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg);                          // Send Register
  IIC_RepeatStart();                            // Send Repeat Start
  IIC_CycleWrite(address+1);                    // Send IIC "Read" Address
  b = IIC_CycleRead(1);                         // *** Dummy read: reads "IIC_ReadAddress" value ***
  b = IIC_StopRead();                           // Send Stop Read Command IICD
  return b;
}


/*********************************************************\
* IIC Write Multiple Registers
\*********************************************************/
void IIC_RegWriteN(uint8_t address, uint8_t reg1,uint8_t N,uint8_t *array)
{
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg1);                         // Send Register
  while (N>0)                                   // Send N Values
  {
    IIC_CycleWrite(*array);
    array++;
    N--;
  }
  IIC_Stop();                                   // Send Stop
}


/*********************************************************\
* IIC Read Multiple Registers
\*********************************************************/
void IIC_RegReadN(uint8_t address, uint8_t reg1,uint8_t N,uint8_t *array)
{
  uint8_t b;
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg1);                         // Send Register
  IIC_RepeatStart();                            // Send Repeat Start
  IIC_CycleWrite(address+1);                    // Send IIC "Read" Address
  b = IIC_CycleRead(N);                  // *** Dummy read: reads "IIC_ReadAddress" value ***
  while (N>1)                                   // Read N-1 Register Values
  {
    N--;
    b = IIC_CycleRead(N); 
    *array = b;
    array++;
    
  }
  b = IIC_StopRead();                           // Send Stop Read which returns the IICD
   *array = b; 
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

/*********************************************************\
* Initiate IIC Start Condition
\*********************************************************/
void IIC_Start(void)
{
  IICC_MST = 1;
  timeout = 0;
  while ((!IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x01;
} //*** Wait until BUSY=1


/*********************************************************\
* Initiate IIC Stop Condition
\*********************************************************/
void IIC_Stop(void)
{
  IICC_MST = 0;
  timeout = 0;
  while ( (IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x02;
} //*** Wait until BUSY=0

/*********************************************************\
* Initiate IIC Stop Condition
\*********************************************************/
uint8_t IIC_StopRead(void)
{
  IICC_MST = 0;
  timeout = 0;
  while ( (IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x02;
  
  return IICD;
} //*** Wait until BUSY=0


/*********************************************************\
* Initiate IIC Repeat Start Condition
\*********************************************************/
void IIC_RepeatStart(void)
{
  IICC_RSTA = 1;
  timeout = 0;
  while ((!IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x04;
} //*** Wait until BUSY=1


/*********************************************************\
* IIC Delay
\*********************************************************/
void IIC_Delay(void)
{
  uint8_t IICd;
  for (IICd=0; IICd<100; IICd++);
}


/*********************************************************\
* IIC Cycle Write
\*********************************************************/
void IIC_CycleWrite(uint8_t bout)
{
  timeout = 0;
  while ((!IICS_TCF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x08; 
  IICD = bout; 
  timeout = 0;
  while ((!IICS_IICIF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x10;
  IICS_IICIF = 1; 
  if (IICS_RXAK)
    error |= 0x20;
}


/*********************************************************\
* IIC Cycle Read
\*********************************************************/
uint8_t IIC_CycleRead(uint8_t byteLeft)
{
  uint8_t bread; 
  timeout = 0;
  while ((!IICS_TCF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error|=0x08;
  IICC_TX = 0;
  IICC_TXAK = byteLeft<=1 ? 1 : 0;  //Set NACK when reading the last byte
  bread = IICD; 
  timeout = 0;
  while ((!IICS_IICIF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x10;
  IICS_IICIF=1;
  return bread;
}
