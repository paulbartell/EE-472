#include "tasks.h"
#include "drivers/rit128x96x4.h"

/*
1. Temperature: <temperature> C 
2. Systolic pressure: <systolic pressure> mm Hg 
3. Diastolic pressure: <diastolic pressure> mm Hg 
4. Pulse rate: <pulse rate> BPM 
5. Battery: <charge remaining>
*/


// Precondition; OLED initialized
void oledDisplay(void* taskDataPtr)
{
  // Initialize the OLED Display
  RIT128x96x4Init(1000000);
  
  DisplayData* displayDataPtr = (DisplayData*) taskDataPtr;
  
  RIT128x96x4StringDraw("Temperature:", 0, 0, 15);
  RIT128x96x4StringDraw(displayDataPtr->tempCorrected, 10, 8, 15); 
  
  RIT128x96x4StringDraw("Systolic pressure:", 0, 16, 15); 
  RIT128x96x4StringDraw(displayDataPtr->sysCorrected, 10, 24, 15); 
  
  RIT128x96x4StringDraw("Diastolic pressure:", 0, 32, 15);
  RIT128x96x4StringDraw(displayDataPtr->diasCorrected, 10, 40, 15);
  
  RIT128x96x4StringDraw("Pulse rate:", 0, 48, 15);  
  RIT128x96x4StringDraw(displayDataPtr->prCorrected, 10, 56, 15); 
  
  RIT128x96x4StringDraw("Battery:", 0, 64, 15); 
  RIT128x96x4StringDraw(displayDataPtr->battCorrected, 10, 72, 15);
}
