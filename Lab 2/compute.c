/****************************************** 
* task name: compute
* task inputs: void* pointer to a ComputeData struct (see tasks.h)
* task outputs: void
* task description: Computes and corrects raw data to a displayable form.
* author: Ryan McDaniel
******************************************/ 

#include "tasks.h"
#include "schedule.h"

// We are only working with two decimal points
#define NUM_DEC 2

// Because there are only two decimal points, round up the third
#define ROUND 0.005

// Make a fixed point for two decimals by multiplying by 100
#define FPOINT 100

// Correction factors (Addative and Multiplicative)
#define TEMPCA 5
#define TEMPCM 0.75
#define SYSCA 9
#define SYSCM 2
#define DIACA 6
#define DIACM 1.5
#define PRCA 8
#define PRCM 3
#define BATTDIV 2.0
#define STR_SIZE 15

/*
1. Temperature in Celsius: tempCorrected = 5 + 0.75�tempRaw 
2. Systolic Pressure in mm Hg: sysCorrected = 9 + 2�systolicRaw 
3. Diastolic Pressure in mm Hg: diasCorrected = 6 + 1.5�diastolicRaw 
4. Pulse Rate in beats per minute: prCorrected = 8 + 3�bpRaw 
*/

void toString(int number, char* convertedNumber);
int findSize(int number);
int getDigits(int magnitude);

extern unsigned long globalTime;

// 2 Decimal places
void compute(void* taskDataPtr)
{   
  if(globalTime % MAJORCYCLECOUNT == 0)
  {
    int storageSpace = 0;
    char newData[STR_SIZE];

    ComputeData* computeDataPtr = (ComputeData*) taskDataPtr;
    int* temperatureRaw = (int*)(computeDataPtr->temperatureRawBuf->headPtr);
    int* prRaw = (int*)(computeDataPtr->pulseRateRawBuf->headPtr);
    unsigned short* battRaw = computeDataPtr->batteryState;

    // Messed up right now
    int* systolicRaw = (int*)(computeDataPtr->bloodPressRawBuf->headPtr);
    int* diastolicRaw = (int*)(computeDataPtr->bloodPressRawBuf->headPtr);

    // The most we want to deal with is two digits, so we round up the ten-thousandth's place
    // and multiply by 100 to move those decimals to the left side of the decimal point.
    // Casting to an integer will truncate everything after the decimal point, so we will have
    // an integer whose last two digits are actually decimal values.
    //
    // Problem: Numbers less than 0.1

    // Temperature correction
    storageSpace = (int)(FPOINT*(TEMPCA + TEMPCM*(*(temperatureRaw)) + ROUND));

    // Store the corrected data
    toString(storageSpace, newData);
    cBuffPut(computeDataPtr->tempCorrectedBuf, newData);

    // Systolic correction
    storageSpace = (int)(FPOINT*(SYSCA + SYSCM*(*(systolicRaw)) + ROUND));

    // Store the corrected data MESSED UP
    toString(storageSpace, newData);
    cBuffPut(computeDataPtr->bloodPressCorrectedBuf, newData);

    // Disastolic correction
    storageSpace = (int)(FPOINT*(DIACA + DIACM*(*(diastolicRaw)) + ROUND));

    // Store the corrected data MESSED UP
    toString(storageSpace, newData);
    cBuffPut(computeDataPtr->bloodPressCorrectedBuf, newData);

    // Pulse correction
    storageSpace = (int)(FPOINT*(PRCA + PRCM*(*(prRaw)) + ROUND));

    // Store the corrected data
    toString(storageSpace, newData);
    cBuffPut(computeDataPtr->prCorrectedBuf, newData);

    // Battery correction
    storageSpace = (int)(FPOINT * ( (*(battRaw)) / BATTDIV + ROUND));

    // Store the corrected data
    toString(storageSpace, newData);
    cBuffPut(computeDataPtr->battCorrected, newData);
  }
  return;
}


// Converts a two decimal fixed point number into a string with the decimal point
// in the correct space.
void toString(int number, char* convertedNumber)
{
  // Find out what magnitude the number is in terms of 10^x
  int magnitude = findSize(number);
  
  // Dividing magnitude by 10 will reveal the number of digits
  int totalDigits = getDigits(magnitude);
  
  // The decimal point will be in this index of the string
  int decimalSpot = totalDigits - NUM_DEC;
  
  // Stores each digit as they are separated from the orignal number
  int singleDigit;
  
  // Counts the number of digits we have gone through\
  // NOTE: MAY REFACTOR AS INDEX
  int digit = 0;
  
  // Used in loops to calculate 10^x. Reset after each calculation
  int base = 1;

  // Loop counting
  int i = 0;
  int j = 0;
  
  for (i = 0; i < totalDigits; i++)
  {
    // Placing the deciman in the right spot
    if (i == decimalSpot)
    {
      // Add the decimal
      convertedNumber[digit] = '.';
      
      // Increment the index so that it isn't written over next loop
      digit++;
      
      // Calculate 10^j
      for (j = 0; j < totalDigits - i; j++) base = 10*base;
      
      // Extact the first digit after the decimal point
      singleDigit = (number % (base)) / (base/10);
      convertedNumber[digit] = (char)(singleDigit + '0');
    } 
    
    // Treat the last number seperately to avoid dividing by zero
    else if (totalDigits - i == 1)
    {
      singleDigit = number % 10;
      convertedNumber[digit] = (char)(singleDigit + '0');
    }
    
    // Grab numbers one at a time
    else
    {
      // Calculate 10^j
      for (j = 0; j < totalDigits - i; j++) base = 10*base;
      
      // Grab one digit
      singleDigit = (number % (base)) / (base/10);
      convertedNumber[digit] = (char)(singleDigit + '0');
    }
    // Increment the index and reset base
    digit++;
    base = 1;
  }
  
  // Terminate the string
  convertedNumber[digit] = '\0';
  return;
}

// Returns the magnitude of a number as 10^x
// Supports numbers up to 999999, or 9999.99 as a fixed point
int findSize(int number)
{
  if (number >= 100000) return 100000;
  if (number >= 10000) return 10000;
  if (number >= 1000) return 1000;
  if (number >= 100) return 100;
  if (number >= 10) return 10;
  else return 1;
}
 


// Finds the number of digits by consecutively dividng by 10
// Precondition: Magnitude is a factor of 10
int getDigits(int magnitude)
{
  int numDigits = 0;
  while (magnitude > 1) 
  {
    magnitude = magnitude / 10;
    numDigits++;
  }
  return numDigits + 1;
}