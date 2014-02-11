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
#define CLEAR "                       "
#define LINE 8
#define CNTRST 15
#define L_ALLIGN 0
#define R_ALLIGN 65
#define C_ALLIGN 40
#define MAX_LINES 11

void clearOLED(int numLines);

typedef enum {BLOOD, TEMP, PULSE, BATT} measurement;
typedef enum {NORMAL, ANNUNCIATE} displayMode;

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
    // Grab all the data from the structure
    DisplayData* displayDataPtr = (DisplayData*) taskDataPtr;
    char* temperaturePtr = displayDataPtr->tempCorrectedBuf->headPtr;
    char* pulseRatePtr = displayDataPtr->prCorrectedBuf->headPtr;
    char* batteryPtr = displayDataPtr->battCorrected->headPtr;
    char* systolicPressPtr = displayDataPtr->bloodPressCorrectedBuf->headPtr;
    char* diatolicPressPtr = displayDataPtr->bloodPressCorrectedBuf->headPtr;
    
    // Controlling display modes
    static displayMode lastMode = NORMAL;
    measurement myMeasurement = (measurement) *(displayDataPtr->measurementSelection);
    displayMode currentMode = (displayMode) *(displayDataPtr->mode);
    
    // Clear everything if the mode changes
    if (currentMode != lastMode) {
      clearOLED(MAX_LINES);
    }
    if (currentMode == ANNUNCIATE) 
    {
      // Ignore scroll changes
      if (*(myScroll) == 1) *(myScroll) = 0;
      
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
        
      // Directions for changing views
      RIT128x96x4StringDraw("'D' to change Display", L_ALLIGN, LINE*11, CNTRST);
    } 
    else if (currentMode == NORMAL) 
    {

      // Display the normal menu
      RIT128x96x4StringDraw("0 Blood Pressure         ", L_ALLIGN, LINE*0, CNTRST);
      RIT128x96x4StringDraw("1 Temperature", L_ALLIGN, LINE*2, CNTRST);
      RIT128x96x4StringDraw("2 Pulse Rate", L_ALLIGN, LINE*4, CNTRST);
      RIT128x96x4StringDraw("3 Battery", L_ALLIGN, LINE*6, CNTRST);
      RIT128x96x4StringDraw("'D' to change Display", L_ALLIGN, LINE*9, CNTRST);
      RIT128x96x4StringDraw("'9' to scroll", L_ALLIGN, LINE*10, CNTRST);
    
      // Clear all measurement lines in case selected measurement changes
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*1, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*3, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*5, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*7, CNTRST);
      
      // Handle scrolling
      if (*(myScroll) == 1) 
      {
        myMeasurement = (measurement)(((int)myMeasurement + 1) % NUM_MEAS);
        *(myScroll) = 0;
      }
      
      // Display Blood pressure measurements
      if (myMeasurement == BLOOD)
      {
        RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*0, CNTRST);
        RIT128x96x4StringDraw("Systolic", L_ALLIGN, LINE*0, CNTRST);
        RIT128x96x4StringDraw("Diastolic", R_ALLIGN, LINE*0, CNTRST);
        RIT128x96x4StringDraw(systolicPressPtr, L_ALLIGN, LINE*1, CNTRST);
        RIT128x96x4StringDraw("mmHg", L_ALLIGN + 38, LINE*1, CNTRST);
        RIT128x96x4StringDraw(diatolicPressPtr, R_ALLIGN, LINE*1, CNTRST);
        RIT128x96x4StringDraw("mmHg", R_ALLIGN + 35, LINE*1, CNTRST); 
      }
      
      // Display Temperature measurements
      else if (myMeasurement == TEMP)
      {
        RIT128x96x4StringDraw(temperaturePtr, L_ALLIGN, LINE*3, CNTRST);
        RIT128x96x4StringDraw("C", L_ALLIGN + 38, LINE*3, CNTRST); 
      }

      // Display Pulse measurements
      else if (myMeasurement == PULSE)
      {
        RIT128x96x4StringDraw(pulseRatePtr, L_ALLIGN, LINE*5, CNTRST);
        RIT128x96x4StringDraw("BPM", L_ALLIGN + 38, LINE*5, CNTRST); 
      }

      // Display Battery measurements
      else if (myMeasurement == BATT)
      {
        RIT128x96x4StringDraw(batteryPtr, L_ALLIGN, LINE*7, CNTRST);
        RIT128x96x4StringDraw("%", L_ALLIGN + 38, LINE*7, CNTRST); 
      }
    }
    lastMode = currentMode;
    *(displayDataPtr->measurementSelection) = (unsigned short) myMeasurement;
  }
}

// Clears all the lines used by the program
void clearOLED(int numLines) 
{
  int i = 0;
  for (i = 0; i < numLines; i++)
  {
    RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*i, CNTRST);
  }
}
