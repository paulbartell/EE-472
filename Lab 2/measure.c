#include "tasks.h"
#include "schedule.h"
#define INTSYS 80
#define INTDIAS 80

extern unsigned long globaltime;

void measure(void* taskDataPtr)
{
  if(globaltime % MAJORCYCLECOUNT == 0)
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
    
    MeasureData* measureDataPtr = (MeasureData*) taskDataPtr;
    
    // Measure Temperature
    temp = *(measureDataPtr->temperatureRaw);
    if (revTemp == 0) {
      if (temp > 50) {
        revTemp = 1;
      }
      if (even) {
        temp += 2;
      } else {
        temp--;
      }
    } else {
      if (temp < 15) {
        revTemp = 0;
      }
      if (even) {
        temp -= 2;
      } else {
        temp++;
      }
    }
    
    
    // Measure Systolic
    syst = *(measureDataPtr->systolicPressRaw);
    if (syst > 100){
      // Set complete to true
      sysComplete = 1;  
      // Reset systolicPressRaw to initial value
      if (diaComplete == 1){
        diaComplete = 0;
        sysComplete = 0;
        dias = INTDIAS;
        syst = INTSYS;
      }
    } else {
      if (even) {
        syst += 3;
      } else {
        syst--;
      }
      sysComplete = 0;
    }
    
    
    // Measure Diastolic
    dias = *(measureDataPtr->diastolicPressRaw);
    if (dias < 40){
      diaComplete = 1;
      // Set diaComplete to 0
      // Reset diastolicPressRaw to initial value
      if (sysComplete == 1) {
        diaComplete = 0;
        sysComplete = 0;
        dias = INTDIAS;
        syst = INTSYS;
      }
    } else {
      if (even){
        dias -= 2;
      } else {
        dias++;
      }
      diaComplete = 0;
    }
    
    
    // Measure Pulse Rate
    pulse = *(measureDataPtr->pulseRateRaw);
    if (revPulse == 0) {
      if (pulse > 40) {
        revPulse = 1;
      }  
      if (even) {
          pulse--;
      } else {
          pulse += 3;
      }
    } else {
      if(pulse < 15){
        revPulse = 0;
      } 
      if (even) {
        pulse++;
      } else {
        pulse -= 3;
      }
    }
    *(measureDataPtr->temperatureRaw)=temp;
    *(measureDataPtr->systolicPressRaw)=syst;
    *(measureDataPtr->diastolicPressRaw)=dias;
    *(measureDataPtr->pulseRateRaw) = pulse;
    
    even = !even;
  }
}
