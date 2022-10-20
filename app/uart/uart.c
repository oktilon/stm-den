#include "uart.h"

#define RBUF_SIZE   256     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define TBUF_SIZE   256     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

struct buf_st {
    u16 in;                 // Next In Index
    u16 out;                // Next Out Index
    u8 buf [RBUF_SIZE];     // Buffer
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((u16)(rbuf.in - rbuf.out))

static struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((u16)(tbuf.in - tbuf.out))

static unsigned int tx_restart = 1;     // NZ if TX restart is required

static void buffer_Init (void) {
    tbuf.in = 0;
    tbuf.out = 0;
    tx_restart = 1;

    rbuf.in = 0;
    rbuf.out = 0;
}

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
    buffer_Init();
    Uart_Gpio_init();
    Uart_main_init();
}

void ESP_UART_IRQ_Handler(void) {
    volatile unsigned int IIR;
    struct buf_st *p;

    IIR = ESP_UART->SR;
    if (IIR & USART_FLAG_RXNE) {                  // read interrupt
        ESP_UART->SR &= ~USART_FLAG_RXNE;	          // clear interrupt

        p = &rbuf;

        if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
            p->buf [p->in & (RBUF_SIZE-1)] = (ESP_UART->DR & 0x1FF);
            p->in++;
        }
    }

    if (IIR & USART_FLAG_TXE) {
        ESP_UART->SR &= ~USART_FLAG_TXE;	          // clear interrupt

        p = &tbuf;

        if (p->in != p->out) {
            ESP_UART->DR = (p->buf [p->out & (TBUF_SIZE-1)] & 0x1FF);
            p->out++;
            tx_restart = 0;
        } else {
            tx_restart = 1;
            ESP_UART->CR1 &= ~USART_FLAG_TXE;		      // disable TX interrupt if nothing to send
        }
    }
}

int UART_SendByte(u8 c) {
  struct buf_st *p = &tbuf;

    // If the buffer is full, return an error value
    if (SIO_TBUFLEN >= TBUF_SIZE)
        return (-1);

    // Add data to the transmit buffer.
    p->buf [p->in & (TBUF_SIZE - 1)] = c;
    p->in++;

    // If transmit interrupt is disabled, enable it
    if (tx_restart) {
        tx_restart = 0;
        // enable TX interrupt
        ESP_UART->CR1 |= USART_FLAG_TXE;
    }

    return (0);
}

int UART_SendWord(u32 word) {
    u32 w = word;
    struct buf_st *p = &tbuf;

    // If the buffer is full, return an error value
    if (SIO_TBUFLEN >= TBUF_SIZE)
        return (-1);

    // Add data to the transmit buffer.
    for(int i = 0; i < 4; i++) {
        u8 c = (w & 0xFF000000) >> 24;
        p->buf [p->in & (TBUF_SIZE - 1)] = c;
        p->in++;
        w = w << 8;
    }

    // If transmit interrupt is disabled, enable it
    if (tx_restart) {
        tx_restart = 0;
        // enable TX interrupt
        ESP_UART->CR1 |= USART_FLAG_TXE;
    }

    return (0);
}

int UART_SendBuffer(u8 *buf, u8 len) {
    struct buf_st *p = &tbuf;

    // If the buffer is full, return an error value
    if (SIO_TBUFLEN >= TBUF_SIZE)
        return (-1);

    // Add data to the transmit buffer.
    for(int i = 0; i < len; i++) {
        p->buf [p->in & (TBUF_SIZE - 1)] = buf[i];
        p->in++;
    }

    // If transmit interrupt is disabled, enable it
    if (tx_restart) {
        tx_restart = 0;
        // enable TX interrupt
        ESP_UART->CR1 |= USART_FLAG_TXE;
    }

    return (0);
}

int UART_SendString(char *buf, u8 len) {
    struct buf_st *p = &tbuf;

    // If the buffer is full, return an error value
    if (SIO_TBUFLEN >= TBUF_SIZE)
        return (-1);

    // Add data to the transmit buffer.
    for(int i = 0; i < len; i++) {
        p->buf [p->in & (TBUF_SIZE - 1)] = (u8)(buf[i]);
        p->in++;
    }

    // If transmit interrupt is disabled, enable it
    if (tx_restart) {
        tx_restart = 0;
        // enable TX interrupt
        ESP_UART->CR1 |= USART_FLAG_TXE;
    }

    return (0);
}


/*------------------------------------------------------------------------------
  GetKey
  receive a character
 *------------------------------------------------------------------------------*/
int UART_GetByte(void) {
    struct buf_st *p = &rbuf;

    if (SIO_RBUFLEN == 0)
        return (-1);

    return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

u32 UART_GetCommand(void) {
    u32 ret = 0;
    int i = 0;
    struct buf_st *p = &rbuf;

    if (SIO_RBUFLEN == 0)
        return (0);

    for(i = 0; i < 4; i++) {
        ret = ret << 8;
        ret |= (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
    }
    return ret;
}

u16 UART_GetDataSize(void) {
    return SIO_RBUFLEN;
}