/****************************************** 
* task name: Keypad
* task inputs: a void* pointer to a Keypad struct
* task outputs: mode, scroll, measurementSelection,
*				alarm acknowledge
* task description: Helps user navigate menus on
* device to display data on OLED using 4x4 keypad
* author: Alyanna Castillo
******************************************/ 
#include "FreeRTOS.h"
#include "task.h"

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "tasks.h"
#include "schedule.h"

#define SCANALL 0
#define TEMP 1
#define BLOOD 2
#define PULSE 3
#define EKG 4
#define BATT 5
extern xTaskHandle taskList[];
/**
*       Main function for the keypad task
*       @param taskDataPtr Pointer to the KeypadData struct
*/
void keypad(void* taskDataPtr)
{
  KeypadData* keypadDataPtr = (KeypadData*) taskDataPtr;  
  int inputs[4][4];
  keypadSetUp();
  portTickType xLastWakeTime;
  const portTickType xFrequency = 1000; // for 1Hz operation
  xLastWakeTime = xTaskGetTickCount();
  
  while(1)
  {
    // Col 1
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 0);
    inputs[0][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    inputs[0][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
    inputs[0][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
    inputs[0][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 255);    
    
    //Col 2
    GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 0);
    inputs[1][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    inputs[1][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
    inputs[1][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
    inputs[1][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 255);
    
    // Col 3
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 0);
    inputs[2][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    inputs[2][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
    inputs[2][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
    inputs[2][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 255);
    
    // Col 4
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 0);
    inputs[3][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    inputs[3][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
    inputs[3][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
    inputs[3][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 255);
    
    // Measurement Selection

    if(inputs[0][0] == 0) 
    {
      (*(keypadDataPtr->measurementSelection)) = SCANALL;
      // Press 0: Scan all
    } 
    if (inputs[1][0] == 0) 
    {
      (*(keypadDataPtr->measurementSelection)) = TEMP;
      // Press 1: Temperature
    } 
    if (inputs[2][0] == 0) 
    {
      (*(keypadDataPtr->measurementSelection)) = BLOOD;
      // Press 2: Pulse Rate
    } 
    if (inputs[3][0] == 0)
    {
          (*(keypadDataPtr->measurementSelection)) = PULSE;
    // Press 3: Pulse Rate
    }
    if (inputs[0][1] == 0)
    {
      (*(keypadDataPtr->measurementSelection)) = EKG;
    // Press 4: EKG 
    }
    if (inputs[1][1] == 0)
    {
      (*(keypadDataPtr->measurementSelection)) = BATT;
    // Press 4: EKG 
    }
    
    // Mode Selection
    // Press D
    if(inputs[1][3] == 0) 
    {
      *(keypadDataPtr->mode) = !(*(keypadDataPtr->mode));
    }
    
    // Scroll button
    // Press 9
    if(inputs[1][2] == 0) 
    {
      *(keypadDataPtr->scroll) = 1;
      (*(keypadDataPtr->measurementSelection)) =  ((*(keypadDataPtr->measurementSelection)) + 1) % 6;
      if((*(keypadDataPtr->measurementSelection)) == 0) (*(keypadDataPtr->measurementSelection)) = 1;
    }
    
    // Acknowledge
    // Press A
    if(inputs[2][2] == 0)
    {
      (*(keypadDataPtr->alarmAcknowledge)) = 1;
    }
    vTaskResume(taskList[TASK_DISPLAY]);
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

/**
*       Sets up hardware (GPIO port C,D, E) for the keypad task.
*/
void keypadSetUp(void)
{
  // Enable GPIO port E
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | 
                 GPIO_PIN_7, GPIO_DIR_MODE_IN);
  
  // Set up rows                      Row 1        Row 2       Row 3
  GPIOPadConfigSet(GPIO_PORTC_BASE,  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | 
                   GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
  
  // Set up columns
  //                                    Column 2
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
  //                                    Column 1  Column 3     Column 4     
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);
}