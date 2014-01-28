#include "tasks.h"

void status(void* taskDataPtr)
{
    // Pointer re-cast as pointer to the Status task's data structure
    StatusData* statusDataPtr = (StatusData*) taskDataPtr;

    (*(statusDataPtr->batteryState))--;
}
