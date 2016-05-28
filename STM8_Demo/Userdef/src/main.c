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
/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);
static void CLK_Config(void);
static void GPIO_Config(void);
/* Private functions ---------------------------------------------------------*/

static void CLK_Config(void)
{
    /* Select HSI as system clock source */
    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    /* system clock prescaler: 2*/
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);
    while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
    {}
}

/**
  * @brief  Configure TIM Channels GPIO 
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
    //Key pin init
    GPIO_Init(KEY_GPIO_PORT, KEY_GPIO_PIN, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(KEY_EXTI_PIN, EXTI_Trigger_Falling);
    
    GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PIN, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(KEY1_EXTI_PIN, EXTI_Trigger_Falling);
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

    Infrared_Init();
    
    GPIO_Config();
    
    MMA8652_Init();

    MMA8652_ConfigTransient(64, 100);
        
    /* Enable general interrupts */
    enableInterrupts();

    SysTick_Start();
    
    while(1)
    {
        MMA8652_server();
        Infrared_server();
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
