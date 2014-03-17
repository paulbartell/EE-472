/****************************************** 
* task name: warningAlarm
* task inputs: a void* pointer to a WarningAlarmData struct
* task outputs: PG1 (PWM buzzer), PE0-PE2 (GYR LEDs)
* task description: Evaluates raw data values for alarm and warning states.
Outputs audible and visual warnings and alerts when values are outside of 5%
and 10% respectively.
* author: Paul Bartell
******************************************/ 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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

#define TEMP_WARNING_LOW 473
#define TEMP_WARNING_HIGH 477
#define TEMP_ALARM_LOW 471
#define TEMP_ALARM_HIGH 479

#define BPS_WARNING 58
#define BPD_WARNING 52
#define BPS_ALARM 61
#define BPD_ALARM 54

#define BATT_WARNING_LOW 38
#define BATT_ALARM_LOW 36

// Number of major cycles an alarm acknowledgement should last
#define ACK_MAX_TIME 5

#define TONE 1200

// defines a state variable for keeping track of Alarm state.
enum _myState { NORMAL = 0, WARNING = 1, ALARM = 2, ACK = 3, NEWWARNING = 4, NEWALARM = 5 };
typedef enum _myState State;

extern xTaskHandle taskList[];


// Warning and Alarm flag initialization
Bool bpOutOfRange = FALSE;
Bool tempOutOfRange = FALSE;
Bool pulseOutOfRange = FALSE;
Bool batteryOutOfRange = FALSE;

Bool lastBp = FALSE;
Bool lastTemp = FALSE;
Bool lastPulse = FALSE;
Bool lastBatt = FALSE;

Bool bpAlarm = FALSE;
Bool tempAlarm = FALSE;
Bool pulseAlarm = FALSE;
Bool batteryAlarm = FALSE;

long numWarnings = 0;

State state = NORMAL;

unsigned int ackCounter = 0;
unsigned long button = 0;




void warningAlarmSetup(void)
{
  // Compute the period for 440Hz
  unsigned long ulPeriod = SysCtlClockGet() / TONE;
  // Set the PWM clock reference
  SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
  // Enable GPIOG for PWM output
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  // Enable PWM0 Peripherial
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  // Configure Pin PG1 as a PWM output
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1); 
  // Configure PWM Generator for audio-output
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0, 
                  PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
  // set PWM period/frequency (Note of the buzzer)
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
  // set pulse width (Volume of the buzzer)
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0);//ulPeriod / 2);
  // Enable the PWM output
  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
  
  
  // Enable GPIO port E (GYR LEDs)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  // Set PE0 (Green), PE1 (Yellow), PE2 (RED) as outputs
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
  
  // Turn on GYR leds (default state)
  GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ), 255);
  
  return;
}

