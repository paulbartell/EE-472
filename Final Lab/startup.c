/****************************************** 
* task name: startup
* task inputs: a void* pointer
* task outputs: 
* task description: Initializes hardware and data structs
* author: Paul Bartell
******************************************/ 
/* Scheduler includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "tasks.h"
#include "driverlib/timer.h"

#define STR_SIZE 16
#define BUF_CAPACITY 8
#define PRSAMPLETIME 20
#define EKG_CAPACITY 256
#define EKG_FREQ 10000
#define NUM_TASKS 9

extern void SysTickIntHandler(void);

xTaskHandle taskList[NUM_TASKS];

// Default values
unsigned int temperatureRaw[BUF_CAPACITY] = {75, 75, 75, 75, 75, 75, 75, 75};
CircularBuffer temperatureRawBuf;

unsigned int systolicPressRaw[BUF_CAPACITY] = {80, 80, 80, 80, 80, 80, 80, 80};
CircularBuffer systolicPressRawBuf;

unsigned int diastolicPressRaw[BUF_CAPACITY] = {80, 80, 80, 80, 80, 80, 80, 80};
CircularBuffer diastolicPressRawBuf;

unsigned int pulseRateRaw[BUF_CAPACITY] =  {50, 50, 50, 50, 50, 50, 50, 50};
CircularBuffer pulseRateRawBuf;

signed int EKGRaw[EKG_CAPACITY];
signed int EKGRawTemp[EKG_CAPACITY] = { 0 };
signed int EKGFreq[BUF_CAPACITY * 2]; 
CircularBuffer ekgFreqBuf;

EKGData ekgData = {&EKGRaw, &EKGRawTemp, &ekgFreqBuf};

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

unsigned char ekgCorrected[BUF_CAPACITY][STR_SIZE];
CircularBuffer ekgCorrectedBuf;

unsigned short displayMode = 0;
unsigned short displayScroll = 0;

ComputeData computeData = {&temperatureRawBuf,
  &systolicPressRawBuf, &diastolicPressRawBuf, &pulseRateRawBuf, &ekgFreqBuf, &tempCorrectedBuf,
  &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf, &pulseRateCorrectedBuf, &ekgCorrectedBuf,
  &battCorrectedBuf, &batteryState};


DisplayData displayData = {&tempCorrectedBuf, &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf,
  &pulseRateCorrectedBuf, &battCorrectedBuf,  &ekgCorrectedBuf, &measurementSelection, &displayMode,
  &displayScroll};

StatusData statusData = {&batteryState};

unsigned short alarmAcknowledge = 0;

WarningAlarmData warningAlarmData = {&temperatureRawBuf, &systolicPressRawBuf, &diastolicPressRawBuf,
  &pulseRateRawBuf, &batteryState, &alarmAcknowledge};

CommunicationsData communicationsData = {&tempCorrectedBuf, 
  &systolicPressCorrectedBuf, &diastolicPressCorrectedBuf, &pulseRateCorrectedBuf, &ekgCorrectedBuf, &battCorrectedBuf};

KeypadData keypadData = {&displayMode, &displayScroll, &measurementSelection, 
  &alarmAcknowledge};


void startup(void* taskDataPtr)
{
  
  // Setup Timer2A for pulse rate sampling
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  TimerConfigure(TIMER2_BASE,TIMER_CFG_A_ONE_SHOT);
  TimerLoadSet(TIMER2_BASE, TIMER_A, ( PRSAMPLETIME * SysCtlClockGet() ) );
  IntEnable(INT_TIMER2A);
  IntPrioritySet(INT_TIMER2A, 0x00); // Set highest priority
  TimerEnable(TIMER2_BASE, TIMER_A);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  
  // Setup Timer 2B for ekg sampling
  
  //TimerLoadSet(TIMER2_BASE, TIMER_B, ( SysCtlClockGet() / EKG_FREQ) );
  //TimerEnable(TIMER2_BASE, TIMER_B);
  //TimerIntEnable(TIMER2_BASE, TIMER_TIMB_TIMEOUT);
  
  // PF0 for pulse counting
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
  IntEnable(INT_GPIOF);
  
  // ADC0 for analog measurements
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  
  // setup circular buffers
  cBuffInit(&temperatureRawBuf, temperatureRaw, BUF_CAPACITY, sizeof(temperatureRaw)/BUF_CAPACITY);
  cBuffInit(&systolicPressRawBuf, systolicPressRaw, BUF_CAPACITY, sizeof(systolicPressRaw)/BUF_CAPACITY);
  cBuffInit(&diastolicPressRawBuf, diastolicPressRaw, BUF_CAPACITY, sizeof(diastolicPressRaw)/BUF_CAPACITY);
  cBuffInit(&pulseRateRawBuf, pulseRateRaw, BUF_CAPACITY, sizeof(pulseRateRaw)/BUF_CAPACITY);
  cBuffInit(&ekgFreqBuf, EKGFreq, BUF_CAPACITY * 2, sizeof(signed int));
  
  cBuffInit(&tempCorrectedBuf, tempCorrected, BUF_CAPACITY, sizeof(tempCorrected)/BUF_CAPACITY);
  cBuffInit(&systolicPressCorrectedBuf, systolicPressCorrected, BUF_CAPACITY, sizeof(systolicPressCorrected)/BUF_CAPACITY);
  cBuffInit(&diastolicPressCorrectedBuf, diastolicPressCorrected, BUF_CAPACITY, sizeof(diastolicPressCorrected)/BUF_CAPACITY);
  cBuffInit(&pulseRateCorrectedBuf, pulseRateCorrected, BUF_CAPACITY, sizeof(pulseRateCorrected)/BUF_CAPACITY);
  cBuffInit(&battCorrectedBuf, battCorrected, BUF_CAPACITY, sizeof(battCorrected)/BUF_CAPACITY);
  cBuffInit(&ekgCorrectedBuf, ekgCorrected, BUF_CAPACITY, sizeof(ekgCorrected)/BUF_CAPACITY);
  
  IntMasterEnable(); // enable all interrupts

  
  // Run task setup functions
//  warningAlarmSetup();
//  oledDisplaySetup();
//  keypadSetUp();
}

void schedulerInit()
{
  // Initialize the task queue
    //         TaskFtn,           "Name",      Stack sz,params,         priority, handle ptr location 
  xTaskCreate(measure,          "Measure",       200, &measureData,      3,     &taskList[0]);
  xTaskCreate(compute,          "Compute",       100, &computeData,      2,     &taskList[1]);
  xTaskCreate(oledDisplay,      "Display",       300, &displayData,      1,     &taskList[2]);
  xTaskCreate(warningAlarm,     "Warning Alarm", 100, &warningAlarmData, 1,     &taskList[3]);
  xTaskCreate(status,           "Status",        100, &statusData,       1,     &taskList[4]);
  xTaskCreate(keypad,           "Keypad",        100, &keypadData,       1,     &taskList[5]);
  xTaskCreate(communication,    "Communication", 100, &communicationsData,1,    &taskList[6]);
  xTaskCreate(ekgCapture,       "EKG Capture",   100, &ekgData,           4,    &taskList[7]);
  xTaskCreate(ekgProcess,       "EKG Processing",500, &ekgData,           2,    &taskList[8]);
}
