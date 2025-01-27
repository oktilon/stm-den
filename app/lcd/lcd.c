#include "lcd.h"
#include "sys.h"
#include "spi.h"
#include "commands.h"
#include "font.h"

static const uint8_t init_commands[] = {
    // Power control A
    6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
    // Power control B
    4, LCD_POWERB, 0x00, 0xC1, 0x30,
    // Driver timing control A
    4, LCD_DTCA, 0x85, 0x00, 0x78,
    // Driver timing control B
    3, LCD_DTCB, 0x00, 0x00,
    // Power on sequence control
    5, LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,
    // Pump ratio control
    2, LCD_PRC, 0x20,
    // Power control 1
    2, LCD_POWER1, 0x23,
    // Power control 2
    2, LCD_POWER2, 0x10,
    // VCOM control 1
    3, LCD_VCOM1, 0x3E, 0x28,
    // VCOM cotnrol 2
    2, LCD_VCOM2, 0x86,
    // Memory access control
    2, LCD_MAC, 0x48,
    // Pixel format set
    2, LCD_PIXEL_FORMAT, 0x55,
    // Frame rate control
    3, LCD_FRMCTR1, 0x00, 0x18,
    // Display function control
    4, LCD_DFC, 0x08, 0x82, 0x27,
    // 3Gamma function disable
    2, LCD_3GAMMA_EN, 0x00,
    // Gamma curve selected
    2, LCD_GAMMA, 0x01,
    // Set positive gamma
    16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    16, LCD_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    0
};

_lcd_dev lcddev;

u16 POINT_COLOR = 0x0000;
u16 BACK_COLOR = 0xFFFF;
u16 DeviceCode;

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_REG(u8 data) {
    LCD_CS_CLR;
    LCD_RS_CLR;
    SPI_WriteByte(LCD_SPI, data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_DATA(u8 data) {
    LCD_CS_CLR;
    LCD_RS_SET;
    SPI_WriteByte(LCD_SPI, data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
 ******************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    SPI_WriteByte(SPI1, Data >> 8);
    SPI_WriteByte(SPI1, Data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y); //���ù��λ��
    Lcd_WriteData_16Bit(POINT_COLOR);
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
 ******************************************************************************/
void LCD_Clear(u16 Color)
{
    unsigned int i, m;
    LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);
    LCD_CS_CLR;
    LCD_RS_SET;
    for (i = 0; i < lcddev.height; i++)
    {
        for (m = 0; m < lcddev.width; m++)
        {
            Lcd_WriteData_16Bit(Color);
        }
    }
    LCD_CS_SET;
}

/*******************************************************************
 * @name       :void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
 * @date       :2018-08-09
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area
								color:the filled color value
 * @retvalue   :None
********************************************************************/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color) {
	u16 i, j;
	u16 width  = ex - sx + 1;
	u16 height = ey - sy + 1;
	LCD_SetWindows(sx, sy, ex, ey);
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
		    Lcd_WriteData_16Bit(color);
        }
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);
}

/*******************************************************************
 * @name       :void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09
 * @function   :Draw a line between two points
 * @parameters :x1:the beginning x coordinate of the line
                y1:the beginning y coordinate of the line
                x2:the ending x coordinate of the line
                y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr = 0,
        yerr = 0,
        delta_x,
        delta_y,
        distance;
	int incx,
        incy,
        uRow,
        uCol;

	delta_x = x2 - x1;
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

    if( delta_x > 0 ) {
        incx=1;
    } else if( delta_x == 0 ) {
        incx=0;
    } else {
        incx =- 1;
        delta_x =- delta_x;
    }

    if( delta_y > 0 ) {
        incy = 1;
    } else if( delta_y == 0 ) {
        incy = 0;
    } else {
        incy =- 1;
        delta_y =- delta_y;
    }

    if( delta_x > delta_y ) {
        distance = delta_x;
    } else {
        distance = delta_y;
    }

    for( t = 0; t <= distance + 1; t++ ) {
		LCD_DrawPoint(uRow, uCol);
		xerr += delta_x;
		yerr += delta_y;
		if( xerr > distance ) {
			xerr -= distance;
			uRow += incx;
		}
		if( yerr > distance ) {
			yerr -= distance;
			uCol += incy;
		}
	}
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = LCD_CTRL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(LCD_CTRL_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LCD_SPI_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(LCD_SPI_GPIO, &GPIO_InitStructure);

    GPIO_PinAFConfig(LCD_SPI_GPIO, LCD_CLK_SRC, GPIO_AF_SPI1);
    GPIO_PinAFConfig(LCD_SPI_GPIO, LCD_MISO_SRC, GPIO_AF_SPI1);
    GPIO_PinAFConfig(LCD_SPI_GPIO, LCD_MOSI_SRC, GPIO_AF_SPI1);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_RESET(void)
{
    LCD_RST_CLR;
    delay_ms(100);
    LCD_RST_SET;
    delay_ms(50);
}

static void LCD_configure()
{
    u8 count, ix;
    u8 *address = (u8 *)init_commands;

    LCD_CS_CLR;
    while (1)
    {
        count = *(address++);
        if (count-- == 0)
            break;
        LCD_WR_REG(*(address++));
        for(ix = 0; ix < count; ix++) {
            LCD_WR_DATA(address[ix]);
        }
        address += count;
    }
    LCD_CS_SET;

    // LCD_setOrientation(0);
}

/*
static void LCD_configure()
{
    LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC9); // C1
        LCD_WR_DATA(0X30);
    LCD_WR_REG(0xED);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0X12);
        LCD_WR_DATA(0X81);
    LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x85);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x02);
    LCD_WR_REG(0xF7);
        LCD_WR_DATA(0x20);
    LCD_WR_REG(0xEA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC0);  // Power control
        LCD_WR_DATA(0x1B); // VRH[5:0]
    LCD_WR_REG(0xC1);  // Power control
        LCD_WR_DATA(0x00); // SAP[2:0];BT[3:0] 01
    LCD_WR_REG(0xC5);  // VCM control
        LCD_WR_DATA(0x30); // 3F
        LCD_WR_DATA(0x30); // 3C
    LCD_WR_REG(0xC7);  // VCM control2
        LCD_WR_DATA(0XB7);
    LCD_WR_REG(0x36); // Memory Access Control
        LCD_WR_DATA(0x08);
    LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xB6); // Display Function Control
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2); // 3Gamma Function Disable
        LCD_WR_DATA(0x00);
    LCD_WR_REG(0x26); // Gamma curve selected
        LCD_WR_DATA(0x01);
    LCD_WR_REG(0xE0); // Set Gamma
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x2A);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x0E);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0XA9);
        LCD_WR_DATA(0x43);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
    LCD_WR_REG(0XE1); // Set Gamma
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x15);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x2B);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xef);
} */

