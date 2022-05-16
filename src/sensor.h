/*
This file constains the pin declaration for the SPI connection of the sensor with the board. In addition
it gives the constructor for the functions that are later declared in the sensor.cpp file.
*/
#ifndef SENSOR_H
#define SENSOR_H

#include <mbed.h>
#include "constants.h"

static SPI spi(PE_6, PE_5, PE_2);
static DigitalOut chip_select(PE_4);

void setupSensor();
int readPressure(float &pressure);

#endif