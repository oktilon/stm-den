#ifndef __DS1307_H
#define __DS1307_H

#include "stm32f4xx_gpio.h"
#include "i2c.h"

#define DS_I2C          I2C1
#define DS1307_ADDR     ((uint8_t)0b1101000)

#define DS1307_SEC      ((uint8_t)0x00)
#define DS1307_MIN      ((uint8_t)0x01)
#define DS1307_HOUR     ((uint8_t)0x02)
#define DS1307_DAY      ((uint8_t)0x03)
#define DS1307_DATE     ((uint8_t)0x04)
#define DS1307_MON      ((uint8_t)0x05)
#define DS1307_YEAR     ((uint8_t)0x06)
#define DS1307_CTRL     ((uint8_t)0x07)
// RAM 0-55
#define DS3107_RAM(x)   ((uint8_t)(0x08+(x))

#define DS_I2C_GPIO     GPIOB
// PB6 - I2C1 SCL
#define DS_SCL_PIN      GPIO_Pin_6
#define DS_SCL_SRC      GPIO_PinSource6
// PB7 - I2C1 SDA
#define DS_SDA_PIN      GPIO_Pin_7
#define DS_SDA_SRC      GPIO_PinSource7

void DS1307_init(void);
void DS1307_SetDateTime(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
void DS1307_GetTime(u8 *hour, u8 *min, u8 *sec);

#endif //__DS1307_H