#include "lib/ILI9341/text.h"
#include "lib/tinystd/itoa.h"
#include "lib/encoder/encoder.h"

void _init(void) {;}

int main(void) {
    LCD_init();
    GPIO_init();
    ENC_init();
    LCD_setOrientation(ORIENTATION_PORTRAIT);
    LCD_fillScreen(BLACK);

    char buf[16];
    LCD_setTextBgColor(BLACK);
    LCD_setTextSize(3);
    u16 ov = 111;
    u16 encVal;
    u8 cnt = 0;
    u8 on = 0;
    while (1) {
        if (ov != (encVal = ENC_getValue())) {
            LCD_setCursor(0, 0);
            itoa(encVal, buf, 10);
            LCD_fillRect(0, 0, 100, 24, BLACK);
            LCD_writeString((unsigned char*)buf);
            ov = encVal;
        }
        delay_ms(10);
        cnt++;
        if(cnt > 50) {
            cnt = 0;
            if(on) {
                GPIO_ResetBits(GPIOD, GPIO_Pin_15);
                on = 0;
            } else {
                GPIO_SetBits(GPIOD, GPIO_Pin_15);
                on = 1;
            }
        }
    }

    while (1);
}

