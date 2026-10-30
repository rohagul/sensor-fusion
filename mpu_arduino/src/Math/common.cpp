#include "common.h"
#include <math.h>

int16_t getAverage(const int16_t array[][3], int arraySize,int index) {
  if (arraySize == 0) return 0;

  long sum = 0;
  for (int i = 0; i < arraySize; i++) {
    sum += array[i][index];
  }

  return (int16_t)lroundf((float)sum / arraySize);
}

