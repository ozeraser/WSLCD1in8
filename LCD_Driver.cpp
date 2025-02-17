#include "MicroBitPin.h"
#include "mbed.h"
#include "LCD_Driver.h"
#include "SPI_RAM.h"

//spi
SPI lcd_spi(MOSI, MISO, SCK);
#define LCD_SPI_Write_Byte(value) lcd_spi.write(value)

//LCD
DigitalOut LCD_RST(MICROBIT_PIN_P8);
DigitalOut LCD_DC(MICROBIT_PIN_P12);
DigitalOut LCD_CS(MICROBIT_PIN_P16);
PwmOut LCD_BL(MICROBIT_PIN_P1);

#define LCD_RST_0 LCD_RST = 0
#define LCD_RST_1 LCD_RST = 1
#define LCD_DC_0 LCD_DC = 0
#define LCD_DC_1 LCD_DC = 1
#define LCD_CS_0 LCD_CS = 0
#define LCD_CS_1 LCD_CS = 1

//delay
#define Driver_Delay_ms(xms) wait_ms(xms)

//SPI Ram
SPIRAM *spiram;

/*********************************************
function:
    Initialization system
*********************************************/
void LCD_Driver::LCD_SPI_Init(void)
{
    lcd_spi.format(8,0);
    lcd_spi.frequency(1000000);
	LCD_CS_1;
}

/*******************************************************************************
function:
    Hardware reset
*******************************************************************************/
void LCD_Driver::LCD_Reset(void)
{
    LCD_RST_1;
    Driver_Delay_ms(100);
    LCD_RST_0;
    Driver_Delay_ms(100);
    LCD_RST_1;
    Driver_Delay_ms(100);
}

/*******************************************************************************
function:
    Write register address and data
*******************************************************************************/
void LCD_Driver::LCD_WriteReg(UBYTE Reg)
{
    LCD_DC_0;
    LCD_CS_0;
    LCD_SPI_Write_Byte(Reg);
    LCD_CS_1;
}

void LCD_Driver::LCD_WriteData_8Bit(UBYTE Data)
{
    LCD_DC_1;
    LCD_CS_0;
    LCD_SPI_Write_Byte(Data);
    LCD_CS_1;
}

