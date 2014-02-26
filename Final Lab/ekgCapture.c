/****************************************** 
* task name: EKG Capture 
* task inputs: a void* pointer to a EKGData struct. 
* task outputs: EKG data in the EKGData struct 
* task description: Reads EKG signals simulated by
function generator
* author: Ryan McDaniel 
******************************************/
#include "FreeRTOS.h"
#include "task.h"

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"

#include "tasks.h"

#include <stdio.h>
#include <string.h>
#define EKG 3

unsigned short index = 0;
extern xTaskHandle taskList[];


void ekgCapture(void* taskDataPtr)
{
  // Access the passed in EKGData struct 
  EKGData* ekgDataPtr = (EKGData*) taskDataPtr; 
  
  // Set the clocking to run directly from the crystal.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
  
  ADCSequenceConfigure(ADC0_BASE, EKG, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, EKG, 0,
                           ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, EKG);
  
  ADCIntClear(ADC0_BASE, EKG);
  portTickType xLastWakeTime;
  const portTickType xFrequency = 2000; // for 0.5Hz operation
  xLastWakeTime = xTaskGetTickCount();
  
  while (1) {
    // Trigger the sample sequence.
    ADCProcessorTrigger(ADC0_BASE, EKG);
    
    // Wait until the sample sequence has completed.
    while(!ADCIntStatus(ADC0_BASE, EKG, false))
    {
    }
    
    ADCIntClear(ADC0_BASE, EKG);
    
    if(256 == index) {
      index = 0;
    }
    
    // Read the value from the ADC.
    ADCSequenceDataGet(ADC0_BASE, EKG, &(ekgDataPtr->EKGRawBuf[index]));
    
    vTaskResume(taskList[TASK_EKGPROCESS]); // Resume EKGPROCESS task
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
  
  
}