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

    KEY_UP,			                /* 按键松开 */
	KEY_DOWN,			            /* 按键按下 */
	KEY_LONG_PRESS_NONE,            /* 按键无长按 */    
	KEY_LONG_PRESS,			        /* 按键长按 */
}KEY_STATUS;


typedef struct
{
	bool (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */
	u8   DownFilterCnt;			 /* 按键按下滤波计数器 */
	u8   DownFilterTime;		 /* 按键消抖时间 */
	u8   UpFilterCnt;            /* 按键弹开滤波计数器 */
	u8   UpFilterTime;           /* 按键弹开时间 */
	u8   LongCnt;		         /* 长按计数器 */
	u8   LongTime;		         /* 按键按下持续时间, 0表示不检测长按 */
	u8   KeyState;			     /* 记录按键当前状态 1:按下，0:弹起 */
	KEY_STATUS IsKeyDown;	     /* 按键按下标志 */
	KEY_STATUS IsKeyLong;	     /* 按键长按标志 */
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
