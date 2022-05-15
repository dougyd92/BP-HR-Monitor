#include "sensor.h"
#include <mbed.h>

static const uint8_t exit_standby_cmd[3] = {0xAA, 0x00, 0x00};
static const uint8_t read_measurement_cmd[4] = {0xF0, 0x00, 0x00, 0x00};

void setupSensor()
{
  // Chip must be deselected
  chip_select = 1;

  // Setup the spi for 8 bit data, high steady state clock,
  // second edge capture, with a 5KHz clock rate
  spi.format(8, 0);
  spi.frequency(50000);
}

int readPressure(float &pressure)
{
  int32_t raw_pressure;

  uint8_t read_buffer[4];

  chip_select = 0;
  spi.write((const char *)exit_standby_cmd, 3, (char *)read_buffer, 3);
  chip_select = 1;

  thread_sleep_for(6);

  chip_select = 0;
  spi.write((const char *)read_measurement_cmd, 4, (char *)read_buffer, 4);
  chip_select = 1;

  // Check status value
  if (read_buffer[0] == 0xFF)
    return SENSOR_CONNECTION_ERROR;

  raw_pressure = ((((uint32_t)read_buffer[1]) << 16) | ((uint32_t)read_buffer[2]) << 8) | ((uint8_t)read_buffer[3]);
  pressure = (((((float)raw_pressure) - 419430.4) * (300)) / (3774873.6 - 419430.4));

  printf("Pressure: %4.5f \n", pressure);

  return 0;
}