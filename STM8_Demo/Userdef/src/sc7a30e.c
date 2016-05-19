/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : sc7a30e.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/12
  Last Modified :
  Description   : SC7A30E driver
  Function List :
  History       :
  1.Date        : 2016/5/12
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
static void SC7A30E_Config(void);

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
static u32 test_pin_high_tick;
static u8 test_pin_high_flag = 0;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define SC7A30E_I2C            I2C1
#define SC7A30E_ADDR           0x3A /*uint8_t(0x1D << 1)*/
#define SC7A30E_I2C_SPEED      50000 

#define SC7A30E_I2C_CLK                     CLK_Peripheral_I2C1
#define SC7A30E_I2C_SCL_PIN                 GPIO_Pin_1                  /* PC.01 */
#define SC7A30E_I2C_SCL_GPIO_PORT           GPIOC                       /* GPIOC */
#define SC7A30E_I2C_SDA_PIN                 GPIO_Pin_0                  /* PC.00 */
#define SC7A30E_I2C_SDA_GPIO_PORT           GPIOC                       /* GPIOC */
#define SC7A30E_I2C_SMBUSALERT_PIN          GPIO_Pin_7                  /* PC.04 */
#define SC7A30E_I2C_SMBUSALERT_GPIO_PORT    GPIOB                       /* GPIOC */

#define TEST_PIN_TIGGLE_THRES_MS    200

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void SC7A30E_LowLevel_Init(void)
{
    CLK_PeripheralClockConfig(SC7A30E_I2C_CLK, ENABLE);

    /*!< DeInitializes the SC7A30E_I2C */
    I2C_DeInit(SC7A30E_I2C);
    
    /*!< Configure SC7A30E_I2C pin: SMBUS ALERT */
    GPIO_Init(  SC7A30E_I2C_SMBUSALERT_GPIO_PORT,
                SC7A30E_I2C_SMBUSALERT_PIN, 
                GPIO_Mode_In_FL_IT);
}

static void TestPin_Init(void)
{
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow);
}

static void TestPin_setValue(void)
{
    u32 tick = SysTick_Get();
    if (!test_pin_high_flag \
        && (tick - test_pin_high_tick > TEST_PIN_TIGGLE_THRES_MS))
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        test_pin_high_tick = tick;
        test_pin_high_flag = 1;
    }
}


void sc7a30e_init(void)
{
    SC7A30E_LowLevel_Init();
    
    I2C_Init(SC7A30E_I2C, SC7A30E_I2C_SPEED, 0x00, I2C_Mode_I2C, I2C_DutyCycle_2,
           I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

    /*!< SC7A30E_I2C Init */
    I2C_Cmd(SC7A30E_I2C, ENABLE);

    SC7A30E_Config();

    TestPin_Init();
}

/*****************************************************************************
 Prototype    : sc7a30e_readBytes
 Description  : read by I2C
                Timing for one byte:
                    Master-->ST-->SDA+W------>SUB----->SR-->SAD+R--------------->NMAK-->SP
                    Salve--------------->SAK------SAK-------------->SAK-->SDATA
                Timing for mul bytes:
                    Master-->ST-->SDA+W------>SUB----->SR-->SAD+R--------------->MAK----->MAK------>NMAK-->SP
                    Salve--------------->SAK------SAK-------------->SAK-->SDATA----->DATA---->DATA
 
 Input        : const uint8_t u8StartAddr  
                uint8_t *pRegValue         
                const uint8_t nLen         
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/13
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
uint8_t sc7a30e_readBytes(      SC7A30E_REG u8StartAddr, 
                                uint8_t *pRegValue, 
                                const uint8_t nLen)
{
    uint8_t count = 0;
    
    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(SC7A30E_I2C, I2C_FLAG_BUSY))
    {}

    /* Enable acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(SC7A30E_I2C, ENABLE);

    /*--------------------------- Transmission Phase ----------------------------*/
    /* Send START condition */
    I2C_GenerateSTART(SC7A30E_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send slave address for write */
    I2C_Send7bitAddress(SC7A30E_I2C, SC7A30E_ADDR, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {     
    }

    /* Send the specified register data pointer */
    I2C_SendData(SC7A30E_I2C, u8StartAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
    }

    /*------------------------------ Reception Phase ----------------------------*/
    /* Send Re-STRAT condition */
    I2C_GenerateSTART(SC7A30E_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send slave address for read */
    I2C_Send7bitAddress(SC7A30E_I2C, SC7A30E_ADDR, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
    {
    }

    for (count = 0; count < nLen-1; count++)
    {
        /* Test on EV7 and clear it */
        while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
        {
        }
        
        /* Store received data */
        pRegValue[count] = I2C_ReceiveData(SC7A30E_I2C);
    }
    
    /* Disable acknowledgement */
    I2C_AcknowledgeConfig(SC7A30E_I2C, DISABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(SC7A30E_I2C, ENABLE);
    
    /* Test on EV7 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
    {
    }
    
    /* Store received data */
    pRegValue[count++] = I2C_ReceiveData(SC7A30E_I2C);

    /*!< Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(SC7A30E_I2C, ENABLE);
    
    return count;
}

/*****************************************************************************
 Prototype    : sc7a30e_writeBytes
 Description  : write by I2C
                Timing for one byte:
                    Master-->ST-->SDA+W------>SUB----->DATA----->SP
                    Salve--------------->SAK------SAK------>SAK
                Timing for mul bytes:
                    Master-->ST-->SDA+W------>SUB----->DATA----->DATA----->SP
                    Salve--------------->SAK------SAK------>SAK------->SAK
 
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/12
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
uint8_t sc7a30e_writeBytes(     SC7A30E_REG u8StartRegAddr, 
                                const uint8_t *pu8RegValue,
                                const uint8_t nLen)
{
    uint8_t count = 0;
    
    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(SC7A30E_I2C, I2C_FLAG_BUSY));

    /* Enable SC7A30E_I2C acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(SC7A30E_I2C, ENABLE);

    /*--------------------------- Transmission Phase ----------------------------*/
    /* Send SC7A30E_I2C START condition */
    I2C_GenerateSTART(SC7A30E_I2C, ENABLE);

    /* Test on SC7A30E_I2C EV5 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send STSC7A30E slave address for write */
    I2C_Send7bitAddress(SC7A30E_I2C, SC7A30E_ADDR, I2C_Direction_Transmitter);

    /* Test on SC7A30E_I2C EV6 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {     
    }

    /* Send the specified register data pointer */
    I2C_SendData(SC7A30E_I2C, u8StartRegAddr);

    /* Test on SC7A30E_I2C EV8 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
    }

    for (count = 0; count < nLen-1; count++)
    {
        /*!< Send the byte to be written */
        I2C_SendData(SC7A30E_I2C, pu8RegValue[count]);

        /*!< Test on EV8 and clear it */
        while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {}
    }
    
    /*!< Send the byte to be written */
    I2C_SendData(SC7A30E_I2C, pu8RegValue[count++]);

    /*!< Test on EV8 and clear it */
    while (!I2C_CheckEvent(SC7A30E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    /*!< Send STOP condition */
    I2C_GenerateSTOP(SC7A30E_I2C, ENABLE);

    return count;
}

/*****************************************************************************
 Prototype    : sc7a30e_interruptHandle
 Description  : 
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/12
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void sc7a30e_interruptHandle(void)
{

}


static void SC7A30E_Config(void)
{

}


/*****************************************************************************
 Prototype    : sc7a30e_server
 Description  : test server
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2016/5/13
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void sc7a30e_server(void)
{

}

