#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

#define ESP_UART    UART4

#define ESP_UART_GPIO GPIOC
// PC11 - UART4 Rx
#define ESP_RX_PIN  GPIO_Pin_11
#define ESP_RX_SRC  GPIO_PinSource11
// PC10 - UART4 Tx
#define ESP_TX_PIN  GPIO_Pin_10
#define ESP_TX_SRC  GPIO_PinSource10

#define ESP_UART_IRQn UART4_IRQn
#define ESP_UART_IRQ_Handler  UART4_IRQHandler

void UART_init(void);
int UART_SendByte(u8 c);
int UART_GetByte(void);
u16 UART_GetDataSize(void);

void ESP_UART_IRQ_Handler(void);

#endif //__UART_H