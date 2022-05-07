#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

#define STATUS_BYTE 0xF0
#define OUT_DATA_1 0x00
#define OUT_DATA_2 0x00
#define OUT_DATA_3 0x00

#define BACKGROUND 1
#define FOREGROUND 0

SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk
DigitalOut chip_select(PA_4);

char display_buf[2][60];

uint8_t read_buf[32];

LCD_DISCO_F429ZI lcd;

// sets the background layer
// to be visible, transparent, and
// resets its colors to all black

void setup_background_layer()
{
  lcd.SelectLayer(BACKGROUND);
  lcd.Clear(LCD_COLOR_BLACK);
  lcd.SetBackColor(LCD_COLOR_BLACK);
  lcd.SetTextColor(LCD_COLOR_GREEN);
  lcd.SetLayerVisible(BACKGROUND, ENABLE);
  lcd.SetTransparency(BACKGROUND, 0x7Fu);
}

// resets the foreground layer to
// all black
void setup_foreground_layer()
{
  lcd.SelectLayer(FOREGROUND);
  lcd.Clear(LCD_COLOR_BLACK);
  lcd.SetBackColor(LCD_COLOR_BLACK);
  lcd.SetTextColor(LCD_COLOR_LIGHTGREEN);
}


void pressureReadingScene()
{
  int32_t raw_gx;
  float gx;
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
  //uint8_t status;

  chip_select = 0;
  uint8_t status = spi.write(0xAA);
  spi.write(0x00);
  spi.write(0x00);
  printf("Status 0 %02X \n",status);
  chip_select = 1;
 
  thread_sleep_for(12);

  chip_select = 0;
  status = spi.write(0xF0);

  data1 = spi.write(OUT_DATA_1);
  data2 = spi.write(OUT_DATA_2);
  data3 = spi.write(OUT_DATA_3);

  chip_select = 1;

  raw_gx = ((((uint32_t)data1) << 16) | ((uint32_t)data2) << 8) | ((uint8_t)data3);
  gx = (((((float)raw_gx) - 419430.4) * (300)) / (3774873.6 - 419430.4));
  printf("Actual new pressure: %4.5f \n", gx);

  snprintf(display_buf[0], 60, "Pressure %4.5f mmHg",gx);

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);

  thread_sleep_for(100);
}

int main()
{
  // Chip must be deselected
  chip_select = 1;

  // Setup the spi for 8 bit data, high steady state clock,
  // second edge capture, with a 5KHz clock rate
  spi.format(8, 0);
  spi.frequency(50000);

  setup_background_layer();
  setup_foreground_layer();

  snprintf(display_buf[0], 60, "Hello, world!");
  snprintf(display_buf[1], 60, "From Doug and Mateo");

  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);

  while (1)
  {

    pressureReadingScene();
  }
}
