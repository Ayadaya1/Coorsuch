#include "ssd1306.h"

// Databuffer voor het scherm
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Een scherm-object om lokaal in te werken
static SSD1306_t SSD1306;

int ssd1306_Init(I2C_HandleTypeDef i2c_port, uint8_t address)
{	
  SSD1306.I2C_port = i2c_port;
  SSD1306.Address = address;
  // Waiting for SEG/COM ON after reset
  HAL_Delay(100);
  // Initialize LCD
  uint8_t init_data[] = {0xAE, 0xA6, 0x20, 0x00, 0x21, 0x00, 0x7F, 0x22, 0x00, 0x07, 0xA1, 0xC8, 0xA8, 0x3F, 0x81, 0x7F, 0x8D, 0x14, 0xAF};
  // 0xAE - display off
  // 0xA6 - normal (not inversed)
  // 0x20, 0x00 - page horizontal adressing mode
  // 0x21, 0x00, 0x7F - column address from 0 to 127
  // 0x22, 0x00, 0x07 - page address from 0 to 7
  // 0xA1 - segment re-map (vertical mirroring)
  // 0xC8 - COM scan direction (horizontal mirroring)
  // 0xA8, 0x3F - multiplex ratio
  // 0x81, 0x7F - contrast ratio 127 
  // 0x8D, 0x14 - enable charge pump
  // 0xAF - display on (only just after enabling charge pump)
  ssd1306_SendToDisplay(Commands, init_data, sizeof(init_data));
  // clearing screen
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  // setting default position
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;
return 0;
}

void ssd1306_Fill(SSD1306_COLOR color) 
{
  uint8_t toBuffer = 0x00; // black
  if(color == White)
  {
    toBuffer = 0xFF; // white
  }
  for(uint32_t i = 0; i < sizeof(SSD1306_Buffer); i++)
  {
    SSD1306_Buffer[i] = toBuffer;
  }
}



void ssd1306_UpdateScreen(void) 
{
  uint8_t update_region_data[6] = {0x21, 0x00, 0x7F, 0x22, 0x00, 0x07};
  //0x21, 0x00, 0x7F - column address from 0 to 127
  //0x22, 0x00, 0x07 - page address from 0 to 7
  ssd1306_SendToDisplay(Commands, update_region_data, 6);

  //update pages from 0 to 7
  for(uint32_t page = 0; page < 8; page++) 
  {
    ssd1306_SendToDisplay(Datas, &SSD1306_Buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
  }
}


void ssd1306_DrawPixel(uint32_t x, uint32_t y, SSD1306_COLOR color)
{
  if((x < SSD1306_WIDTH) && (y < SSD1306_HEIGHT)) 
  {
    if (color == White)
    {
      SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } 
    else 
    {
      SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
  }
}


void ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
  uint32_t pixel, x;
  
  if ((((SSD1306.CurrentX + Font.FontWidth) < SSD1306_WIDTH)  && ((SSD1306.CurrentY + Font.FontHeight) < SSD1306_HEIGHT)))
  {
    // write char to display buffer
    for (uint32_t y = 0; y < Font.FontHeight; y++)
    {
      // get font pixel
      if(ch < 127)
       pixel = Font.fontEn[(ch - 32) * Font.FontHeight + y];
      else
       pixel = Font.fontRu[(ch - 192) * Font.FontHeight + y]; 
      // write pixel to display buffer
      x = Font.FontWidth;
      while(x--)
      {
        if (pixel & 0x0001) 
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + x, (SSD1306.CurrentY + y), color);
        } 
        else 
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + x, (SSD1306.CurrentY + y), !color);
        }
        pixel >>= 1;
      }
    } 
  }
  
  // going to next position
  SSD1306.CurrentX += Font.FontWidth;
}


void ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
  // We schrijven alle char tot een nulbyte
  while (*str) 
  {
    ssd1306_WriteChar(*str, Font, color);
    
    // Volgende char
    str++;
  }
  
}


void ssd1306_SetCursor(uint32_t x, uint32_t y) 
{
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

void ssd1306_SetContrast(uint8_t contrast)
{
  //contrast command with empty data
  uint8_t contrast_data[2] = {0x81};
  //adding contrast data
  contrast_data[1] = contrast;
  //and send to display
  ssd1306_SendToDisplay(Commands, contrast_data, 2);
}



static void ssd1306_SendToDisplay(SSD1306_DATA_TYPE type, uint8_t *data, uint8_t length)
{
  HAL_I2C_Mem_Write(&(SSD1306.I2C_port), SSD1306.Address, type, 1, data, length, 6);
}
