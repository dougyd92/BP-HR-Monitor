/*
This file is created to analyze the pressure data that it is read by the sensor. Inside this
file the are two functions, findMinIndex and analyze_data. The first function is used to find
the index of the local minimum inside the array of measured data. The second function is used
to determine the systolic and diastolic pressure by finding the local maximum of the data in
the graph.
*/

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "constants.h"

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

int analyze_data(float *pressureY, int numReadings, int &systolic_pressure, int &diastolic_pressure, int &Heart_Rate)
{
  int startingIndex;
  int localMax[200];
  int numBeats = 0;
  int total_Time_Beats = 0;
  for (int i = 0; i < numReadings; i++)
  {
    if (pressureY[i] > MAX_PRESSURE && pressureY[i] > pressureY[i + 1])
    {
      startingIndex = i + 1;
      break;
    }
  }
  for (int i = startingIndex; i < numReadings - 1; i++)
  {
    if (pressureY[i] > pressureY[i - 1] && pressureY[i] > pressureY[i + 1])
    {
      localMax[numBeats] = i;
      numBeats++;
    }
  }
  if (numBeats == 0 || numBeats < 3)
  {
    return NO_PULSE_ERROR;
  }

  float max_amplitude = 0;
  int max_amp_index = startingIndex;

  for (int i = 0; i < numBeats - 1; i++)
  {
    total_Time_Beats += localMax[i + 1] - localMax[i];

    int min_index = findMinIndex(localMax[i], localMax[i + 1], pressureY);
    float amplitude = pressureY[localMax[i]] - pressureY[min_index];

    if (amplitude > max_amplitude)
    {
      max_amplitude = amplitude;
      max_amp_index = localMax[i];
    }
  }

  // Blood pressure
  systolic_pressure = pressureY[localMax[0]];

  int diastolic_index = 2 * max_amp_index - localMax[0];
  diastolic_pressure = pressureY[diastolic_index];

  // Heart rate
  Heart_Rate = float(total_Time_Beats) / (numBeats - 1) * (SAMPLE_PERIOD / 1000.0) * 60;

  // Sanity check
  if (diastolic_pressure >= systolic_pressure ||
      systolic_pressure > SANITY_BP_UPPER_BOUND ||
      diastolic_pressure < MIN_PRESSURE)
    return BAD_DATA_ERROR;

  if (Heart_Rate > SANITY_HR_UPPER_BOUND ||
      Heart_Rate < SANITY_HR_LOWER_BOUND)
    return BAD_DATA_ERROR;

  return 0;
}

#endif