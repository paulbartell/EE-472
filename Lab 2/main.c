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


#define STR_SIZE 16
#define BUF_CAPACITY 8

unsigned long globalTime = 0;

// Default values
unsigned int temperatureRaw[BUF_CAPACITY] = {75, 75, 75, 75, 75, 75, 75, 75};
CircularBuffer temperatureRawBuf;

unsigned int bloodPressRaw[BUF_CAPACITY * 2] = {80, 80, 80, 80, 80, 80, 80, 80,
                                 80, 80, 80, 80, 80, 80, 80, 80};
CircularBuffer bloodPressRawBuf;

unsigned int pulseRateRaw[BUF_CAPACITY] =  {50, 50, 50, 50, 50, 50, 50, 50};
CircularBuffer pulseRateRawBuf;

unsigned short batteryState =   200;
unsigned short measurementSelection = 0;

/*
// Good test values
unsigned int temperatureRaw = 35;
unsigned int systolicPressRaw = 40;
unsigned int diastolicPressRaw = 40;
unsigned int pulseRateRaw = 25;
unsigned short batteryState = 200;
//unsigned short batteryState = 42;
*/

MeasureData measureData = {&temperatureRawBuf, &bloodPressRawBuf,
  &pulseRateRawBuf, &measurementSelection};

unsigned char tempCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer tempCorrectedBuf;

unsigned char bloodPressCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer bloodPressCorrectedBuf;

unsigned char pulseRateCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer pulseRateCorrectedBuf;

unsigned char battCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer battCorrectedBuf;

unsigned short mode = 0;

ComputeData computeData = {&temperatureRawBuf,
  &bloodPressRawBuf, &pulseRateRawBuf, &tempCorrectedBuf,
  &bloodPressCorrectedBuf, &pulseRateCorrectedBuf,
  battCorrected};

DisplayData displayData = {&tempCorrectedBuf, &bloodPressCorrectedBuf,
&pulseRateCorrectedBuf, battCorrected, &mode};

StatusData statusData = {&batteryState};


WarningAlarmData warningAlarmData = {&temperatureRawBuf, &bloodPressRawBuf,
  &pulseRateRawBuf, &batteryState};

//TCB taskQueue[TASK_QUEUE_LEN];

// Initialize the scheduler and some hardware
void init()
{
  // Initialize circular buffers.
  //void cBuffInit(CircularBuffer* cb, void* buffPtr, int capacity, int itemSize)
  cBuffInit(&temperatureRawBuf, temperatureRaw, BUF_CAPACITY, sizeof(temperatureRaw) / BUF_CAPACITY);
  cBuffInit(&bloodPressRawBuf, bloodPressRaw, BUF_CAPACITY*2, sizeof(bloodPressRaw) / (BUF_CAPACITY*2));
  cBuffInit(&pulseRateRawBuf, pulseRateRaw, BUF_CAPACITY, sizeof(pulseRateRaw) / BUF_CAPACITY);
  
  
  cBuffInit(&tempCorrectedBuf, tempCorrected, BUF_CAPACITY, sizeof(tempCorrected) / BUF_CAPACITY);
  cBuffInit(&bloodPressCorrectedBuf, bloodPressCorrected, BUF_CAPACITY*2, sizeof(bloodPressCorrected) / (BUF_CAPACITY*2));
  cBuffInit(&pulseRateCorrectedBuf, pulseRateCorrected, BUF_CAPACITY, sizeof(pulseRateCorrected) / BUF_CAPACITY);
  cBuffInit(&battCorrectedBuf, battCorrected, BUF_CAPACITY, sizeof(battCorrected) / BUF_CAPACITY);
  
  
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

