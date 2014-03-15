/****************************************** 
* task name: interrupts
* task description: Contains all Interrupt Service Routienes
* author: Paul Bartell
******************************************/ 
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ints.h"
#include "FreeRTOS.h"
#include "task.h"
#include "schedule.h"
#include "tasks.h"
extern xTaskHandle taskList[];
volatile unsigned long pulseRateCount = 0;
volatile unsigned short pulseRateFlag = 0;
volatile unsigned long pulseRateSample = 0;
short ready = 0;

// Interrupt handler for the pulse rate counter.
void GPIOFIntHandler(void)
{
    pulseRateCount++;
    GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
    //UARTprintf("called\n");
}

// Interrupt handler for the pulse rate sampler. Provides hard realtime sampling.
void prTimerIntHandler(void)
{
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // clear the timer interrupt
  IntDisable(INT_GPIOF);
  IntMasterDisable();
  pulseRateSample = pulseRateCount;
  pulseRateCount = 0;
  pulseRateFlag = 1;
  ready = 1;
  IntMasterEnable();
  IntEnable(INT_GPIOF);
}