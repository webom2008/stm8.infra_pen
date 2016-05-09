#include "mma8652.h"

extern void Delay(__IO uint16_t nCount);

void MMA8652_LowLevel_Init(void)
{
    CLK_PeripheralClockConfig(MMA8652_I2C_CLK, ENABLE);

    /*!< DeInitializes the MMA8652_I2C */
    I2C_DeInit(MMA8652_I2C);
    
    /*!< Configure MMA8652_I2C pin: SMBUS ALERT */
    GPIO_Init(  MMA8652_I2C_SMBUSALERT_GPIO_PORT,
                MMA8652_I2C_SMBUSALERT_PIN, 
                GPIO_Mode_In_FL_IT);
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
}

uint8_t MMA8652_ReadReg(uint8_t RegName)
{
    __IO uint16_t RegValue = 0;
    
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
    I2C_SendData(MMA8652_I2C, RegName);

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
    
    /* Disable MMA8652_I2C acknowledgement */
    I2C_AcknowledgeConfig(MMA8652_I2C, DISABLE);
    
    /* Send MMA8652_I2C STOP Condition */
    I2C_GenerateSTOP(MMA8652_I2C, ENABLE);
    
    /* Test on EV7 and clear it */
    while (!I2C_CheckEvent(MMA8652_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
    {
    }
    
    /* Store MMA8652_I2C received data */
    RegValue = I2C_ReceiveData(MMA8652_I2C);

    /*!< Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(MMA8652_I2C, ENABLE);
    
    /* Return register value */
    return (RegValue);
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

static void MMA8652_InitFreefallMotion(void)
{
    uint8_t addr = 0;
    uint8_t value = 0;

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


void MMA8652_Config(void)
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
        Delay(0xFFFF);
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
    value = FULL_SCALE_2G;
    MMA8652_WriteBytes(addr, &value, 1);

//    MMA8652_InitFreefallMotion();
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

void MMA865x_getXYZ(uint8_t *pStatus,
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

void MMA8652_Test(void)
{
    version = MMA8652_ReadReg( WHO_AM_I_REG);
    MMA865x_getXYZ(&status, &x_mg, &y_mg, &z_mg);
    Delay(0xFFFF);
    Delay(0xFFFF);
    Delay(0xFFFF);
}


