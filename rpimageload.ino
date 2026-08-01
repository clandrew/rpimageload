#include "DEV_Config.h"
#include "LCD_1in69.h"
#include "test_image_1.data.h"
#include "test_image_2.data.h"
#include "test_image_3.data.h"

#define BUTTON1_PIN 14

#define LCD_QTR_WIDTH  60
#define LCD_QTR_HEIGHT 70
UWORD gMiniStaging[LCD_QTR_WIDTH * LCD_QTR_HEIGHT]; // 1/4 size. To be scaled up at the very end.
UWORD gStaging[LCD_1IN69_WIDTH * LCD_1IN69_HEIGHT ];
int gImageIndex=0;
bool gButtonPress=false;
bool gInvert = false;
int ganimationCounter=0;

void InitializeButton(uint gpio)
{
  // The buttons use pull-up resistors
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_pull_up(gpio);
}

void setup() 
{
  DEV_ModuleInit();
  LCD_1IN69_Init();

  InitializeButton(BUTTON1_PIN);
}

void Scaled4xBlit()
{
  // Now do a scaled blit
  size_t rowSize = LCD_1IN69_WIDTH * 2;
  unsigned short* pSrcRow = gMiniStaging;
  unsigned short* pDstRow = gStaging;
  for (int ySrc=0; ySrc<LCD_QTR_HEIGHT; ++ySrc)
  {
    // Copy one row, scaled to 4x width
    unsigned short* pDstPx = pDstRow;
    for (int xSrc=0; xSrc<LCD_QTR_WIDTH; ++xSrc)
    {
      unsigned short px = pSrcRow[xSrc];

      if (gInvert)
      {
        px = ~px;
      }

      *pDstPx = px;
      pDstPx++;
      *pDstPx = px;
      pDstPx++;
      *pDstPx = px;
      pDstPx++;
      *pDstPx = px;
      pDstPx++;
    }

    // Copy the previous row three more times.
    unsigned short* pCopied = pDstRow;
    pDstRow += LCD_1IN69_WIDTH;
    memcpy(pDstRow, pCopied, rowSize);
    pDstRow += LCD_1IN69_WIDTH;
    memcpy(pDstRow, pCopied, rowSize);
    pDstRow += LCD_1IN69_WIDTH;
    memcpy(pDstRow, pCopied, rowSize);
    pDstRow += LCD_1IN69_WIDTH;

    pSrcRow += LCD_QTR_WIDTH;
  }
}

void DrawToStaging()
{
  // Copy from the test image to ministaging
  {
    unsigned short* pSrc;
    if (gImageIndex == 0)
    {
      pSrc = test_image_1_data;
    }
    else if (gImageIndex == 1)
    {
      pSrc = test_image_2_data;
    }
    else if (gImageIndex == 2)
    {
      pSrc = test_image_3_data;
    }
    unsigned short* pDst = gMiniStaging;
    size_t copySize = test_image_1_width * test_image_1_height * 2;    
    memcpy(pDst, pSrc, copySize);
  }

  // Scaled blit from ministaging to staging
  Scaled4xBlit();
}

void CopyStagingToScreen()
{
  LCD_1IN69_SetWindows(0, 0, LCD_1IN69_WIDTH, LCD_1IN69_HEIGHT);
  DEV_Digital_Write(LCD_DC,1);
  DEV_SPI_Write_nByte((uint8_t*)&gStaging, LCD_1IN69_WIDTH * LCD_1IN69_HEIGHT * 2);
}

void AdvanceAnimation()
{
  ganimationCounter++;
  if (ganimationCounter > 3)
  {
    gImageIndex = (gImageIndex+1) % 3;
    ganimationCounter = 0;
  }
}

// the loop function runs over and over again forever
void loop()
{
  DrawToStaging();
  CopyStagingToScreen();
  AdvanceAnimation();

  bool buttonPress = !gpio_get(BUTTON1_PIN);
  if (gButtonPress == false && buttonPress == true)
  {
    gInvert = !gInvert;
  }  
  gButtonPress = buttonPress;

  
  delay(50); // Wait this number of ms
}
