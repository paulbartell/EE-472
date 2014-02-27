/****************************************** 
* task name: measure 
* task inputs: a void* pointer to a MeasureData struct. 
* task outputs: measurement data in the MeasureData struct 
* task description: Simulates measurement of Temperature, Blood Pressure, 
and Pulse Rate. 
* author: Alyanna Castillo 
******************************************/
#include "FreeRTOS.h"
#include "task.h"

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
#define TEMPSEQ 2

extern unsigned long pulseRateSample;
extern unsigned short pulseRateFlag;
extern short ready;
extern xTaskHandle taskList[];

void measureTemp(MeasureData* measureDataPtr);
void measurePulseRate(MeasureData* measureDataPtr);
void measureBloodPressure(MeasureData* measureDataPtr);

void measureSetup()
{
  // Setup ADC Sequencer 2 for temperature measurement
  ADCSequenceConfigure(ADC0_BASE, TEMPSEQ, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, TEMPSEQ, 0,
                           ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
  ADCSequenceEnable(ADC0_BASE, TEMPSEQ);
  ADCIntClear(ADC0_BASE, TEMPSEQ);
}

void measure(void* taskDataPtr)
{ 
  measureSetup();
  portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();
  const portTickType xFrequency = 5000; // for 1Hz operation
  
  // Access the passed in MeasureData struct 
  MeasureData* measureDataPtr = (MeasureData*) taskDataPtr; 
  while( 1 )
  {
    switch(*(measureDataPtr->measurementSelection)) 
    {
    case 0:
      measureTemp(measureDataPtr);
      measureBloodPressure(measureDataPtr);
      measurePulseRate(measureDataPtr);
      vTaskResume(taskList[TASK_EKGCAPTURE]);
      break;
    case 1: 
      measureTemp(measureDataPtr);
      break;
    case 2: 
      measureBloodPressure(measureDataPtr);
      break;
    case 3: 
      measurePulseRate(measureDataPtr);
      break;
    case 4:
      vTaskResume(taskList[TASK_EKGCAPTURE]);
      break;
    }

    vTaskResume(taskList[TASK_COMPUTE]); // Resume compute task
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    //vTaskDelay(5000);
  }
} 

void measureTemp(MeasureData* measureDataPtr) {
  unsigned int temp;
  
  // Enable the first sample sequencer to capture the value of channel 0 when
  // the processor trigger occurs.

  // Trigger the sample sequence.
  ADCProcessorTrigger(ADC0_BASE, TEMPSEQ);
  
  // Wait until the sample sequence has completed.
  while(!ADCIntStatus(ADC0_BASE, TEMPSEQ, false))
  {
  }
  
  ADCIntClear(ADC0_BASE, TEMPSEQ);
  
  // Read the value from the ADC.
  ADCSequenceDataGet(ADC0_BASE, TEMPSEQ, &temp);
  
  //  temp = ((1475 * 1023) - (2250 * temp)) / 10230;
  // Change to human range (next lab)
  
  cBuffPut((measureDataPtr->temperatureRawBuf), &temp); 
}

void measurePulseRate(MeasureData* measureDataPtr) {
  // Measure Pulse Rate every 20 s
  if(pulseRateFlag) 
  { 
    float low = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 0.85; 
    float high = (*(measureDataPtr->pulseRateRawBuf->headPtr)) * 1.15; 
    
    // Adds value to buffer if it goes +15% of -15% of previous value 
    if((pulseRateSample < low) || (pulseRateSample > high))
    { 
      cBuffPut((measureDataPtr->pulseRateRawBuf), &pulseRateSample); 
    } 
    ready = 0;
    //IntMasterEnable();
    TimerEnable(TIMER2_BASE, TIMER_A);
  } 
}

void measureBloodPressure(MeasureData* measureDataPtr) {
  static int even = 1; 
  static int diaComplete = 0; 
  static int sysComplete = 0; 
  int syst = *(measureDataPtr->systolicPressRawBuf->headPtr); 
  int dias = *(measureDataPtr->diastolicPressRawBuf->headPtr); 
  
  if (syst > 100) 
  { 
    // Set complete to true 
    sysComplete = 1; 
    // Reset systolicPressRaw to initial value 
    if (1 == diaComplete) 
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
    if (1 == sysComplete) 
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
  even = !even; 
  cBuffPut((measureDataPtr->systolicPressRawBuf), &syst); 
  cBuffPut((measureDataPtr->diastolicPressRawBuf), &dias); 
}