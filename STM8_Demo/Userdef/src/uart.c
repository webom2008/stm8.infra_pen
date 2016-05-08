/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : uart.c
* Function  : send & recv of uart
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/09
*********************************************************************************************************/
#include "stm8s.h"
#include "uart.h"
#include "key.h"
#include "encoder.h"
#include "rbuf.h"
#include "led.h"
#include "version.h"


#define _UART_C_

/* Private macro -------------------------------------------------------------*/
#define CountOf(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/

rbuf_t rUart1RxBuf;
u8 Uart1RxBuf[UART1_RX_BUF_SIZE];


//timer param----------------------------------------------
#define TRANS_TIMEOUT_CNT   75    /* ����MPU��ACK�źŵĳ�ʱʱ�䣬Ϊ2*75=150ms*/
#define REPEAT_TIMES        2     /* �ط����� */

//Pravite Data-----------------------------------------------------------------
TRANS_DATAINFO TransDataInfo[2] = 
{
    {Get_KeyCurData, KEY_PACK_ID, 0, 0, 0, RECIEVE_NONE, 0, REPEAT_TIMES, REPEAT_TIMES, 1, 0, TRANS_TIMEOUT_CNT},
    {Get_EncoderCurrentData, ENCODER_PACK_ID, 0, 0, 0, RECIEVE_NONE, 0, REPEAT_TIMES, REPEAT_TIMES, 1, 0, TRANS_TIMEOUT_CNT},
};
u8 DataPack[COM_PROTOCOL_LEN];
u8 DataPackNum = 0;  //Data package number.
u8 DataSelfNum = 0;

Analysis_Data	 analysisData;
Analysis_Status  analysisStatus;



// Function---------------------------------------------------------------------
/********************************************************************************
 * ������: void Uart_Init(void)
 * ��  ��: ���ڳ�ʼ������
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Uart_Init(void)
{
    UART1_DeInit();
    UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);//Enable RX intterupt
    UART1_Cmd(ENABLE);                       //Enable UART1
    analysisStatus = WaitDstAddr;
}

/********************************************************************************
 * ������:void uart1_rcv_buf_init(void)
 * ��  ��: ���ڳ�ʼ������
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void uart1_rcv_buf_init(void)
{
    rbuf_init(&rUart1RxBuf, Uart1RxBuf, UART1_RX_BUF_SIZE); 
}

/********************************************************************************
 * ������: void Uart_SendByte(u8 data)
 * ��  ��: ���ڷ��͵����ַ�
 * ��  ��: data-->��Ҫ���͵��ַ�
 * ��  ��: ��
 *******************************************************************************/
static void Uart_SendByte(u8 data)
{ 
    UART1_SendData8((unsigned char)data);
    /* wait for complete of send */
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}
/********************************************************************************
 * ������: void Uart_SendString(u8* str, u16 len)  
 * ��  ��: ���ڷ����ַ���
 * ��  ��: str-->��Ҫ���͵��ַ�����len-->��Ҫ�����ַ����ĳ���
 * ��  ��: ��
 *******************************************************************************/
static void Uart_SendString(u8* str, u16 len)    
{
    u16 i=0;
    
    for( ;i<len;i++)
        Uart_SendByte(str[i]); 
}
/********************************************************************************
 * ������: u8 crc8(u8 *buf, int len)
 * ��  ��: CRCУ���㷨
 * ��  ��: *buf-->��ҪУ������ݣ�len-->��ҪУ������ݳ���
 * ��  ��: CRCУ����
 *******************************************************************************/
u8 crc8(u8 *buf, int len)
{
    unsigned char crc = 0;
    for (int i = 0; i < len; i++)
    {
        crc ^= buf[i];
        for(int i = 0; i < 8; i++)
        {
            if(crc&0x01)
            {
                    crc = (crc >> 1) ^ 0x8C;
            }	
            else
            {
                    crc >>= 1;
            }
        }
    }
    return crc;
}


uint8_t KEY_CRC8(const Analysis_Data *pData)
{
    uint8_t buf[DATA_BUF_LEN];
    buf[0] = pData->m_Type;
    buf[1] = pData->m_SN;
    buf[2] = pData->m_Len;
    memcpy(buf+3, pData->m_Data, pData->m_Len);
	return (crc8(buf, pData->m_Len+3));
}

/********************************************************************************
 * ������: u8* Data_Package(TRANS_DATAINFO *_pdata)
 * ��  ��: Ϊָ�������ݴ��
 * ��  ��: TRANS_DATAINFO* _pdata-->��Ҫ��������ݽṹ
 * ��  ��: ָ�����ݰ���ָ��
 *******************************************************************************/