void LCD_Driver::LCD_WriteData_Buf(UWORD Buf, unsigned long Len)
{
    unsigned long i;
    LCD_DC_1;
    LCD_CS_0;
    for(i = 0; i < Len; i++) {
        LCD_SPI_Write_Byte((int)(Buf >> 8));
        LCD_SPI_Write_Byte((int)(Buf & 0XFF));
    }
    LCD_CS_1;
}
/*******************************************************************************
function:
    Common register initialization
*******************************************************************************/
void LCD_Driver::LCD_InitReg(void)
{
    //ST7735R Frame Rate
    LCD_WriteReg(0xB1);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB2);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB3);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB4); //Column inversion
    LCD_WriteData_8Bit(0x07);

    //ST7735R Power Sequence
    LCD_WriteReg(0xC0);
    LCD_WriteData_8Bit(0xA2);
    LCD_WriteData_8Bit(0x02);
    LCD_WriteData_8Bit(0x84);
    LCD_WriteReg(0xC1);
    LCD_WriteData_8Bit(0xC5);

    LCD_WriteReg(0xC2);
    LCD_WriteData_8Bit(0x0A);
    LCD_WriteData_8Bit(0x00);

    LCD_WriteReg(0xC3);
    LCD_WriteData_8Bit(0x8A);
    LCD_WriteData_8Bit(0x2A);
    LCD_WriteReg(0xC4);
    LCD_WriteData_8Bit(0x8A);
    LCD_WriteData_8Bit(0xEE);

    LCD_WriteReg(0xC5); //VCOM
    LCD_WriteData_8Bit(0x0E);

    //ST7735R Gamma Sequence
    LCD_WriteReg(0xe0);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x1a);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x18);
    LCD_WriteData_8Bit(0x2f);
    LCD_WriteData_8Bit(0x28);
    LCD_WriteData_8Bit(0x20);
    LCD_WriteData_8Bit(0x22);
    LCD_WriteData_8Bit(0x1f);
    LCD_WriteData_8Bit(0x1b);
    LCD_WriteData_8Bit(0x23);
    LCD_WriteData_8Bit(0x37);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(0x07);
    LCD_WriteData_8Bit(0x02);
    LCD_WriteData_8Bit(0x10);

    LCD_WriteReg(0xe1);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x1b);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x17);
    LCD_WriteData_8Bit(0x33);
    LCD_WriteData_8Bit(0x2c);
    LCD_WriteData_8Bit(0x29);
    LCD_WriteData_8Bit(0x2e);
    LCD_WriteData_8Bit(0x30);
    LCD_WriteData_8Bit(0x30);
    LCD_WriteData_8Bit(0x39);
    LCD_WriteData_8Bit(0x3f);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(0x07);
    LCD_WriteData_8Bit(0x03);
    LCD_WriteData_8Bit(0x10);

    LCD_WriteReg(0xF0); //Enable test command
    LCD_WriteData_8Bit(0x01);

    LCD_WriteReg(0xF6); //Disable ram power save mode
    LCD_WriteData_8Bit(0x00);

    LCD_WriteReg(0x3A); //65k mode
    LCD_WriteData_8Bit(0x05);

    LCD_WriteReg(0x36); //MX, MY, RGB mode
    LCD_WriteData_8Bit(0xF7 & 0xA0); //RGB color filter panel
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
    Xstart  :   X direction Start coordinates
    Ystart  :   Y direction Start coordinates
    Xend    :   X direction end coordinates
    Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_Driver::LCD_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    //set the X coordinates
    LCD_WriteReg(0x2A);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit((Xstart & 0xff) + 1);
    LCD_WriteData_8Bit(0x00 );
    LCD_WriteData_8Bit(((Xend - 1) & 0xff) + 1);

    //set the Y coordinates
    LCD_WriteReg(0x2B);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit((Ystart & 0xff) + 2);
    LCD_WriteData_8Bit(0x00 );
    LCD_WriteData_8Bit(((Yend - 1) & 0xff)+ 2);

    LCD_WriteReg(0x2C);
}

/********************************************************************************
function:
    Set the display point (Xpoint, Ypoint)
********************************************************************************/
void LCD_Driver::LCD_SetCursor(UWORD Xpoint, UWORD Ypoint)
{
    LCD_SetWindows(Xpoint, Ypoint, Xpoint, Ypoint);
}

/********************************************************************************
function:
    Set show color
********************************************************************************/
void LCD_Driver::LCD_SetColor(UWORD Color, UWORD Xpoint, UWORD Ypoint)
{
    LCD_WriteData_Buf(Color, (unsigned long)Xpoint * (unsigned long)Ypoint);
}

/********************************************************************************
function:
    initialization
********************************************************************************/
void LCD_Driver::LCD_Init(void)
{
	//start spi
	SPI spi(MOSI, MISO, SCK);
	spi.write(0x00);
	
	//back light
    LCD_BL.period(0.02);
    LCD_BL.write(1);
	
    LCD_SPI_Init();

    spiram->SPIRAM_SPI_Init();
    spiram->SPIRAM_Set_Mode(BYTE_MODE);

    //Hardware reset
    LCD_Reset();

    //Set the initialization register
    LCD_InitReg();

    //sleep out
    LCD_WriteReg(0x11);
    Driver_Delay_ms(120);

    //Turn on the LCD display
    LCD_WriteReg(0x29);
}

void LCD_Driver::LCD_SetBL(int Lev)
{
    float light = (float)Lev / 10;
    LCD_BL.write(light);
}

/********************************************************************************
function:
    Clear screen
********************************************************************************/
void LCD_Driver::LCD_Clear(UWORD Color)
{
    LCD_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetColor(Color, LCD_WIDTH + 2, LCD_HEIGHT + 2);
}

