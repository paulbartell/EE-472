/****************************************** 
* task name: status
* task inputs: a void* pointer to a StatusData struct
* task outputs: Battery status
* task description: Updates the battery status every major cycle.
* author: Alyanna Castillo
******************************************/ 
#include "FreeRTOS.h"
#include "task.h"

#include "tasks.h"
#define INTBAT 200

void status(void* taskDataPtr)
{
  // Pointer re-cast as pointer to the Status task's data structure
  StatusData* statusDataPtr = (StatusData*) taskDataPtr;
  portTickType xLastWakeTime;
  const portTickType xFrequency = 5000; // for 1/5Hz operation
  
  while(1)
  {
     xLastWakeTime = xTaskGetTickCount();
    // Reset battery once it reaches zero to continue simulation
    if ((*(statusDataPtr->batteryState)) == 0) 
    {
      (*(statusDataPtr->batteryState)) = INTBAT;
    } 
    else 
    {
      (*(statusDataPtr->batteryState))--;
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}
