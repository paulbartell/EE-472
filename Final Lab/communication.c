/****************************************** 
* task name: communication
* task inputs: void* pointer to a CommunicationData struct (see tasks.h)
* task outputs: void
* task description: Outputs data to serial console when called.
* author: Paul Bartell
******************************************/ 
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "tasks.h"
#include "schedule.h"

extern volatile unsigned long globalTime;

void communication(void* taskDataPtr)
{
  while(1)
  {
    CommunicationsData* communicationsDataPtr = (CommunicationsData*) taskDataPtr;
    UARTprintf("%c",'\x0C');// Clear the terminal
    UARTprintf("A Warning has occured\n");
    UARTprintf("Temperature:         %s C\n", communicationsDataPtr->tempCorrectedBuf->headPtr);
    UARTprintf("Systolic pressure:   %s mmHg\n", communicationsDataPtr->systolicPressCorrectedBuf->headPtr); 
    UARTprintf("Diastolic pressure:  %s mmHg\n", communicationsDataPtr->diastolicPressCorrectedBuf->headPtr);
    UARTprintf("Pulse rate:          %s BPM\n", communicationsDataPtr->prCorrectedBuf->headPtr); 
    UARTprintf("Battery:             %s %%\n", communicationsDataPtr->battCorrected->headPtr);
    // UARTprintf("Time: %d\n",globalTime); // For timing debugging
    removeFlags[TASK_COMMUNICATION] = 1;
    vTaskDelay(1000);
  }
  
}