#include "tasks.h"
#include "main.h"

#define NUM_TASKS 5
#define TASK_QUEUE_LEN 6

unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 50;

MeasureData measureData = {&temperatureRaw, &systolicPressRaw,
                  &diastolicPressRaw, &pulseRateRaw};

unsigned char* tempCorrected;
unsigned char* systolicPressCorrected;
unsigned char* diastolicPressCorrected;
unsigned char* pulseRateCorrected;

unsigned short batteryState;

unsigned char bpOutOfRange;
unsigned char tempOutOfRange;
unsigned char pulseOutOfRange;

Bool bpHigh;
Bool tempHigh;
Bool pulseLow;

// Initialize data passing structs


TCB taskQueue[TASK_QUEUE_LEN];

// Initialize our variables
void init()
{
  
  // Initialize the task queue
  taskQueue[0] = (TCB) {&measure,&measureData};
  taskQueue[1] = (TCB) {&measure,&measureData};
  taskQueue[2] = (TCB) {&measure,&measureData};
  taskQueue[3] = (TCB) {&measure,&measureData};
  taskQueue[4] = (TCB) {&measure,&measureData};
  taskQueue[5] = (TCB) {&measure,&measureData};
  
  
}

// Main entry point
void main()
{
  init();
  
  while(1)
  {
    for(int i = 0; i < NUM_TASKS; i++)
    {
      //tick
      
      
      // Run task
      (*taskQueue[i].myTask)(taskQueue[i].taskDataPtr);
      
      //tock
      
      
    }
  }
}

// Software delay function
void delayMs(unsigned long delayTime)
{
    volatile unsigned long i = 0; 
    volatile unsigned long j = 0; 
      
    for (i = delayTime; i > 0; i--) 
    { 
        for (j = 0; j < 740; j++); 
    } 
}

