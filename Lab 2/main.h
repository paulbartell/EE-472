/****************************************** 
* File name: main.h
* File description: Defines a TaskControlBlock (TCB) and function prototypes for
    main-file functions (delay, init, and runTasks)
* author: Paul Bartell
******************************************/ 

void main();
void runTasks();
void init();
void delayMs(unsigned long);

struct TaskControlBlock
{
  void (*myTask)(void*);
  void* taskDataPtr;
};
typedef struct TaskControlBlock TCB;