#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

#define STATUS_BYTE 0xF0
#define OUT_DATA_1 0x00
#define OUT_DATA_2 0x00
#define OUT_DATA_3 0x00

#define BACKGROUND 1
#define FOREGROUND 0
#define GRAPH_PADDING 5

#define WAITING_STATE 0
#define READING_STATE 1
#define ANALYSIS_STATE 2
#define RESULTS_STATE 3
#define ERROR_STATE 4

#define BUTTON_PUSH_EVENT 1
#define PRESSURE_MIN_EVENT 2
#define ANALYSIS_COMPLETE_EVENT 3
#define ERROR_EVENT 4

SPI spi(PE_6, PE_5, PE_2); // mosi, miso, sclk
DigitalOut chip_select(PE_4);

char display_buf[4][60];
InterruptIn buttonInterrupt(USER_BUTTON, PullDown);

uint8_t read_buf[32];

LCD_DISCO_F429ZI lcd;

uint32_t graph_width = lcd.GetXSize() - 2 * GRAPH_PADDING;
uint32_t graph_height = graph_width;

uint8_t state;
bool stateChanged;

float pressureY[1000];
int Heart_Rate;
int systolic_pressure;
int diastolic_pressure;
uint16_t numReadings;

bool MaxPressure_Reached = false;
bool Beats_Error = false;
bool Array_Error = false;
bool Slow_deflation = false;

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

// draws a rectangle with horizontal tick marks
// on the background layer. The spacing between tick
// marks in pixels is taken as a parameter
void draw_graph_window(uint32_t horiz_tick_spacing)
{
  lcd.SelectLayer(BACKGROUND);

  lcd.DrawRect(GRAPH_PADDING, GRAPH_PADDING, graph_width, graph_width);
  // draw the x-axis tick marks
  for (uint32_t i = 0; i < graph_width; i += horiz_tick_spacing)
  {
    lcd.DrawVLine(GRAPH_PADDING + i, graph_height, GRAPH_PADDING);
  }
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
    switch (event)
    {
    case BUTTON_PUSH_EVENT:
      state = WAITING_STATE;
      stateChanged = true;
      break;
    case PRESSURE_MIN_EVENT:
      state = ANALYSIS_STATE;
      stateChanged = true;
      break;
    default:
      break;
    }
    break;
  case ANALYSIS_STATE:
    if (event == ANALYSIS_COMPLETE_EVENT)
    {
      state = RESULTS_STATE;
      stateChanged = true;
    }else if (event == ERROR_EVENT)
    {
      state = ERROR_STATE;
      stateChanged = true;
    }
    break;
  case RESULTS_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      state = WAITING_STATE;
      stateChanged = true;
    }
    break;
  case ERROR_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      state = WAITING_STATE;
      stateChanged = true;
    }
    break;
  }
}

// Button Interrupt
void buttonEvent()
{
  stateMachine(BUTTON_PUSH_EVENT);
}

void setUpPressureReadingScene()
{
  lcd.Clear(LCD_COLOR_BLACK);
  draw_graph_window(10);
  numReadings = 0;
  MaxPressure_Reached = false;
}

void pressureReadingScene()
{
  int32_t raw_pressure;
  float pressure;
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
  // uint8_t status;

  chip_select = 0;
  uint8_t status = spi.write(0xAA);
  spi.write(0x00);
  spi.write(0x00);
  // printf("Status 0 %02X \n",status);
  chip_select = 1;

  thread_sleep_for(6);

  chip_select = 0;
  status = spi.write(0xF0);

  data1 = spi.write(OUT_DATA_1);
  data2 = spi.write(OUT_DATA_2);
  data3 = spi.write(OUT_DATA_3);

  chip_select = 1;

  raw_pressure = ((((uint32_t)data1) << 16) | ((uint32_t)data2) << 8) | ((uint8_t)data3);
  pressure = (((((float)raw_pressure) - 419430.4) * (300)) / (3774873.6 - 419430.4));
  //printf("Actual new pressure: %4.5f %d \n", pressure,numReadings);
  printf("%4.5f\n",pressure);

  snprintf(display_buf[0], 60, "Pressure %4.5f mmHg", pressure);

  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
  pressureY[numReadings] = pressure;

  uint16_t mapY = GRAPH_PADDING + graph_height - pressure / 200 * graph_height;
  lcd.DrawPixel(numReadings + GRAPH_PADDING, mapY, LCD_COLOR_BLUE);

  if (pressure >= 150)
  {
    MaxPressure_Reached = true;
  }
  else if (MaxPressure_Reached && pressure < 30)
  {
    stateMachine(PRESSURE_MIN_EVENT);
  }
  if (MaxPressure_Reached && pressureY[numReadings - 1] - pressure > 4)
  {
    snprintf(display_buf[1], 60, "Realeasing air too fast");
    lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
  }
  else
  {
    lcd.ClearStringLine(LINE(16));
  }

  numReadings++;
  printf("Num %d\n", numReadings);
  if(numReadings >= 10)
  {
    Array_Error = true;
    stateMachine(ERROR_EVENT);
    return;
  }
}

