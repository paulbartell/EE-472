/****************************************** 
* task name: communication
* task inputs: void* pointer to a CommunicationData struct (see tasks.h)
* task outputs: void
* task description: Outputs data to serial console when called.
* author: Paul Bartell
******************************************/ 
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "FreeRTOS.h"
#include "task.h"
#include "utils/uartstdio.h"
#include "tasks.h"

extern volatile unsigned long globalTime;

void communication(void* taskDataPtr)
{
  // Setup UART
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);
  
  while(1)
  {
    
    CommunicationsData* communicationsDataPtr = (CommunicationsData*) taskDataPtr;
    UARTprintf("%c",'\x0C');// Clear the terminal
    UARTprintf("Temperature:         %s C\n", communicationsDataPtr->tempCorrectedBuf->headPtr);
    UARTprintf("Systolic pressure:   %s mmHg\n", communicationsDataPtr->systolicPressCorrectedBuf->headPtr); 
    UARTprintf("Diastolic pressure:  %s mmHg\n", communicationsDataPtr->diastolicPressCorrectedBuf->headPtr);
    UARTprintf("Pulse rate:          %s BPM\n", communicationsDataPtr->prCorrectedBuf->headPtr); 
    UARTprintf("EKG:                 %s Hz\n", communicationsDataPtr->ekgCorrectedBuf->headPtr);
    UARTprintf("Battery:             %s %%\n", communicationsDataPtr->battCorrected->headPtr);
    // UARTprintf("Time: %d\n",globalTime); // For timing debugging
    vTaskSuspend(NULL); // Wait until resumed by warningAlarm or Display
  }
}
