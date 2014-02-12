/****************************************** 
* function name: schedule
* function inputs: void
* function outputs: void
* function description: Scheduler and Global data initialization happens here //UPDATEME
* author: Paul Bartell, Ryan McDaniel
******************************************/ 
#include "schedule.h"
#include "tasks.h"

TCB* taskQueueHead = NULL; // Head of task queue
TCB* curTask = NULL;       // Current task

extern TCB taskList[NUM_TASKS];
extern volatile unsigned long globalTime;

void majorCycleInitializeQueue()
{
  taskQueueHead = NULL; // Reset the task queue
  addTask(TASK_MEASURE);
  addTask(TASK_KEYPAD);
  addTask(TASK_DISPLAY);
  addTask(TASK_WARNINGALARM);
  addTask(TASK_STATUS);
}

void runTasks()
{
  curTask = taskQueueHead;
  
  while(curTask != NULL)
  {
   (*curTask->taskFtn)(curTask->taskDataPtr); // run task
    curTask = curTask->next; // update next ptr
  }
}

void removeTask(int taskID)
{
  TCB* cur = &taskList[taskID];
  if(taskQueueHead == cur)
  {
    taskQueueHead = taskQueueHead->next;
  }
  cur->next->previous = cur->previous;
  cur->previous->next = cur->next;
}

// Adds a task with the given ID to the queue after the current task.
void addTask(unsigned int taskID)
{
  if(taskQueueHead == NULL)
  {
    taskQueueHead = &taskList[taskID];
    taskQueueHead->next = taskQueueHead;
    taskQueueHead->previous = NULL;
  }else{        // alread have tasks scheduled
    TCB* temp = curTask->next;
    curTask->next = &taskList[taskID];
    curTask->next->previous = curTask;
    curTask->next->next = temp;
  }
}