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
extern void startup();
extern void schedulerInit();


/**
*       Application Main entry point.
*/
 void main()
{
  // Use 8MHZ crystal, and the PLL to get to 50MHZ
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |   
                   SYSCTL_XTAL_8MHZ); 
  
  startup();
  schedulerInit();  // Initialize the task queue and tasks
  schedulerStart(); // Handoff to scheduler
  return;
}

/**
*       Software delay. Deprecated
*       @param delayTime delay in miliseconds.
*/
void delayMs(unsigned long delayTime)
{
    volatile unsigned long i = 0; 
    volatile unsigned long j = 0; 
      
    for (i = delayTime; i > 0; i--) 
    { 
        for (j = 0; j < 741; j++); 
    } 
}

