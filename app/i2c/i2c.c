#include "i2c.h"

void I2C_Write(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    // START
    I2Cx->CR1 |= I2C_CR1_START;
    while(!((I2Cx->SR1) & I2C_SR1_SB)); // Wait for StartBit (SB)

    // DEVICE_ADDR(7bit) + READ_FLAG=0
    I2Cx->DR = (chip << 1); // SEND: Addr + Write=0
    while(!((I2Cx->SR1) & I2C_SR1_ADDR)); // Wait for Address sent (ADDR)
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDRESS
    I2Cx->DR = reg; // SEND: Reg addr
    while(!((I2Cx->SR1) & I2C_SR1_BTF)); // Wait for Byte transmit finished

    // DATA
    I2Cx->DR = data; // Write
    while(!((I2Cx->SR1) & I2C_SR1_TXE)); // Wait until Tx empty
    while(!((I2Cx->SR1) & I2C_SR1_BTF)); // Wait for Byte transmit finished

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