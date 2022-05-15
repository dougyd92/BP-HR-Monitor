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
    return 1; // Beats_Error
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
  Heart_Rate = float(total_Time_Beats) / (numBeats - 1) * (SAMPLE_PERIOD / 1000.0) * 60;
  printf("Heart Rate: %d \n", Heart_Rate);

  return 0;
}

#endif