void main();
void init();
void delayMs(unsigned long);

enum _myBool { FALSE = 0, TRUE = 1 }; 
 typedef enum _myBool Bool; 

struct TaskControlBlock
{
  void (*myTask)(void*);
  void* taskDataPtr;
};
typedef struct TaskControlBlock TCB;