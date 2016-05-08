#include "led.h"
#include "stm8s.h"

#define LED_PORT (GPIOA)
#define LED_PIN  (GPIO_PIN_3)


void LedGpio_Init(void)
{ 
  GPIO_Init(LED_PORT,LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);/* Output */
}


void Led_On(void)
{
  GPIO_WriteHigh(LED_PORT, LED_PIN);
}

void Led_Off(void)
{
  GPIO_WriteLow(LED_PORT, LED_PIN);
}











