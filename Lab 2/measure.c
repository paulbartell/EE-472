/******************************************
* task name: measure
* task inputs: a void* pointer to a MeasureData struct.
* task outputs: measurement data in the MeasureData struct
* task description: Simulates measurement of Temperature, Blood Pressure,
    and Pulse Rate.
* author: Alyanna Castillo
******************************************/

#include "tasks.h"
#include "schedule.h"
#define INTSYS 80
#define INTDIAS 80

extern unsigned long globalTime;
extern unsigned long pulseRateCount;

void measure(void* taskDataPtr)
{
  if(globalTime % MAJORCYCLECOUNT == 0)
  {
    // Variables to be measured
    int temp;
    int syst;
    int dias;
    int pulse;

    // Variables to be initialized once
    // Change during every method call
    static int diaComplete = 0;
    static int sysComplete = 0;

    static int revTemp = 0;
    static int revPulse = 0;
    static int even = 1;

    // Access the passed in MeasureData struct
    MeasureData* measureDataPtr = (MeasureData*) taskDataPtr;

    // Measure Temperature
    temp = *(measureDataPtr->temperatureRawBuf->headPtr);
    if (revTemp == 0)
    {
      if (temp > 50)
      {
        revTemp = 1;
      }
      if (even)
      {
        temp += 2;
      }
      else
      {
        temp--;
      }
    }
    else
    {
      if (temp < 15)
      {
        revTemp = 0;
      }
      if (even)
      {
        temp -= 2;
      }
      else
      {
        temp++;
      }
    }


    // Measure Systolic
    syst = *(measureDataPtr->systolicPressRawBuf->headPtr);
    if (syst > 100)
    {
      // Set complete to true
      sysComplete = 1;
      // Reset systolicPressRaw to initial value
      if (diaComplete == 1)
      {
        diaComplete = 0;
        sysComplete = 0;
        dias = INTDIAS;
        syst = INTSYS;
      }
    }
    else
    {
      if (even)
      {
        syst += 3;
      } else {
        syst--;
      }
      sysComplete = 0;
    }


    // Measure Diastolic
    dias = *(measureDataPtr->diastolicPressRawBuf->headPtr);
    if (dias < 40)
    {
      diaComplete = 1;
      // Set diaComplete to 0
      // Reset diastolicPressRaw to initial value
      if (sysComplete == 1)
      {
        diaComplete = 0;
        sysComplete = 0;
        dias = INTDIAS;
        syst = INTSYS;
      }
    }
    else
    {
      if (even)
      {
        dias -= 2;
      }
      else
      {
        dias++;
      }
      diaComplete = 0;
    }


    // Measure Pulse Rate
    float low = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 0.85;
    float high = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 1.15;

    // Adds value to buffer if it goes +15% of -15% of previous value
    if((pulseRateCount < low) || (pulseRateCount > high)){
      cBuffPut((measureDataPtr->pulseRateRawBuf), &pulseRateCount);
    }
    // Reset to zero for next count
    pulseRateCount = 0;

    cBuffPut((measureDataPtr->temperatureRawBuf), &temp);
    cBuffPut((measureDataPtr->systolicPressRawBuf), &syst);
    cBuffPut((measureDataPtr->diastolicPressRawBuf), &dias);

    even = !even;
  }
}
