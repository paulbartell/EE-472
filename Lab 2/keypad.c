/****************************************** 
* task name: Keypad
* task inputs: a void* pointer to a Keypad struct
* task outputs: mode, scroll, measurementSelection,
*				alarm acknowledge
* task description: Helps user navigate menus on
* device to display data on OLED using 4x4 keypad
* author: Alyanna Castillo
******************************************/ 

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "tasks.h"
#include "schedule.h"

#define BLOOD 0
#define TEMP 1
#define PULSE 2
#define BATT 3

void keypad(void* taskDataPtr)
{
	KeypadData* keypadDataPtr = (KeypadData*) taskDataPtr;  
	int inputs[4][4];
	
	// Row 1
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 0);
	inputs[0][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
	inputs[0][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
	inputs[0][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
	inputs[0][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 255);    

	// Row 2
	// GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 0);
	// inputs[1][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
	// inputs[1][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
	// inputs[1][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
	// inputs[1][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
	// GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 255);

	// Row 3
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 0);
	// inputs[2][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
	inputs[2][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
	// inputs[2][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
	// inputs[2][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 255);

	// Row 4
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 0);
	// inputs[3][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
	inputs[3][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
	// inputs[3][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
	inputs[3][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 255);

	// Measurement Selection
	// Press 0
	if(inputs[0][0] == 0) 
	{
		(*(keypadDataPtr->measurementSelection)) = BLOOD;
	// Press 1
	} 
	else if (inputs[0][1] == 0) 
	{
		(*(keypadDataPtr->measurementSelection)) = TEMP;
	// Press 2
	} 
	else if (inputs[0][2] == 0) 
	{
		(*(keypadDataPtr->measurementSelection)) = PULSE;
	// Press 3
	} 
	else if (inputs[0][3])
	{
		(*(keypadDataPtr->measurementSelection)) = BATT;
	}

	// Mode: Toggle Switch
	// Press D
	if(inputs[3][1] == 0) 
	{
		*(keypadDataPtr->mode) = !(*(keypadDataPtr->mode));
	}
	
	// Scroll button
	// Press 9
	if(inputs[2][1] == 0) 
	{
		*(keypadDataPtr->scroll) = 1;
	}

	// Acknowledge
	// Press F
	if(inputs[3][3] == 0)
	{
		(*(keypadDataPtr->alarmAcknowledge)) = 1;
	}
}

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