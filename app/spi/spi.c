#include "spi.h"

/*****************************************************************************
 * @date       :2018-08-09
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :SPIx: SPI type,x for 1,2,3
                Byte:Data to be written
 * @retvalue   :Data received by the bus
******************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte) {
    while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);
    SPIx->DR=Byte;
    while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);
    return SPIx->DR;
}

/*****************************************************************************
 * @date       :2018-08-09
 * @function   :Set hardware SPI Speed
 * @parameters :SPIx: SPI type,x for 1,2,3
                SpeedSet:0-high speed
                         1-low speed
 * @retvalue   :None
******************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet) {
    SPIx->CR1&=0XFFC7;
    if(SpeedSet==1)
    {
        SPIx->CR1|=SPI_BaudRatePrescaler_2; //Fsck=Fpclk/2
    }
    else
    {
        SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
    }
    SPIx->CR1|=1<<6;
}

/*****************************************************************************
 * @name       :void SPI_Init(SPI_TypeDef* SPIx)
 * @date       :2018-08-09
 * @function   :Initialize the STM32 hardware SPI1
 * @parameters :SPIx: SPI type,x for 1,2,3
 * @retvalue   :None
******************************************************************************/
void SPI_Initialize(SPI_TypeDef* SPIx) {
    SPI_InitTypeDef  SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE);
}