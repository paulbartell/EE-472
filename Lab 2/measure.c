#include "tasks.h"
#define INTSYS 80
#define INTDIAS 80

void measure(void* taskDataPtr)
{
    // Variables to be measured
    int temp;
    int syst;
    int dias;
    int pulse;

    // Variables to be initialized once
    // Change during every method call
    static int diaComplete = 0;
    static int sysComplete = 0;

    static int revTemp = 0;
    static int revPulse = 0;
    static int even = 1;

    MeasureData* measureDataPtr = (MeasureData*) taskDataPtr;

	// Measure Temperature
    temp = *(measureDataPtr->temperatureRaw);
    if (revTemp == 0) {
        if (temp > 50) {
            revTemp = 1;
        } else if (even) {
            temp += 2;
        } else {
            temp--;
        }
    }
    if (revTemp) {
        if (temp > 50) {
            revTemp = 0;
        } else if (even) {
            temp -= 2;
        } else {
            temp++;
        }
    }
    *(measureDataPtr->temperatureRaw)=temp;

	// Measure Systolic
    syst = *(measureDataPtr->systolicPressRaw);
    if (syst > 100){
        sysComplete = 1;
         // Set complete to true
         // Reset systolicPressRaw to initial value
         if (diaComplete == 1){
            sysComplete = 0;
			syst = INTSYS;
         }
    } else {
        if (even) {
            syst += 3;
        } else {
            syst--;
        }
        sysComplete = 0;
    }
    *(measureDataPtr->systolicPressRaw)=syst;
	// Measure Diastolic
    dias = *(measureDataPtr->diastolicPressRaw);
    if (dias < 40){
        diaComplete = 1;
        // Set diaComplete to 0
        // Reset diastolicPressRaw to initial value
        if (sysComplete = 1) {
            diaComplete = 0;
			dias = INTDIAS;
        }
    } else {
        if (even){
            dias -= 2;
        } else {
            dias++;
        }
        diaComplete = 0;
    }
    *(measureDataPtr->diastolicPressRaw)=dias;

    // Measure Pulse Rate
    pulse = *(measureDataPtr->pulseRateRaw);
	if (revPulse) {
		if (pulse > 40) {
			revPulse = 1;
		} else if (even) {
			pulse--;
		} else {
			pulse += 3;
		}
	} else {
		if(pulse < 15){
			revPulse = 0;
		} else if (even) {
			pulse++;
		} else {
			pulse -= 3;
		}
	}
	*(measureDataPtr->pulseRateRaw) = pulse;

	even = !even;
}
