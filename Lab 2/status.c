#include "tasks.h"
#include "schedule.h"

extern unsigned long globaltime;

void status(void* taskDataPtr)
{
  if(globaltime % MAJORCYCLECOUNT == 0)
  {
    // Pointer re-cast as pointer to the Status task's data structure
    StatusData* statusDataPtr = (StatusData*) taskDataPtr;
    (*(statusDataPtr->batteryState))--;
  }
}
