#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"

void I2C_Initialize(I2C_TypeDef *I2Cx, u32 speed = 100000, u16 addr = 0x00, u16 ack = I2C_Ack_Enable, u16 ack_size = I2C_AcknowledgedAddress_7bit);
void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data);
u8 I2C_Read(I2C_TypeDef *I2Cx, u8 chip, u8 reg);

u8 I2C_Read2(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data);
void I2C_Write2(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data);

#endif //__I2C_H