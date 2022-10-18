#include "board.h"

u16 clockArrows[60][3] = {
     {0, 120, 60}
    ,{1, 130, 61}
    ,{2, 141, 62}
    ,{3, 151, 65}
    ,{4, 161, 69}
    ,{5, 170, 73}
    ,{6, 179, 79}
    ,{7, 187, 86}
    ,{8, 194, 93}
    ,{9, 201, 101}
    ,{10, 207, 110}
    ,{11, 211, 119}
    ,{12, 215, 129}
    ,{13, 218, 139}
    ,{14, 219, 150}
    ,{15, 220, 160}
    ,{16, 219, 170}
    ,{17, 218, 181}
    ,{18, 215, 191}
    ,{19, 211, 201}
    ,{20, 207, 210}
    ,{21, 201, 219}
    ,{22, 194, 227}
    ,{23, 187, 234}
    ,{24, 179, 241}
    ,{25, 170, 247}
    ,{26, 161, 251}
    ,{27, 151, 255}
    ,{28, 141, 258}
    ,{29, 130, 259}
    ,{30, 120, 260}
    ,{31, 110, 259}
    ,{32, 99, 258}
    ,{33, 89, 255}
    ,{34, 79, 251}
    ,{35, 70, 247}
    ,{36, 61, 241}
    ,{37, 53, 234}
    ,{38, 46, 227}
    ,{39, 39, 219}
    ,{40, 33, 210}
    ,{41, 29, 201}
    ,{42, 25, 191}
    ,{43, 22, 181}
    ,{44, 21, 170}
    ,{45, 20, 160}
    ,{46, 21, 150}
    ,{47, 22, 139}
    ,{48, 25, 129}
    ,{49, 29, 119}
    ,{50, 33, 110}
    ,{51, 39, 101}
    ,{52, 46, 93}
    ,{53, 53, 86}
    ,{54, 61, 79}
    ,{55, 70, 73}
    ,{56, 79, 69}
    ,{57, 89, 65}
    ,{58, 99, 62}
    ,{59, 110, 61}
};

u16 arrowColor = YELLOW;

static void drawTime(void) {
    static int oldSec = 0;
    int sec = (upTime / 1000) % 60;

    if(sec == oldSec) return;

    u16 oldX = clockArrows[oldSec][1];
    u16 oldY = clockArrows[oldSec][2];
    u16 newX = clockArrows[sec][1];
    u16 newY = clockArrows[sec][2];
    POINT_COLOR = BLACK;
    LCD_DrawLine(120, 160, oldX, oldY);

    POINT_COLOR = arrowColor;
    LCD_DrawLine(120, 160, newX, newY);

    oldSec = sec;
}

int main(void) {
    init_hardware();

    u8 id = BME280_GetChipId();

    u8 buf[8] = { 0, 0, 0, 0, 0 };
    u8 on = 0;

    LCD_Fill(1, 1, 10, 10, BLUE);
    LCD_Fill(30, 1, 40, 10, BLUE);

    BACK_COLOR = BLACK;
    POINT_COLOR = RED;
    u16 x = LCD_PrintString(2, 15, 12, "Size=12x6", 0);
    POINT_COLOR = BRRED;
    LCD_PrintString(x + 5, 15, 16, "Size=16x8", 0);

    // UART_SendByte(0x33);
    // UART_SendByte(0x34);
    // UART_SendByte(0x35);

    while (1) {
        drawTime();
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
                    arrowColor = RED;
                } else {
                    PDout(12) = 0;
                    PDout(14) = 0;
                    arrowColor = YELLOW;
                }
            } else {
                if(buf[0] == 0x31) {
                    PDout(12) = 1;
                    PDout(14) = 1;
                    arrowColor = RED;
                } else {
                    PDout(12) = 0;
                    PDout(14) = 0;
                    arrowColor = YELLOW;
                }
            }
        }
    }

    while (1);
}

