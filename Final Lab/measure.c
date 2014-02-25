/****************************************** 
* task name: measure 
* task inputs: a void* pointer to a MeasureData struct. 
* task outputs: measurement data in the MeasureData struct 
* task description: Simulates measurement of Temperature, Blood Pressure, 
    and Pulse Rate. 
* author: Alyanna Castillo 
******************************************/
  
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"

#include "schedule.h"
#include "tasks.h"

#define INTSYS 80 
#define INTDIAS 80 
  
extern unsigned long globalTime; 
extern unsigned long pulseRateSample;
extern unsigned short pulseRateFlag;

void measureTemp(MeasureData* measureDataPtr)
void measurePulseRate(MeasureData* measureDataPtr)
void measureBloodPressure(MeasureData* measureDataPtr, int sysComplete, int diaComplete)

void measure(void* taskDataPtr)
{ 
    // Variables to be initialized once 
    // Change during every method call 
    int diaComplete = 0; 
    int sysComplete = 0; 
    int even = 1; 
  
    // Access the passed in MeasureData struct 
    MeasureData* measureDataPtr = (MeasureData*) taskDataPtr; 
  
	while( 1 )
    {
		switch() 
		{
		case 0:
			measureTemp(measureDataPtr);
			measureBloodPressure(measureDataPtr, sysComplete, diaComplete);
			measurePulse(measureDataPtr);
			break;
		case 1: 
			measureTemp(measureDataPtr);
			break;
		case 2: 
			measureBloodPressure(measureDataPtr, sysComplete, diaComplete);
			break;
		case 3: 
			measurePulse(measureDataPtr);
			break;
		}
		
		even = !even; 
		addFlags[TASK_COMPUTE] = 1;
		removeFlags[TASK_MEASURE] = 1;
		vTaskDelay(2000);
	}
} 

void measureTemp(MeasureData* measureDataPtr) {
	unsigned int temp;
	
	// Enable the first sample sequencer to capture the value of channel 0 when
	// the processor trigger occurs.
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
	ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
	ADCSequenceEnable(ADC0_BASE, 3);
	
	// Trigger the sample sequence.
	ADCProcessorTrigger(ADC0_BASE, 3);
	
	// Wait until the sample sequence has completed.
	while(!ADCIntStatus(ADC0_BASE, 3, false))
	{
	}
	
	// Read the value from the ADC.
	ADCSequenceDataGet(ADC0_BASE, 3, &temp);

	cBuffPut((measureDataPtr->temperatureRawBuf), &temp); 
}

void measurePulseRate(MeasureData* measureDataPtr) {
	// Measure Pulse Rate every minute (5 major cycles) 
    if(pulseRateFlag)
    { 
        float low = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 0.85; 
        float high = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 1.15; 
  
        // Adds value to buffer if it goes +15% of -15% of previous value 
        if((pulseRateSample < low) || (pulseRateSample > high))
        { 
          cBuffPut((measureDataPtr->pulseRateRawBuf), &pulseRateSample); 
        } 
    } 
}

void measureBloodPressure(MeasureData* measureDataPtr, int sysComplete, int diaComplete) {
    int syst = *(measureDataPtr->systolicPressRawBuf->headPtr); 
	int dias = *(measureDataPtr->diastolicPressRawBuf->headPtr); 
	
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
	cBuffPut((measureDataPtr->systolicPressRawBuf), &syst); 
    cBuffPut((measureDataPtr->diastolicPressRawBuf), &dias); 
}