static void LCD_exitStandby() {
    LCD_WR_REG(LCD_SLEEP_OUT);
    delay_ms(150);
    LCD_WR_REG(LCD_DISPLAY_ON);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_Init(void)
{
    LCD_GPIOInit();
    SPI_Initialize(LCD_SPI);
    //    SPI_SetSpeed(SPI1,SPI_BaudRatePrescaler_2);

    LCD_RESET();
    LCD_configure();

    LCD_exitStandby();

    LCD_direction(2);
    LCD_LED_SET;
    LCD_Clear(BLACK);
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
                                yStar:the bebinning y coordinate of the LCD display window
                                xEnd:the endning x coordinate of the LCD display window
                                yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);

    LCD_WriteRAM_Prepare();
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
                                Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0 = 0 degree
                          1 = 90 degree
                          2 = 180 degree
                          3 = 270 degree
 * @retvalue   :None
******************************************************************************/
void LCD_direction(u8 direction)
{

    lcddev.setxcmd = LCD_COLUMN_ADDR;
    lcddev.setycmd = LCD_PAGE_ADDR;
    lcddev.wramcmd = LCD_GRAM;
    switch (direction)
    {
    case 0:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(LCD_MAC, (1 << 3) | (0 << 5) | (0 << 6) | (0 << 7)); // BGR==1, MV==0, MX==0, MY==0
        break;
    case 1:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(LCD_MAC, (1 << 3) | (1 << 5) | (0 << 6) | (1 << 7)); // BGR==1, MV==1, MX==0, MY==1
        break;
    case 2:
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;
        LCD_WriteReg(LCD_MAC, (1 << 3) | (0 << 5) | (0 << 6) | (1 << 7)); // BGR==1, MV==0, MX==0, MY==1
        break;
    case 3:
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;
        LCD_WriteReg(LCD_MAC, (1 << 3) | (1 << 5) | (0 << 6) | (1 << 7)); // BGR==1, MV==1, MX==0, MY==1
        break;
    default:
        break;
    }
}

void LCD_PutChar(u16 x, u16 y, u16 fc, u16 bc, char ch, u8 size, u8 mode) {
    u8 temp;
    u8 pos, t;
    u16 colortemp = POINT_COLOR;

    u8 num = ch - ' ';
    LCD_SetWindows(x, y, x + size / 2 - 1, y + size - 1);
    if( !mode ) {
        for( pos = 0; pos < size; pos++) {
            if( size == 12 ) {
                temp = asc2_1206[num][pos];   // 12x6
            } else {
                temp = asc2_1608[num][pos];   // 16x8
            }

            for( t = 0; t < size / 2; t++) {
                if( temp & 0x01) {
                    Lcd_WriteData_16Bit(fc);
                } else {
                    Lcd_WriteData_16Bit(bc);
                }
                temp >>= 1;
            }
        }
    } else {
        for( pos = 0; pos < size; pos++) {
            if( size == 12 ) {
                temp = asc2_1206[num][pos];
            } else {
                temp = asc2_1608[num][pos];
            }

            for( t = 0; t < size / 2; t++) {
                POINT_COLOR = fc;
                if( temp & 0x01) {
                    LCD_DrawPoint(x + t, y + pos);
                }
                temp >>= 1;
            }
        }
    }
    POINT_COLOR = colortemp;
    LCD_SetWindows(0, 0, lcddev.width-1, lcddev.height-1);
}

u16 LCD_PrintString(u16 x, u16 y, u8 size, char *p, u8 mode) {
    while((*p<='~')&&(*p>=' ')) {
        if( x > (lcddev.width - 1) || y > (lcddev.height-1) ) {
            return x;
        }
        LCD_PutChar(x, y, POINT_COLOR, BACK_COLOR, *p, size, mode);
        x += size/2;
        p++;
    }
    return x;
}
