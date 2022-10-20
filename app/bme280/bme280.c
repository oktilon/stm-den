#include "bme280.h"

u16 dig_T1;
s16 dig_T2, dig_T3;
u16 dig_P1;
s16 dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
s16 dig_H2, dig_H4, dig_H5;
u8 dig_H1, dig_H3, dig_H6;

void BME280_init(void) {
    I2C_GPIO_Initialize(BME280_I2C_GPIO, BME280_SCL_PIN, BME280_SDA_PIN);
    I2C_Initialize(BME280_I2C, 100000, 0x00, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
}

static u16 BME280_Read2bReg(uint8_t reg1) {
    u16 lsb = I2C_Read(BME280_I2C, BME280_ADDR, reg1);
    u16 msb = I2C_Read(BME280_I2C, BME280_ADDR, reg1 + 1);
    return (msb << 8) | lsb;
}

static u16 BME280_Read2bVal(uint8_t reg1) {
    u16 msb = I2C_Read(BME280_I2C, BME280_ADDR, reg1);
    u16 lsb = I2C_Read(BME280_I2C, BME280_ADDR, reg1 + 1);
    return (msb << 8) | lsb;
}

static u32 BME280_Read3bVal(uint8_t reg1) {
    u32 msb = I2C_Read(BME280_I2C, BME280_ADDR, reg1);
    u32 lsb = I2C_Read(BME280_I2C, BME280_ADDR, reg1 + 1);
    u32 xlsb = I2C_Read(BME280_I2C, BME280_ADDR, reg1 + 2);
    return (msb << 12) | (lsb << 4) | (xlsb >> 4);
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t t_fine;
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) {
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *
    ((BME280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P) {
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BME280_S64_t)dig_P6;
    var2 = var2 + ((var1*(BME280_S64_t)dig_P5)<<17);
    var2 = var2 + (((BME280_S64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (BME280_S64_t)dig_P3)>>8) + ((var1 * (BME280_S64_t)dig_P2)<<12);
    var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)dig_P1)>>33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BME280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BME280_S64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)dig_P7)<<4);
    return (BME280_U32_t)p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
static BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H) {
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
    ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
    ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
    ((BME280_S32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r>>12);
}

u8 BME280_GetChipId(void) {
    return I2C_Read(BME280_I2C, BME280_ADDR, BME280_CHIP_ID);
}

void BME280_ReadCalibration() {
    // Temperature
    dig_T1 = BME280_Read2bReg(BME280_T1);
    dig_T2 = BME280_Read2bReg(BME280_T2);
    dig_T3 = BME280_Read2bReg(BME280_T3);
    // Pressure
    dig_P1 = BME280_Read2bReg(BME280_P1);
    dig_P2 = BME280_Read2bReg(BME280_P2);
    dig_P3 = BME280_Read2bReg(BME280_P3);
    dig_P4 = BME280_Read2bReg(BME280_P4);
    dig_P5 = BME280_Read2bReg(BME280_P5);
    dig_P6 = BME280_Read2bReg(BME280_P6);
    dig_P7 = BME280_Read2bReg(BME280_P7);
    dig_P8 = BME280_Read2bReg(BME280_P8);
    dig_P9 = BME280_Read2bReg(BME280_P9);
    // Humidity
    dig_H1 = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H1);
    dig_H2 = BME280_Read2bReg(BME280_H2);
    dig_H3 = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H3);
    dig_H6 = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H6);
    // H4.H5
    u16 h4_msb = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H4_H);
    u16 h4_h5  = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H4_H5);
    u16 h5_lsb = I2C_Read(BME280_I2C, BME280_ADDR, BME280_H5_L);
    dig_H4 = (h4_msb << 4) | (h4_h5 & 0x0F);
    dig_H5 = ((h4_h5 & 0xF0) << 4) | h5_lsb;

    // Setup mode
    I2C_Write(BME280_I2C, BME280_ADDR, BME280_CTRL_HUM, 0x04);
    I2C_Write(BME280_I2C, BME280_ADDR, BME280_CTRL_MEAS, 0x93);
    // I2C_Write(BME280_I2C, BME280_ADDR, BME280_CONFIG, 0x00);
}

s32 BME280_GetTemperature() {
    s32 ret = 0;
    u32 val = BME280_Read3bVal(BME280_TEMP_MSB);
    ret = BME280_compensate_T_int32(val);
    return ret;
}

u32 BME280_GetPressure() {
    u32 ret = 0U;
    u32 val = BME280_Read3bVal(BME280_PRESS_MSB);
    ret = BME280_compensate_P_int64(val);
    return ret;
}

u32 BME280_GetHumidity() {
    u32 ret = 0U;
    u32 val = BME280_Read2bVal(BME280_HUM_MSB);
    ret = bme280_compensate_H_int32(val);
    return ret;
}