void waitingScene()
{
  snprintf(display_buf[0], 60, "TODO");
  snprintf(display_buf[1], 60, "Instructions go here");

  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
}

int findMinIndex(int start, int end, float *arr)
{
  float min = arr[start];
  int min_index = start;
  for (int i = start; i <= end; i++)
  {
    if (arr[i] < min)
    {
      min = arr[i];
      min_index = i;
    }
  }
  return min_index;
}

void dataAnalysis()
{
  // Heart Rate
  //(present value - last value)/200ms
  snprintf(display_buf[0], 60, "Analyzing data...");
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);

  int startingIndex;
  int localMax[200];
  int localMin[200];
  int numBeats = 0;
  int minCounter = 0;
  int total_Time_Beats = 0;
  for (int i = 0; i < numReadings; i++)
  {
    if (pressureY[i] > 150 && pressureY[i] > pressureY[i+1])
    {
      startingIndex = i+1;
      break;
    }
  }
  printf("Starting index %d \n", startingIndex);
  for (int i = startingIndex; i < numReadings - 1; i++)
  {
    if (pressureY[i] > pressureY[i - 1] && pressureY[i] > pressureY[i + 1])
    {
      localMax[numBeats] = i;
      numBeats++;
    }
  }
  printf("Num beats %d \n", numBeats);
  if (numBeats == 0 || numBeats < 3)
  {
    Beats_Error = true;
    stateMachine(ERROR_EVENT);
    return;
  }
  
  float max_amplitude = 0;
  int max_amp_index = startingIndex;

  for (int i = 0; i < numBeats - 1; i++)
  {
    total_Time_Beats += localMax[i + 1] - localMax[i];

    int min_index = findMinIndex(localMax[i], localMax[i + 1], pressureY);
    printf("Min index between %d %d = %d \n",localMax[i],localMax[i+1],min_index);
    float amplitude = pressureY[localMax[i]] - pressureY[min_index];
    printf("Amplitude: %4.0f \n", amplitude);

    if (amplitude > max_amplitude)
    {
      max_amplitude = amplitude;
      max_amp_index = localMax[i];
    }
  }
  printf("Max amplitude index %d\n", max_amp_index);
  printf("Local max %d\n", localMax[0]);
  // Blood pressure
  systolic_pressure = pressureY[localMax[0]];

  int diastolic_index = 2 * max_amp_index - localMax[0];
  diastolic_pressure = pressureY[diastolic_index];

  printf("Diastolic index %d\n", diastolic_index);
  // Heart rate
  Heart_Rate = float(total_Time_Beats) / (numBeats - 1) * (0.2) * 60;
  printf("Heart Rate: %d \n", Heart_Rate);

  stateMachine(ANALYSIS_COMPLETE_EVENT);
}

void resultsScene()
{
  snprintf(display_buf[0], 60, "Your Heart Rate is: ");
  snprintf(display_buf[1], 60, "%d BPM", Heart_Rate);
  snprintf(display_buf[2], 60, "Your blood pressure is:");
  snprintf(display_buf[3], 60, "%d / %d", systolic_pressure, diastolic_pressure);

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[2], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[3], LEFT_MODE);
}

void errorScene()
{
  if(Beats_Error)
  {
    snprintf(display_buf[0], 60, "ERROR!!!!");
    snprintf(display_buf[1], 60, "%d BPM", Heart_Rate);
    snprintf(display_buf[2], 60, "Your blood pressure is:");
    snprintf(display_buf[3], 60, "%d / %d", systolic_pressure, diastolic_pressure);

    lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[2], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[3], LEFT_MODE);
  }else if (Array_Error)
  {
    snprintf(display_buf[0], 60, "ERROR array!");
    snprintf(display_buf[1], 60, "%d BPM", Heart_Rate);
    snprintf(display_buf[2], 60, "Your blood pressure is:");
    snprintf(display_buf[3], 60, "%d / %d", systolic_pressure, diastolic_pressure);

    lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[2], LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[3], LEFT_MODE);
  }
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
    switch (state)
    {
    case WAITING_STATE:
      if (stateChanged)
      {
        lcd.Clear(LCD_COLOR_BLACK);
        stateChanged = false;
      }
      waitingScene();
      break;
    case READING_STATE:
      if (stateChanged)
      {
        setUpPressureReadingScene();
        stateChanged = false;
      }
      pressureReadingScene();
      break;
    case ANALYSIS_STATE:
      if (stateChanged)
      {
        lcd.Clear(LCD_COLOR_BLACK);
        stateChanged = false;
      }
      dataAnalysis();
      break;
    case RESULTS_STATE:
      if (stateChanged)
      {
        lcd.Clear(LCD_COLOR_BLACK);
        stateChanged = false;
      }
      resultsScene();
      break;
    case ERROR_STATE:
      if (stateChanged)
      {
        lcd.Clear(LCD_COLOR_BLACK);
        stateChanged = false;
      }
      errorScene();
      break;
    default:
      break;
    }
    // stateChanged = false;
    thread_sleep_for(200);
  }
}
