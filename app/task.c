#include "lcd.h"
#include "task.h"

void checkUartCommand(void) {
    char buf[24] = { 0 };
    char dig[8] = { 0 };
    u32 pack, val;
    u8 cmd, sz;
    u16 x;
    if (UART_GetDataSize() >= 4) {
        pack = UART_GetCommand();
        if(!(IS_VALID_COMMAND(pack) || IS_VALID_MANUAL(pack))) return;
        cmd = (pack & 0xFF00) >> 8;

        itoas(cmd, dig, 16);
        itoas(pack, buf, 16);
        x = LCD_PrintString(2, 20, 12, buf, 0);
        x = LCD_PrintString(x, 20, 12, "=", 0);
        x = LCD_PrintString(x, 20, 12, dig, 0);
        x = LCD_PrintString(x, 20, 12, "-", 0);

        switch(cmd) {
            case 0x00: // Led OFF
            case 0x30: // 0 = Led OFF
            case 0x66: // f = Led OFF
                PDout(12) = 0;
                PDout(14) = 0;
                LCD_PrintString(x, 20, 12, "off    ", 0);
                UART_SendByte(0x30);
                break;

            case 0x01: // Led ON
            case 0x31: // 1 = Led ON
            case 0x6f: // o = Led ON
                PDout(12) = 1;
                PDout(14) = 1;
                LCD_PrintString(x, 20, 12, "on     ", 0);
                UART_SendByte(0x31);
                break;

            case 0x02: // Get Temperature binary
                LCD_PrintString(x, 20, 12, "iTemp  ", 0);
                UART_SendWord(temp);
                break;

            case 0x32: // 2 = Get Temperature text
            case 0x74: // t = Get Temperature text
                LCD_PrintString(x, 20, 12, "sTemp  ", 0);
                sz = itoas(temp, buf, 10);
                UART_SendString(buf, sz);
                break;

            case 0x03: // Get Humidity binary
                UART_SendWord(hum);
                break;

            case 0x33: // 3 = Get Humidity text
            case 0x68: // h = Get Humidity text
                LCD_PrintString(x, 20, 12, "sHum   ", 0);
                val = BME280_GetHumidity(1);
                sz = itoas(val, buf, 10);
                UART_SendString(buf, sz);
                break;

            case 0x04: // Get Pressure binary
                UART_SendWord(press);
                break;

            case 0x34: // 4 = Get Pressure text
            case 0x70: // p = Get Pressure text
                LCD_PrintString(x, 20, 12, "sPres", 0);
                val = BME280_GetPressure(1);
                sz = itoas(val, buf, 10);
                UART_SendString(buf, sz);
                break;
        }
    }
}