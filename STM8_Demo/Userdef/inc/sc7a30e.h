/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : sc7a30e.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2016/5/12
  Last Modified :
  Description   : sc7a30e.c header file
  Function List :
  History       :
  1.Date        : 2016/5/12
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __SC7A30E_H__
#define __SC7A30E_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    R_OUT_TEMP_L    = 0x0C,
    R_OUT_TEMP_H    = 0x0D,
        
    R_WHO_AM_I      = 0x0F,
    
    RW_USER_CAL     = 0x13,
    
    RW_NVM_WR       = 0x1E,
    RW_TEMP_CFG,
    RW_CTRL_REG1,
    RW_CTRL_REG2,
    RW_CTRL_REG3,
    RW_CTRL_REG4,
    RW_CTRL_REG5,
    RW_CTRL_REG6,
    RW_REFERENCE,
    RW_STATUS_REG,
    R_OUT_X_L       = 0x28,
    R_OUT_X_H,
    R_OUT_Y_L,
    R_OUT_Y_H,
    R_OUT_Z_L,
    R_OUT_Z_H,
    RW_FIFO_CTRL_REG,
    R_FIFO_SRC_REG,
    RW_INT1_CFG,
    R_INT1_SOURCE,
    RW_INT1_THS,
    RW_INT1_DURATION,
    
    RW_CLICK_CFG    = 0x38,
    R_CLICK_SRC,
    RW_CLICK_THS,
    RW_TIME_LIMIT,
    RW_TIME_LATENCY,
    RW_TIME_WINDOW,
    RW_ACT_THS,
    RW_ACT_DURATION,
} SC7A30E_REG;

/************************* RW_CTRL_REG1 Detail ********************************/
#define CTRL_REG1_LPEN_MASK     (uint8_t)(0x01<<3)
#define CTRL_REG1_ZEN_MASK      (uint8_t)(0x01<<2)
#define CTRL_REG1_YEN_MASK      (uint8_t)(0x01<<1)
#define CTRL_REG1_XEN_MASK      (uint8_t)(0x01<<0)

typedef enum
{
    ODR_PWRDOWN     = (uint8_t)(0x00<<4),
    ODR_1Hz         = (uint8_t)(0x01<<4),
    ODR_10Hz        = (uint8_t)(0x02<<4),
    ODR_25Hz        = (uint8_t)(0x03<<4),
    ODR_50Hz        = (uint8_t)(0x04<<4),
    ODR_100Hz       = (uint8_t)(0x05<<4),
    ODR_200Hz       = (uint8_t)(0x06<<4),
    ODR_400Hz       = (uint8_t)(0x07<<4),
    ODR_1600Hz      = (uint8_t)(0x08<<4),   /* Low power mode (1.6KHz)*/
    ODR_5000Hz      = (uint8_t)(0x09<<4),   /* Normal (1.25kHz)/ Low power mode (5KHz)*/
} ODR_TypeDef; /* Out Data rate configuration */


/************************* RW_CTRL_REG2 Detail ********************************/
typedef enum
{
    HPM_NORMAL_RESET_READING    = (uint8_t)(0x00<<6),
    HPM_REFERENCE               = (uint8_t)(0x01<<6),
    HPM_NORMAL                  = (uint8_t)(0x02<<6),
    HPM_AUTO_RESET              = (uint8_t)(0x03<<6),
} HighPassMode_TypeDef; /* High pass filter mode selection */

typedef enum
{
    HPCF_MOD1           = (uint8_t)(0x00<<4),
    HPCF_MOD2           = (uint8_t)(0x01<<4),
    HPCF_MOD3           = (uint8_t)(0x02<<4),
    HPCF_MOD4           = (uint8_t)(0x03<<4),
} HPCF_TypeDef; /* High pass filter cut off frequency selection */

#define CTRL_REG2_FDS_MASK      = (uint8_t)(0x01<<3) /* Filtered data selection */
#define CTRL_REG2_HPCLICK_MASK  = (uint8_t)(0x01<<2) /* High pass filter enabled for CLICK function */
#define CTRL_REG2_HPIS2_MASK    = (uint8_t)(0x01<<1) /* High pass filter enabled for AOI function on interrupt 2 */
#define CTRL_REG2_HPIS1_MASK    = (uint8_t)(0x01)    /* High pass filter enabled for AOI function on interrupt 1 */


/************************* RW_CTRL_REG3 Detail ********************************/
#define CTRL_REG3_I1_CLICK_MASK     = (uint8_t)(0x01<<7) /* CLICK interrupt on INT1.(0: Disable; 1: Enable)*/
#define CTRL_REG3_I1_AOI1_MASK      = (uint8_t)(0x01<<6) /* AOI1 interrupt on INT1.*/
#define CTRL_REG3_I1_AOI2_MASK      = (uint8_t)(0x01<<5) /* AOI2 interrupt on INT1.*/
#define CTRL_REG3_I1_DRDY1_MASK     = (uint8_t)(0x01<<4) /* DRDY1 interrupt on INT1.*/
#define CTRL_REG3_I1_DRDY2_MASK     = (uint8_t)(0x01<<3) /* DRDY2 interrupt on INT1.*/
#define CTRL_REG3_HI1_WTM_MASK      = (uint8_t)(0x01<<2) /* FIFO Watermark interrupt on INT1.*/
#define CTRL_REG3_I1_OVERRUN_MASK   = (uint8_t)(0x01<<1) /* FIFO Overrun interrupt on INT1.*/


