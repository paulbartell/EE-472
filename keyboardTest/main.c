#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include <stdio.h>

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
    RIT128x96x4Init(1000000);
}

void main(){
    int inputs[4][4];
    char number[10];
    int i = 0;
    int j = 0;
    keypadSetUp();
    
    while(1) {
      // Columm 1
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 0);
      inputs[0][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
      inputs[0][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
      inputs[0][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
      inputs[0][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_4), 255);    
      
      // Column 2
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 0);
      inputs[1][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
      inputs[1][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
      inputs[1][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
      inputs[1][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3), 255);
      
      // Column 3
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 0);
      inputs[2][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
      inputs[2][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
      inputs[2][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
      inputs[2][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6), 255);
      
      // Column 4
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 0);
      inputs[3][0] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
      inputs[3][1] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5);
      inputs[3][2] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
      inputs[3][3] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
      GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_7), 255);
      
      
      // Display to the OLED Display
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          RIT128x96x4StringDraw("   ", 20*i, 20*j, 15);          
          sprintf(number, "%d", inputs[i][j]);
          RIT128x96x4StringDraw(number, 20*i, 20*j, 15);
        }
      }
    }
}