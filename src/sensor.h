#ifndef SENSOR_H
#define SENSOR_H

#include <mbed.h>

static SPI spi(PE_6, PE_5, PE_2);
static DigitalOut chip_select(PE_4);

void setupSensor();
float readPressure();

#endif