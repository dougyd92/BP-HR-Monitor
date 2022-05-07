#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

#define STATUS_BYTE 0xF0
#define OUT_DATA_1 0x00
#define OUT_DATA_2 0x00
#define OUT_DATA_3 0x00

#define BACKGROUND 1
#define FOREGROUND 0

#define WAITING_STATE 0
#define READING_STATE 1

#define BUTTON_PUSH_EVENT 1

SPI spi(PE_6, PE_5, PE_2); // mosi, miso, sclk
DigitalOut chip_select(PE_4);

char display_buf[2][60];
InterruptIn buttonInterrupt(USER_BUTTON,PullDown);

uint8_t read_buf[32];

LCD_DISCO_F429ZI lcd;

uint8_t state;
bool stateChanged;

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

void stateMachine(uint8_t event)

{
  switch (state)
  {
  case WAITING_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      state = READING_STATE;
      stateChanged = true;
    }
    break;
  case READING_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      state = WAITING_STATE;
      stateChanged = true;
    }
    break;
  }
}

//Button Interrupt
void buttonEvent()
{
  stateMachine(BUTTON_PUSH_EVENT);  
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
  //printf("Status 0 %02X \n",status);
  chip_select = 1;
 
  thread_sleep_for(6);

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
}

void waitingScene()
{
  snprintf(display_buf[0], 60, "TODO");
  snprintf(display_buf[1], 60, "Instructions go here");

  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
}

int main()
{
  state = WAITING_STATE;
  stateChanged = true;

  setup_background_layer();
  setup_foreground_layer();
  snprintf(display_buf[0], 60, "Initializing...");
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);

  // Chip must be deselected
  chip_select = 1;

  // Setup the spi for 8 bit data, high steady state clock,
  // second edge capture, with a 5KHz clock rate
  spi.format(8, 0);
  spi.frequency(50000);

  buttonInterrupt.rise(&buttonEvent);

  while (1)
  {
    if (stateChanged) {
      lcd.Clear(LCD_COLOR_BLACK);
      stateChanged = false;
    }
    switch (state)
    {
    case WAITING_STATE:
      waitingScene();
      break;
    case READING_STATE:
      pressureReadingScene();
      break;
    default:
      break;
    }

    thread_sleep_for(100);
  }
}
