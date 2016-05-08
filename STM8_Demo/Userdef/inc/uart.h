/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : uart.h
* Function  : send & recv of uart
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/09
*********************************************************************************************************/
#ifndef __UART_H_
#define __UART_H_
#include "stm8s.h"
#include "rbuf.h"

#define UART1_RX_BUF_SIZE    128
#define DATA_BUF_LEN         64

//Communication protocol----------------------------------------------
#define COM_PROTOCOL_LEN    7
#define COM_PROTOCOL_DESADD 0xAA
#define COM_DATA_LEN        0x01
#define COM_PROTOCOL_SRCADD 0xEF
#define KEY_PACK_ID         0x01
#define ENCODER_PACK_ID     0x02

#define ALARM_LED_ID        0x04
#define ALARM_LED_OFF       0x00
#define ALARM_LED_ON        0x01
#define VERSION_CMD			0x0F

typedef enum _RECIEVE_DATA
{
    RECIEVE_ERROR   = 0x14,
    RECIEVE_CORRECT = 0x13,
    RECIEVE_NONE    = 0x11,
}RecieveData;

//Transmite Data Information------------------------------------------
typedef struct 
{    
    u8 (*GetCurDataFunc)(void);     /* ???????? */
    u8 DataID;                      /* ??????,KEY:0x01,ECD:0x02 */
    u8 u8NewData;                   /* ??????????? */
    u8 u8OldData;                   /* ??????????? */   
    u8 Databuf;                     /* ????,??????????? */
    RecieveData RevData;            /* ?MPU???ACK?? */
    u8 SendStat;                    /* ??????,0:???,1:???*/
    u8 RepeatCnt;                   /* ?????? */
    u8 RepeatTimes;                 /* ???? */
    u8 TimeOutStat;                 /* ???? */
    u16 TimeOutCnt;                 /* ?????*/
    u16 TimeOutTime;                /* ????*/
}TRANS_DATAINFO;


typedef struct {

	uint8_t m_DstAddr;				//????
	uint8_t m_SrcAddr;				//???
	uint8_t m_Type;					//???
	uint8_t m_SN; 					//???
	uint8_t m_Len;					//????
	uint8_t m_Data[DATA_BUF_LEN]; 	//???
	uint8_t m_CRC;					//CRC???
}Analysis_Data;

typedef enum {

    WaitDstAddr,
    WaitSrcAddr,
    WaitType,
    WaitSN,
    WaitLen,
    WaitData,
    WaitCRC
} Analysis_Status;

#ifdef _UART_C_
#define INTERFACE
#else
#define INTERFACE extern 
#endif

INTERFACE TRANS_DATAINFO TransDataInfo[];

INTERFACE rbuf_t rUart1RxBuf;
INTERFACE u8 Uart1RxBuf[];
INTERFACE void Uart_Init(void);
INTERFACE void Transmit_DataPackToMPU(void);
INTERFACE void LongPress_TransmitData(void);
INTERFACE void uart1_rcv_buf_init(void);
INTERFACE void UartData_Analysia(void);
INTERFACE void SelfCheck_TransmitData(void);

INTERFACE u8 crc8(u8 *buf, int len);

#endif