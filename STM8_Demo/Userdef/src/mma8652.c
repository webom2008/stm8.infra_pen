#include "mma8652.h"

void MMA8652_LowLevel_DeInit(void)
{
    
}

void MMA8652_DeInit(void)
{
    
}

void MMA8652_LowLevel_Init(void)
{
    CLK_PeripheralClockConfig(MMA8652_I2C_CLK, ENABLE);

    /*!< DeInitializes the MMA8652_I2C */
    I2C_DeInit(MMA8652_I2C);

    /*!< Configure MMA8652_I2C pins: SCL */
//    GPIO_Init(MMA8652_I2C_SCL_GPIO_PORT, MMA8652_I2C_SCL_PIN, GPIO_Mode_Out_PP_Low_Slow);

//    /*!< Configure MMA8652_I2C pins: SDA */
//    GPIO_Init(MMA8652_I2C_SDA_GPIO_PORT, MMA8652_I2C_SDA_PIN, GPIO_Mode_In_PU_No_IT);

    /*!< Configure MMA8652_I2C pin: SMBUS ALERT */
    GPIO_Init(MMA8652_I2C_SMBUSALERT_GPIO_PORT, MMA8652_I2C_SMBUSALERT_PIN, GPIO_Mode_In_FL_IT);
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
}

uint8_t MMA8652_ReadReg(uint8_t RegName)
{
    __IO uint16_t RegValue = 0;

//    __IO uint32_t timeout = 0xFFFF;

//    /*!< Wait the end of last communication */
//    for (;timeout > 0; timeout--);
    
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

void MMA8652_WriteReg(uint8_t RegName, uint8_t RegValue)
{
    
}

void MMA8652_Config(void)
{
    
}

void MMA865x_Active (void)
{
    
}


