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
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"

#include "inc/hw_memmap.h"

#include "tasks.h"

#include <stdio.h>
#include <string.h>
#define EKGSEQ 3

volatile unsigned long index = 0;
extern xTaskHandle taskList[];
extern signed long EKGRaw[];


void ekgCapture(void* taskDataPtr)
{
  // Access the passed in EKGData struct 
  EKGData* ekgDataPtr = (EKGData*) taskDataPtr; 
  
  // Set the clocking to run directly from the crystal.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);
  ADCSequenceDisable(ADC0_BASE, EKGSEQ);
  IntPrioritySet(INT_ADC0SS3, 0x00);
  ADCSequenceConfigure(ADC0_BASE, EKGSEQ, ADC_TRIGGER_PROCESSOR, 0x00);
  ADCSequenceStepConfigure(ADC0_BASE, EKGSEQ, 0,
                           ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCIntEnable(ADC0_BASE, EKGSEQ);
  TimerControlTrigger(TIMER2_BASE, TIMER_B, true);
  ADCIntClear(ADC0_BASE, EKGSEQ);
  ADCIntEnable(ADC0_BASE, EKGSEQ);
  IntEnable(INT_ADC0SS3);
  
  
  ADCIntClear(ADC0_BASE, EKGSEQ);
  portTickType xLastWakeTime;
  const portTickType xFrequency = 5000; // for 1/5 hz operation
  xLastWakeTime = xTaskGetTickCount();
  
  while (1) {
    // Trigger the sample sequence.
    ADCSequenceEnable(ADC0_BASE, EKGSEQ);
//    TimerEnable(TIMER2_BASE, TIMER_B);
//    
//    // Wait until the sample sequence has completed.
//    while(!(index == 256))
//    {
//    }
//    UARTprintf("done\n");
//    for(int i = 0; i < 256; i++)
//    {
//      UARTprintf("%d,",EKGRawBuf[i]);
//    }
//    UARTprintf("\n");
//    index = 0;
//    TimerDisable(TIMER2_BASE, TIMER_B);
    taskENTER_CRITICAL();
    for(int i = 0; i < 256; i++)
    {
      ADCProcessorTrigger(ADC0_BASE, EKGSEQ);
      SysCtlDelay(SysCtlClockGet()/8000/3);
    }
    taskEXIT_CRITICAL();
//        for(int i = 0; i < 256; i++)
//    {
//      UARTprintf("%d,",EKGRaw[i]);
//    }
    // Read the value from the ADC.
    //ADCSequenceDataGet(ADC0_BASE, EKGSEQ, &(ekgDataPtr->EKGRawBuf[index]));
    index = 0;
    vTaskResume(taskList[TASK_EKGPROCESS]); // Resume EKGPROCESS task
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    
  }
  
  
}

void ekgInterrupt()
{
  ADCIntClear(ADC0_BASE, EKGSEQ);
  ADCSequenceDataGet(ADC0_BASE, EKGSEQ, &EKGRaw[index]);
  index++;
  
  if(index == 256)
  {
    ADCSequenceDisable(ADC0_BASE, EKGSEQ);
  }
  
}