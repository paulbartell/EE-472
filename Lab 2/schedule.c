/****************************************** 
* function name: schedule
* function inputs: void
* function outputs: void
* function description: Scheduler and Global data initialization happens here //UPDATEME
* author: Paul Bartell, Ryan McDaniel
******************************************/ 
#include "schedule.h"

TCB* taskListHead;
TCB taskList[NUM_TASKS];



void schedulerInit()
{
  // Initialize the task queue
  taskList[0] = (TCB) {&measure,&measureData};
  taskList[1] = (TCB) {&compute,&computeData};
  taskList[2] = (TCB) {&oledDisplay,&displayData};
  taskList[3] = (TCB) {&warningAlarm,&warningAlarmData};
  taskList[4] = (TCB) {&status,&statusData};
  
}

void removeTask(int taskID)
{
  
}

void addTask(int taskID)
{
  
}