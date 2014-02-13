/****************************************** 
* task name: startup
* task inputs: a void* pointer
* task outputs: 
* task description: Initializes hardware and data structs
* author: Paul Bartell
******************************************/ 

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "schedule.h"
#include "utils/uartstdio.h"
#include "tasks.h"

#define STR_SIZE 16
#define BUF_CAPACITY 8

extern void SysTickIntHandler(void);

TCB taskList[NUM_TASKS];

// Default values
unsigned int temperatureRaw[BUF_CAPACITY] = {75, 75, 75, 75, 75, 75, 75, 75};
CircularBuffer temperatureRawBuf;

unsigned int systolicPressRaw[BUF_CAPACITY] = {80, 80, 80, 80, 80, 80, 80, 80};
CircularBuffer systolicPressRawBuf;

unsigned int diastolicPressRaw[BUF_CAPACITY] = {80, 80, 80, 80, 80, 80, 80, 80};
CircularBuffer diastolicPressRawBuf;

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

MeasureData measureData = {&temperatureRawBuf, &systolicPressRawBuf, &diastolicPressRawBuf,
  &pulseRateRawBuf, &measurementSelection};

unsigned char tempCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer tempCorrectedBuf;

unsigned char systolicPressCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer systolicPressCorrectedBuf;

unsigned char diastolicPressCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer diastolicPressCorrectedBuf;

unsigned char pulseRateCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer pulseRateCorrectedBuf;

unsigned char battCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer battCorrectedBuf;

unsigned short displayMode = 0;
unsigned short displayScroll = 0;

ComputeData computeData = {&temperatureRawBuf,
  &systolicPressRawBuf, &diastolicPressRawBuf, &pulseRateRawBuf, &tempCorrectedBuf,
  &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf, &pulseRateCorrectedBuf,
  &battCorrectedBuf, &batteryState};


DisplayData displayData = {&tempCorrectedBuf, &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf,
  &pulseRateCorrectedBuf, &battCorrectedBuf, &measurementSelection, &displayMode,
  &displayScroll};

StatusData statusData = {&batteryState};

unsigned short alarmAcknowledge = 0;

WarningAlarmData warningAlarmData = {&temperatureRawBuf, &systolicPressRawBuf, &diastolicPressRawBuf,
  &pulseRateRawBuf, &batteryState, &alarmAcknowledge};

CommunicationsData communicationsData = {&tempCorrectedBuf, 
  &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf, &pulseRateCorrectedBuf, &battCorrectedBuf};

KeypadData keypadData = {&displayMode, &displayScroll, &measurementSelection, 
  &alarmAcknowledge}; // WHAT ABOUT SELECT.. IS THAT DIFFERENT? USED?


void startup(void* taskDataPtr)
{
  // Setup UART for communications task
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);
  
  // Setup SysTick timer for global time base
  SysTickPeriodSet(SysCtlClockGet() / 50);
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
  
  // setup circular buffers
  //void cBuffInit(CircularBuffer* cb, void* buffPtr, int capacity, int itemSize)
  cBuffInit(&temperatureRawBuf, temperatureRaw, BUF_CAPACITY, sizeof(temperatureRaw)/BUF_CAPACITY);
  cBuffInit(&systolicPressRawBuf, systolicPressRaw, BUF_CAPACITY, sizeof(systolicPressRaw)/BUF_CAPACITY);
  cBuffInit(&diastolicPressRawBuf, diastolicPressRaw, BUF_CAPACITY, sizeof(diastolicPressRaw)/BUF_CAPACITY);
  cBuffInit(&pulseRateRawBuf, pulseRateRaw, BUF_CAPACITY, sizeof(pulseRateRaw)/BUF_CAPACITY);
  
  cBuffInit(&tempCorrectedBuf, tempCorrected, BUF_CAPACITY, sizeof(tempCorrected)/BUF_CAPACITY);
  cBuffInit(&systolicPressCorrectedBuf, systolicPressCorrected, BUF_CAPACITY, sizeof(systolicPressCorrected)/BUF_CAPACITY);
  cBuffInit(&diastolicPressCorrectedBuf, diastolicPressCorrected, BUF_CAPACITY, sizeof(diastolicPressCorrected)/BUF_CAPACITY);
  cBuffInit(&pulseRateCorrectedBuf, pulseRateCorrected, BUF_CAPACITY, sizeof(pulseRateCorrected)/BUF_CAPACITY);
  cBuffInit(&battCorrectedBuf, battCorrected, BUF_CAPACITY, sizeof(battCorrected)/BUF_CAPACITY);
  

}

void schedulerInit()
{
  // Initialize the task queue
  // taskList[0] = (TCB) {&startup, NULL, NULL, NULL};
  taskList[0] = (TCB) {&measure,&measureData,NULL,NULL};
  taskList[1] = (TCB) {&compute,&computeData,NULL,NULL};
  taskList[2] = (TCB) {&oledDisplay,&displayData,NULL,NULL};
  taskList[3] = (TCB) {&warningAlarm,&warningAlarmData,NULL,NULL};
  taskList[4] = (TCB) {&status,&statusData,NULL,NULL};
  taskList[5] = (TCB) {&keypad,&keypadData,NULL,NULL};
  taskList[6] = (TCB) {&communication,&communicationsData,NULL,NULL};
  taskList[7] = (TCB) {NULL,NULL,NULL,NULL};
}