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

unsigned short addFlags[] = {0,0,0,0,0,0,0,0};
unsigned short removeFlags[] = {0,0,0,0,0,0,0,0};


void majorCycleInitializeQueue()
{
  taskQueueHead = NULL; // Reset the task queue
  addTask(TASK_MEASURE);
  addTask(TASK_COMMUNICATION);
  addTask(TASK_STATUS);
  addTask(TASK_WARNINGALARM);
  addTask(TASK_DISPLAY);
  addTask(TASK_KEYPAD);
}

void schedulerStart()
{
  while (1) {
    if((globalTime % MAJORCYCLECOUNT == 0))
    {
      majorCycleInitializeQueue();
    }
    else if(globalTime % (HALFHZCOUNT) == 0)
    {
      //addTask(TASK_KEYPAD);
      addFlags[TASK_KEYPAD] = 1;
    }
    runTasks();
    unsigned long thisGTime = globalTime;
    while(thisGTime == globalTime);             // delay until the end of the minor cycle
  }
}

void runTasks()
{
  curTask = taskQueueHead;
  
  while(curTask != NULL)
  {
   (*curTask->taskFtn)(curTask->taskDataPtr); // run task
   
   for(int i = 0; i < NUM_TASKS; i++)
   {
     if(addFlags[i])
     {
       addTask(i);
       addFlags[i] = 0;
     }
   }
   
   curTask = curTask->next; // update next ptr
   
   for(int i = 0; i < NUM_TASKS; i++)
   {
     if(removeFlags[i])
     {
       removeTask(i);
       removeFlags[i] = 0;
     }
   }

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
  cur->next = NULL;
  cur->previous = NULL;
}

// Adds a task with the given ID to the queue after the current task.
void addTask(unsigned int taskID)
{
  if(taskQueueHead == NULL)
  {
    taskQueueHead = &taskList[taskID];
    // Set both to &taskList for circular
    taskQueueHead->next = NULL;
    taskQueueHead->previous = NULL;
    curTask = taskQueueHead;
  } else {        // alread have tasks scheduled
    TCB* temp = curTask->next;
    temp->previous = &taskList[taskID];
    curTask->next = &taskList[taskID];
    curTask->next->previous = curTask;
    curTask->next->next = temp;
  }
}