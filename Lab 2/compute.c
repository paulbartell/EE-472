#include <stdio.h>
#include "tasks.h"
#define NUM_DEC 2
#define ROUND 0.005
#define FPOINT 100

/*
1. Temperature in Celsius: tempCorrected = 5 + 0.75•tempRaw 
2. Systolic Pressure in mm Hg: sysCorrected = 9 + 2•systolicRaw 
3. Diastolic Pressure in mm Hg: diasCorrected = 6 + 1.5•diastolicRaw 
4. Pulse Rate in beats per minute: prCorrected = 8 + 3•bpRaw 
*/

unsigned char* toString(int number, char* convertedNumber);
int findSize(int number);

// 2 Decimal places
void compute(void* taskDataPtr)
{   
  int storageSpace = 0;
  ComputeData* computeDataPtr = (ComputeData*) taskDataPtr;
  
  // Temperature correction
  storageSpace = (int)(FPOINT*(5 + 0.75*(*(computeDataPtr->temperatureRaw)) + ROUND));
  toString(storageSpace, computeDataPtr->tempCorrected);
  
  // Systolic correction
  storageSpace = (int)(FPOINT*(9 + 2*(*(computeDataPtr->systolicPressRaw)) + ROUND));
  toString(storageSpace, computeDataPtr->sysCorrected);
  
  // Disastolic correction
  storageSpace = (int)(FPOINT*(6 + 1.5*(*(computeDataPtr->diastolicPressRaw)) + ROUND));
  toString(storageSpace, computeDataPtr->diasCorrected);
  
  // Pulse correction
  storageSpace = (int)(FPOINT*(8 + 3*(*(computeDataPtr->pulseRateRaw)) + ROUND));
  toString(storageSpace, computeDataPtr->prCorrected);
  
  // Battery correction
  storageSpace = (int)(FPOINT*(*(computeDataPtr->batteryState)/2.0) + ROUND));
  toString(storageSpace, computeDataPtr->battCorrected);
}

void toString(int number, char* convertedNumber)
{
  int magnitude = findSize(number);
  int totalDigits = magnitude / 10;
  int decimalSpot = i - NUM_DEC;
  int i = 0;
  int j = 0;
  int singleDigit;
  int digit = 0;
  int base = 10;
  
  for (i = 0; i < totalDigits; i++)
  {
    if (totalDigits - i == decimalSpot)
    {
      convertedNumber[digit] = '.';
      digit++;
      for (j = 0; j < totalDigits - i; j++) base = 10*base;
      singleDigit = (number % (base)) / (base/10);
      convertedNumber[digit] = (char)(singleDigit + '0');
    } 
    else if (totalDigits - i == 1)
    {
      singleDigit = number % 10;
      convertedNumber[digit] = (char)(singleDigit + '0');
    }
    else
    {
      for (j = 0; j < totalDigits - i; j++) base = 10*base;
      singleDigit = (number % (base)) / (base/10);
      convertedNumber[digit] = (char)(singleDigit + '0');
    }
    digit++;
    base = 10;
  }
  convertedNumber[digit] = '\0';
}

int findSize(int number)
{
  if (number >= 100000) return 100000;
  if (number >= 10000) return 10000;
  if (number >= 1000) return 1000;
  if (number >= 100) return 100;
}
*/