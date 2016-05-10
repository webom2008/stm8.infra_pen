/**
  ******************************************************************************
  * @file    GPIO/GPIO_Toggle/main.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_PERIOD                    104
#define TIM1_PRESCALER                 0
#define TIM1_REPTETION_COUNTER         0

#define CCR1_Val                     52

#define DEADTIME                        1
#define TIM4_PERIOD                   255

#define TIM2_PERIOD                     104
#define TIM2_PULSE                      52
    
/* define the GPIO port and pins connected to Leds mounted on STM8L152X-EVAL board */
#define LED_GPIO_PORT       GPIOE
#define LED_GPIO_PIN        GPIO_Pin_7
#define BLUE_GPIO_PORT      GPIOC
#define BLUE_GPIO_PIN       GPIO_Pin_7
#define KEY_GPIO_PORT       GPIOB
#define KEY_GPIO_PIN        GPIO_Pin_4
#define KEY_EXTI_PIN        EXTI_Pin_4
#define KEY1_GPIO_PORT      GPIOB
#define KEY1_GPIO_PIN       GPIO_Pin_5
#define KEY1_EXTI_PIN       EXTI_Pin_5


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t g_Timer_Count = 0;
uint32_t g_Send_Count = 0;
/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);
static void CLK_Config(void);
static void GPIO_Config(void);
void TIM1_Config(void);
static void TIM4_Config(void);
void TIM2_Config(void);
/* Private functions ---------------------------------------------------------*/

extern uint32_t TimeCounter;

static void CLK_Config(void)
{
    /* Select HSI as system clock source */
    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    /* system clock prescaler: 2*/
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);
    while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
    {}

    /* Enable I2C1 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);

    /* Enable TIM2 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    
    /* Enable TIM3 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

    /* Enable TIM4 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
}

/**
  * @brief  Configure TIM Channels GPIO 
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
    //LED gpio init
//    GPIO_Init(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
//    GPIO_Init(BLUE_GPIO_PORT, BLUE_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
    
    //IR send pin init
    GPIO_Init(IR_GPIO_SEND_PORT, IR_GPIO_SEND_PIN, GPIO_Mode_Out_PP_Low_Fast);
        
    //IR receive pin init
    GPIO_Init(IR_GPIO_REV_PORT, IR_GPIO_REV_PIN, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(IR_REV_EXTI_PIN, EXTI_Trigger_Falling);
    
    //Key pin init
    GPIO_Init(KEY_GPIO_PORT, KEY_GPIO_PIN, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(KEY_EXTI_PIN, EXTI_Trigger_Falling);
    
    GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PIN, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(KEY1_EXTI_PIN, EXTI_Trigger_Falling);
}

/**
  * @brief  Configure TIM1 peripheral 
  * @param  None
  * @retval None
  */
void TIM1_Config(void)
{
  TIM1_DeInit();
  
  /* Time Base configuration */
  TIM1_TimeBaseInit(TIM1_PRESCALER, TIM1_CounterMode_Up, TIM1_PERIOD, TIM1_REPTETION_COUNTER);

  /* Channels 1, 2 and 3 Configuration in TIMING mode */
  /* Toggle Mode configuration: Channel1 */
  TIM1_OC1Init(TIM1_OCMode_Toggle, TIM1_OutputState_Enable, TIM1_OutputNState_Disable,
               CCR1_Val, TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set,
               TIM1_OCNIdleState_Set);
  TIM1_OC1PreloadConfig(DISABLE);

  TIM1_ARRPreloadConfig(ENABLE);

  /* Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
  /* TIM1 counter disable */
  TIM1_Cmd(DISABLE);
}

void TIM2_Config(void)
{
  /* TIM2 configuration:
     - TIM2 ETR is mapped to LSE
     - TIM2 counter is clocked by LSE div 4
      so the TIM2 counter clock used is LSE / 4 = 32.768 / 4 = 8.192 KHz
    TIM2 Channel1 output frequency = TIM2CLK / (TIM2 Prescaler * (TIM2_PERIOD + 1))
                                   = 8192 / (1 * 8) = 1024 Hz */
  /* Time Base configuration */
  TIM2_TimeBaseInit(TIM2_Prescaler_2, TIM2_CounterMode_Up, TIM2_PERIOD);

  /* Channel 1 configuration in PWM1 mode */
  /* TIM2 channel Duty cycle is 100 * TIM2_PULSE / (TIM2_PERIOD + 1) = 100 * 4/8 = 50 % */
  TIM2_OC2Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, TIM2_PULSE, TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);

  /* TIM2 Main Output Enable */
  TIM2_CtrlPWMOutputs(ENABLE);

  /* TIM2 counter enable */
  TIM2_Cmd(DISABLE);
}

/**
  * @brief  Configure TIM4 peripheral 
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* Time Base configuration */
  //TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);
  TIM4_TimeBaseInit(TIM4_Prescaler_4, 99);
  /* TIM4 update interrupt enable */
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  /* TIM4 counter enable */
  TIM4_Cmd(ENABLE);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    CLK_Config();

    SysTick_Init();
    
    TIM2_Config();

    TIM4_Config();
    
    GPIO_Config();
    
    MMA8652_Init();

    MMA8652_ConfigTransient(64, 100);
    
    ClearQueue(pIrQueue);
    
    /* Enable general interrupts */
    enableInterrupts();

    SysTick_Start();
    
    while(1)
    {
        MMA8652_server();
        IrNec_server();
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint16_t nCount)
{
  /* Decrement nCount value */
//  nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
//  nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
//  nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
//  nop();nop();nop();
  while (nCount != 0)
  {
    nCount--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
