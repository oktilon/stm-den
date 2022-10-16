#include "sys.h"

#ifndef _SPI_H_
#define _SPI_H_

#define dd

u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte);
void SPI_Initialize(SPI_TypeDef* SPIx);
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet);

#endif