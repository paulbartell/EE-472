/****************************************** 
* function name: schedule
* function inputs: void
* function outputs: void
* function description: Scheduler and Global data initialization happens here //UPDATEME
* author: Paul Bartell, Ryan McDaniel
******************************************/ 
#include "schedule.h"
#include "tasks.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"


TCB* taskQueueHead = NULL; // Head of task queue
TCB* curTask = NULL;       // Current task

extern TCB taskList[NUM_TASKS];
extern volatile unsigned long globalTime;

unsigned short addFlags[] = {0,0,0,0,0,0,0,0};
unsigned short removeFlags[] = {0,0,0,0,0,0,0,0};

/**
*   Resets and adds the proper tasks to the task queue at the beginning of a major cycle
*/
void majorCycleInitializeQueue()
{
  taskQueueHead = NULL; // Reset the task queue
  addTask(TASK_MEASURE);
  addTask(TASK_STATUS);
  addTask(TASK_WARNINGALARM);
  addTask(TASK_DISPLAY);
  addTask(TASK_KEYPAD);
}

/**
*   Starts the scheduler, runs all of the tasks, and blocks until shutdown.
*/
void schedulerStart()
{
  // Setup GPIO F3 as output for timing measurement
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
  
  while (1) {
    if((globalTime % MAJORCYCLECOUNT == 0))
    {
      majorCycleInitializeQueue();
    }
    else if(globalTime % (HALFHZCOUNT) == 0)
    {
      addFlags[TASK_KEYPAD] = 1;
    }
    runTasks();
    unsigned long thisGTime = globalTime;
    while(thisGTime == globalTime);             // delay until the end of the minor cycle
  }
}

/**
*       Checks add/remove flags (for task add/removal at runtime) and runs
*            tasks in the task queue.
*/
void runTasks()
{
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 1 << 3); // Tick for timing test
  curTask = taskQueueHead;
  
  while(curTask != NULL)
  {
   (*curTask->taskFtn)(curTask->taskDataPtr); // run task
   
   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);    // Tock for timing test
   
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

/**
*       Removes the task with the given task ID number.
*       @param taskID ID number of the task to remove
*/
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

/**
*       Adds a task with the given ID to the queue after the current task.
*       @param taskID ID of the task to add to the queue.
*/
void addTask(unsigned int taskID)
{
  if(taskQueueHead == NULL)
  {
    taskQueueHead = &taskList[taskID];
    // Set both to &taskList for circular
    taskQueueHead->next = NULL;
    taskQueueHead->previous = NULL;
    curTask = taskQueueHead;
  } else {        // already have some tasks scheduled
    TCB* temp = curTask->next;
    temp->previous = &taskList[taskID];
    curTask->next = &taskList[taskID];
    curTask->next->previous = curTask;
    curTask->next->next = temp;
  }
}