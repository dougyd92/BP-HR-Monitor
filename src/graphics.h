#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

#define BACKGROUND 1
#define FOREGROUND 0
#define GRAPH_PADDING 5

LCD_DISCO_F429ZI lcd;

uint32_t graph_width = lcd.GetXSize() - 2 * GRAPH_PADDING;
uint32_t graph_height = graph_width;

char display_buf[4][60];

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

void clear_screen()
{
  lcd.Clear(LCD_COLOR_BLACK);
}

void display_current_pressure(float pressure)
{
  snprintf(display_buf[0], 60, "Pressure %4.5f mmHg", pressure);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
}

void graph_pressure_value(float pressure, int index)
{
  uint16_t mapY = GRAPH_PADDING + graph_height - pressure / 200 * graph_height;
  lcd.DrawPixel(index + GRAPH_PADDING, mapY, LCD_COLOR_BLUE);
}

void display_slow_down_message()
{
  snprintf(display_buf[1], 60, "Slow Down");
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
}

void clear_slow_down_message()
{
  lcd.ClearStringLine(LINE(16));
}

void display_instructions()
{
  snprintf(display_buf[0], 60, "TODO");
  snprintf(display_buf[1], 60, "Instructions go here");

  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_analyzing_data_message()
{
  snprintf(display_buf[0], 60, "Analyzing data...");
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
}

void display_results(int hr, int systolic, int diastolic)
{
  snprintf(display_buf[0], 60, "Your Heart Rate is: ");
  snprintf(display_buf[1], 60, "%d BPM", hr);
  snprintf(display_buf[2], 60, "Your blood pressure is:");
  snprintf(display_buf[3], 60, "%d / %d", systolic, diastolic);

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)display_buf[2], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[3], LEFT_MODE);
}

void displayHeartBeatNotDetected()
{
  snprintf(display_buf[0], 60, "No heartbeat detected.");
  snprintf(display_buf[1], 60, "Are you a zombie?");
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[1], LEFT_MODE);
}

void displayTimeOutError()
{
  snprintf(display_buf[0], 60, "Timed out");
  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)display_buf[0], LEFT_MODE);
}
#endif