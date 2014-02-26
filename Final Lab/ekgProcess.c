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

#include "inc/hw_memmap.h"

#include <stdio.h>
#include <string.h>

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
	while(1)
	{
		EKGData* ekgDataPtr = (EKGData*) taskDataPtr;
		
		// Returns index of peak frequency
		ekgDataPtr->EKGFreqBuf[] = optfft(*(ekgDataPtr->EKGRawBuf), *(ekgDataPtr->EKGTempBuf)) / 256 ;
		                                                                                                                                
		vTaskDelay(2000);
	}
}