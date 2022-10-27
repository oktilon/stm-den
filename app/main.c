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

static int pow10(int val) {
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

static u8 drawTime(u16 x) {
    static u8 oldSec = 200;
    static u8 oldMin = 200;
    static u8 oldHour = 200;
    char buf[9];
    u8 h, m, s, sec, min, hour, h24;

    DS1307_GetBcdTime(&h, &m, &s);
    sec = bcd2dec(s);
    min = bcd2dec(m);
    h24 = bcd2dec(h);
    if(sec > 59 || min > 59 || h24 > 23) return 1;
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
    LCD_PrintString(x + 3, 2, 16, buf, 0);

    oldSec = sec;
    oldMin = min;
    oldHour = hour;
    return 1;
}

static int displayValue(int x, int y, u32 val, int del, int dec, char *units) {
    char txt[24] = { 0 };
    int div = pow10(del);
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
    init_hardware();
    delay_ms(50);
    // DS1307_SetDateTime(22, 10, 19, 22, 23, 30);

    // u8 id = BME280_GetChipId();
    BME280_ReadCalibration();

    u8 on = 0, tck = 0;

    while(!tck) {
        tck = checkTime();
    }


    BACK_COLOR = BLACK;
    POINT_COLOR = RED;
    u16 x = LCD_PrintString(2, 2, 16, "Time:", 0);
    drawClockFace();

    // UART_SendByte(0x33);
    // UART_SendByte(0x34);
    // UART_SendByte(0x35);

    while (1) {
        tck = drawTime(x);
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

            displayValue(2, 275, temp, 2, 2, "^C");
            displayValue(120, 275, hum, 3, 2, "%");
            displayValue(2, 293, press, 6, 2, "kPa");
            u32 mmhg = ((double)press / 133322.0) * 100.0;
            displayValue(120, 293, mmhg, 2, 2, "mmHg");
        }

        checkUartCommand();
    }
}

