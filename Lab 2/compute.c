#include "tasks.h"

// We are only working with two decimal points
#define NUM_DEC 2

// Because there are only two decimal points, round up the third
#define ROUND 0.005

// Make a fixed point for two decimals by multiplying by 100
#define FPOINT 100

/*
1. Temperature in Celsius: tempCorrected = 5 + 0.75�tempRaw 
2. Systolic Pressure in mm Hg: sysCorrected = 9 + 2�systolicRaw 
3. Diastolic Pressure in mm Hg: diasCorrected = 6 + 1.5�diastolicRaw 
4. Pulse Rate in beats per minute: prCorrected = 8 + 3�bpRaw 
*/

void toString(int number, char* convertedNumber);
int findSize(int number);
int getDigits(int magnitude);

// 2 Decimal places
void compute(void* taskDataPtr)
{   
  int storageSpace = 0;
  ComputeData* computeDataPtr = (ComputeData*) taskDataPtr;
  
  // The most we want to deal with is two digits, so we round up the ten-thousandth's place
  // and multiply by 100 to move those decimals to the left side of the decimal point.
  // Casting to an integer will truncate everything after the decimal point, so we will have
  // an integer whose last two digits are actually decimal values.
  //
  // Problem: Numbers less than 0.1
  
  // Temperature correction
  storageSpace = (int)(FPOINT*(5 + 0.75*(*(computeDataPtr->temperatureRaw)) + ROUND));
  
  // Store the corrected data
  toString(storageSpace, computeDataPtr->tempCorrected);
  
  // Systolic correction
  storageSpace = (int)(FPOINT*(9 + 2*(*(computeDataPtr->systolicPressRaw)) + ROUND));
  
  // Store the corrected data
  toString(storageSpace, computeDataPtr->sysCorrected);
  
  // Disastolic correction
  storageSpace = (int)(FPOINT*(6 + 1.5*(*(computeDataPtr->diastolicPressRaw)) + ROUND));
  
  // Store the corrected data
  toString(storageSpace, computeDataPtr->diasCorrected);
  
  // Pulse correction
  storageSpace = (int)(FPOINT*(8 + 3*(*(computeDataPtr->pulseRateRaw)) + ROUND));
  
  // Store the corrected data
  toString(storageSpace, computeDataPtr->prCorrected);
  
  // Battery correction
  storageSpace = (int)(FPOINT*(*(computeDataPtr->batteryState)/2.0) + ROUND));
  
  // Store the corrected data
  toString(storageSpace, computeDataPtr->battCorrected);
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
  while (magnitude > 1) {
    magnitude = magnitude / 10;
    numDigits++;
  }
  return numDigits + 1;
}