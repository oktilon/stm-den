#ifndef __LCD_H
#define __LCD_H
#include "sys.h"
#include "stdlib.h"

typedef struct
{
    u16 width;   //LCD width
    u16 height;  //LCD height
    u16 id;      //LCD ID
    u8  dir;     //LCD orientation (0=0deg, 1=90deg, 2=180deg, 3=270deg)
    u16 wramcmd; //gramָ cmd
    u16 setxcmd; //set x pos
    u16 setycmd; //set y pos
}_lcd_dev;

//LCD Device strust
extern _lcd_dev lcddev;

#define USE_HORIZONTAL       0
#define LCD_W 240
#define LCD_H 320

extern u16  POINT_COLOR;
extern u16  BACK_COLOR;

//======== LCD PINs =======
#define LED  0
#define RST  2
#define RS   3
#define CS   4

#define LCD_CTRL_GPIO   GPIOA
#define LCD_CTRL_PINS   GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
#define LCD_LED_SRC     GPIO_PinSource0
#define LCD_CS_SRC      GPIO_PinSource2
#define LCD_RS_SRC      GPIO_PinSource3
#define LCD_RST_SRC     GPIO_PinSource4

#define LCD_SPI_GPIO    GPIOA
#define LCD_SPI_PINS    GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
#define LCD_CLK_SRC     GPIO_PinSource5
#define LCD_MISO_SRC    GPIO_PinSource6
#define LCD_MOSI_SRC    GPIO_PinSource7

#define LCD_LED PAout(LED)
#define LCD_CS  PAout(CS)
#define LCD_RS  PAout(RS)
#define LCD_RST PAout(RST)
//=========================

#define LCD_SPI       SPI1

#define  LCD_CS_SET   LCD_CS=1
#define  LCD_RS_SET   LCD_RS=1
#define  LCD_RST_SET  LCD_RST=1
#define  LCD_LED_SET  LCD_LED=1


#define  LCD_CS_CLR  LCD_CS=0
#define  LCD_RS_CLR  LCD_RS=0
#define  LCD_RST_CLR LCD_RST=0
#define  LCD_LED_CLR  LCD_LED=0

// Color palette
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40
#define BRRED       0XFC07
#define GRAY        0X8430
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LIGHTGRAY   0XEF5B
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);
u16  LCD_ReadPoint(u16 x,u16 y);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);

u16 LCD_RD_DATA(void);
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction );


/*
#if LCD_USE8BIT_MODEL==1//ʹ��8λ������������ģʽ
    #define LCD_WR_DATA(data){\
    LCD_RS_SET;\
    LCD_CS_CLR;\
    DATAOUT(data);\
    LCD_WR_CLR;\
    LCD_WR_SET;\
    DATAOUT(data<<8);\
    LCD_WR_CLR;\
    LCD_WR_SET;\
    LCD_CS_SET;\
    }
    #else//ʹ��16λ������������ģʽ
    #define LCD_WR_DATA(data){\
    LCD_RS_SET;\
    LCD_CS_CLR;\
    DATAOUT(data);\
    LCD_WR_CLR;\
    LCD_WR_SET;\
    LCD_CS_SET;\
    }
#endif
*/

#endif