void LCD_Driver::LCD_ClearBuf(void)
{
    UWORD x, y;
    UWORD Color = 0x0000;  //TBD make this dark an option
    for (y = 0; y < 128; y++) {
        for (x = 0; x < 160; x++ ) {//1 pixel = 2 byte
            spiram->SPIRAM_WR_Byte((x + y * 160)* 2, Color >> 8);
            spiram->SPIRAM_WR_Byte((x + y * 160)* 2 + 1, Color);
        }
    }
}

void LCD_Driver::LCD_SetPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    UWORD Addr = (Xpoint + Ypoint * 160)* 2;
    spiram->SPIRAM_WR_Byte(Addr, Color >> 8);
    spiram->SPIRAM_WR_Byte(Addr + 1, Color & 0xff);
}

void LCD_Driver::LCD_Display(void)
{
    UWORD x, y;
    UBYTE RBUF[160 * 2 * 2];//read tow lines
    memset(RBUF, 0xff, sizeof(RBUF));

    spiram->SPIRAM_Set_Mode(STREAM_MODE);
    LCD_SetWindows(0, 0, 160, 128);
    for (y = 0; y < 128 / 2; y++) {//line
        spiram->SPIRAM_RD_Stream(y * 160 * 2 * 2, RBUF, 160 * 2 * 2);

        LCD_DC_1;
        LCD_CS_0;
        for (x = 0; x < 160 * 2; x++ ) {
            LCD_SPI_Write_Byte((uint8_t)RBUF[x * 2]);
            LCD_SPI_Write_Byte((uint8_t)RBUF[x * 2 + 1]);
        }
        LCD_CS_1;
    }
}

void LCD_Driver::LCD_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    UWORD x, y;
    UBYTE RBUF[(Xend - Xstart + 1) * 2];
    memset(RBUF, 0xff, sizeof(RBUF));

    spiram->SPIRAM_Set_Mode(STREAM_MODE);
    LCD_SetWindows(Xstart, Ystart, Xend, Yend);
    for (y = Ystart; y < Yend; y++) {//line
        spiram->SPIRAM_RD_Stream((y * 160 + Xstart) * 2, RBUF, sizeof(RBUF));

        LCD_DC_1;
        LCD_CS_0;
        for (x = 0; x < Xend - Xstart; x++) {
            LCD_SPI_Write_Byte((uint8_t)RBUF[x * 2]);
            LCD_SPI_Write_Byte((uint8_t)RBUF[x * 2 + 1]);
        }
        LCD_CS_1;
    }
}

void LCD_Driver::LCD_DrawPoint(int x, int y, int Color, int Dot)
{
    int XDir_Num ,YDir_Num;
    for(XDir_Num = 0; XDir_Num < Dot; XDir_Num++) {
        for(YDir_Num = 0; YDir_Num < Dot; YDir_Num++) {
            LCD_SetPoint(x + XDir_Num - Dot, y + YDir_Num - Dot, Color);
        }
    }
}
void LCD_Driver::LCD_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int colour)   
{   
    unsigned int t;    
    int xerr=0,yerr=0,delta_x,delta_y,distance;    
    int incx,incy,uRow,uCol;    
    delta_x=x2-x1;    
    delta_y=y2-y1;    
    uRow=x1;    
    uCol=y1;    
    if(delta_x>0)incx=1;   
    else if(delta_x==0)incx=0;    
    else {incx=-1;delta_x=-delta_x;}    
    if(delta_y>0)incy=1;    
    else if(delta_y==0)incy=0;    
    else{incy=-1;delta_y=-delta_y;}    
    if( delta_x>delta_y)distance=delta_x;   
    else distance=delta_y;    
    for(t=0;t<=distance+1;t++ )   
    {     
        LCD_DrawPoint(uRow,uCol,colour,1);   
        xerr+=delta_x ;    
        yerr+=delta_y ;    
        if(xerr>distance)    
        {    
            xerr-=distance;    
            uRow+=incx;    
        }    
        if(yerr>distance)    
        {    
            yerr-=distance;    
            uCol+=incy;    
        }    
    }     
}

