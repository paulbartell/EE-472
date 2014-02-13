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
#include "utils/uartstdio.h"

volatile unsigned long globalTime = 0;
volatile unsigned long pulseRateCount = 0;

void SysTickIntHandler(void)
{
  globalTime++;
  
}


void GPIOFIntHandler(void)
{
  pulseRateCount++;
  GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
}

