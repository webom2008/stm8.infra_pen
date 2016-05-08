/*********************************************************************************************************
*                                Guangzhou CVTE electronics Co.,LTD.                                          
*                                            Key_Board
* Filename  : Key.h
* Function  : Collect status of six keys
* Version   : V1.00
* Author    : Liudingbang 
* Date      : 2013/01/04
*********************************************************************************************************/
#ifndef __KEY_H_
#define __KEY_H_

#define KEY_NUM     6

typedef enum _KEY_STATUS
{
	KEY_NONE = 0,			

    KEY_UP,			                /* �����ɿ� */
	KEY_DOWN,			            /* �������� */
	KEY_LONG_PRESS_NONE,            /* �����޳��� */    
	KEY_LONG_PRESS,			        /* �������� */
}KEY_STATUS;


typedef struct
{
	bool (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */
	u8   DownFilterCnt;			 /* ���������˲������� */
	u8   DownFilterTime;		 /* ��������ʱ�� */
	u8   UpFilterCnt;            /* ���������˲������� */
	u8   UpFilterTime;           /* ��������ʱ�� */
	u8   LongCnt;		         /* ���������� */
	u8   LongTime;		         /* �������³���ʱ��, 0��ʾ����ⳤ�� */
	u8   KeyState;			     /* ��¼������ǰ״̬ 1:���£�0:���� */
	KEY_STATUS IsKeyDown;	     /* �������±�־ */
	KEY_STATUS IsKeyLong;	     /* ����������־ */
}KEY_INFO;


#ifdef _KEY_C_
#define INTERFACE
#else
#define INTERFACE extern 
#endif


INTERFACE u8 u8OldKeyData;
INTERFACE u8 u8NewKeyData;
INTERFACE KEY_INFO KeyInfo[KEY_NUM];

INTERFACE void Key_Init(void);
INTERFACE u8 Get_KeyCurData(void);
INTERFACE void DetectKey(KEY_INFO*_pKey);
INTERFACE void Keys_Scan(void);

#undef INTERFACE

#endif
