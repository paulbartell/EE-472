/****************************************** 
* task name: EKG Capture 
* task inputs: a void* pointer to a EKGData struct. 
* task outputs: EKG data in the EKGData struct 
* task description: Reads EKG signals simulated by
function generator
* author: Ryan McDaniel 
******************************************/

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "tasks.h"
#include "optfft.h"

#include "inc/hw_memmap.h"

#include <stdio.h>
#include <string.h>
#define Fs 7500
#define N 256

/*

Determine frequency from the index: 
f=fs *m_index /N

f: freq of the measured signal 
fs: sample freq 
m_index: index which corresponds to the maximum magnitude 
Mag^2 = real^2 +imag^2 
N: sequence length or N=2^m    

*/

void ekgProcess(void* taskDataPtr) {
  EKGData* ekgDataPtr = (EKGData*) taskDataPtr; 
  unsigned int freq = 0;
  
  
  while(1)
  {
    EKGData* ekgDataPtr = (EKGData*) taskDataPtr;
    
    // scale buffer input
    for(int i = 0; i < 256; i++)
    {
      ekgDataPtr->EKGRawTempBuf[i] = 0;
      ekgDataPtr->EKGRawBuf[i] = (ekgDataPtr->EKGRawBuf[i] >>  4) - 32;
    }
    freq = optfft(ekgDataPtr->EKGRawBuf,ekgDataPtr->EKGRawTempBuf);
    
    freq = Fs*freq / N;
    
    UARTprintf("Freq: %d\n",freq);
    
    cBuffPut(ekgDataPtr->EKGFreqBuf,&freq);
    // Returns index of peak frequency
    //ekgDataPtr->EKGFreqBuf[] = optfft(*(ekgDataPtr->EKGRawBuf), *(ekgDataPtr->EKGTempBuf)) / 256 ;
    
    vTaskSuspend(NULL);
  }
}