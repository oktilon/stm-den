#ifndef __BOARD_H
#define __BOARD_H
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "sys.h"
#include "spi.h"
#include "lcd.h"
#include "uart.h"
#include "bme280.h"
#include "ds1307.h"

extern RCC_ClocksTypeDef RCC_Clocks;

void init_hardware(void);

#endif //__BOARD_H