/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : mma8652.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/10
  Last Modified :
  Description   : mma8652.c header file
  Function List :
  History       :
  1.Date        : 2016/5/10
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
static void MMA8652_LowLevel_Init(void);
static void MMA8652_Config(void);
static void MMA865x_getXYZ( uint8_t *pStatus,
                            int16_t *pX_mg, 
                            int16_t *pY_mg,
                            int16_t *pZ_mg);

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
#define TEST_PIN_TIGGLE_THRES_MS    500

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


static void MMA8652_LowLevel_Init(void)
{
    CLK_PeripheralClockConfig(MMA8652_I2C_CLK, ENABLE);

    /*!< DeInitializes the MMA8652_I2C */
    I2C_DeInit(MMA8652_I2C);
    
    /*!< Configure MMA8652_I2C pin: SMBUS ALERT */
    GPIO_Init(  MMA8652_I2C_SMBUSALERT_GPIO_PORT,
                MMA8652_I2C_SMBUSALERT_PIN, 
                GPIO_Mode_In_FL_IT);
}

static void TestPin_Init(void)
{
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow);
}

static void TestPin_setValue(void)
{
    u32 tick = SysTick_Get();
    if (tick - test_pin_high_tick > TEST_PIN_TIGGLE_THRES_MS)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        test_pin_high_tick = tick;
        test_pin_high_flag = 1;
    }
}

