#include <mbed.h>
#include "sensor.h"
#include "graphics.h"
#include "analysis.h"
#include "constants.h"

#define WAITING_STATE 0
#define READING_STATE 1
#define ANALYSIS_STATE 2
#define RESULTS_STATE 3
#define ERROR_STATE 4

#define BUTTON_PUSH_EVENT 1
#define PRESSURE_MIN_EVENT 2
#define ANALYSIS_COMPLETE_EVENT 3
#define ERROR_EVENT 4

#define PRESSURE_READY_FLAG 0b1

volatile uint8_t state;
volatile bool stateChanged;

bool Beats_Error = false;
bool Array_Error = false;
bool Slow_deflation = false;

InterruptIn buttonInterrupt(USER_BUTTON, PullDown);

Ticker read_pressure_ticker;
EventFlags flags;

void tickerCallback()
{
  flags.set(PRESSURE_READY_FLAG);
}

uint8_t read_buf[32];

float pressureY[PRESSURE_BUFFER_SIZE];
int Heart_Rate;
int systolic_pressure;
int diastolic_pressure;
uint16_t numReadings;

bool MaxPressure_Reached = false;

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
    case ERROR_EVENT:
      next_state = ERROR_STATE;
      break;
    default:
      break;
    }
    break;
  case ANALYSIS_STATE:
    if (event == ANALYSIS_COMPLETE_EVENT)
    {
      next_state = RESULTS_STATE;
    }
    else if (event == ERROR_EVENT)
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
  draw_graph_window();
  numReadings = 0;
  MaxPressure_Reached = false;
}

void pressureReadingScene()
{
  flags.wait_all(PRESSURE_READY_FLAG);

  float pressure = readPressure();

  display_current_pressure(pressure);

  pressureY[numReadings] = pressure;

  graph_pressure_value(pressure, numReadings);

  if (pressure >= MAX_PRESSURE)
  {
    MaxPressure_Reached = true;
  }
  else if (MaxPressure_Reached && pressure < MIN_PRESSURE)
  {
    stateMachine(PRESSURE_MIN_EVENT);
  }
  if (MaxPressure_Reached && pressureY[numReadings - 1] - pressure > MAX_DEFLATION_RATE)
  {
    display_slow_down_message();
  }
  else
  {
    clear_slow_down_message();
  }

  numReadings++;
  // printf("Num %d\n", numReadings);
  if (numReadings >= PRESSURE_BUFFER_SIZE)
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

void analysisScene()
{
  display_analyzing_data_message();

  int error = analyze_data(pressureY, numReadings, systolic_pressure, diastolic_pressure, Heart_Rate);

  if (error != 0)
  {
    Beats_Error = true;
    stateMachine(ERROR_EVENT);
  }
  else
  {
    stateMachine(ANALYSIS_COMPLETE_EVENT);
  }
}

void resultsScene()
{
  display_results(Heart_Rate, systolic_pressure, diastolic_pressure);
}

void errorScene()
{
  if (Beats_Error)
  {
    displayHeartBeatNotDetected();
  }
  else if (Array_Error)
  {
    displayTimeOutError();
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

  std::chrono::milliseconds ticker_period{SAMPLE_PERIOD};
  read_pressure_ticker.attach(tickerCallback, ticker_period);

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
      analysisScene();
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
        clear_screen();
        stateChanged = false;
      }
      errorScene();
      break;
    default:
      break;
    }
  }
}
