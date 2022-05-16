/*
This file contains all the constants that are used along the program. All of this constants
contain important information that support the correct functionality of the program. 
*/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SAMPLE_PERIOD 100
#define PRESSURE_BUFFER_SIZE 1000

#define MAX_PRESSURE 150
#define MIN_PRESSURE 30

#define MAX_DEFLATION_RATE 4

#define NO_PULSE_ERROR 1
#define TIMEOUT_ERROR 2
#define BAD_DATA_ERROR 4
#define SENSOR_CONNECTION_ERROR 8

// Highest recorded blood pressure in history
#define SANITY_BP_UPPER_BOUND 370

// Highest/lowest recorded heart rate in history
#define SANITY_HR_UPPER_BOUND 480
#define SANITY_HR_LOWER_BOUND 25

#endif