#ifndef __BME280_H
#define __BME280_H

#include "stm32f4xx_gpio.h"
#include "i2c.h"

// SDO == VCC
// #define BME280_ADDR         ((uint8_t)0b1110111)
// SDO == GND
#define BME280_ADDR         ((uint8_t)0b1110110)

#define BME280_HUM_LSB      ((uint8_t)0xFE)
#define BME280_HUM_MSB      ((uint8_t)0xFD)
#define BME280_TEMP_XLSB    ((uint8_t)0xFC)
#define BME280_TEMP_LSB     ((uint8_t)0xFB)
#define BME280_TEMP_MSB     ((uint8_t)0xFA)
#define BME280_PRESS_XLSB   ((uint8_t)0xF9)
#define BME280_PRESS_LSB    ((uint8_t)0xF8)
#define BME280_PRESS_MSB    ((uint8_t)0xF7)
#define BME280_CONFIG       ((uint8_t)0xF5)
#define BME280_CTRL_MEAS    ((uint8_t)0xF4)
#define BME280_STATUS       ((uint8_t)0xF3)
#define BME280_CTRL_HUM     ((uint8_t)0xF2)
#define BME280_RESET        ((uint8_t)0xE0)
#define BME280_CHIP_ID      ((uint8_t)0xD0)
#define BME280_T1           ((uint8_t)0x88)
#define BME280_T2           ((uint8_t)0x8A)
#define BME280_T3           ((uint8_t)0x8C)
#define BME280_P1           ((uint8_t)0x8E)
#define BME280_P2           ((uint8_t)0x90)
#define BME280_P3           ((uint8_t)0x92)
#define BME280_P4           ((uint8_t)0x94)
#define BME280_P5           ((uint8_t)0x96)
#define BME280_P6           ((uint8_t)0x98)
#define BME280_P7           ((uint8_t)0x9A)
#define BME280_P8           ((uint8_t)0x9C)
#define BME280_P9           ((uint8_t)0x9E)
#define BME280_H1           ((uint8_t)0xA1)
#define BME280_H2           ((uint8_t)0xE1)
#define BME280_H3           ((uint8_t)0xE3)
#define BME280_H4_H         ((uint8_t)0xE4)
#define BME280_H4_H5        ((uint8_t)0xE5)
#define BME280_H5_L         ((uint8_t)0x65)
#define BME280_H6           ((uint8_t)0xE7)

typedef signed long          BME280_S32_t;
typedef unsigned long        BME280_U32_t;
typedef signed long long     BME280_S64_t;

// PB10 - I2C1 SCL
// PB11 - I2C1 SDA
#define BME280_I2C_GPIO     GPIOB
#define BME280_SCL_PIN      10
#define BME280_SDA_PIN      11
#define BME280_I2C          I2C2

void BME280_init(void);
u8 BME280_GetChipId(void);
void BME280_ReadCalibration();
s32 BME280_GetTemperature();
u32 BME280_GetPressure();
u32 BME280_GetHumidity();

#endif //__BME280_H