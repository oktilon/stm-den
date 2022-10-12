#ifndef TEST1_DMA_H
#define TEST1_DMA_H

#include "control.h"
#include <stm32f4xx_spi.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>

#define dmaWait() while(SPI_I2S_GetFlagStatus(SPI_MASTER,SPI_I2S_FLAG_BSY) == SET);

#define dmaStartRx() DMA_Init(DMA2_Stream2, &dmaStructure); \
    DMA_Cmd(DMA2_Stream2, ENABLE);

#define dmaStartTx() DMA_Init(DMA2_Stream3, &dmaStructure); \
    DMA_Cmd(DMA2_Stream3, ENABLE);

void dmaInit(void);

void dmaSendCmd(u8 cmd);
void dmaSendCmdCont(u8 cmd);

void dmaReceiveDataCont8(u8 *data);

void dmaSendData8(u8 *data, u32 n);
void dmaSendData16(u16 *data, u32 n);

void dmaSendDataCont8(u8 *data, u32 n);
void dmaSendDataCont16(u16 *data, u32 n);

void dmaSendDataBuf16();
void dmaSendDataContBuf16(u16 *data, u32 n);

void dmaFill16(u16 color, u32 n);

#endif //TEST1_DMA_H
