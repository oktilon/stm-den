#include "i2c.h"

void I2C_Initialize(I2C_TypeDef *I2Cx, u32 speed, u16 addr, u16 ack, u16 ack_size) {
    I2C_InitTypeDef I2C_InitStruct;

    // I2C_DeInit(I2Cx);

    I2C_InitStruct.I2C_Mode        = I2C_Mode_I2C;
    I2C_InitStruct.I2C_ClockSpeed  = speed;
    I2C_InitStruct.I2C_DutyCycle   = I2C_DutyCycle_2; // Only for Fast mode ClockSpeed = 400000
    I2C_InitStruct.I2C_OwnAddress1 = addr;
    I2C_InitStruct.I2C_Ack         = ack;
    I2C_InitStruct.I2C_AcknowledgedAddress = ack_size;
    I2C_Init(I2Cx, &I2C_InitStruct);

    I2C_Cmd(I2Cx, ENABLE);
}

void I2C_GPIO_Initialize(GPIO_TypeDef *GPIOx, u8 pinSCL, u8 pinSDA) {
    GPIO_InitTypeDef GPIO_InitStructure;
    u16 gpioSCL = 1 << pinSCL;
    u16 gpioSDA = 1 << pinSDA;

    GPIO_InitStructure.GPIO_Pin   = gpioSCL | gpioSDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOx, pinSCL, I2C_AF);
    GPIO_PinAFConfig(GPIOx, pinSDA, I2C_AF);
}

static void waitForFlag(I2C_TypeDef *I2Cx, u32 flag, FlagStatus state) {
    u8 timeout = 255;
    while(I2C_GetFlagStatus(I2Cx, flag) != state && timeout > 0) {
        timeout--;
    }
}

void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    // START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);

    // SLAVE ADDR + WRITE
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDR
    waitForFlag(I2Cx, I2C_FLAG_TXE, SET);
    I2C_SendData(I2Cx, reg);

    // SEND DATA
    waitForFlag(I2Cx, I2C_FLAG_TXE, SET);
    I2C_SendData(I2Cx, data);

    waitForFlag(I2Cx, I2C_FLAG_TXE, SET);
    waitForFlag(I2Cx, I2C_FLAG_BTF, SET);
    I2C_GenerateSTOP(I2Cx, ENABLE);
}

u8 I2C_Read(I2C_TypeDef *I2Cx, u8 chip, u8 reg) {
    u8 ret = 0;
    // START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);

    // SLAVE ADDR + READ
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDR
    I2C_SendData(I2Cx, reg);
    waitForFlag(I2Cx, I2C_FLAG_BTF, SET);

    // REPEAT START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);

    // SLAVE ADDR
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Receiver);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR2; // Clear ADDR

    // GET DATA
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_RXNE, SET);
    ret = I2C_ReceiveData(I2Cx);

    waitForFlag(I2Cx, I2C_FLAG_BUSY, RESET);
    return ret;
}

int I2C_ReadBytes(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 *buf, u8 size) {
    u8 left = size;
    u8 ix = 0;

    // One byte only
    if(size == 1) {
        buf[0] = I2C_Read(I2Cx, chip, reg);
        return 1;
    }

    // START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);

    // SLAVE ADDR + READ
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDR
    I2C_SendData(I2Cx, reg);
    waitForFlag(I2Cx, I2C_FLAG_BTF, SET);

    // REPEAT START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);

    // SLAVE ADDR
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Receiver);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR2; // Clear ADDR

    // GET DATA
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_RXNE, SET);
    buf[ix] = I2C_ReceiveData(I2Cx);

    waitForFlag(I2Cx, I2C_FLAG_BUSY, RESET);
    return 0;
}