#include "board.h"
#include "time.h"

u8 hourVal(u8 hour, u8 min) {
    u8 ret = hour;
    if(ret > 12) ret -= 12;
    ret = (ret * 60 + min) / 12;
    return ret;
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

static void drawCipherblat() {
    for(u8 val = 0; val < 60; val++) {
        drawMark(val);
    }
}

static u8 valIsNear(u16 val, u16 val2) {
    return (val2 >= val && val2 <= (val + 5)) ? 1 : 0;
}

static void drawTime(u16 x) {
    static u8 oldSec = 200;
    static u8 oldMin = 200;
    static u8 oldHour = 200;
    char buf[9];
    u8 h, m, s, sec, min, hour, h24;

    DS1307_GetBcdTime(&h, &m, &s);
    sec = bcd2dec(s);
    min = bcd2dec(m);
    h24 = bcd2dec(h);
    hour = hourVal(h24, min);

    if(sec == oldSec) return;

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
    LCD_PrintString(x + 5, 15, 16, buf, 0);

    oldSec = sec;
    oldMin = min;
    oldHour = hour;
}

int main(void) {
    init_hardware();

    // DS1307_SetDateTime(22, 10, 19, 22, 23, 30);

    u8 id = BME280_GetChipId();

    u8 buf[8] = { 0, 0, 0, 0, 0 };
    u8 on = 0;

    BACK_COLOR = BLACK;
    POINT_COLOR = RED;
    u16 x = LCD_PrintString(2, 15, 12, "Time:", 0);
    drawCipherblat();

    // UART_SendByte(0x33);
    // UART_SendByte(0x34);
    // UART_SendByte(0x35);

    while (1) {
        drawTime(x);
        if ((upTime % 500) == 0 && upTime) {
            if(on) {
                PDout(13)=0;
                PDout(15)=1;
                on = 0;
            } else {
                PDout(13)=1;
                PDout(15)=0;
                on = 1;
            }
        }
        if (UART_GetDataSize() >= 4) {
            buf[0] = UART_GetByte();
            __asm("NOP");
            buf[1] = UART_GetByte();
            __asm("NOP");
            buf[2] = UART_GetByte();
            __asm("NOP");
            buf[3] = UART_GetByte();
            __asm("NOP");
            if(buf[0] == 0xAA && buf[1] == 0x55 && buf[3] == 0x55) {
                if(buf[2] == 0x01) {
                    PDout(12) = 1;
                    PDout(14) = 1;
                } else {
                    PDout(12) = 0;
                    PDout(14) = 0;
                }
            } else {
                if(buf[0] == 0x31) {
                    PDout(12) = 1;
                    PDout(14) = 1;
                } else {
                    PDout(12) = 0;
                    PDout(14) = 0;
                }
            }
        }
    }

    while (1);
}

