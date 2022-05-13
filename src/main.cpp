#include <mbed.h>
#include "sensor.h"
#include "graphics.h"



#define WAITING_STATE 0
#define READING_STATE 1
#define ANALYSIS_STATE 2
#define RESULTS_STATE 3
#define ERROR_STATE 4


#define BUTTON_PUSH_EVENT 1
#define PRESSURE_MIN_EVENT 2
#define ANALYSIS_COMPLETE_EVENT 3
#define ERROR_EVENT 4

volatile uint8_t state;
volatile bool stateChanged;

InterruptIn buttonInterrupt(USER_BUTTON, PullDown);

uint8_t read_buf[32];

float pressureY[1000];
int Heart_Rate;
int systolic_pressure;
int diastolic_pressure;
uint16_t numReadings;

bool MaxPressure_Reached = false;

bool Beats_Error = false;
bool Array_Error = false;
bool Slow_deflation = false;



void stateMachine(uint8_t event)
{
  uint8_t next_state = state;

  switch (state)
  {
  case WAITING_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      next_state = READING_STATE;
    }
    break;
  case READING_STATE:
    switch (event)
    {
    case BUTTON_PUSH_EVENT:
      next_state = WAITING_STATE;
      break;
    case PRESSURE_MIN_EVENT:
      next_state = ANALYSIS_STATE;
      break;
    default:
      break;
    }
    break;
  case ANALYSIS_STATE:
    if (event == ANALYSIS_COMPLETE_EVENT)
    {
      next_state = RESULTS_STATE;
    }else if (event == ERROR_EVENT)
    {
      next_state = ERROR_STATE;
    }
    break;
  case RESULTS_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      next_state = WAITING_STATE;
    }
    break;
  case ERROR_STATE:
    if (event == BUTTON_PUSH_EVENT)
    {
      next_state = WAITING_STATE;
    }
    break;
  }

  if (next_state != state)
  {
    stateChanged = true;
    state = next_state;
  }
}

// Button Interrupt
void buttonEvent()
{
  stateMachine(BUTTON_PUSH_EVENT);
}

void setUpPressureReadingScene()
{
  clear_screen();
  draw_graph_window(10);
  numReadings = 0;
  MaxPressure_Reached = false;
}

void pressureReadingScene()
{
  float pressure = readPressure();

  display_current_pressure(pressure);

  pressureY[numReadings] = pressure;

  graph_pressure_value(pressure, numReadings);

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
    display_slow_down_message();
  }
  else
  {
    clear_slow_down_message();
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
  display_instructions();
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

  display_analyzing_data_message();

  int startingIndex;
  int localMax[200];
  int numBeats = 0;
  int total_Time_Beats = 0;
  for (int i = 0; i < numReadings; i++)
  {
    if (pressureY[i] > 150 && pressureY[i] > pressureY[i + 1])
    {
      startingIndex = i + 1;
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
    printf("Min index between %d %d = %d \n", localMax[i], localMax[i + 1], min_index);
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
  display_results(Heart_Rate, systolic_pressure, diastolic_pressure);
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

  setupSensor();

  buttonInterrupt.rise(&buttonEvent);

  while (1)
  {
    switch (state)
    {
    case WAITING_STATE:
      if (stateChanged)
      {
        clear_screen();
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
        clear_screen();
        stateChanged = false;
      }
      dataAnalysis();
      break;
    case RESULTS_STATE:
      if (stateChanged)
      {
        clear_screen();
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
