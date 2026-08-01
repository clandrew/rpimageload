#include "LCD_1in69.h"
#include "DEV_Config.h"
#include "pico/stdlib.h" // For sleep_ms

static void LCD_1IN69_Reset(void)
{
    LCD_1IN69_RST_1;
    sleep_ms(100);
    LCD_1IN69_RST_0;
    sleep_ms(100);
    LCD_1IN69_RST_1;
    sleep_ms(100);
}

static void LCD_1IN69_SendCommand(UBYTE Reg)
{
    LCD_1IN69_DC_0;
    DEV_SPI_WriteByte(Reg);
}

static void LCD_1IN69_SendData_8Bit(UBYTE Data)
{
    LCD_1IN69_DC_1;
    DEV_SPI_WriteByte(Data);
}

static void LCD_1IN69_SendData_16Bit(UWORD Data)
{
    LCD_1IN69_DC_1;
    DEV_SPI_WriteByte((Data >> 8) & 0xFF);
    DEV_SPI_WriteByte(Data & 0xFF);
}

static void LCD_1IN69_InitReg(void)
{
    LCD_1IN69_SendCommand(0x36);
    LCD_1IN69_SendData_8Bit(0x00);

    LCD_1IN69_SendCommand(0x3A);
    LCD_1IN69_SendData_8Bit(0x05);

    LCD_1IN69_SendCommand(0xB2);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x00);
    LCD_1IN69_SendData_8Bit(0x33);
    LCD_1IN69_SendData_8Bit(0x35);

    LCD_1IN69_SendCommand(0xB7);
    LCD_1IN69_SendData_8Bit(0x11);

    LCD_1IN69_SendCommand(0xBB);
    LCD_1IN69_SendData_8Bit(0x35);

    LCD_1IN69_SendCommand(0xC0);
    LCD_1IN69_SendData_8Bit(0x2C);

    LCD_1IN69_SendCommand(0xC2);
    LCD_1IN69_SendData_8Bit(0x01);

    LCD_1IN69_SendCommand(0xC3);
    LCD_1IN69_SendData_8Bit(0x0D);

    LCD_1IN69_SendCommand(0xC4);
    LCD_1IN69_SendData_8Bit(0x20);

    LCD_1IN69_SendCommand(0xC6);
    LCD_1IN69_SendData_8Bit(0x13);

    LCD_1IN69_SendCommand(0xD0);
    LCD_1IN69_SendData_8Bit(0xA4);
    LCD_1IN69_SendData_8Bit(0xA1);

    LCD_1IN69_SendCommand(0xD6);
    LCD_1IN69_SendData_8Bit(0xA1);

    LCD_1IN69_SendCommand(0xE0);
    LCD_1IN69_SendData_8Bit(0xF0);
    LCD_1IN69_SendData_8Bit(0x06);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x0A);
    LCD_1IN69_SendData_8Bit(0x09);
    LCD_1IN69_SendData_8Bit(0x26);
    LCD_1IN69_SendData_8Bit(0x29);
    LCD_1IN69_SendData_8Bit(0x33);
    LCD_1IN69_SendData_8Bit(0x41);
    LCD_1IN69_SendData_8Bit(0x18);
    LCD_1IN69_SendData_8Bit(0x16);
    LCD_1IN69_SendData_8Bit(0x15);
    LCD_1IN69_SendData_8Bit(0x29);
    LCD_1IN69_SendData_8Bit(0x2D);

    LCD_1IN69_SendCommand(0xE1);
    LCD_1IN69_SendData_8Bit(0xF0);
    LCD_1IN69_SendData_8Bit(0x04);
    LCD_1IN69_SendData_8Bit(0x08);
    LCD_1IN69_SendData_8Bit(0x08);
    LCD_1IN69_SendData_8Bit(0x07);
    LCD_1IN69_SendData_8Bit(0x03);
    LCD_1IN69_SendData_8Bit(0x28);
    LCD_1IN69_SendData_8Bit(0x32);
    LCD_1IN69_SendData_8Bit(0x40);
    LCD_1IN69_SendData_8Bit(0x3B);
    LCD_1IN69_SendData_8Bit(0x19);
    LCD_1IN69_SendData_8Bit(0x18);
    LCD_1IN69_SendData_8Bit(0x2A);
    LCD_1IN69_SendData_8Bit(0x2E);

    LCD_1IN69_SendCommand(0xE4);
    LCD_1IN69_SendData_8Bit(0x25);
    LCD_1IN69_SendData_8Bit(0x00);
    LCD_1IN69_SendData_8Bit(0x00);

    LCD_1IN69_SendCommand(0x21);

    LCD_1IN69_SendCommand(0x11);
    sleep_ms(120);
    LCD_1IN69_SendCommand(0x29);
}

static void LCD_1IN69_SetAttributes()
{
    // Get the screen scan direction
    UBYTE MemoryAccessReg = 0x00;

    // Get GRAM and LCD width and height   
    MemoryAccessReg = 0X00; // Scan dir is always vertical.

    // Set the read / write scan direction of the frame memory
    LCD_1IN69_SendCommand(0x36); // MX, MY, RGB mode
    LCD_1IN69_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}

void LCD_1IN69_Init()
{
    // Hardware reset
    LCD_1IN69_Reset();

    // Set the resolution and scanning method of the screen
    LCD_1IN69_SetAttributes();

    // Set the initialization register
    LCD_1IN69_InitReg();
}

void LCD_1IN69_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{    
    // Scan dir is always vertical
    // set the X coordinates
    LCD_1IN69_SendCommand(0x2A);
    LCD_1IN69_SendData_8Bit(Xstart >> 8);
    LCD_1IN69_SendData_8Bit(Xstart);
    LCD_1IN69_SendData_8Bit((Xend-1) >> 8);
    LCD_1IN69_SendData_8Bit(Xend-1);

    // set the Y coordinates
    LCD_1IN69_SendCommand(0x2B);
    LCD_1IN69_SendData_8Bit((Ystart+20) >> 8);
    LCD_1IN69_SendData_8Bit(Ystart+20);
    LCD_1IN69_SendData_8Bit((Yend+20-1) >> 8);
    LCD_1IN69_SendData_8Bit(Yend+20-1);
    
    LCD_1IN69_SendCommand(0x2C);   
}

void LCD_1IN69_Display(UWORD *Image)
{
    UWORD j;
    
    LCD_1IN69_SetWindows(0, 0, LCD_1IN69_WIDTH, LCD_1IN69_HEIGHT);
    LCD_1IN69_DC_1;
    for (j=0; j<LCD_1IN69_HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j * LCD_1IN69_WIDTH], LCD_1IN69_WIDTH * 2);
    }
}

void LCD_1IN69_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    UWORD j;
    
    LCD_1IN69_SetWindows(Xstart, Ystart, Xend, Yend);
    LCD_1IN69_DC_1;
    for (j=0; j<Yend-Ystart; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j * (Xend-Xstart)], (Xend-Xstart) * 2);
    }
}