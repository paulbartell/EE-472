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
#include "main.h"       // For function prototypes
#include "schedule.h"   // For scheduler constants/timing



extern volatile unsigned long globalTime;

// Initialize the scheduler and some hardware
void init()
{
  
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
  
  majorCycleInitializeQueue();
  runTasks();
  
//  // Setup GPIO G3 as output for timing measurement
//  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
//  
//  while(1)
//  {
//    // run major cycle tasks
//    runTasks();
//    
//    // Major cycle Delay
//    delayMs(MAJORCYCLEDELAYMS); 
//    globalTime++;
//    
//    for(int j = 1; j < MAJORCYCLECOUNT; j++)
//    {
//      // Write PE3 high for timing test
//      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 1 << 3);
//      
//      // run all minor cycle tasks
//      runTasks();
//      
//      globalTime++;
//      
//      // Minor cycle delay
//      delayMs(MINORCYCLEDELAYMS); 
//      
//      // Write PE3 low for timing test
//      GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);
//    }
//    // delayMs(1); // Enable for timing testing
//  }
  return;
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

