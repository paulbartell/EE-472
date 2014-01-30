#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pwm.h"

#include "tasks.h"
#include "schedule.h"

// Raw data ranges for various measurements
// Rounded down for early detection

#define PULSE_WARNING_LOW 16
#define PULSE_WARNING_HIGH 32
#define PULSE_ALARM_LOW 15
#define PULSE_ALARM_HIGH 34

#define TEMP_WARNING_LOW 39
#define TEMP_WARNING_HIGH 46
#define TEMP_ALARM_LOW 37
#define TEMP_ALARM_HIGH 48

#define BPS_WARNING 58
#define BPD_WARNING 52
#define BPS_ALARM 61
#define BPD_ALARM 54

#define BATT_WARNING_LOW 38
#define BATT_ALARM_LOW 36

#define ACK_MAX_TIME 2 


enum _myState { NORMAL = 0, WARNING = 1, ALARM = 2, ACK = 3 };
 typedef enum _myState State;

extern unsigned long globaltime;

Bool bpOutOfRange = FALSE;
Bool tempOutOfRange = FALSE;
Bool pulseOutOfRange = FALSE;
Bool batteryOutOfRange = FALSE;

Bool bpAlarm = FALSE;
Bool tempAlarm = FALSE;
Bool pulseAlarm = FALSE;
Bool batteryAlarm = FALSE;
State state = NORMAL;

unsigned int ackCounter = 0;
unsigned long button = 0;


void warningAlarmSetup(void)
{
  unsigned long ulPeriod = SysCtlClockGet() / 1200; // 440 Hz
  SysCtlPWMClockSet(SYSCTL_PWMDIV_1); // Set the PWM clock reference
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); // Enable GPIOG
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);  // Enable PWM0
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1); // Pin type as PWM
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC); // Configure PWM type
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod); // Set to 440Hz
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 2); // set pulse width
  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
 

  // Enable GPIO port E (GYR LEDs)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  // Enable GPIO port F (Select Switch)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Set PE0 (Green), PE1 (Yellow), PE2 (RED) as outputs
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
  // Set PF1 (select switch) as input;
  GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                   GPIO_PIN_TYPE_STD_WPU);

  GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ), 255);

}

void warningAlarm(void* taskDataPtr)
{
  WarningAlarmData* warningAlarmData = (WarningAlarmData*) taskDataPtr;

  if(globaltime % MAJORCYCLECOUNT == 0)
  {
    // reset warning flags
    bpOutOfRange = FALSE;
    tempOutOfRange = FALSE;
    pulseOutOfRange = FALSE;
    batteryOutOfRange = FALSE;

    // reset alarm flags
    bpAlarm = FALSE;
    tempAlarm = FALSE;
    pulseAlarm = FALSE;
    batteryAlarm = FALSE;


    // Pulse rate checks
    if((*(warningAlarmData->pulseRateRaw) < PULSE_WARNING_LOW ) ||
       (*(warningAlarmData->pulseRateRaw) > PULSE_WARNING_HIGH ))
    {
      pulseOutOfRange = TRUE;
      
      if((*(warningAlarmData->pulseRateRaw) < PULSE_ALARM_LOW ) ||
         (*(warningAlarmData->pulseRateRaw) > PULSE_ALARM_HIGH ))
      {
        pulseAlarm = TRUE;
      }
    }

    // Temperature checks
    if((*(warningAlarmData->temperatureRaw) < TEMP_WARNING_LOW )||
       (*(warningAlarmData->temperatureRaw) > TEMP_WARNING_HIGH))
    {
      tempOutOfRange = TRUE;
      if( (*(warningAlarmData->temperatureRaw) < TEMP_ALARM_LOW) ||
         (*(warningAlarmData->temperatureRaw) > TEMP_ALARM_HIGH))
      {
        tempAlarm = TRUE;
      }
    }

    // Blood Pressure checks
    if((*(warningAlarmData->systolicPressRaw) > BPS_WARNING) ||
       (*(warningAlarmData->diastolicPressRaw) > BPD_WARNING))
    {
      bpOutOfRange = TRUE;
      if((*(warningAlarmData->systolicPressRaw) > BPS_ALARM) ||
         (*(warningAlarmData->diastolicPressRaw) > BPD_ALARM))
      {
        bpAlarm = TRUE;
      }
    }

    // Battery Checks
    if( *(warningAlarmData->batteryState) < BATT_WARNING_LOW)
    {
      batteryOutOfRange = TRUE;
      if( *(warningAlarmData->batteryState) < BATT_ALARM_LOW)
      {
        batteryAlarm = TRUE;
      }
    }
    // Set state variable
    if(state == ACK)
    {
      if(ackCounter >= ACK_MAX_TIME)
      {
        state = NORMAL;
        ackCounter = 0;
      }
      else
      {
        ackCounter++;
      }
    }
    if(state != ACK)
    {
      if (bpAlarm || tempAlarm || pulseAlarm || batteryAlarm)
      {
        state = ALARM;
      }
      else if(bpOutOfRange || tempOutOfRange || pulseOutOfRange || batteryOutOfRange)
      {
        state = WARNING;
      }
      else
      {
        state = NORMAL;
      }
    }
  }
  
  button = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));
  // Update state WRT button every task call
  if(button == 0 && state == ALARM)
  {
    state = ACK;
  }
  
  
  // Do half HZ stuff start
  if(globaltime % HALFHZCOUNT == 0)
  {
    if(pulseOutOfRange)
    {
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0), 0); // flash G led off
    }
    if(batteryOutOfRange && (state == ALARM))
    {
      //Turn on buzzer
      PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    }
  }
  // Do half hz stuff stop
  if(globaltime % HALFHZCOUNT == HALFHZDELAY)
  {
    GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0), 255); // Flash G led on
    PWMGenDisable(PWM0_BASE, PWM_GEN_0); // turn off buzzer
  }
  // Do 1 HZ stuff start
  if(globaltime % T1HZCOUNT == 0)
  {
    if(tempOutOfRange)
    {
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_2), 0);
    }
  }
  // Do 1 hz stuff stop
  if(globaltime % T1HZCOUNT == T2HZDELAY)
  {
    GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_2), 255);
  }
  // Do 2 HZ stuff start
  if(globaltime % T2HZCOUNT == 0)
  {
    if(bpOutOfRange)
    {
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_1), 0);
    }
    if(state == ALARM)
    {
     PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    }
  }
  // Do 2 HZ stuff stop
  if(globaltime % T2HZCOUNT == T2HZDELAY)
  {
    GPIOPinWrite(GPIO_PORTE_BASE,( GPIO_PIN_1), 255);
    PWMGenDisable(PWM0_BASE, PWM_GEN_0); // Turn off buzzer
  }
}
