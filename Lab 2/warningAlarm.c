#include "tasks.h"

#define PULSE_WARNING_LOW 56
#define PULSE_WARNING_HIGH 102
#define PULSE_ALARM_LOW 56
#define PULSE_ALARM_HIGH 104

#define TEMP_WARNING_LOW 
#define TEMP_WARNING_HIGH 
#define TEMP_ALARM_LOW 
#define TEMP_ALARM_HIGH

#define BPS_WARNING 
#define BPD_WARNING
#define BPS_ALARM
#define BPD_ALARM

#define BATT_WARNING_LOW 20
#define BATT_ALARM_LOW 18

#define TRUE 1
#define FALSE 0

extern unsigned long globaltime;

static unsigned int alarm = 0;
static unsigned int warning = 0;
static unsigned int audiowarning = 0;

void warningAlarm(void* taskDataPtr)
{
  WarningAlarmData* warningAlarmData = (WarningAlarmData*) taskDataPtr;
  
  if(globaltime % MAJORCYCLECOUNT == 0)
  {
    // reset alarm/warning flags
    alarm = 0;
    audiowarning = 0;
    
    // Pulse rate checks
    if(warningAlarmData->pulseRateRaw < PULSE_WARNING_LOW ||
       warningAlarmData->pulseRateRaw > PULSE_WARNING_HIGH)
    {
      //Blink green LED with a two second period
      if(warningAlarmData->pulseRateRaw < PULSE_ALARM_LOW ||
         warningAlarmData->pulseRateRaw > PULSE_ALARM_HIGH)
      {
        alarm = TRUE;
      }
    }
    
    // Temperature checks
    if(warningAlarmData->temperatureRaw < TEMP_WARNING_LOW ||
       warningAlarmData->temperatureRaw > TEMP_WARNING_HIGH)
    {
      //Blink green LED with a two second period
      if(warningAlarmData->temperatureRaw < TEMP_ALARM_LOW ||
         warningAlarmData->temperatureRaw > TEMP_ALARM_HIGH)
      {
        alarm = TRUE;
      }
    }
    
    // Blood Pressure checks
    if(warningAlarmData->systolicPressRaw > BPS_WARNING ||
       warningAlarmData->diastolicPressRaw > BPD_WARNING)
    {
      //Blink green LED with a two second period
      if(warningAlarmData->systolicPressRaw > BP_ALARM ||
         warningAlarmData->diastolicPressRaw > BPD_ALARM)
      {
        alarm = TRUE;
      }
    }
    
    // Battery Checks
    if(warningAlarmData->batteryState < BATT_WARNING_LOW)
    {
      //Buzz at .5hz
      audiowarning = TRUE;
      if(warningAlarmData->batteryState < BATT_ALARM_LOw)
      {
        alarm = TRUE;
      }
    }
  }
  
  if(globaltime % HALFHZCOUNT == 0)
  {
    // Do half HZ stuff start
  }
  else if(globaltime % HALFHZCOUNT = HALFHZDELAY)
  {
    // Do half hz stuff stop
  }
  
  if(globaltime % 2HZCOUNT == 0)
  {
    // Do 2 HZ stuff start
  }
  else if(globaltime % 2HZCOUNT = 2HZDELAY)
  {
    // Do 2 HZ stuff stop
  }
  
}
