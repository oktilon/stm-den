#include "board.h"

// void _init(void) {;}

int main(void) {
    init_hardware();
    LCD_LED_CLR;
    // ENC_init();

    uint16_t tx[3] = {
        0xAA,
        0x11,
        0x55
    };

    LCD_LED_SET;
    SPI_WriteByte(LCD_SPI, tx[0]);
    SPI_WriteByte(LCD_SPI, tx[1]);
    SPI_WriteByte(LCD_SPI, tx[2]);
    LCD_LED_CLR;
    /*
    LCD_setOrientation(ORIENTATION_PORTRAIT);
    LCD_fillScreen(BLACK);*/

    char buf[16];
    /*LCD_setTextBgColor(BLACK);
    LCD_setTextSize(3);*/
    u16 ov = 10;
    u16 dec = 0;
    u8 cnt = 0;
    u8 on = 0;
    while (1) {
        if (ov != dec) {
            // LCD_setCursor(0, 0);
            //itoa(dec, buf, 10);
            // LCD_fillRect(0, 0, 100, 24, BLACK);
            // LCD_writeString((unsigned char*)buf);
            ov = dec;
        }
        delay_ms(10);
        cnt++;
        if(cnt > 50) {
            cnt = 0;
            dec++;
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
    }

    while (1);
}

