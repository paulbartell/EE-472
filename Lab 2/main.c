#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "tasks.h"
#include "main.h"
#include "schedule.h"

#define NUM_TASKS 5
#define TASK_QUEUE_LEN 6
#define STR_SIZE 16

unsigned int globaltime = 0;

unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 50;
unsigned short batteryState = 200;

MeasureData measureData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw};

unsigned char tempCorrected[STR_SIZE];
unsigned char systolicPressCorrected[STR_SIZE];
unsigned char diastolicPressCorrected[STR_SIZE];
unsigned char pulseRateCorrected[STR_SIZE];
unsigned char battCorrected[STR_SIZE];

ComputeData computeData = {&temperatureRaw, &systolicPressRaw,
  &diastolicPressRaw, &pulseRateRaw, &batteryState, tempCorrected, systolicPressCorrected, diastolicPressCorrected,
  pulseRateCorrected, battCorrected};

DisplayData displayData = {tempCorrected, systolicPressCorrected, diastolicPressCorrected,
  pulseRateCorrected, battCorrected};

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
  taskQueue[5] = (TCB) {(void*)0,(void*)0};
  
  // Call any setup functions needed for each task.
  warningAlarmSetup();
  oledDisplaySetup();
  
}

// Main entry point
void main()
{
  
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
  // Use 8MHZ crystal directly
  
  init();
  
  // Setup GPIO G3 as output for timing measurement
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
  
  while(1)
  {
    
    runTasks(); // run major cycle tasks
    delayMs(MAJORCYCLEDELAYMS); // Major cycle 
    //SysCtlDelay(MAJORCYCLEDELAYMS*SysCtlClockGet()/3000);
    globaltime++;
    
    for(int j = 1; j < MAJORCYCLECOUNT; j++)
    {
      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 1 << 3); //tick
      
      runTasks(); // run all minor cycle tasks
      
      globaltime++;
      
      delayMs(MINORCYCLEDELAYMS); // Minor cycle delay
      //sysCtlDelay(MINORCYCLEDELAYMS*SysCtlClockGet()/3000);
      
      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);//tock
    }
  }
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

