/****************************************** 
* File name: schedule.h
* File description: Defines constants related to scheduler timing required
    for periodic tasks.
* author: Paul Bartell
******************************************/ 

#define MINORCYCLEPERSEC 50
#define MINORCYCLESPERMIN 3000
#define MINORCYCLELENGTHMS 20
#define MAJORCYCLEDELAYMS 4
#define MINORCYCLEDELAYMS 20
#define MAJORCYCLECOUNT 250
#define HALFHZCOUNT 100
#define HALFHZDELAY 50
#define T1HZCOUNT 50
#define T1HZDELAY 25
#define T2HZCOUNT 25
#define T2HZDELAY 12

#define NUM_TASKS 8

struct TaskControlBlock
{
  void (*taskFtn)(void*);
  void* taskDataPtr;
  struct TaskControlBlock* next;
  struct TaskControlBlock* previous;
};
typedef struct TaskControlBlock TCB;

void majorCycleInitializeQueue();
void runTasks();
void removeTask(int taskID);
void addTask(unsigned int taskID);