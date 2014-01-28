#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "tasks.h"
#include "schedule.h"

// Raw data ranges for various measurements
// Rounded down for early detection

#define PULSE_WARNING_LOW 16
#define PULSE_WARNING_HIGH 31
#define PULSE_ALARM_LOW 15
#define PULSE_ALARM_HIGH 32

#define TEMP_WARNING_LOW 41
#define TEMP_WARNING_HIGH 43
#define TEMP_ALARM_LOW 41
#define TEMP_ALARM_HIGH 43

#define BPS_WARNING 85
#define BPD_WARNING 52
#define BPS_ALARM 61
#define BPD_ALARM 54

#define BATT_WARNING_LOW 20
#define BATT_ALARM_LOW 15

#define TRUE 1
#define FALSE 0

extern unsigned long globaltime;

static unsigned int alarm = 0;
static unsigned int warning = 0;
static unsigned int audiowarning = 0;

static Bool bpHigh = FALSE;
static Bool tempHigh = FALSE;
static Bool pulseLow = FALSE;

void warningAlarmSetup(void)
{
  // Enable GPIO port E
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  // Enable GPIO port G
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  // Set PG1 as an output for buzzer
  GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_1);
  // Set PE0 (Green), PE1 (Yellow), PE2 (RED) as outputs
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);

}

void warningAlarm(void* taskDataPtr)
{
  WarningAlarmData* warningAlarmData = (WarningAlarmData*) taskDataPtr;

  if(globaltime % MAJORCYCLECOUNT == 0)
  {
    // reset alarm/warning flags
    alarm = 0;
    audiowarning = 0;

    // Pulse rate checks
    if((*(warningAlarmData->pulseRateRaw) < PULSE_WARNING_LOW ) ||
       (*(warningAlarmData->pulseRateRaw) > PULSE_WARNING_HIGH ))
    {
      //Blink green LED with a two second period
      if((*(warningAlarmData->pulseRateRaw) < PULSE_ALARM_LOW ) ||
         (*(warningAlarmData->pulseRateRaw) > PULSE_ALARM_HIGH ))
      {
        alarm = 1;
      }
    }

    // Temperature checks
    if((*(warningAlarmData->temperatureRaw) < TEMP_WARNING_LOW )|| (*(warningAlarmData->temperatureRaw) > TEMP_WARNING_HIGH))
    {
      //Blink green LED with a two second period
      if( (*(warningAlarmData->temperatureRaw) < TEMP_ALARM_LOW) ||
         (*(warningAlarmData->temperatureRaw) > TEMP_ALARM_HIGH))
      {
        alarm = TRUE;
      }
    }

    // Blood Pressure checks
    if((*(warningAlarmData->systolicPressRaw) > BPS_WARNING) ||
       (*(warningAlarmData->diastolicPressRaw) > BPD_WARNING))
    {
      //Blink green LED with a two second period
      if((*(warningAlarmData->systolicPressRaw) > BPS_ALARM) ||
         (*(warningAlarmData->diastolicPressRaw) > BPD_ALARM))
      {
        alarm = TRUE;
      }
    }

    // Battery Checks
    if( *(warningAlarmData->batteryState) < BATT_WARNING_LOW)
    {
      //Buzz at .5hz
      audiowarning = TRUE;
      if( *(warningAlarmData->batteryState) < BATT_ALARM_LOW)
      {
        alarm = TRUE;
      }
    }
  }

  if(globaltime % HALFHZCOUNT == 0)
  {
    // Do half HZ stuff start
  }
  if(globaltime % HALFHZCOUNT == HALFHZDELAY)
  {
    // Do half hz stuff stop
  }

  if(globaltime % T2HZCOUNT == 0)
  {
    // Do 2 HZ stuff start
  }
  if(globaltime % T2HZCOUNT == T2HZDELAY)
  {
    // Do 2 HZ stuff stop
  }
  GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2), 1 << 3);
}
