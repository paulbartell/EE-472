#include "inc/hw_types.h"

#include "utils/uartstdio.h"

#include "tasks.h"

extern unsigned long globalTime;
void communication(void* taskDataPtr)
{
  CommunicationsData* communicationsDataPtr = (CommunicationsData*) taskDataPtr;
  
  UARTprintf("0x0c");// Clear the terminal
  UARTprintf("Temperature:         %s C\n", communicationsDataPtr->tempCorrectedBuf->headPtr);
  UARTprintf("Systolic pressure:   %s mmHg\n", communicationsDataPtr->systolicPressCorrectedBuf->headPtr); // FIXME
  UARTprintf("Diastolic pressure:  %s mmHg\n", communicationsDataPtr->diastolicPressCorrectedBuf->headPtr); // FIXME
  UARTprintf("Pulse rate:          %s BPM\n", communicationsDataPtr->prCorrectedBuf->headPtr); 
  UARTprintf("Battery:             %d %%\n", communicationsDataPtr->battCorrected->headPtr);
  
}