static u8* Data_Package(TRANS_DATAINFO *_pdata)
{
    u8 i = 0;
    
    DataPack[i++] = COM_PROTOCOL_DESADD;                  /* Ŀ�ĵ�ַ */
    DataPack[i++] = COM_PROTOCOL_SRCADD;                  /* Դ��ַ */ 
    DataPack[i++] = _pdata->DataID;                       /* ����ʾ */
    DataPack[i++] = DataPackNum;                          /* ����� */
    DataPack[i++] = COM_DATA_LEN;                         /* ���ݳ��� */
    DataPack[i++] = _pdata->Databuf;                      /* �����ݶ� */
    DataPack[i] = crc8(DataPack+2, COM_PROTOCOL_LEN - 3); /* CRCУ���ֶ� */

    return DataPack;
}

static u8* DataAlarmLed_Package(u8 data)
{
    u8 i = 0;
    
    DataPack[i++] = COM_PROTOCOL_DESADD;                  /* Ŀ�ĵ�ַ */
    DataPack[i++] = COM_PROTOCOL_SRCADD;                  /* Դ��ַ */ 
    DataPack[i++] = 0x04;                                 /* ����ʾ */
    DataPack[i++] = DataPackNum;                          /* ����� */
    DataPack[i++] = COM_DATA_LEN;                         /* ���ݳ��� */
    DataPack[i++] = data;                      /* �����ݶ� */
    DataPack[i] = crc8(DataPack+2, COM_PROTOCOL_LEN - 3); /* CRCУ���ֶ� */
	(DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; /* ����� */

    return DataPack;
}
/********************************************************************************
 * ������: void Send_SpecifyDataPack(TRANS_DATAINFO* _pdata)
 * ��  ��: ����ָ�������ݰ�
 * ��  ��: TRANS_DATAINFO* _pdata-->��Ҫ��������ݽṹ
 * ��  ��: ��
 *******************************************************************************/
static void Send_SpecifyDataPack(TRANS_DATAINFO* _pdata)
{
    _pdata->u8NewData = _pdata->GetCurDataFunc();

    if(_pdata->TimeOutStat == 1)
        _pdata->RevData = RECIEVE_NONE;
    
    if(_pdata->u8OldData != _pdata->u8NewData)//�а��������������ʱ
    {
        _pdata->u8OldData = _pdata->u8NewData;
        
        if(_pdata->u8NewData != 0)
        { 
             _pdata->TimeOutStat = 1;
            _pdata->SendStat = 1;
            _pdata->Databuf   = _pdata->u8NewData;
                
            (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; //�����
            Uart_SendString(Data_Package(_pdata), COM_PROTOCOL_LEN);
        }
    }
    else
    {
        if(_pdata->SendStat == 1)
        {
            if(_pdata->TimeOutCnt > _pdata->TimeOutTime)
            {
                _pdata->TimeOutCnt = 0;
            }
            else if(_pdata->TimeOutCnt < _pdata->TimeOutTime)//��ʼ�����Ƿ�ʱ
            {
                _pdata->TimeOutCnt++;
            }
            

            switch(_pdata->RevData)
            {
                case RECIEVE_ERROR:
                {
                    _pdata->TimeOutCnt = 0;              /* ��ʱ�������¿�ʼ */
                    _pdata->RepeatCnt = _pdata->RepeatTimes;
                    _pdata->RevData = RECIEVE_NONE;
                   (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; /* ����� */
                    Uart_SendString(Data_Package(_pdata), COM_PROTOCOL_LEN);
                }
                    break;
                case RECIEVE_CORRECT:
                {
                    _pdata->TimeOutStat = 1;
                    _pdata->SendStat = 0;
                    _pdata->RepeatCnt = _pdata->RepeatTimes;
                    _pdata->TimeOutCnt = 0;
                    _pdata->RevData = RECIEVE_NONE;
                }
                    break;
                case RECIEVE_NONE:
                {
                    if(_pdata->TimeOutCnt == _pdata->TimeOutTime)
                    {
                        _pdata->TimeOutCnt = 0;
                        if((_pdata->RepeatCnt > 0)&&(_pdata->RepeatCnt <= _pdata->RepeatTimes))
                        {
                            _pdata->TimeOutStat = 1;
                            _pdata->RepeatCnt--;
                            (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; //�����
                            Uart_SendString(Data_Package(_pdata), COM_PROTOCOL_LEN);
                        }
                        if(_pdata->RepeatCnt == 0)
                        {
                            _pdata->TimeOutStat = 1;
                            _pdata->SendStat = 0;
                            _pdata->RepeatCnt = _pdata->RepeatTimes;
                            _pdata->RevData = RECIEVE_NONE; 
                            return;
                        }  
                    }
                }
                    break;
                default:                                     
                    _pdata->TimeOutCnt = 0;                 /* ��ʱ�������¿�ʼ */
                    _pdata->RepeatCnt = _pdata->RepeatTimes;
                    _pdata->RevData = RECIEVE_NONE;
                   (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++;    /* ����� */
                    Uart_SendString(Data_Package(_pdata), COM_PROTOCOL_LEN);
                    break;
            }
            
        }
    }
}
/********************************************************************************
 * ������: void Transmit_DataPackToMPU(void)
 * ��  ��: ���Ͱ�����Encoder���ݸ�MPU.
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Transmit_DataPackToMPU(void)
{
    Send_SpecifyDataPack(&TransDataInfo[0]);  
    Send_SpecifyDataPack(&TransDataInfo[1]);
}
/********************************************************************************
 * ������: void LongPress_TransmitData(void)
 * ��  ��: ������ʱ����
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void LongPress_TransmitData(void)
{
    if(KeyInfo[0].IsKeyLong == KEY_LONG_PRESS||KeyInfo[1].IsKeyLong == KEY_LONG_PRESS||\
       KeyInfo[2].IsKeyLong == KEY_LONG_PRESS||KeyInfo[3].IsKeyLong == KEY_LONG_PRESS||\
       KeyInfo[4].IsKeyLong == KEY_LONG_PRESS||KeyInfo[5].IsKeyLong == KEY_LONG_PRESS)
    {
        (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; //�����
        Uart_SendString(Data_Package(&TransDataInfo[0]), COM_PROTOCOL_LEN);
    }

    else if(EcdKeyInfo.IsKeyLong == KEY_LONG_PRESS)
    {
        (DataPackNum == 255)? (DataPackNum = 1):DataPackNum++; //�����
        Uart_SendString(Data_Package(&TransDataInfo[1]), COM_PROTOCOL_LEN);    
    }
}

/********************************************************************************
 * ������: void SelfCheck_TransmitData(void)
 * ��  ��: ������ʱ����
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void SelfCheck_TransmitData(void)
{
	int i = 0;
	u8 selfDataPack[COM_PROTOCOL_LEN];
	
    selfDataPack[i++] = COM_PROTOCOL_DESADD;                  /* Ŀ�ĵ�ַ */
    selfDataPack[i++] = COM_PROTOCOL_SRCADD;                  /* Դ��ַ */ 
    selfDataPack[i++] = 0x03;                                 /* ����ʾ */
    selfDataPack[i++] = DataSelfNum;                          /* ����� */
    selfDataPack[i++] = COM_DATA_LEN;                         /* ���ݳ��� */
    selfDataPack[i++] = 0xAA;                                 /* �����ݶ� */
    selfDataPack[i] = crc8(selfDataPack+2, COM_PROTOCOL_LEN - 3); /* CRCУ���ֶ� */

	(DataSelfNum == 255)? (DataSelfNum = 1):DataSelfNum++; //�����
    Uart_SendString(selfDataPack, COM_PROTOCOL_LEN); 
}


/*******************************************************************************
 * ������: Uart1_get_pkt
 * ��  ��: �ӻ�������һ֡����
 * ��  ��: ��
 * ��  ��: ����ture ��ʾ �õ�һ������
 ******************************************************************************/

static uint8_t Uart1_Get_Pkt(void)
{
    uint8_t data;
	static uint8_t dataCnt = 0;

	while(1)
	{
		if(rbuf_get_data(&rUart1RxBuf, &data) == TRUE)
		{
			switch(analysisStatus)
			{
				case WaitDstAddr:
				{
					if(data == COM_PROTOCOL_SRCADD)
					{
						analysisData.m_DstAddr = data;
						analysisStatus = WaitSrcAddr;
					}
				}
				break;
				case WaitSrcAddr:
				{
					if(data == COM_PROTOCOL_DESADD)
					{
						analysisData.m_SrcAddr = data;
						analysisStatus = WaitType;
					}
					else
					{
						analysisStatus = WaitDstAddr;
					}
				}
				break;
				case WaitType:
				{
					analysisData.m_Type = data;
					analysisStatus = WaitSN;

				}
				break;
				case WaitSN:
				{
					analysisData.m_SN = data;
					analysisStatus = WaitLen;
				}
				break;			
			  case WaitLen:
				{
					if(DATA_BUF_LEN >= data)
					{
						analysisData.m_Len = data;	
						if(data > 0)
						{
					 	 	analysisStatus = WaitData;
						}
						else
						{
							analysisStatus = WaitCRC;
						}
					}
					else
					{
						analysisStatus = WaitDstAddr;
					}

				}
				break;
				case WaitData:
				{
					if(dataCnt < analysisData.m_Len)
					{
							analysisData.m_Data[dataCnt++] = data;
							if(dataCnt == analysisData.m_Len)
							{
								analysisStatus = WaitCRC;
								dataCnt = 0;
							}
							else
							{
								analysisStatus = WaitData;	
							}
					}
				}
				break;
				case WaitCRC:
				{
					analysisStatus = WaitDstAddr;
					analysisData.m_CRC = data;
					if(analysisData.m_CRC == KEY_CRC8(&analysisData))
					{
						return TRUE;
					}
					else
					{
//						printf("%x %x %x %x %x ", analysisData.m_DstAddr, analysisData.m_SrcAddr, analysisData.m_SN, analysisData.m_Type, analysisData.m_Len);
					}
				}
				break;	
				default:
				{
					analysisStatus = WaitDstAddr;
				}
				break;
			}		
		}
		else
		{
			return FALSE;               //������û�������˳���
		}
	}
} 

/*
static u8 Uart1_get_pkt(void)
{
    u8 data;
	
    for (;;)
	{//��buffer. * pout��,ȡ���ݵ� * data��,���ݲ�Ϊ�շ���true��һ��ȡһ��
        if (rbuf_get_data(&rUart1RxBuf, &data) == true) 
		{			
  			if (RxCounter < UART1_RX_BUF_SIZE) 
			{ 
				RxBuffer[RxCounter++] = data;			
				if(RxCounter >=2 &&(RxBuffer[RxCounter - 1] == COM_PROTOCOL_DESADD))  //�жϰ�ͷ�Ƿ���ȷ
				{
					if(RxBuffer[RxCounter - 2] == COM_PROTOCOL_SRCADD)
					{ 
						//�õ��˰�ͷ
						RxBuffer[0] = COM_PROTOCOL_SRCADD;
						RxBuffer[1] = COM_PROTOCOL_DESADD;
						RxCounter = 2;
					}
				}
  			}
         	else
		 	{
				RxCounter = 0;
         	}   			
            if (RxBuffer[0] == COM_PROTOCOL_SRCADD && (RxCounter == 7) && RxBuffer[RxCounter-1]== crc8(RxBuffer+2, COM_PROTOCOL_LEN -3)) 
			{                           // got a packet
                RxBuffer[RxCounter] = 0;
				RxCounter = 0;      //�õ�һ֡���ݺ�������� 
		   		return true;
            }			         			
      
        } 
		else   //û�����ݷ��� 
		{
			return false;		
        }
    }
}
*/

/*******************************************************************************
 * ������:   UartData_Analysia
 * ��  ��: ����MPU ���͵Ĵ�������
 * ��  ��: ��
 * ��  ��: ��
 ******************************************************************************/
void UartData_Analysia(void)
{
	if(Uart1_Get_Pkt() == TRUE)   
	{     // got a packet
            switch(analysisData.m_Type)
            {
               case KEY_PACK_ID:
                      TransDataInfo[0].RevData = analysisData.m_Data[0];
                      TransDataInfo[0].TimeOutStat = 0;
                    break;
               case ENCODER_PACK_ID:
                      TransDataInfo[1].RevData = analysisData.m_Data[0];
                      TransDataInfo[1].TimeOutStat = 0;
                    break;
				case ALARM_LED_ID:
				{
					if(analysisData.m_Data[0] == ALARM_LED_OFF)
					{
						Led_Off();
						Uart_SendString(DataAlarmLed_Package(analysisData.m_Data[0]),COM_PROTOCOL_LEN);
					}
					else if(analysisData.m_Data[0] == ALARM_LED_ON)
					{
						Led_On();
				    	Uart_SendString(DataAlarmLed_Package(analysisData.m_Data[0]),COM_PROTOCOL_LEN);
					}	
				}
				break;
				case VERSION_CMD: //add by xxw
				{
					Uart_SendString(Version_Package(VERSION_CMD), PACKGE_LEN);
					break;
				}
               default:
                      TransDataInfo[0].TimeOutStat = 1;
                      TransDataInfo[1].TimeOutStat = 1;
                    break;              
            }
	}	
}

#undef _UART_C_

