#include "uart.h"

static void Uart_Gpio_init() {
    GPIO_InitTypeDef gpioStructure;

    gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;
    gpioStructure.GPIO_Pin   = ESP_RX_PIN | ESP_TX_PIN;
    gpioStructure.GPIO_Mode  = GPIO_Mode_AF;
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    gpioStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(ESP_UART_GPIO, &gpioStructure);
    GPIO_PinAFConfig(ESP_UART_GPIO, ESP_RX_SRC, GPIO_AF_UART4);
    GPIO_PinAFConfig(ESP_UART_GPIO, ESP_TX_SRC, GPIO_AF_UART4);
}

static void Uart_main_init() {
    USART_InitTypeDef usartInit;

    USART_DeInit(ESP_UART);

    usartInit.USART_BaudRate            = 115200;
    usartInit.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartInit.USART_Parity              = USART_Parity_No;
    usartInit.USART_StopBits            = USART_StopBits_1;
    usartInit.USART_WordLength          = USART_WordLength_8b;
    USART_Init(ESP_UART, &usartInit);

    ESP_UART->CR1 |= USART_CR1_RXNEIE;

    USART_Cmd(ESP_UART, ENABLE);


    NVIC_ClearPendingIRQ(ESP_UART_IRQn);
    NVIC_SetPriority(ESP_UART_IRQn, 2);
    NVIC_EnableIRQ(ESP_UART_IRQn);

    USART_ITConfig(ESP_UART, USART_IT_RXNE, ENABLE);
}

void UART_init(void) {
    Uart_Gpio_init();
    Uart_main_init();
}

void ESP_UART_IRQ_Handler(void) {
    __NOP();
}