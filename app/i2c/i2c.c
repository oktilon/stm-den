#include "i2c.h"

void I2C_Initialize(I2C_TypeDef *I2Cx, u32 speed, u16 addr, u16 ack, u16 ack_size) {
    I2C_InitTypeDef I2C_InitStruct;

    I2C_DeInit(I2Cx);

    I2C_InitStruct.I2C_Mode        = I2C_Mode_I2C;
    I2C_InitStruct.I2C_ClockSpeed  = 100000;
    I2C_InitStruct.I2C_DutyCycle   = I2C_DutyCycle_2; // Only for Fast mode ClockSpeed = 400000
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack         = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2Cx, &I2C_InitStruct);

    I2C_Cmd(I2Cx, ENABLE);
}

static void waitSR1Flag(I2C_TypeDef *I2Cx, u16 flag) {
    u8 timeout = 255;
    while(((I2Cx->SR1) & flag) == 0 && timeout > 0) {
        timeout--;
    }
}

static void waitBusyFlag(I2C_TypeDef *I2Cx) {
    u8 timeout = 255;
    while(((I2Cx->SR2) & I2C_SR2_BUSY) > 0 && timeout > 0) {
        timeout--;
    }
}

void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    // START
    I2Cx->CR1 |= I2C_CR1_START;
    waitSR1Flag(I2Cx, I2C_SR1_SB); // Wait for StartBit (SB)
    (void) I2Cx->SR1; // Clear SB

    // DEVICE_ADDR(7bit) + READ_FLAG=0
    I2Cx->DR = (chip << 1); // SEND: Addr + Write=0
    waitSR1Flag(I2Cx->SR1, I2C_SR1_ADDR); // Wait for Address sent (ADDR)
    (void) I2Cx->SR1; // Clear ADDR
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDRESS
    I2Cx->DR = reg; // SEND: Reg addr
    waitSR1Flag(I2Cx->SR1, I2C_SR1_BTF); // Wait for Byte transmit finished

    // DATA
    I2Cx->DR = data; // Write
    waitSR1Flag(I2Cx, I2C_SR1_TXE); // Wait until Tx empty
    waitSR1Flag(I2Cx, I2C_SR1_BTF); // Wait for Byte transmit finished

    // STOP
    I2Cx->CR1 |= I2C_CR1_STOP;

    // WAIT WHILE BUSY
    while((I2Cx->SR2) & I2C_SR2_BUSY);
}

u8 I2C_Read(I2C_TypeDef *I2Cx, u8 chip, u8 reg) {
    u8 ret = 0;

    // START
    I2Cx->CR1 |= I2C_CR1_START;
    while(!((I2Cx->SR1) & I2C_SR1_SB)); // Wait for StartBit (SB)

    // DEVICE_ADDR(7bit) + WRITE_FLAG=0
    I2Cx->DR = (chip << 1); // SEND: Addr + Write=0
    while(!((I2Cx->SR1) & I2C_SR1_ADDR)); // Wait for Address sent (ADDR)
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDRESS
    I2Cx->DR = reg; // SEND: Reg addr
    while(!((I2Cx->SR1) & I2C_SR1_BTF)); // Wait for Byte transmit finished

    // REPEATED START
    I2Cx->CR1 |= I2C_CR1_START;
    while(!((I2Cx->SR1) & I2C_SR1_SB)); // Wait for StartBit (SB)

    // DEVICE_ADDR(7bit) + READ_FLAG=1
    I2Cx->DR = (chip << 1) | I2C_Direction_Receiver; // SEND: Addr + Read=1
    while(!((I2Cx->SR1) & I2C_SR1_ADDR)); // Wait for Address sent (ADDR)
    //Before Clearing Addr bit by reading SR2, we have to cancel ack.
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
    (void) I2Cx->SR2; // Clear ADDR

    //Order a STOP condition
    //Note: Spec_p583 says this should be done just after clearing ADDR
    //If it is done before ADDR is set, a STOP is generated immediately as the clock is being streched
    I2Cx->CR1 |= I2C_CR1_STOP;

    // READ DATA
    while(!((I2Cx->SR1) & I2C_SR1_RXNE)); // Wait Rx not EMPTY
    ret = I2Cx->DR;

    // WAIT WHILE BUSY
    while((I2Cx->SR2) & I2C_SR2_BUSY);

    //Enable the Acknowledgement again
    I2Cx->CR1 |= ((uint16_t)I2C_CR1_ACK);

    return ret;
}

void I2C_Write2(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    I2C_GenerateSTART(I2Cx, ENABLE);
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx, reg);
    I2C_SendData(I2Cx, data);
    I2C_GenerateSTOP(I2Cx, ENABLE);
}

u8 I2C_Read2(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    u8 ret = 0;
    I2C_GenerateSTART(I2Cx, ENABLE);
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx, reg);

    I2C_GenerateSTART(I2Cx, ENABLE);
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Receiver);
    ret = I2C_ReceiveData(I2Cx);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return ret;
}