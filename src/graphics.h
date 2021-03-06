/*
This file contains all the information and functions that are used to create the graphs and 
display information and data on the LCD depending on the which state of the exeuction process
is the user. In addition, all the data and constants for setting up the LCD are described here
as well as the all the data and information that it is displayed on the screen when a new scene
is being executed.
*/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"
#include "constants.h"

#define BACKGROUND 1
#define FOREGROUND 0
#define GRAPH_PADDING 5
#define GRAPH_TICK_SPACING 10

LCD_DISCO_F429ZI lcd;

uint32_t graph_width = lcd.GetXSize() - 2 * GRAPH_PADDING;
uint32_t graph_height = graph_width;

void clear_line(uint32_t line)
{
  char clear[] = "                                                            ";
  lcd.DisplayStringAt(0, line, (uint8_t *)clear, LEFT_MODE);
}

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
void draw_graph_window()
{
  lcd.SelectLayer(BACKGROUND);

  lcd.DrawRect(GRAPH_PADDING, GRAPH_PADDING, graph_width, graph_width);
  // draw the x-axis tick marks
  for (uint32_t i = 0; i < graph_width; i += GRAPH_TICK_SPACING)
  {
    lcd.DrawVLine(GRAPH_PADDING + i, graph_height, GRAPH_PADDING);
  }
}

void clear_screen()
{
  lcd.Clear(LCD_COLOR_BLACK);
}

void display_current_pressure(float pressure)
{
  char display_buf[1][60];
  lcd.SetFont(&Font16);
  snprintf(display_buf[0], 60, "Pressure %4.5f mm Hg", pressure);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
}

void graph_pressure_value(float pressure, int index)
{
  uint16_t mapY = GRAPH_PADDING + graph_height - pressure / 200 * graph_height;
  lcd.DrawPixel(index + GRAPH_PADDING, mapY, LCD_COLOR_BLUE);
}

void display_slow_down_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Slow Down");
  lcd.SetFont(&Font16);
  clear_line(LINE(16));
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_start_deflating_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Start deflating now");
  lcd.SetFont(&Font16);
  clear_line(LINE(16));
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_start_inflating_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Start inflating now");
  lcd.SetFont(&Font16);
  clear_line(LINE(16));
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_keep_deflating_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Keep deflating");
  lcd.SetFont(&Font16);
  clear_line(LINE(16));
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_keep_inflating_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Keep inflating");
  lcd.SetFont(&Font16);
  clear_line(LINE(16));
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_instructions()
{
  char display_buf[12][60];
  snprintf(display_buf[0], 60, "Douglas de Jesus and Mateo Bonilla");

  snprintf(display_buf[1], 60, "Instructions:");
  snprintf(display_buf[2], 60, "Place the cuff on");
  snprintf(display_buf[3], 60, "your right arm.");
  snprintf(display_buf[4], 60, "Press the blue button");
  snprintf(display_buf[5], 60, "to begin.");
  snprintf(display_buf[6], 60, "Inflate to at least");
  snprintf(display_buf[7], 60, "%dmmHg.", MAX_PRESSURE);
  snprintf(display_buf[8], 60, "Deflate slowly until");
  snprintf(display_buf[9], 60, "%dmm Hg.", MIN_PRESSURE);

  snprintf(display_buf[10], 60, "Press the");
  snprintf(display_buf[11], 60, "blue button");

  lcd.SetFont(&Font12);
  lcd.DisplayStringAt(0, LINE(0), (uint8_t *)display_buf[0], LEFT_MODE);

  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[2], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[3], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)display_buf[4], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(6), (uint8_t *)display_buf[5], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(7), (uint8_t *)display_buf[6], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(8), (uint8_t *)display_buf[7], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(9), (uint8_t *)display_buf[8], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)display_buf[9], LEFT_MODE);

  lcd.SetFont(&Font24);
  lcd.DisplayStringAt(0, LINE(9), (uint8_t *)display_buf[10], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)display_buf[11], LEFT_MODE);
}

void display_analyzing_data_message()
{
  char display_buf[1][60];
  snprintf(display_buf[0], 60, "Analyzing data...");
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_results(int hr, int systolic, int diastolic)
{
  char display_buf[4][60];
  snprintf(display_buf[0], 60, "Heart Rate: ");
  snprintf(display_buf[1], 60, "%d BPM", hr);
  snprintf(display_buf[2], 60, "Blood pressure:");
  snprintf(display_buf[3], 60, "%d/%d mm Hg", systolic, diastolic);

  lcd.SetFont(&Font20);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.SetFont(&Font24);
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.SetFont(&Font20);
  lcd.DisplayStringAt(0, LINE(8), (uint8_t *)display_buf[2], LEFT_MODE);
  lcd.SetFont(&Font24);
  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)display_buf[3], LEFT_MODE);
}

void displayHeartBeatNotDetected()
{
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"No pulse detected.", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"Are you a zombie?", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Make sure the cuff is", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"tight and positioned", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"correctly.", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"Make sure to deflate", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"slowly.", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Press the blue button", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(11), (uint8_t *)"to try again.", LEFT_MODE);
}

void displayTimeOutError()
{
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Took too long to", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"reach the target", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)"pressure.", LEFT_MODE);

  char display_buf[2][60];
  snprintf(display_buf[0], 60, "%dmm Hg.", MAX_PRESSURE);
  snprintf(display_buf[1], 60, "to %dmm Hg.", MIN_PRESSURE);
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"First, inflate to", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(6), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"Then, deflate slowly", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"(but not too slowly)", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(9), (uint8_t *)display_buf[1], LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(11), (uint8_t *)"Press the blue button", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(12), (uint8_t *)"to try again.", LEFT_MODE);
}

void displayBadDataError()
{
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Unable to get an", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"accurate reading.", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Make sure the cuff is", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"tight and positioned", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"correctly.", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"Make sure to deflate", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"slowly.", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Press the blue button", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(11), (uint8_t *)"to try again.", LEFT_MODE);
}

void displaySensorConnectionError()
{
  lcd.SetFont(&Font16);
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Could not communicate", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"with the sensor.", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Check the connection", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"and try again.", LEFT_MODE);

  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"Press the blue button", LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(11), (uint8_t *)"to try again.", LEFT_MODE);
}

#endif
