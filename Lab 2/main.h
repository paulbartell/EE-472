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