void MMA8652_Init(void)
{
    MMA8652_LowLevel_Init();
    
//    I2C_Init(MMA8652_I2C, MMA8652_I2C_SPEED, 0x00, I2C_Mode_I2C, I2C_DutyCycle_2,
//           I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
    I2C_Init(MMA8652_I2C, MMA8652_I2C_SPEED, 0x00, I2C_Mode_SMBusHost,
           I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

    /*!< Enable SMBus Alert interrupt */
//    I2C_ITConfig(MMA8652_I2C, I2C_IT_ERR, ENABLE);

    /*!< MMA8652_I2C Init */
    I2C_Cmd(MMA8652_I2C, ENABLE);

    MMA8652_Config();

    TestPin_Init();
}

uint8_t MMA8652_ReadBytes(  const uint8_t u8StartAddr, 
                            uint8_t *pRegValue, 
                            const uint8_t nLen)
{
    uint8_t count = 0;
    
    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(MMA8652_I2C, I2C_FLAG_BUSY))
    {}

    /* Enable MMA8652_I2C acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(MMA8652_I2C, ENABLE);

    /*--------------------------- Transmission Phase ----------------------------*/
    /* Send MMA8652_I2C START condition */
    I2C_GenerateSTART(MMA8652_I2C, ENABLE);

    /* Test on MMA8652_I2C EV5 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send STMMA8652 slave address for write */
    I2C_Send7bitAddress(MMA8652_I2C, MMA8652_ADDR, I2C_Direction_Transmitter);

    /* Test on MMA8652_I2C EV6 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {     
    }

    /* Send the specified register data pointer */
    I2C_SendData(MMA8652_I2C, u8StartAddr);

    /* Test on MMA8652_I2C EV8 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
    }

    /*------------------------------ Reception Phase ----------------------------*/
    /* Send Re-STRAT condition */
    I2C_GenerateSTART(MMA8652_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send STMMA8652 slave address for read */
    I2C_Send7bitAddress(MMA8652_I2C, MMA8652_ADDR, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
    {
    }

    for (count = 0; count < nLen-1; count++)
    {
        /* Test on EV7 and clear it */
        while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
        {
        }
        
        /* Store MMA8652_I2C received data */
        pRegValue[count] = I2C_ReceiveData(MMA8652_I2C);
    }
    
    /* Disable MMA8652_I2C acknowledgement */
    I2C_AcknowledgeConfig(MMA8652_I2C, DISABLE);
    
    /* Send MMA8652_I2C STOP Condition */
    I2C_GenerateSTOP(MMA8652_I2C, ENABLE);
    
    /* Test on EV7 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
    {
    }
    
    /* Store MMA8652_I2C received data */
    pRegValue[count++] = I2C_ReceiveData(MMA8652_I2C);

    /*!< Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(MMA8652_I2C, ENABLE);
    
    return count;
}

uint8_t MMA8652_WriteBytes( const uint8_t u8StartRegAddr, 
                            const uint8_t *pu8RegValue,
                            const uint8_t nLen)
{
    uint8_t count = 0;
    
    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(MMA8652_I2C, I2C_FLAG_BUSY));

    /* Enable MMA8652_I2C acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(MMA8652_I2C, ENABLE);

    /*--------------------------- Transmission Phase ----------------------------*/
    /* Send MMA8652_I2C START condition */
    I2C_GenerateSTART(MMA8652_I2C, ENABLE);

    /* Test on MMA8652_I2C EV5 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
    {
    }

    /* Send STMMA8652 slave address for write */
    I2C_Send7bitAddress(MMA8652_I2C, MMA8652_ADDR, I2C_Direction_Transmitter);

    /* Test on MMA8652_I2C EV6 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
    {     
    }

    /* Send the specified register data pointer */
    I2C_SendData(MMA8652_I2C, u8StartRegAddr);

    /* Test on MMA8652_I2C EV8 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
    {
    }

    for (count = 0; count < nLen-1; count++)
    {
        /*!< Send the byte to be written */
        I2C_SendData(MMA8652_I2C, pu8RegValue[count]);

        /*!< Test on EV8 and clear it */
        while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {}
    }
    
    /*!< Send the byte to be written */
    I2C_SendData(MMA8652_I2C, pu8RegValue[count++]);

    /*!< Test on EV8 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    /*!< Send STOP condition */
    I2C_GenerateSTOP(MMA8652_I2C, ENABLE);

    return count;
}

void MMA865x_Active(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;

    addr = CTRL_REG1;
    MMA8652_ReadBytes(addr, &value, 1);
    value |= ACTIVE_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
}

void MMA865x_Standby(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;

    addr = CTRL_REG1;
    MMA8652_ReadBytes(addr, &value, 1);
    value &= ~ACTIVE_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
}

//=========================== 运动检测方法1(无高通滤波) START ==================
/*
* The threshold resolution is 0.063 g/LSB and the threshold register
* has a range of 0 to 127 counts. The maximum range is to ±8 g.
* Note that even when the full scale value is set to ±2 g or ±4 g,
* the motion still detects up to ±8 g
*/
static void MMA8652_ConfigMotion(uint16_t thres_mg, uint16_t count_time_ms)
{
    uint8_t addr = FT_MT_THS_REG;
    uint8_t value = 0x80;   /* DBCNTM = 1*/

    value |= (uint8_t)(thres_mg / 63);
    MMA8652_WriteBytes(addr, &value, 1);
    
    addr = FF_MT_COUNT_REG;
    /* ODR=200Hz, Normal, Time Step = 5ms*/
    value = count_time_ms / 5;
    MMA8652_WriteBytes(addr, &value, 1);
}

static void MMA8652_InitMotionDetect(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;

    /* Mode 4: Motion detection with ELE = 1, OAE = 1 */
    addr = FF_MT_CFG_REG;
    value = ELE_MASK | OAE_MASK | ZEFE_MASK | YEFE_MASK | XEFE_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
    
    MMA8652_ConfigMotion(800, 200);

    /* Enable FF_MT interrupt to wake up */
    addr = CTRL_REG3;
    value = WAKE_FF_MT_MASK;
    MMA8652_WriteBytes(addr, &value, 1);

    /* Enable FF_MT interrupt */
    addr = CTRL_REG4;
    value = INT_EN_FF_MT_MASK;
    MMA8652_WriteBytes(addr, &value, 1);

    /* FF_MT interrupt is rounted to INT1 pin */
    addr = CTRL_REG5;
    value = INT_CFG_FF_MT_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
}
//=========================== 运动检测方法1(无高通滤波) END ====================


//===============================运动检测方法2(高通滤波) START =================
void MMA8652_ConfigTransient(uint16_t thres_mg, uint16_t count_time_ms)
{
    uint8_t addr = TRANSIENT_THS_REG;
    uint8_t value = 0x80;   /* DBCNTM = 1*/

    MMA865x_Standby();
    
    value |= (uint8_t)(thres_mg / 63);
    MMA8652_WriteBytes(addr, &value, 1);
    
    addr = TRANSIENT_COUNT_REG;
    /* ODR=200Hz, Normal, Time Step = 5ms*/
    value = count_time_ms / 5;
    MMA8652_WriteBytes(addr, &value, 1);

    MMA865x_Active();
}

static void MMA8652_InitTransientDetect(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;

    /* Mode 4: Transient detection with high-pass filter */
    addr = TRANSIENT_CFG_REG;
    value = TELE_MASK | ZTEFE_MASK | YTEFE_MASK | XTEFE_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
    

    /* Enable Transient interrupt to wake up */
    addr = CTRL_REG3;
    value = WAKE_TRANS_MASK;
    MMA8652_WriteBytes(addr, &value, 1);

    /* Enable Transient interrupt */
    addr = CTRL_REG4;
    value = INT_EN_TRANS_MASK;
    MMA8652_WriteBytes(addr, &value, 1);

    /* Transient interrupt is rounted to INT1 pin */
    addr = CTRL_REG5;
    value = INT_CFG_TRANS_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
}
//===============================运动检测方法2(高通滤波) END =================

static void delay(__IO uint16_t nCount)
{
    u16 i = 0;
    for (i=0; i < nCount; i++);
}

static void MMA8652_Config(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;
    
    /*
    **  Reset sensor, and wait for reboot to complete
    */
    addr = CTRL_REG2;
    value = RST_MASK;
    MMA8652_WriteBytes(addr, &value, 1);
    do 
    {
        delay(0xFFFF);
        MMA8652_ReadBytes(CTRL_REG2, &value, 1);
    } while (value & RST_MASK);
    
    /*
    **  Configure sensor for:
    **    - Sleep Mode Poll Rate of 50Hz (20ms)
    **    - System Output Data Rate of 200Hz (5ms)
    **    - Normal mode :12-bit ADC
    */
    addr = CTRL_REG1;
    value = ASLP_RATE_20MS + DATA_RATE_5MS;
    MMA8652_WriteBytes(addr, &value, 1);
    
    /*
    **  Configure sensor data for:
    **    - Full Scale of +/-2g
    */
    addr = XYZ_DATA_CFG_REG;
//    value = FULL_SCALE_2G;
    value = HPF_OUT_MASK | FULL_SCALE_2G;
    MMA8652_WriteBytes(addr, &value, 1);

//    MMA8652_InitMotionDetect();
    MMA8652_InitTransientDetect();
}

void MMA8652_InterruptHandle(void)
{
//    uint8_t addr = INT_SOURCE_REG;
//    uint8_t status = 0;
//    MMA8652_ReadBytes(addr, &status, 1);
//    if (status & SRC_TRANS_MASK)
//    {
//        addr = TRANSIENT_SRC_REG;
//        MMA8652_ReadBytes(addr, &status, 1);
//    }

    /* 中断服务程序-力求简洁*/
    uint8_t addr = TRANSIENT_SRC_REG;
    uint8_t status = 0;
    MMA8652_ReadBytes(addr, &status, 1);
    TestPin_setValue();
}



/*adc: [15:8] MSBs, [7:4] LSBs*/
static int16_t adc_convert_to_mg(uint16_t adc)
{
    int16_t value = (int16_t)(adc >> 4); 

    
    if (value & 0x0800)//mask bit 11,negative
    {
        value = (((~value) & 0x07FF) + 1) ; //先转为正数值
        value =  0 - value;                 //转为负值
    }

    /*
    * Range 2g: 1 adc = 0.001g = 1mg
    * Range 4g: 1 adc = 2mg
    * Range 8g: 1 adc = 4mg
    */
    return value;
}

static void MMA865x_getXYZ( uint8_t *pStatus,
                            int16_t *pX_mg, 
                            int16_t *pY_mg,
                            int16_t *pZ_mg)
{
    uint8_t addr = 0;
    uint8_t pBuffer[7] = {0, };
    MMA8652_ReadBytes(addr, pBuffer, sizeof(pBuffer));
    *pStatus = pBuffer[0];
    *pX_mg = adc_convert_to_mg((uint16_t)(pBuffer[1]<<8 | pBuffer[2]));
    *pY_mg = adc_convert_to_mg((uint16_t)(pBuffer[3]<<8 | pBuffer[4]));
    *pZ_mg = adc_convert_to_mg((uint16_t)(pBuffer[5]<<8 | pBuffer[6]));
}


uint8_t status;
uint8_t version;
int16_t x_mg;
int16_t y_mg;
int16_t z_mg;
void MMA8652_server(void)
{
    static u32 server_tick = 0;
    uint8_t addr = WHO_AM_I_REG;
    
    if (IsOnTime(server_tick))
    {
        server_tick = SysTick_Get() + 500;
        if (test_pin_high_flag && \
            (SysTick_Get() - test_pin_high_tick > TEST_PIN_TIGGLE_THRES_MS))
        {
            test_pin_high_flag = 0;
            GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        }
        MMA8652_ReadBytes(addr, &version, 1);
        MMA865x_getXYZ(&status, &x_mg, &y_mg, &z_mg);
    }
}

