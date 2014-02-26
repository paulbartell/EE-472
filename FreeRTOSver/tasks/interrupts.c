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

volatile unsigned long globalTime = 0;
volatile unsigned long pulseRateCount = 0;
volatile unsigned short pulseRateFlag = 0;
volatile unsigned long pulseRateSample = 0;

// Interrupt handler for the pulse rate counter.
void GPIOFIntHandler(void)
{
  pulseRateCount++;
  GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
}

// Interrupt handler for the pulse rate sampler. Provides hard realtime sampling.
void prTimerIntHandler(void)
{
  TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT); // clear the timer interrupt
  IntDisable(INT_GPIOF);
  pulseRateSample = pulseRateCount;
  pulseRateCount = 0;
  pulseRateFlag = 1;
  IntEnable(INT_GPIOF);
}