# LCD1in8

waveshare electronics 1.8" C yotta microbit code.

# Package product
https://www.waveshare.com/1.8inch-lcd-for-micro-bit.htm

# Overview
This is a small subset of commands to talk to the waveshare lcd from the micro:bit using yotta. The official code base uses makecode which can be a tad restrictive if your in a custom microbit project.

This should also talk to any SPI based ST7735S LCD available as long as the following pinouts are defined;

PIN micro:bit PIN	Description
Vcc	3V3	Power
GND	GND	Ground
MISO	P14	SPI data master input/slave output
MOSI	P15	SPI data master output/slave input
SCK	P13	SPI clock input
LCD_CS	P16	LCD chip selection
RAM_CS	P2	SRAM chip selection
DC	P12	LCD data/command
RST	P8	LCD reset
BL	P1	LCD backlight


I compile and run this using yotta as part of my main.cpp


#include "LCD_Driver.h"
LCD_Driver ldriv;

ldriv.LCD_Init(void); //Initialise the LCD
ldriv.LCD_SetBL(int Lev); //doesnt seem to work but should change the lcd brightness
ldriv.LCD_Clear(UWORD Color); // sets the BG colour so to clear just call LCD_Clear(0xFFFF); (for white)
ldriv.LCD_ClearBuf(void); //Clear LCD buffer
ldriv.LCD_Display(void); // Draw the screen with whatever changes you have sent
ldriv.LCD_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend); // TBD not sure
ldriv.LCD_DrawPoint(int x, int y, int Color, int Dot); // draw pixel
ldriv.LCD_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int colour); //Draw a line (really draws several pixels)
ldriv.LCD_DisString(int Xchar, int Ychar, char Charval[], int Color); // Write String
ldriv.LCD_DrawRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int colour, unsigned int fill); // draw a square filled, great for screen refreshs of a small area
ldriv.LCD_DrawCircle(int xc, int yc,int r,unsigned int colour, int fill); // Draw a circle and fill it   
ldriv.LCD_DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color); // Draw a single character

## License

MIT

## Supported targets

* for Yotta/microbit
(The metadata above is needed for package search.)

