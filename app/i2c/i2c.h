#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"

/** @brief i2c1, i2c2, i2c3 AlternativeFunction = 4 */
#define I2C_AF      ((uint8_t)0x04)

void I2C_Initialize(I2C_TypeDef *I2Cx, u32 speed, u16 addr, u16 ack, u16 ack_size);
void I2C_GPIO_Initialize(GPIO_TypeDef *port, u8 pinSCL, u8 pinSDA);
void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data);
u8 I2C_Read(I2C_TypeDef *I2Cx, u8 chip, u8 reg);
int I2C_ReadBytes(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 *buf, u8 size);

#endif //__I2C_H