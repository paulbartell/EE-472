#include "tasks.h"

void status(void* taskDataPtr)
{
    int battery;

    // Pointer re-cast as pointer to the Status task's data structure
    StatusData* statusDataPtr = (StatusData*) taskDataPtr;
    battery = *(statusDataPtr->batteryState);

    // Decrements battery state by one each time it is called.
    battery--;
    *(statusDataPtr->batteryState)=battery;
}