/************************* RW_CTRL_REG4 Detail ********************************/
#define CTRL_REG4_BDU_MASK      = (uint8_t)(0x01<<7)
#define CTRL_REG4_BLE_MASK      = (uint8_t)(0x01<<6)
typedef enum
{
    FS_2G   = (uint8_t)(0x00<<4),
    FS_4G   = (uint8_t)(0x01<<4),
    FS_8G   = (uint8_t)(0x02<<4),
    FS_16G  = (uint8_t)(0x03<<4),
} FullScale_TypeDef; /* Full Scale selection */
#define CTRL_REG4_HR_MASK       = (uint8_t)(0x01<<3) /* High resolution output mode. */
typedef enum
{
    ST_NORMAL       = (uint8_t)(0x00<<1),
    ST_TEST0        = (uint8_t)(0x01<<1),
    ST_TEST1        = (uint8_t)(0x02<<1),
    ST_RESERVED     = (uint8_t)(0x03<<1),
} SelfTest_TypeDef; /* Self test enable */
#define CTRL_REG4_SIM_MASK      = (uint8_t)(0x01)    /* SPI serial interface mode selection. */


/************************* RW_CTRL_REG5 Detail ********************************/





/************************* RW_CTRL_REG6 Detail ********************************/
#define CTRL_REG6_I2_CLICK_MASK     = (uint8_t)(0x01<<7) /* CLICK interrupt on INT2 */
#define CTRL_REG6_I2_INT2_MASK      = (uint8_t)(0x01<<6) /* AOI2 interrupt on INT2 */
#define CTRL_REG6_BOOT_I2_MASK      = (uint8_t)(0x01<<4) /* BOOT status on INT2 */
#define CTRL_REG6_H_LACTIVE_MASK    = (uint8_t)(0x01<<1) /* 0: interrupt active high; 1: interrupt active low */


/************************* INT1_CFG Detail ********************************/
typedef enum
{
    INT_MODE_OR_COMB        = (uint8_t)(0x00<<6), /* OR combination of interrupt events */
    INT_MODE_6D             = (uint8_t)(0x01<<6), /* 6 direction movement recognition */
    INT_MODE_AND_COMB       = (uint8_t)(0x02<<6), /* AND combination of interrupt events */
    INT_MODE_6D_POS         = (uint8_t)(0x03<<6), /* 6 direction position recognition */
} INT_MODE_Typedef;

#define INT1_CFG_ZHIE_MASK      = (uint8_t)(0x01<<5) /* Enable interrupt generation on Z high event or on Direction recognition */
#define INT1_CFG_ZLIE_MASK      = (uint8_t)(0x01<<4) /* Enable interrupt generation on Z low event or on Direction recognition */
#define INT1_CFG_YHIE_MASK      = (uint8_t)(0x01<<3) /* Enable interrupt generation on Y high event or on Direction recognition */
#define INT1_CFG_YLIE_MASK      = (uint8_t)(0x01<<2) /* Enable interrupt generation on Y low event or on Direction recognition */
#define INT1_CFG_XHIE_MASK      = (uint8_t)(0x01<<1) /* Enable interrupt generation on X high event or on Direction recognition */
#define INT1_CFG_XLIE_MASK      = (uint8_t)(0x01)    /* Enable interrupt generation on X low event or on Direction recognition */


/************************* INT1_SRC Detail ********************************/
#define INT1_SRC_IA_MASK      = (uint8_t)(0x01<<6) /* Interrupt active */
#define INT1_SRC_ZH_MASK      = (uint8_t)(0x01<<5) /* Z high */
#define INT1_SRC_ZL_MASK      = (uint8_t)(0x01<<4) /* Z low */
#define INT1_SRC_YH_MASK      = (uint8_t)(0x01<<3) /* Y high */
#define INT1_SRC_YL_MASK      = (uint8_t)(0x01<<2) /* Y low */
#define INT1_SRC_XH_MASK      = (uint8_t)(0x01<<1) /* X high */
#define INT1_SRC_XL_MASK      = (uint8_t)(0x01)    /* X low */


/****************************** INT1_THS Detail ********************************
THS6 - HS0 : Interrupt 1 threshold. Default value: 000 0000
1LSB=16mg @ FS=2g
1LSB=32mg @ FS=4g
1LSB=64mg @ FS=8g
1LSB=128mg@ FS=16g
*/


/***************************** INT1_DURATION Detail ***************************/




extern void sc7a30e_init(void);
extern void sc7a30e_interruptHandle(void);
extern uint8_t sc7a30e_readBytes(   SC7A30E_REG u8StartAddr, 
                                    uint8_t *pRegValue, 
                                    const uint8_t nLen);
extern void sc7a30e_server(void);
extern uint8_t sc7a30e_writeBytes(  SC7A30E_REG u8StartRegAddr, 
                                    const uint8_t *pu8RegValue,
                                    const uint8_t nLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SC7A30E_H__ */