void LCD_Driver::LCD_DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color)
{
    int Page = 0, Column = 0;
    const unsigned char *ptr = &Font12_Table[Char_Offset];

    for(Page = 0; Page < 12; Page ++ ) {
        for(Column = 0; Column < 7; Column ++ ) {
            if(*ptr & (0x80 >> (Column % 8)))
                LCD_SetPoint(Xchar + Column, Ychar + Page, Color);

            //One pixel is 8 bits
            if(Column % 8 == 7)
                ptr++;
        }// Write a line
        if(7 % 8 != 0)
            ptr++;
    }// Write all
}

void LCD_Driver::LCD_DisString(int Xchar, int Ychar, char Charval[], int Color)
{
  int Xpoint = Xchar;
  int Ypoint = Ychar;
  int Font_Height = 12;
  int Font_Width = 7;
  for (int i = 0; Charval[i] != 0; i++){
      int ch_asicc =  (int)Charval[i] - 32;//NULL = 32
      int Char_Offset = ch_asicc * Font_Height;
			// let Char_Offset = ch_asicc * Font_Height *(Font_Width/8 +(Font_Width%8?1:0));
			
            if((Xpoint + Font_Width) > 160) {
                Xpoint = Xchar;
                Ypoint += Font_Height;
            }

            // If the Y direction is full, reposition to(Xstart, Ystart)
            if((Ypoint  + Font_Height) > 128) {
                Xpoint = Xchar;
                Ypoint = Ychar;
            }
            LCD_DisChar_1207(Xpoint, Ypoint, Char_Offset, Color);

            //The next word of the abscissa increases the font of the broadband
            Xpoint += Font_Width;
  }

}

void LCD_Driver::LCD_DrawRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int colour, unsigned int fill)
{  
    int Ypoint;
    if (fill) {
      for(Ypoint = y1; Ypoint < y2; Ypoint++) { //TBD needs to be written to be independant of whats higher.
        LCD_DrawLine(x1, Ypoint, x2, Ypoint, colour);
      }
    }else{
        LCD_DrawLine(x1,y1,x2,y1,colour);   
        LCD_DrawLine(x1,y1,x1,y2,colour);   
        LCD_DrawLine(x1,y2,x2,y2,colour);   
        LCD_DrawLine(x2,y1,x2,y2,colour);
    }   
}   
   
void LCD_Driver::_draw_circle_8(int xc, int yc, int x, int y, unsigned int colour)   
{   
    LCD_DrawPoint(xc + x, yc + y, colour, 3);   //TBD add a size option
    LCD_DrawPoint(xc - x, yc + y, colour, 3);   
    LCD_DrawPoint(xc + x, yc - y, colour, 3);   
    LCD_DrawPoint(xc - x, yc - y, colour, 3);   
    LCD_DrawPoint(xc + y, yc + x, colour, 3);   
    LCD_DrawPoint(xc - y, yc + x, colour, 3);   
    LCD_DrawPoint(xc + y, yc - x, colour, 3);   
    LCD_DrawPoint(xc - y, yc - x, colour, 3);   
}   
   
void LCD_Driver::LCD_DrawCircle(int xc, int yc,int r,unsigned int colour, int fill)   
{   
    int x = 0, y = r, yi, d;   
    d = 3 - 2 * r;   
    if (fill)    
    {          
        while (x <= y)   
        {   
            for (yi = x; yi <= y; yi++)   
                _draw_circle_8(xc, yc, x, yi, colour);   
            if (d < 0)    
            {   
                d = d + 4 * x + 6;   
            }    
            else    
            {   
                d = d + 4 * (x - y) + 10;   
                y--;   
            }   
            x++;   
        }   
    }    
    else    
    {      
        while (x <= y)    
        {   
            _draw_circle_8(xc, yc, x, y, colour);   
            if (d < 0)   
            {   
                d = d + 4 * x + 6;   
            }    
            else    
            {   
                d = d + 4 * (x - y) + 10;   
                y--;   
            }   
            x++;   
        }   
    }   
}   