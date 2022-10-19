#include "bme280.h"

void BME280_init(void) {
    I2C_GPIO_Initialize(BME280_I2C_GPIO, BME280_SCL_PIN, BME280_SDA_PIN);
    I2C_Initialize(BME280_I2C, 100000, 0x00, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
}

u8 BME280_GetChipId(void) {
    return I2C_Read(BME280_I2C, BME280_ADDR, BME280_CHIP_ID);
}