void warningAlarm(void* taskDataPtr)
{
  unsigned long expTime = 28800;
  WarningAlarmData* warningAlarmData = (WarningAlarmData*) taskDataPtr;
  warningAlarmSetup();
  portTickType xLastWakeTime;
  const portTickType xFrequency = 250; // for 2Hz operation, 4Hz calling
  xLastWakeTime = xTaskGetTickCount();
  
  while(1)
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
    int* pulseRateRaw = (int*) warningAlarmData->pulseRateRawBuf->headPtr;
    if((*pulseRateRaw < PULSE_WARNING_LOW ) ||
       (*pulseRateRaw > PULSE_WARNING_HIGH ))
    {
      pulseOutOfRange = TRUE;
      
      if((*pulseRateRaw < PULSE_ALARM_LOW ) ||
         (*pulseRateRaw > PULSE_ALARM_HIGH ))
      {
        pulseAlarm = TRUE;
      }
    }
    
    // Temperature checks
    int* temperatureRaw = (int*) warningAlarmData->temperatureRawBuf->headPtr;
    if((*temperatureRaw < TEMP_WARNING_LOW )||
       (*temperatureRaw > TEMP_WARNING_HIGH))
    {
      tempOutOfRange = TRUE;
      if( (*temperatureRaw < TEMP_ALARM_LOW) ||
         (*temperatureRaw > TEMP_ALARM_HIGH))
      {
        tempAlarm = TRUE;
      }
    }
    
    // Blood Pressure checks
    int* systolicPressRaw = (int*) warningAlarmData->systolicPressRawBuf->headPtr;
    int* diastolicPressRaw = (int*) warningAlarmData->diastolicPressRawBuf->headPtr;
    if((*systolicPressRaw > BPS_WARNING) || //TODO FIXME
       (*diastolicPressRaw > BPD_WARNING))
    {
      bpOutOfRange = TRUE;
      if((*systolicPressRaw > BPS_ALARM) ||
         (*diastolicPressRaw > BPD_ALARM))
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
      if ((bpAlarm || tempAlarm || pulseAlarm || batteryAlarm))
      {
        if(state == NEWALARM)
        {
          state = ALARM;
        }
        else
        {
          state = NEWALARM;
          vTaskResume(taskList[TASK_COMMUNICATION]); // Resume communication task
        }
      }
      else if(bpOutOfRange || tempOutOfRange || pulseOutOfRange || batteryOutOfRange)
      {
        if(state == NEWWARNING)
        {
          state = WARNING;
        }
        else
        {
          state = NEWWARNING;
          vTaskResume(taskList[TASK_COMMUNICATION]); // Resume communication task
        }
      }
      else
      {
        state = NORMAL;
      }
    }
    
    // Check for Number of warnings
    if ((lastBp != bpOutOfRange) && !lastBp) numWarnings++;
    if ((lastTemp != tempOutOfRange) && !lastTemp) numWarnings++;
    if ((lastPulse != pulseOutOfRange) && !lastPulse) numWarnings++;
    if ((lastBatt != batteryOutOfRange)&& !lastBatt) numWarnings++;
    
    lastBp = bpOutOfRange;
    lastTemp = tempOutOfRange;
    lastPulse = pulseOutOfRange;
    lastBatt = batteryOutOfRange;
    
    // Precondition: infinite timer
    if ((xTaskGetTickCount() / 1000) >= expTime) {
      numWarnings = 0;
      expTime += 28800;
    }
    for(int i = 0; i < 8; i++)
    {
      button = *(warningAlarmData->alarmAcknowledge);
      // Update state WRT button every task call
      if(button == 1 && (state == ALARM || state == NEWALARM))
      {
        state = ACK;
        *(warningAlarmData->alarmAcknowledge) = 0;
        
      }
      
      // Half-hz operations begin
      if(i % 8 == 0)
      {
        if(pulseOutOfRange)
        {
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0), 0);    // flash G led off
        }
        if(batteryOutOfRange && (state == ALARM || state == NEWALARM))
        {
          PWMGenEnable(PWM0_BASE, PWM_GEN_0);               //Turn on buzzer
        }
      }
      
      // Half-hz operations end
      if(i % 8 == 4)
      {
        GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0), 255);    // Turn G led on
        PWMGenDisable(PWM0_BASE, PWM_GEN_0);                // turn off buzzer
      }
      
      // 1 Hz operations being
      if(i % 4 == 0)
      {
        if(tempOutOfRange)
        {
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_2), 0);    // red LED off
        }
      }
      
      // 1 Hz operations end
      if(i % 4 == 2)
      {
        GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_2), 255);    // red LED on
      }
      
      // 2Hz operations start
      if(i % 2 == 0)
      {
        if(bpOutOfRange)
        {
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_1), 0);    // Yellow led off
        }
        if(state == ALARM || state == NEWALARM)
        {
          PWMGenEnable(PWM0_BASE, PWM_GEN_0);                // Enable Alarm buzzer
        }
      }
      
      
      // 2 Hz operations end
      if(i % 2 == 1)
      {
        GPIOPinWrite(GPIO_PORTE_BASE,( GPIO_PIN_1), 255);   // Yellow LED on
        PWMGenDisable(PWM0_BASE, PWM_GEN_0);                // Turn off buzzer
      }
      vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
  }
}
