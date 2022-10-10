#ifndef TEST2_ENCODER_H
#define TEST2_ENCODER_H

#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>

#define ENC_CLK_PIN GPIO_Pin_7
#define ENC_DT_PIN  GPIO_Pin_6
#define ENC_SW_PIN  GPIO_Pin_9
#define ENCODER_GPIOx GPIOB
#define ENCODER_GPIO_RCC RCC_AHB1Periph_GPIOB
#define ENCODER_TIMx TIM4
#define ENCODER_TIM_RCC RCC_APB1Periph_TIM4

u16  ENC_getValue();
void ENC_init();

#endif //TEST2_ENCODER_H
