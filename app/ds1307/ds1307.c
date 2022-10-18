#include "ds1307.h"

static void DS1307_WriteReg(u8 reg, u8 data) {
    I2C_Write(DS_I2C, DS1307_ADDR, reg, data);
}

static u8 DS1307_ReadReg(u8 reg) {
    return I2C_Read(DS_I2C, DS1307_ADDR, reg);
}

void DS1307_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin   = DS_SCL_PIN | DS_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(DS_I2C_GPIO, &GPIO_InitStructure);

    GPIO_PinAFConfig(DS_I2C_GPIO, DS_SCL_SRC, GPIO_AF_I2C1);
    GPIO_PinAFConfig(DS_I2C_GPIO, DS_SDA_SRC, GPIO_AF_I2C1);


    I2C_Initialize(DS_I2C, 10000, 0x00, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

    delay_ms(100);

    // DS1307_WriteReg(DS1307_CTRL, 0x00);
}

u8 bcd2dec(u8 bcd) {
	u8 u = bcd & 0xF;
	u8 d = (bcd & 0xF0) >> 4;
	return u + 10 * d;
}
u8 dec2bcd(u8 dec) {
	u8 d = (dec / 10) << 4;
	u8 u = dec % 10;
	return d + u;
}

void DS1307_SetDateTime(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec) {
    u8 y = year > 100 ? year % 100 : year;
    DS1307_WriteReg(DS1307_YEAR, dec2bcd(y));
    DS1307_WriteReg(DS1307_MON, dec2bcd(month));
    DS1307_WriteReg(DS1307_DATE, day);
    DS1307_WriteReg(DS1307_HOUR, dec2bcd(hour));
    DS1307_WriteReg(DS1307_MIN, dec2bcd(min));
    DS1307_WriteReg(DS1307_SEC, dec2bcd(sec));
}

void DS1307_GetTime(u8 *hour, u8 *min, u8 *sec) {
    *hour = bcd2dec(DS1307_ReadReg(DS1307_HOUR));
    *min = bcd2dec(DS1307_ReadReg(DS1307_MIN));
    *sec = bcd2dec(DS1307_ReadReg(DS1307_SEC));
}
