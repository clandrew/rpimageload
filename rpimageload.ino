#include "DEV_Config.h"
#include "LCD_1in69.h"
#include "test_image_1.data.h"

// The color format is RGB565. bbbb bxxx xxxx xxxx
#define WHITE       0xFFFF
#define RED         0x07E0
#define GREEN       0x001F
#define BLUE        0xF800

#define BUTTON1_PIN 14
#define BUTTON2_PIN 15
#define BUTTON3_PIN 16

#define LCD_QTR_WIDTH  60
#define LCD_QTR_HEIGHT 70
UWORD gMiniStaging[LCD_QTR_WIDTH * LCD_QTR_HEIGHT]; // 1/4 size. To be scaled up at the very end.

UWORD gStaging[LCD_1IN69_WIDTH * LCD_1IN69_HEIGHT ];

const int ballDiam = 50;
int ballX = 50;
int ballY = 50;
int ballVX = 5;
int ballVY = 5;
int ballColorIndex = 0;

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
  InitializeButton(BUTTON2_PIN);
  InitializeButton(BUTTON3_PIN);
}

static const UWORD colors[] = {RED, GREEN, BLUE };
void IncrementBallColor()
{
  ballColorIndex = (ballColorIndex + 1) % count_of(colors);
}

UWORD GetBallColor()
{
  return colors[ballColorIndex];
}

void MoveBall()
{
  // Horizontal
  ballX += ballVX;
  if (ballX <= 0)
  {
    ballX = 0;
    ballVX = -ballVX;
    IncrementBallColor();
  }
  if (ballX > LCD_1IN69_WIDTH-ballDiam-1)
  {
    ballX = LCD_1IN69_WIDTH-ballDiam-1;
    ballVX = -ballVX;
    IncrementBallColor();
  }
  
  // Vertical
  ballY += ballVY;
  if (ballY <= 0)
  {
    ballY = 0;
    ballVY = -ballVY;
    IncrementBallColor();
  }
  if (ballY > LCD_1IN69_HEIGHT-ballDiam-1)
  {
    ballY = LCD_1IN69_HEIGHT-ballDiam-1;
    ballVY = -ballVY;
    IncrementBallColor();
  }  
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
  // Clear the background to white
  for (int i=0; i<LCD_1IN69_WIDTH * LCD_1IN69_HEIGHT; ++i )
  {
    gStaging[i] = WHITE;
  }
  
  // Copy from the test image to ministaging
  {
    unsigned short* pSrc = test_image_1_data;
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

// the loop function runs over and over again forever
void loop()
{
  MoveBall();
  DrawToStaging();
  CopyStagingToScreen();

  if (!gpio_get(BUTTON1_PIN)) 
  {
      IncrementBallColor();
  }
  
  delay(50); // Wait this number of ms
}
