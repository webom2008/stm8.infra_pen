/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : Key.c
* Function  : Collect status of six keys
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/04
*********************************************************************************************************/
#define _KEY_C_

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "key.h"
#include "encoder.h"
#include "timer.h"


/* Private define ------------------------------------------------------------*/

/* Evalboard I/Os configuration */
#define KEYS_PORT_B (GPIOB)
#define KEYS_PORT_C (GPIOC)
//PORTB
#define KEY1_PIN    (GPIO_PIN_5) //pb5
#define KEY2_PIN    (GPIO_PIN_4) //pc4
//PORTC
#define KEY3_PIN    (GPIO_PIN_4)//pb4
#define KEY4_PIN    (GPIO_PIN_5)//pc5
#define KEY5_PIN    (GPIO_PIN_3)//pc3
#define KEY6_PIN    (GPIO_PIN_6)//pc6

#define KEY_OFF     0
#define KEY_ON      1
#define KEY_NUM     6

#define KEY_DOWN_FILTER_CNT 2   /* �������������˲�Ϊ10ms����ʱ��ÿ�ж�һ��Ϊ5ms */ 
#define KEY_UP_FILTER_CNT   10  /* �������������˲�Ϊ50ms*/
#define KEY_LONGPRESS_CNT   400 /* ���������˲�Ϊ2s */

static bool IsDownKey1(void) {if((GPIO_ReadInputData(KEYS_PORT_B) & KEY1_PIN) == 0x00) return TRUE; return FALSE;}
static bool IsDownKey2(void) {if((GPIO_ReadInputData(KEYS_PORT_C) & KEY2_PIN) == 0x00) return TRUE; return FALSE;}
static bool IsDownKey3(void) {if((GPIO_ReadInputData(KEYS_PORT_B) & KEY3_PIN) == 0x00) return TRUE; return FALSE;}
static bool IsDownKey4(void) {if((GPIO_ReadInputData(KEYS_PORT_C) & KEY4_PIN) == 0x00) return TRUE; return FALSE;}
static bool IsDownKey5(void) {if((GPIO_ReadInputData(KEYS_PORT_C) & KEY5_PIN) == 0x00) return TRUE; return FALSE;}
static bool IsDownKey6(void) {if((GPIO_ReadInputData(KEYS_PORT_C) & KEY6_PIN) == 0x00) return TRUE; return FALSE;}


KEY_INFO KeyInfo[KEY_NUM] = 
{
    {IsDownKey1, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},
    {IsDownKey2, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},
    {IsDownKey3, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},
    {IsDownKey4, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},
    {IsDownKey5, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},
    {IsDownKey6, 0, KEY_DOWN_FILTER_CNT, KEY_UP_FILTER_CNT, KEY_UP_FILTER_CNT, 0, KEY_LONGPRESS_CNT, 0, KEY_UP, KEY_LONG_PRESS_NONE},    
};

