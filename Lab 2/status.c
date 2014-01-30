#include "tasks.h"
#include "schedule.h"
#define INTBAT 200
extern unsigned long globaltime;

void status(void* taskDataPtr)
{
  if(globaltime % MAJORCYCLECOUNT == 0)
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
}
