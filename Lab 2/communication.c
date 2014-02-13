#include "inc/hw_types.h"

#include "utils/uartstdio.h"

#include "tasks.h"
#include "schedule.h"

extern volatile unsigned long globalTime;


void communication(void* taskDataPtr)
{
  CommunicationsData* communicationsDataPtr = (CommunicationsData*) taskDataPtr;
  
  UARTprintf("%c",'\x0C');// Clear the terminal
  UARTprintf("Temperature:         %s C\n", communicationsDataPtr->tempCorrectedBuf->headPtr);
  UARTprintf("Systolic pressure:   %s mmHg\n", communicationsDataPtr->systolicPressCorrectedBuf->headPtr); // FIXME
  UARTprintf("Diastolic pressure:  %s mmHg\n", communicationsDataPtr->diastolicPressCorrectedBuf->headPtr); // FIXME
  UARTprintf("Pulse rate:          %s BPM\n", communicationsDataPtr->prCorrectedBuf->headPtr); 
  UARTprintf("Battery:             %s %%\n", communicationsDataPtr->battCorrected->headPtr);
  UARTprintf("Time: %d\n",globalTime);
  removeFlags[TASK_COMMUNICATION] = 1;
}