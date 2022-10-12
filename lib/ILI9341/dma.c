#include "dma.h"

DMA_InitTypeDef dmaStructure;

#define DMA_BUF_SIZE 2048
u16 dmaBufIndex = 0;
u16 dmaBuffer[DMA_BUF_SIZE];

void dmaInit(void) {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2, ENABLE);

    // TX
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);

    // RX
    NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI_MASTER, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SPI_MASTER, SPI_I2S_DMAReq_Rx, ENABLE);
}

//<editor-fold desc="Dma init options and start">

inline static void dmaReceive8(u8 *data, u32 n) {
    dmaStructure.DMA_Memory0BaseAddr    = (u32) data;
    dmaStructure.DMA_BufferSize         = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    dmaStartRx();
}

inline static void dmaSend8(u8 *data, u32 n) {
    DMA_StructInit(&dmaStructure);
    dmaStructure.DMA_PeripheralBaseAddr = (u32) &(SPI_MASTER->DR);
    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

    dmaStructure.DMA_Memory0BaseAddr    = (u32) data;
    dmaStructure.DMA_BufferSize         = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    dmaStartTx();
}

inline static void dmaSendCircular16(u16 *data, u32 n) {
    DMA_StructInit(&dmaStructure);
    dmaStructure.DMA_PeripheralBaseAddr = (u32) &(SPI_MASTER->DR);
    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

    dmaStructure.DMA_Memory0BaseAddr    = (u32) data;
    dmaStructure.DMA_BufferSize         = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Circular;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    dmaStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dmaStartTx();
}

inline static void dmaSend16(u16 *data, u32 n) {
    DMA_StructInit(&dmaStructure);
    dmaStructure.DMA_PeripheralBaseAddr = (u32) &(SPI_MASTER->DR);
    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

    dmaStructure.DMA_Memory0BaseAddr    = (u32) data;
    dmaStructure.DMA_BufferSize         = n;

    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    dmaStartTx();
}

//</editor-fold>

//<editor-fold desc="DMA send receive functions">

inline void dmaSendCmd(u8 cmd) {
    TFT_CS_RESET;
    TFT_DC_RESET;
    dmaSend8(&cmd, 1);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendCmdCont(u8 cmd) {
    TFT_DC_RESET;
    dmaSend8(&cmd, 1);
    dmaWait();
}

inline void dmaReceiveDataCont8(u8 *data) {
    u8 dummy = 0xFF;
    dmaSend8(&dummy, 1);
    dmaReceive8(data, 1);
    dmaWait();
}

inline void dmaSendData8(u8 *data, u32 n) {
    TFT_CS_RESET;
    TFT_DC_SET;
    dmaSend8(data, n);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendDataCont8(u8 *data, u32 n) {
    TFT_DC_SET;
    dmaSend8(data, n);
    dmaWait();
}

inline void dmaSendData16(u16 *data, u32 n) {
    TFT_CS_RESET;
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
    TFT_CS_SET;
}

inline void dmaSendDataCont16(u16 *data, u32 n) {
    TFT_DC_SET;
    dmaSend16(data, n);
    dmaWait();
}

inline void dmaSendDataBuf16() {
    if(dmaBufIndex == 0) return;
    TFT_DC_SET;
    dmaSend16(dmaBuffer, dmaBufIndex);
    dmaBufIndex = 0;
    dmaWait();
}

inline void dmaSendDataContBuf16(u16 *data, u32 n) {
    while (n--) {
        dmaBuffer[dmaBufIndex] = *data++;
        if (dmaBufIndex == DMA_BUF_SIZE - 1) {
            dmaSendDataBuf16();
        }
        dmaBufIndex++;
    }
}


static inline void dmaSendDataCircular16(u16 *data, u32 n) {
    TFT_DC_SET;
    dmaSendCircular16(data, n);
    dmaWait();
}

//</editor-fold>

inline void dmaFill16(u16 color, u32 n) {
    TFT_CS_RESET;
    dmaSendCmdCont(LCD_GRAM);
    while (n != 0) {
        u16 ts = (u16) (n > UINT16_MAX ? UINT16_MAX : n);
        dmaSendDataCircular16(&color, ts);
        n -= ts;
    }
    TFT_CS_SET;
}

//<editor-fold desc="IRQ handlers">

void DMA1_Channel3_IRQHandler(void) {
    // Tx
    if (DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3) == SET) {
        DMA_Cmd(DMA2_Stream3, DISABLE);
        DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
    }
    // Rx
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2) == SET) {
        DMA_Cmd(DMA2_Stream2, DISABLE);
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
    }
}

//</editor-fold>
