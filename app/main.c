#include "board.h"
#include "time.h"
#include "task.h"

s32 temp = 0;
u32 press = 0,
    hum = 0;

u8 hourVal(u8 hour, u8 min) {
    u8 ret = hour;
    if(ret >= 12) ret -= 12;
    ret = (ret * 60 + min) / 12;
    return ret;
}

static int ppow10(int val) {
    static int pows[10] = { 1, 10, 100, 1000, 10000
        , 100000, 1000000, 10000000
        , 100000000, 1000000000 };
    if(val < 0 || val > 9) return 0;
    return pows[val];
}

static void drawArrow(u8 val, u32 offset) {
    u16 x = clockArrows[val][offset];
    u16 y = clockArrows[val][offset + 1];
    LCD_DrawLine(120+1, 160+1, x+1, y+1);
    LCD_DrawLine(120-1, 160+1, x-1, y+1);
    LCD_DrawLine(120+1, 160-1, x+1, y-1);
    LCD_DrawLine(120-1, 160-1, x-1, y-1);
}

static void drawMark(u8 val) {
    u16 xb = clockArrows[val][SECONDS_ARROW];
    u16 yb = clockArrows[val][SECONDS_ARROW + 1];
    u16 xe = clockArrows[val][CIPHER_OUTER];
    u16 ye = clockArrows[val][CIPHER_OUTER + 1];
    if(val % 5 == 0) {
        POINT_COLOR = RED;
        LCD_DrawLine(xb+1, yb+1, xe+1, ye+1);
        LCD_DrawLine(xb-1, yb+1, xe-1, ye+1);
        LCD_DrawLine(xb+1, yb-1, xe+1, ye-1);
        LCD_DrawLine(xb-1, yb-1, xe-1, ye-1);
    } else {
        POINT_COLOR = BLUE;
        LCD_DrawLine(xb, yb, xe, ye);
    }
}

static void drawClockFace() {
    for(u8 val = 0; val < 60; val++) {
        drawMark(val);
    }
}

static u8 valIsNear(u16 val, u16 val2) {
    return (val2 >= val && val2 <= (val + 5)) ? 1 : 0;
}

static u8 drawTime(u16 x, u32 *pNextSwitch) {
    static u8 oldSec = 200;
    static u8 oldMin = 200;
    static u8 oldHour = 200;
    static u8 intSec = 0, intMin = 0, intHour = 0;
    u8 isManual = 0;
    char buf[9];
    u8 h, m, s, sec, min, hour, h24;
    u16 xc;

    DS1307_GetBcdTime(&h, &m, &s);
    sec = bcd2dec(s);
    min = bcd2dec(m);
    h24 = bcd2dec(h);

    if(sec > 59 || min > 59 || h24 > 23) {
        isManual = 1;
        if (oldSec == 200) {
            intSec = INIT_SECOND;
            intMin = INIT_MINUTE;
            intHour = INIT_HOUR;
        } else {
            if (*pNextSwitch < upTime) {
                *pNextSwitch = upTime + 1000;
                intSec++;
                if(intSec > 59) {
                    intSec = 0;
                    intMin++;
                }
                if(intMin > 59) {
                    intMin = 0;
                    intHour++;
                }
                if(intHour > 23) {
                    intHour = 0;
                }
            }
        }
        sec = intSec;
        min = intMin;
        h24 = intHour;
        s = dec2bcd(intSec);
        m = dec2bcd(intMin);
        h = dec2bcd(intHour);
    }

    POINT_COLOR = WHITE;
    buf[0] = 0x30 + (h >> 4);
    buf[1] = 0x30 + (h & 0xF);
    buf[2] = ':';
    buf[3] = 0x30 + (m >> 4);
    buf[4] = 0x30 + (m & 0xF);
    buf[5] = ':';
    buf[6] = 0x30 + (s >> 4);
    buf[7] = 0x30 + (s & 0xF);
    buf[8] = 0;
    xc = LCD_PrintString(x + 3, 2, 16, buf, 0);

    if (isManual) {
        POINT_COLOR = RED;
        LCD_PrintString(xc, 2, 16, " (TCK) ", 0);
    } else {
        POINT_COLOR = GREEN;
        LCD_PrintString(xc, 2, 16, " (RTC) ", 0);
    }

    hour = hourVal(h24, min);

    if(sec == oldSec) return 0;



    POINT_COLOR = BLACK;
    if(oldSec != 200) {
        drawArrow(oldSec, SECONDS_ARROW);
        if(min != oldMin) drawArrow(oldMin, SECONDS_ARROW);
        if(hour != oldHour) drawArrow(oldHour, HOURS_ARROW);
    }

    POINT_COLOR = GREEN;
    if(hour != oldHour || valIsNear(hour, oldMin) || valIsNear(hour, oldSec)) drawArrow(hour, HOURS_ARROW);
    POINT_COLOR = CYAN;
    if(min != oldMin || valIsNear(min, oldSec)) drawArrow(min, SECONDS_ARROW);
    POINT_COLOR = YELLOW;
    drawArrow(sec, SECONDS_ARROW);

    oldSec = sec;
    oldMin = min;
    oldHour = hour;
    return 1;
}