void KeyGpio_Init(void)
{
    GPIO_Init(KEYS_PORT_B, (KEY1_PIN|KEY3_PIN), GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(KEYS_PORT_C, (KEY2_PIN|KEY4_PIN|KEY5_PIN|KEY6_PIN), GPIO_MODE_IN_FL_NO_IT);
}
/********************************************************************************
 * ������: Key_Init(void)
 * ��  ��: ������ʼ��
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Key_Init(void)
{
    KeyGpio_Init();
}

/********************************************************************************
 * ������: void DetectKey(KEY_INFO*_pKey)
 * ��  ��: ��ⰴ��״̬
 * ��  ��: �����ṹ����ָ��
 * ��  ��: ��
 *******************************************************************************/
static void DetectKey(KEY_INFO*_pKey)
{
    if (_pKey->IsKeyDownFunc())
    {
        if (_pKey->DownFilterCnt > _pKey->DownFilterTime)
        {
            _pKey->DownFilterCnt = 0;
        }
        else if (_pKey->DownFilterCnt < _pKey->DownFilterTime)	//�˲��������� <  �˲�ʱ��
        {
            _pKey->DownFilterCnt++;
        }
        else
        {
            if (_pKey->KeyState == 0)
            {
                _pKey->KeyState = 1;
                /* ���ð������������ļ���ֵ */
                _pKey->UpFilterCnt = _pKey->UpFilterTime;
            
            }
        
            if (_pKey->LongTime > 0)	  // �жϳ�����
            {
                if (_pKey->LongCnt < _pKey->LongTime)
                {
                    /* ��ⰴ������ */
                    if ((++_pKey->LongCnt == _pKey->LongTime)&&(_pKey->IsKeyLong != KEY_LONG_PRESS))
                    {
                        _pKey->IsKeyLong = KEY_LONG_PRESS;
                    
                        if (_pKey->IsKeyDown != KEY_DOWN)
                        {
                            _pKey->IsKeyDown = KEY_DOWN;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(_pKey->UpFilterCnt > _pKey->UpFilterTime)
        {
            _pKey->UpFilterCnt = _pKey->UpFilterTime;
        }
        if(_pKey->UpFilterCnt > 0) //�������������˲�
        {
            _pKey->UpFilterCnt--;
        }
        else
        {
            if (_pKey->KeyState == 1)
            {
                _pKey->KeyState = 0;
            
                if (_pKey->IsKeyDown != KEY_DOWN)
                {
                    _pKey->IsKeyDown = KEY_DOWN;
                }
            
                if (_pKey->IsKeyLong == KEY_LONG_PRESS)
                {
                    _pKey->IsKeyLong = KEY_LONG_PRESS_NONE;
                    _pKey->IsKeyDown = KEY_UP;
                }
                
                _pKey->DownFilterCnt = 0;
                _pKey->LongCnt = 0;
                return ;
            }
        
        }
        
        if(_pKey->KeyState == 0)    //��ֹ�����ɿ�������״̬���ı�
        {
            _pKey->IsKeyLong = KEY_LONG_PRESS_NONE;
            _pKey->IsKeyDown = KEY_UP;
        }
        _pKey->DownFilterCnt = 0;
        _pKey->LongCnt = 0;
    }
}

/********************************************************************************
 * ������: void Keys_Scan(void)
 * ��  ��: ������а���״̬
 * ��  ��: ��
 * ��  ��: ��
 *******************************************************************************/
void Keys_Scan(void)
{
    u8 i;

    for(i = 0; i < KEY_NUM; i++)
        DetectKey(&KeyInfo[i]);
    DetectKey(&EcdKeyInfo);
}
/********************************************************************************
 * ������: u8 Get_KeyCurData(void)
 * ��  ��: ��ȡ������״̬����
 * ��  ��: ��
 * ��  ��: ����״̬����
 *******************************************************************************/
u8 Get_KeyCurData(void)
{
    u8 i;
    u8 NewKeyData = 0;

    for(i = 0; i < KEY_NUM; i++)
    {
        if(KeyInfo[i].IsKeyDown == KEY_DOWN)
            NewKeyData |= (0x01 << i);
        else
            NewKeyData &= ~(0x01 << i);
    }  

    /* 6����������һ�������������ͱ�ʾ����λ */
    if(KeyInfo[0].IsKeyLong == KEY_LONG_PRESS||KeyInfo[1].IsKeyLong == KEY_LONG_PRESS||\
       KeyInfo[2].IsKeyLong == KEY_LONG_PRESS||KeyInfo[3].IsKeyLong == KEY_LONG_PRESS||\
       KeyInfo[4].IsKeyLong == KEY_LONG_PRESS||KeyInfo[5].IsKeyLong == KEY_LONG_PRESS)
    {
        NewKeyData |= 0x80;
    }
    else
    {
        NewKeyData &= ~(0x80);
    }
    
    return NewKeyData;
}


#undef _KEY_C_

