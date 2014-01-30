/****************************************** 
* task name: status
* task inputs: a void* pointer to a StatusData struct
* task outputs: Battery status
* task description: Updates the battery status every major cycle.
* author: Alyanna Castillo
******************************************/ 

#include "tasks.h"
#include "schedule.h"
#define INTBAT 200
extern unsigned long globalTime;

void status(void* taskDataPtr)
{
  if(globalTime % MAJORCYCLECOUNT == 0)
  {
    // Pointer re-cast as pointer to the Status task's data structure
    StatusData* statusDataPtr = (StatusData*) taskDataPtr;
    
    // Reset battery once it reaches zero to continue simulation
    if ((*(statusDataPtr->batteryState)) == 0) {
      (*(statusDataPtr->batteryState)) = INTBAT;
    } else {
      (*(statusDataPtr->batteryState))--;
    }
  }
  return;
}
