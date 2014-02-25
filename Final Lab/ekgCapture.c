/****************************************** 
* task name: EKG Capture 
* task inputs: a void* pointer to a EKGData struct. 
* task outputs: EKG data in the EKGData struct 
* task description: Reads EKG signals simulated by
					function generator
* author: Ryan McDaniel 
******************************************/

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"

#include <stdio.h>
#include <string.h>
#define TEMP 3

void ekgCapture(void* taskData)
{
    unsigned long EKGRaw;
	
	// Access the passed in EKGData struct 
    EKGData* ekgDataPtr = (EKGData*) taskDataPtr; 
	
    // Set the clocking to run directly from the crystal.

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
                          ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
    ADCSequenceEnable(ADC0_BASE, 3);

    ADCIntClear(ADC0_BASE, 3);
    
    while (1) {
      // Trigger the sample sequence.
      ADCProcessorTrigger(ADC0_BASE, 3);
      
      // Wait until the sample sequence has completed.
      while(!ADCIntStatus(ADC0_BASE, 3, false))
      {
      }
      
      ADCIntClear(ADC0_BASE, 3);
      
      // Read the value from the ADC.
      ADCSequenceDataGet(ADC0_BASE, 3, &EKGRaw);
    }
	/*
	addFlags[TASK_EKGPROCESS] = 1;
	removeFlags[TASK_EKGCAPTURE] = 1;
	*/
}