/****************************************** 
* function name: schedule
* function inputs: void
* function outputs: void
* function description: Scheduler and Global data initialization happens here //UPDATEME
* author: Paul Bartell, Ryan McDaniel
******************************************/ 
#include "schedule.h"

TCB* taskListHead = NULL;
TCB taskList[NUM_TASKS];

#define TASK_STARTUP 0
#define TASK_MEASURE 1
#define TASK_COMPUTE 2
#define TASK_DISPLAY 3
#define TASK_WARNINGALARM 4
#define TASK_STATUS 5
#define TASK_COMMUNICATION 6

void schedulerInit()
{
  // Initialize the task queue
  taskList[0] = (TCB) {&startup, NULL, NULL, NULL};
  taskList[1] = (TCB) {&measure,&measureData,NULL,NULL};
  taskList[2] = (TCB) {&compute,&computeData};
  taskList[3] = (TCB) {&oledDisplay,&displayData,NULL,NULL};
  taskList[4] = (TCB) {&warningAlarm,&warningAlarmData,NULL,NULL};
  taskList[5] = (TCB) {&status,&statusData,NULL,NULL};
  taskList[6] = (TCB) {&keypad,&keypadData,NULL,NULL);
  taskList[7] = (TCB) {&communication,&communicationData,NULL,NULL);
  taskList[8] = (TCB) {NULL,NULL,NULL,NULL};
}

void runScheduler()
{
  
}

void removeTask(int taskID)
{
  TCB* cur = &taskList[taskID];
  if(taskListHead == cur)
  {
    taskListHead = taskListHead->next;
  }
  cur->next->previous = cur->previous;
  cur->previous->next = cur->next;
}

void addTask(int taskID)
{
  if(taskListHead == NULL)
  {
    taskListHead = taskList[taskID];
  }else{
    TCB* temp = taskListHead->next;
    taskListHead->next = taskList[taskID];
    taskList->next->previous = taskListHead;
    taskListHead->next->next = temp;
  }
}