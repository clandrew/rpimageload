
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// Actual Pico GPIO wiring
#define LCD_DIN  19   // MOSI - Pico's default hardware SPI0 MOSI pin
#define LCD_CLK  18   // SCK  - Pico's default hardware SPI0 SCK pin
#define LCD_CS   17   // CS
#define LCD_DC   22   // DC
#define LCD_RST  13   // RST
#define LCD_BL   21   // BL

//LCD
#define LCD_CS_0		DEV_Digital_Write(LCD_CS,0)
#define LCD_CS_1		DEV_Digital_Write(LCD_CS,1)

#define LCD_RST_0		DEV_Digital_Write(LCD_RST,0)
#define LCD_RST_1		DEV_Digital_Write(LCD_RST,1)

#define LCD_DC_0		DEV_Digital_Write(LCD_DC,0)
#define LCD_DC_1		DEV_Digital_Write(LCD_DC,1)

#define LCD_BL_0		DEV_Digital_Write(LCD_BL,0)
#define LCD_BL_1		DEV_Digital_Write(LCD_BL,1)

#define LCD_SetBacklight(Value) DEV_SetBacklight(Value)

#ifdef __cplusplus
extern "C" {
#endif

UBYTE DEV_ModuleInit(void);
void DEV_ModuleExit(void);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_SetBacklight(UWORD Value);

#ifdef __cplusplus
}
#endif
#endif
