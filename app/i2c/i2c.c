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
    waitSR1Flag(I2Cx, I2C_SR1_ADDR); // Wait for Address sent (ADDR)
    (void) I2Cx->SR1; // Clear ADDR
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDRESS
    I2Cx->DR = reg; // SEND: Reg addr
    waitSR1Flag(I2Cx, I2C_SR1_BTF); // Wait for Byte transmit finished

    // DATA
    I2Cx->DR = data; // Write
    waitSR1Flag(I2Cx, I2C_SR1_TXE); // Wait until Tx empty
    waitSR1Flag(I2Cx, I2C_SR1_BTF); // Wait for Byte transmit finished

    // STOP
    I2Cx->CR1 |= I2C_CR1_STOP;

    // WAIT WHILE BUSY
    waitBusyFlag(I2Cx);
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

static void waitForFlag(I2C_TypeDef *I2Cx, u32 flag, FlagStatus state) {
    u8 timeout = 255;
    while(I2C_GetFlagStatus(I2Cx, flag) != state && timeout > 0) {
        timeout--;
    }
}

void I2C_Write2(I2C_TypeDef *I2Cx, u8 chip, u8 reg, u8 data) {
    I2C_GenerateSTART(I2Cx, ENABLE);
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx, reg);
    I2C_SendData(I2Cx, data);
    I2C_GenerateSTOP(I2Cx, ENABLE);
}

u8 I2C_Read2(I2C_TypeDef *I2Cx, u8 chip, u8 reg) {
    u8 ret = 0;
    // START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);
    (void) I2Cx->SR1; // Clear SB

    // SLAVE ADDR
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Transmitter);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR1; // Clear ADDR
    (void) I2Cx->SR2; // Clear ADDR

    // REGISTER ADDR
    I2C_SendData(I2Cx, reg);
    waitForFlag(I2Cx, I2C_FLAG_BTF, SET);

    // REPEAT START
    I2C_GenerateSTART(I2Cx, ENABLE);
    waitForFlag(I2Cx, I2C_FLAG_SB, SET);
    (void) I2Cx->SR1; // Clear SB

    // SLAVE ADDR
    I2C_Send7bitAddress(I2Cx, chip << 1, I2C_Direction_Receiver);
    waitForFlag(I2Cx, I2C_FLAG_ADDR, SET);
    (void) I2Cx->SR1; // Clear ADDR
    (void) I2Cx->SR2; // Clear ADDR

    // GET DATA
    waitForFlag(I2Cx, I2C_FLAG_RXNE, SET);
    ret = I2C_ReceiveData(I2Cx);

    // STOP
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return ret;
}