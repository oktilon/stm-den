#ifndef TEST1_ILI9341_CORE_H
#define TEST1_ILI9341_CORE_H

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>
#include "delay.h"
#include "commands.h"
#include "dma.h"

#define LCD_setAddressWindowToWrite(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    dmaSendCmd(LCD_GRAM)

#define LCD_setAddressWindowToRead(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    dmaSendCmd(LCD_RAMRD)

void system_clock_init(void);
void systick_init(void);

u16 LCD_getWidth();
u16 LCD_getHeight();

void LCD_init();
void GPIO_init();

void LCD_setSpi8(void);
void LCD_setSpi16(void);

void LCD_setOrientation(u8 o);
void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2);

#endif //TEST1_ILI9341_CORE_H
