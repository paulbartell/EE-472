/****************************************** 
* task name: startup
* task inputs: a void* pointer
* task outputs: 
* task description: Initializes hardware and data structs
* author: Paul Bartell
******************************************/ 

#define UART_BUFFERED
#define PART_LM3S2110

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "interrupts.c"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "schedule.h"
#include "utils/uartstdio.h"

void startup(void* taskDataPtr)
{
  // Setup UART for communications task
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);
  
  // Setup SysTick timer for global time base
  SysTickPeriodSet(SysCtlClockGet() / MINORCYCLEPERSEC);
  SysTickIntRegister(SysTickIntHandler);
  IntMasterEnable();
  SysTickIntEnable();
  SysTickEnable();
  
  // PF0 for pulse counting
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
  IntEnable(INT_GPIOF);
  
  
}