static int displayValue(int x, int y, u32 val, int del, int dec, char *units) {
    char txt[24] = { 0 };
    int div = ppow10(del);
    int v1 = val / div;
    int v2 = val % div;
    int len, i, j;
    itoas(v1, txt, 10);
    x = LCD_PrintString(x, y, 16, txt, 0);
    x = LCD_PrintString(x, y, 16, ",", 0);
    x -= 3;
    len = itoas(v2, txt, 10);
    if(len < del) {
        for(j = del, i = len; i >= 0; j--, i--) {
            txt[j] = txt[i];
        }
        for(i = 0; i <= j; i++) {
            txt[i] = '0';
        }
    }
    if(dec < del) {
        txt[dec] = 0;
    }
    x = LCD_PrintString(x, y, 16, txt, 0);
    x = LCD_PrintString(x, y, 16, units, 0);
    return x;
}

static u8 checkTime() {
    return 1;
}

int main(void) {
    u32 nextSwitch = 0;
    init_hardware();
    delay_ms(50);
    // DS1307_SetDateTime(26, 5, 26, 13, 5, 45);

    u8 id = BME280_GetChipId();
    BME280_ReadCalibration();

    char buf[32] = "0x__";
    if((id >> 4) > 9) {
        buf[2] = 'A' + ((id >> 4) - 10);
    } else {
        buf[2] = '0' + (id >> 4);
    }
    if((id & 0xF) > 9) {
        buf[3] = 'A' + ((id & 0xF) - 10);
    } else {
        buf[3] = '0' + (id & 0xF);
    }


    // UART_SendCrLf();
    // UART_SendString("BME280 ID=0x", 10);
    // UART_SendString(buf + 6, 2);
    // UART_SendCrLf();

    u8 on = 0, tck = 0;

    while(!tck) {
        tck = checkTime();
    }


    BACK_COLOR = BLACK;
    POINT_COLOR = RED;
    u16 x = LCD_PrintString(2, 2, 16, "Time:", 0);
    POINT_COLOR = BLUE;
    u16 chipX = LCD_PrintString(2, 20, 16, "BME280 id=", 0);
    POINT_COLOR = CYAN;
    LCD_PrintString(chipX + 2, 20, 16, buf, 0);
    drawClockFace();


    while (1) {
        tck = drawTime(x, &nextSwitch);
        if (tck) {
            if(on) {
                PDout(13)=0;
                PDout(15)=1;
                on = 0;
            } else {
                PDout(13)=1;
                PDout(15)=0;
                on = 1;
            }
            temp = BME280_GetTemperature();
            press = BME280_GetPressure(0);
            hum = BME280_GetHumidity(0);

            POINT_COLOR = GREEN;
            displayValue(2, 275, temp, 2, 2, "^C");
            POINT_COLOR = YELLOW;
            displayValue(120, 275, hum, 3, 2, "%");
            POINT_COLOR = WHITE;
            displayValue(2, 293, press, 6, 2, "kPa");
            u32 mmhg = ((double)press / 133322.0) * 100.0;
            POINT_COLOR = CYAN;
            displayValue(120, 293, mmhg, 2, 2, "mmHg");

            // itoas(upTime, buf, 10);
            // LCD_PrintString(100, 145, 16, buf, 0);

            nextSwitch = upTime + 1000;
        }

        checkUartCommand();
    }
}

