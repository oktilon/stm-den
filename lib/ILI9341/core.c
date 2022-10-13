#include "core.h"

RCC_ClocksTypeDef RCC_Clocks;

static u16 screen_width  = LCD_PIXEL_WIDTH,
    screen_height = LCD_PIXEL_HEIGHT;

//<editor-fold desc="Init commands">

static const uint8_t init_commands[] = {
        // Power control A
        6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
        // Power control B
        4, LCD_POWERB, 0x00, 0xC1, 0x30,
        // Driver timing control A
        4, LCD_DTCA, 0x85, 0x00, 0x78,
        // Driver timing control B
        3, LCD_DTCB, 0x00, 0x00,
        // Power on sequence control
        5, LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,
        // Pump ratio control
        2, LCD_PRC, 0x20,
        // Power control 1
        2, LCD_POWER1, 0x23,
        // Power control 2
        2, LCD_POWER2, 0x10,
        // VCOM control 1
        3, LCD_VCOM1, 0x3E, 0x28,
        // VCOM cotnrol 2
        2, LCD_VCOM2, 0x86,
        // Memory access control
        2, LCD_MAC, 0x48,
        // Pixel format set
        2, LCD_PIXEL_FORMAT, 0x55,
        // Frame rate control
        3, LCD_FRMCTR1, 0x00, 0x18,
        // Display function control
        4, LCD_DFC, 0x08, 0x82, 0x27,
        // 3Gamma function disable
        2, LCD_3GAMMA_EN, 0x00,
        // Gamma curve selected
        2, LCD_GAMMA, 0x01,
        // Set positive gamma
        16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        16, LCD_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        0
};

void system_clock_init(void) {
    SystemCoreClockUpdate();
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();
    RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 4); // Main delimiter, Main multiplier, Delimiter, AHB Delimiter
    RCC->CFGR = 0;
    RCC->CFGR = (RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_HPRE_DIV1);
    RCC_PLLCmd(ENABLE);
    int limit = 1000;
    while((RCC->CR & RCC_CR_PLLRDY) == 0 && limit > 0);                       // wait pll ready
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    RCC->DCKCFGR |=  RCC_DCKCFGR_TIMPRE;                         // clock timer

    // RCC_PCLK2Config(RCC_HCLK_Div2);  // From source variant

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // SPI1 pin's
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // UART4 pin's
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // LED pin's
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}

void systick_init(void) {
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}

//</editor-fold>

//<editor-fold desc="LCD initialization functions">

static void LCD_pinsInit() {
    SPI_InitTypeDef  spiStructure;
    GPIO_InitTypeDef gpioStructure;

    // GPIO speed by default
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // GPIO for CS/DC/LED/RESET
    gpioStructure.GPIO_Pin  = TFT_CS_PIN | TFT_DC_PIN | TFT_RESET_PIN | TFT_LED_PIN;
    gpioStructure.GPIO_Mode = GPIO_Mode_OUT; // GPIO_Mode_Out_PP;
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &gpioStructure);

    // GPIO for SPI
    gpioStructure.GPIO_Pin  = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF; // GPIO_Mode_AF_PP
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(SPI_MASTER_GPIO, &gpioStructure);

    // GPIO for SPI
    gpioStructure.GPIO_Pin  = SPI_MASTER_PIN_MISO;
    gpioStructure.GPIO_Mode = GPIO_Mode_IN; // GPIO_Mode_IPD
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    gpioStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(SPI_MASTER_GPIO, &gpioStructure);

    // SPI
    SPI_StructInit(&spiStructure);
    spiStructure.SPI_Mode              = SPI_Mode_Master;
    spiStructure.SPI_NSS               = SPI_NSS_Soft;
    // spiStructure.SPI_NSS               = SPI_NSS_Hard;
    spiStructure.SPI_CPOL              = SPI_CPOL_High;
    spiStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    spiStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_Init(SPI_MASTER, &spiStructure);

    SPI_Cmd(SPI_MASTER, ENABLE);
}

