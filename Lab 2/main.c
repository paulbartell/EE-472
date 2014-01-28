#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

#include "tasks.h"
#include "main.h"
#include "schedule.h"



#define NUM_TASKS 5
#define TASK_QUEUE_LEN 6

unsigned int globaltime = 0;

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

WarningAlarmData warningAlarmData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw, &batteryState};

Bool bpHigh;
Bool tempHigh;
Bool pulseLow;

TCB taskQueue[TASK_QUEUE_LEN];

// Initialize the scheduler and some hardware
void init()
{
  
  // Initialize the task queue
  taskQueue[0] = (TCB) {&measure,&measureData};
  taskQueue[1] = (TCB) {&measure,&measureData};
  taskQueue[2] = (TCB) {&measure,&measureData};
  taskQueue[3] = (TCB) {&measure,&measureData};
  taskQueue[4] = (TCB) {&measure,&measureData};
  taskQueue[5] = (TCB) {&measure,&measureData};
  
  // Call any setup functions needed
  warningAlarmSetup();
  //displaySetup();
  
}

// Main entry point
void main()
{
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
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
    globaltime++;
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

