
#include "DEV_Config.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define LCD_SPI_PORT spi0
#define LCD_SPI_BAUD (40 * 1000 * 1000)

void DEV_SetBacklight(UWORD Value)
{
    // On/off backlight control. (Value > 0 turns it on; no PWM dimming here.)
    gpio_put(LCD_BL, Value > 0 ? 1 : 0);
}

void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_put(Pin, Value ? 1 : 0);
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    gpio_set_dir(Pin, Mode == 0 ? GPIO_IN : GPIO_OUT);
}

static void DEV_GPIO_Init(void)
{
    DEV_GPIO_Mode(LCD_CS, 1);
    DEV_GPIO_Mode(LCD_RST, 1);
    DEV_GPIO_Mode(LCD_DC, 1);
    DEV_GPIO_Mode(LCD_BL, 1);

    LCD_CS_1;
    LCD_BL_1;
}

UBYTE DEV_ModuleInit(void)
{
    DEV_GPIO_Init();

    spi_init(LCD_SPI_PORT, LCD_SPI_BAUD);
    spi_set_format(LCD_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Hand CLK and DIN(MOSI) over to the SPI peripheral. CS is left as a
    // plain GPIO since we toggle it manually around each transfer below.
    gpio_set_function(LCD_CLK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_DIN, GPIO_FUNC_SPI);

    return 0;
}

void DEV_SPI_WriteByte(uint8_t Value)
{
    LCD_CS_0;
    spi_write_blocking(LCD_SPI_PORT, &Value, 1);
    LCD_CS_1;
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    LCD_CS_0;
    spi_write_blocking(LCD_SPI_PORT, pData, Len);
    LCD_CS_1;
}

/******************************************************************************
function:	Module exit
******************************************************************************/
void DEV_ModuleExit(void)
{
    LCD_BL_0;
}