void GPIO_init() {
    GPIO_InitTypeDef gpioStructure;

    // LEDs on PD12-PD15
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    gpioStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    gpioStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &gpioStructure);

    // GPIO for UART4
    // PC11 - UART4 Rx
    // PC10 - UART4 Tx
    gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;
    gpioStructure.GPIO_Pin  = GPIO_Pin_11 | GPIO_Pin_10;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF;
    gpioStructure.GPIO_OType = GPIO_OType_PP;
    gpioStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &gpioStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
}

void UART_init() {
    USART_InitTypeDef usartInit;

    USART_DeInit(UART4);

    usartInit.USART_BaudRate = 115200;
    usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartInit.USART_Parity = USART_Parity_No;
    usartInit.USART_StopBits = USART_StopBits_1;
    usartInit.USART_WordLength = USART_WordLength_8b;
    USART_Init(UART4, &usartInit);

    UART4->CR1 |= USART_CR1_RXNEIE;
    
    // USART_ClockInitTypeDef usartClock;
    // usartClock.USART_Clock = 0;
    USART_Cmd(UART4, ENABLE);


    NVIC_ClearPendingIRQ(UART4_IRQn);
    NVIC_SetPriority(UART4_IRQn, 2);
    NVIC_EnableIRQ(UART4_IRQn);
    


    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void LCD_reset() {
    TFT_RST_RESET;
    delay_ms(10);
    TFT_RST_SET;
    delay_ms(50);
}

void LCD_exitStandby() {
    dmaSendCmd(LCD_SLEEP_OUT);
    delay_ms(150);
    dmaSendCmd(LCD_DISPLAY_ON);
}

static void LCD_configure() {
    u8 count;
    u8 *address = (u8 *) init_commands;

    TFT_CS_RESET;
    while (1) {
        count = *(address++);
        if (count-- == 0) break;
        dmaSendCmdCont(*(address++));
        dmaSendDataCont8(address, count);
        address += count;
    }
    TFT_CS_SET;

    LCD_setOrientation(0);
}

void LCD_init() {
    SystemInit();
    system_clock_init();
    systick_init();
    LCD_pinsInit();
    dmaInit();

    LCD_reset();
    LCD_exitStandby();
    LCD_configure();

    TFT_LED_SET;
}

//</editor-fold>

//<editor-fold desc="LCD common operations">

void LCD_setOrientation(u8 o) {
    if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR) {
        screen_height = LCD_PIXEL_WIDTH;
        screen_width  = LCD_PIXEL_HEIGHT;
    } else {
        screen_height = LCD_PIXEL_HEIGHT;
        screen_width  = LCD_PIXEL_WIDTH;
    }
    TFT_CS_RESET;
    dmaSendCmdCont(LCD_MAC);
    dmaSendDataCont8(&o, 1);
    TFT_CS_SET;
}

inline void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2) {
    u16 pointData[2];

    TFT_CS_RESET;
    dmaSendCmdCont(LCD_COLUMN_ADDR);
    pointData[0] = x1;
    pointData[1] = x2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();

    dmaSendCmdCont(LCD_PAGE_ADDR);
    pointData[0] = y1;
    pointData[1] = y2;
    LCD_setSpi16();
    dmaSendDataCont16(pointData, 2);
    LCD_setSpi8();
    TFT_CS_SET;
}

inline u16 LCD_getWidth() {
    return screen_width;
}

inline u16 LCD_getHeight() {
    return screen_height;
}

//</editor-fold>

//<editor-fold desc="SPI functions">

inline void LCD_setSpi8(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR1 &= ~SPI_CR1_DFF; // SPI 8
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

inline void LCD_setSpi16(void) {
    SPI_MASTER->CR1 &= ~SPI_CR1_SPE; // DISABLE SPI
    SPI_MASTER->CR1 |= SPI_CR1_DFF;  // SPI 16
    SPI_MASTER->CR1 |= SPI_CR1_SPE;  // ENABLE SPI
}

// </editor-fold>


void UART4_IRQHandler(void) {
    __NOP();
}