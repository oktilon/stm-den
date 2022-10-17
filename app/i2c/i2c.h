#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"


void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data);
u8 I2C_Read(I2C_TypeDef *I2Cx, u8 chip, u8 reg);

#endif //__I2C_H