/****************************************** 
* task name: oledDisplay
* task inputs: void* pointer to a DisplayData struct (see tasks.h)
* task outputs: Text to OLED display
* task description: Takes strings from the displayData struct and outputs them
    to an OLED display
* author: Ryan McDaniel
******************************************/ 

#include "tasks.h"
#include "schedule.h"
#include "drivers/rit128x96x4.h"
#define CLEAR "                   "
#define LINE 8
#define CNTRST 15
#define L_ALLIGN 0
#define R_ALLIGN 65
#define C_ALLIGN 40

/*
1. Temperature: <temperature> C 
2. Systolic pressure: <systolic pressure> mm Hg 
3. Diastolic pressure: <diastolic pressure> mm Hg 
4. Pulse rate: <pulse rate> BPM 
5. Battery: <charge remaining>
*/

extern unsigned long globalTime;

void oledDisplaySetup()
{
  // Initialize the OLED Display
  RIT128x96x4Init(1000000);
}

// print out the current readings
void oledDisplay(void* taskDataPtr)
{
  
  if(globalTime % MAJORCYCLECOUNT == 0)
  {
    DisplayData* displayDataPtr = (DisplayData*) taskDataPtr;
    char* temperaturePtr = displayDataPtr->tempCorrectedBuf->headPtr;
    char* pulseRatePtr = displayDataPtr->prCorrectedBuf->headPtr;
    char* batteryPtr = displayDataPtr->battCorrected->headPtr;
    
    // Not right at the moment
    char* systolicPressPtr = displayDataPtr->bloodPressCorrectedBuf->headPtr;
    char* diatolicPressPtr = displayDataPtr->bloodPressCorrectedBuf->headPtr;
    
    // Print out each roughly like this
    //
    // Systolic Pressure:               Diastolic Pressure:
    //  12345   mm Hg                    12345 mm Hg
    // Pulse Rate:                      Temperature       
    //  12345 BPM                        12345 C    
    //                  Battery:
    //                   100%       
    //
    // Clear each reading with 20 spaces or so each time
    
    // Blood pressures
    RIT128x96x4StringDraw("Systolic", L_ALLIGN, LINE*0, CNTRST);
    RIT128x96x4StringDraw("Diastolic", R_ALLIGN, LINE*0, CNTRST);
    RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*1, CNTRST); 
    
    // Print Systolic pressure, and the units around 38 pixels after
    RIT128x96x4StringDraw(systolicPressPtr, L_ALLIGN, LINE*1, CNTRST);
    RIT128x96x4StringDraw("mmHg", L_ALLIGN + 38, LINE*1, CNTRST);
    
    // Print Diastolic pressure, and the units around 35 pixels after
    RIT128x96x4StringDraw(diatolicPressPtr, R_ALLIGN, LINE*1, CNTRST);
    RIT128x96x4StringDraw("mmHg", R_ALLIGN + 35, LINE*1, CNTRST); 
    
    // Print pulse and temperature
    RIT128x96x4StringDraw("Pulse:", L_ALLIGN, LINE*2, CNTRST);
    RIT128x96x4StringDraw("Temp:", R_ALLIGN, LINE*2, CNTRST);

    RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*3, CNTRST);  
    
    // Print Pulse rate, and the units around 38 pixels after
    RIT128x96x4StringDraw(pulseRatePtr, L_ALLIGN, LINE*3, CNTRST);
    RIT128x96x4StringDraw("BPM", L_ALLIGN + 38, LINE*3, CNTRST);

    // Print the Temperature, and the units around 35 units after
    RIT128x96x4StringDraw(temperaturePtr, R_ALLIGN, LINE*3, CNTRST);
    RIT128x96x4StringDraw("C", R_ALLIGN + 35, LINE*3, CNTRST); 
    
    // Print the battery reading, centered a few lines further down
    RIT128x96x4StringDraw("Battery:", C_ALLIGN, LINE*9, CNTRST);
  
    RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*10, CNTRST); 
    
    // Print battery percent, units around 40 pixels after the reading
    RIT128x96x4StringDraw(batteryPtr, C_ALLIGN, LINE*10, CNTRST);
    RIT128x96x4StringDraw("%", C_ALLIGN + 40, LINE*10, CNTRST);
  }
}
