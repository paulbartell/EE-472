/****************************************** 
* function name: main
* function inputs: void
* function outputs: void
* function description: The main entry point for the patient monitoring system.
    Includes the task scheduler and system data structure initialization.
* author: Paul Bartell, Ryan McDaniel
******************************************/ 

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "tasks.h"      // For task data structs
#include "main.h"       // For TCB struct and function prototypes
#include "schedule.h"   // For scheduler constants/timing

#define NUM_TASKS 5
#define TASK_QUEUE_LEN 6
#define STR_SIZE 16

unsigned long globalTime = 0;

// Default values
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 50;
unsigned short batteryState = 200;

/*
// Good test values
unsigned int temperatureRaw = 35;
unsigned int systolicPressRaw = 40;
unsigned int diastolicPressRaw = 40;
unsigned int pulseRateRaw = 25;
unsigned short batteryState = 200;
//unsigned short batteryState = 42;
*/

MeasureData measureData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw};

unsigned char tempCorrected[STR_SIZE];
unsigned char systolicPressCorrected[STR_SIZE];
unsigned char diastolicPressCorrected[STR_SIZE];
unsigned char pulseRateCorrected[STR_SIZE];
unsigned char battCorrected[STR_SIZE];

ComputeData computeData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw, &batteryState, tempCorrected,
  systolicPressCorrected, diastolicPressCorrected, pulseRateCorrected,
  battCorrected};

DisplayData displayData = {tempCorrected, systolicPressCorrected,
  diastolicPressCorrected, pulseRateCorrected, battCorrected};

StatusData statusData = {&batteryState};


WarningAlarmData warningAlarmData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw, &batteryState};

TCB taskQueue[TASK_QUEUE_LEN];

// Initialize the scheduler and some hardware
void init()
{
  
  // Initialize the task queue
  taskQueue[0] = (TCB) {&measure,&measureData};
  taskQueue[1] = (TCB) {&compute,&computeData};
  taskQueue[2] = (TCB) {&oledDisplay,&displayData};
  taskQueue[3] = (TCB) {&warningAlarm,&warningAlarmData};
  taskQueue[4] = (TCB) {&status,&statusData};
  
  // for future expansion
  taskQueue[5] = (TCB) {(void*)0,(void*)0};
  
  // Call any setup functions needed for each task.
  warningAlarmSetup();
  oledDisplaySetup();
  
}

// Main entry point
void main()
{
  // Use 8MHZ crystal directly
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
  
  // Initialize the task queue and tasks
  init();
  
  // Setup GPIO G3 as output for timing measurement
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
  
  while(1)
  {
    // run major cycle tasks
    runTasks();
    
    // Major cycle Delay
    delayMs(MAJORCYCLEDELAYMS); 
    globalTime++;
    
    for(int j = 1; j < MAJORCYCLECOUNT; j++)
    {
      // Write PE3 high for timing test
      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 1 << 3);
      
      // run all minor cycle tasks
      runTasks();
      
      globalTime++;
      
      // Minor cycle delay
      delayMs(MINORCYCLEDELAYMS); 
      
      // Write PE3 low for timing test
      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);
    }
    // delayMs(1); // Enable for timing testing
  }
  return;
}

void runTasks()
{
  for(int i = 0; i < NUM_TASKS; i++)
  {
    // Run task
    (*taskQueue[i].myTask)(taskQueue[i].taskDataPtr);
  }
}

// Software delay function
void delayMs(unsigned long delayTime)
{
    volatile unsigned long i = 0; 
    volatile unsigned long j = 0; 
      
    for (i = delayTime; i > 0; i--) 
    { 
        for (j = 0; j < 741; j++); 
    } 
}

