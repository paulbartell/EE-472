/****************************************** 
* task name: oledDisplay
* task inputs: void* pointer to a DisplayData struct (see tasks.h)
* task outputs: Text to OLED display
* task description: Takes strings from the displayData struct and outputs them
to an OLED display
* author: Ryan McDaniel
******************************************/ 
#include "FreeRTOS.h"
#include "task.h"

#include "tasks.h"
#include "schedule.h"
#include "drivers/rit128x96x4.h"
#define CLEAR "                       "
#define LINE 8
#define CNTRST 15
#define L_ALLIGN 0
#define R_ALLIGN 65
#define C_ALLIGN 40
#define MAX_LINES 12
#define NUM_OPTS 4

void clearOLED(int numLines);

typedef enum {SCANALL, TEMP, BLOOD, PULSE, EKG, BATT} measurement;
typedef enum {NORMAL, ANNUNCIATE} displayMode;


extern xTaskHandle taskList[];

// print out the current readings
void oledDisplay(void* taskDataPtr)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const portTickType xFrequency = 5000; // for 1Hz operation
  
  RIT128x96x4Init(10000000);
  RIT128x96x4DisplayOff();
  
  while(1)
  {
    
    // Grab all the data from the structure
    DisplayData* displayDataPtr = (DisplayData*) taskDataPtr;
    char* temperaturePtr = displayDataPtr->tempCorrectedBuf->headPtr;
    char* pulseRatePtr = displayDataPtr->prCorrectedBuf->headPtr;
    char* batteryPtr = displayDataPtr->battCorrected->headPtr;
    char* EKGPtr = displayDataPtr->ekgCorrectedBuf->headPtr; 
    char* systolicPressPtr = displayDataPtr->systolicPressCorrectedBuf->headPtr;
    char* diatolicPressPtr = displayDataPtr->diastolicPressCorrectedBuf->headPtr;
    unsigned short* myScroll = displayDataPtr->scroll;
    
    // Controlling display modes
    static displayMode lastMode = SCANALL;
    static measurement lastMeasure = NORMAL;
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
            
      // Print the Temperature, and the units around 35 units after
      RIT128x96x4StringDraw("Temp:", R_ALLIGN, LINE*2, CNTRST);
      RIT128x96x4StringDraw(temperaturePtr, R_ALLIGN, LINE*3, CNTRST);
      RIT128x96x4StringDraw("C", R_ALLIGN + 35, LINE*3, CNTRST); 
      
      // Print Pulse rate, and the units around 38 pixels after
      RIT128x96x4StringDraw(pulseRatePtr, L_ALLIGN, LINE*3, CNTRST);
      RIT128x96x4StringDraw("BPM", L_ALLIGN + 38, LINE*3, CNTRST);
      
      // Print EKG
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*5, CNTRST); 
      RIT128x96x4StringDraw("EKG:", L_ALLIGN, LINE*4, CNTRST);
      RIT128x96x4StringDraw(EKGPtr, L_ALLIGN, LINE*5, CNTRST);
      RIT128x96x4StringDraw("Hz", L_ALLIGN + 38, LINE*5, CNTRST);
      
      // Print the battery reading, centered a few lines further down
      RIT128x96x4StringDraw("Battery:", C_ALLIGN, LINE*9, CNTRST);
      
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*10, CNTRST); 
      
      // Print battery percent, units around 40 pixels after the reading
      RIT128x96x4StringDraw(batteryPtr, C_ALLIGN, LINE*10, CNTRST);
      RIT128x96x4StringDraw("%", C_ALLIGN + 40, LINE*10, CNTRST);
      
      // Directions for changing views
      RIT128x96x4StringDraw("'D' to change Display", L_ALLIGN, LINE*11, CNTRST);
      
      vTaskResume(taskList[TASK_COMMUNICATION]); // Resume communication task
    } 
    else if (currentMode == NORMAL) 
    {
      if (myMeasurement != lastMeasure) {
        clearOLED(MAX_LINES);
      }
      // Display the normal menu
      RIT128x96x4StringDraw("'9' to scroll", L_ALLIGN, LINE*0, CNTRST);
      RIT128x96x4StringDraw("'D' to change Display", L_ALLIGN, LINE*1, CNTRST);
      RIT128x96x4StringDraw("'0' Scan All", L_ALLIGN, LINE*2, CNTRST);
      RIT128x96x4StringDraw("1 Temperature", L_ALLIGN, LINE*3, CNTRST);
      RIT128x96x4StringDraw("2 Blood Pressure            ", L_ALLIGN, LINE*5, CNTRST);
      RIT128x96x4StringDraw("3 Pulse Rate", L_ALLIGN, LINE*7, CNTRST);
      RIT128x96x4StringDraw("4 EKG", L_ALLIGN, LINE*9, CNTRST);
      
      
      // Clear all measurement lines in case selected measurement changes
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*4, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*6, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*8, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*10, CNTRST);
      RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*11, CNTRST);
      
      RIT128x96x4StringDraw("5 Battery", L_ALLIGN, LINE*11, CNTRST);
      
      // Handle scrolling
      if (*(myScroll) == 1) 
      {
        *(myScroll) = 0;
      }
      
      // Display Temperature measurements
      if (myMeasurement == TEMP || myMeasurement == SCANALL)
      {
        RIT128x96x4StringDraw(temperaturePtr, L_ALLIGN, LINE*4, CNTRST-5);
        RIT128x96x4StringDraw("C", L_ALLIGN + 38, LINE*4, CNTRST-5); 
      }
      // Display Blood pressure measurements
      if (myMeasurement == BLOOD || myMeasurement == SCANALL)
      {
        RIT128x96x4StringDraw(CLEAR, L_ALLIGN, LINE*5, CNTRST);
        RIT128x96x4StringDraw("Systolic", L_ALLIGN, LINE*5, CNTRST);
        RIT128x96x4StringDraw("Diastolic", R_ALLIGN, LINE*5, CNTRST);
        RIT128x96x4StringDraw(systolicPressPtr, L_ALLIGN, LINE*6, CNTRST-5);
        RIT128x96x4StringDraw("mmHg", L_ALLIGN + 38, LINE*6, CNTRST-5);
        RIT128x96x4StringDraw(diatolicPressPtr, R_ALLIGN, LINE*6, CNTRST-5);
        RIT128x96x4StringDraw("mmHg", R_ALLIGN + 35, LINE*6, CNTRST-5); 
      }
      
      // Display Pulse measurements
      if (myMeasurement == PULSE || myMeasurement == SCANALL)
      {
        RIT128x96x4StringDraw(pulseRatePtr, L_ALLIGN, LINE*8, CNTRST-5);
        RIT128x96x4StringDraw("BPM", L_ALLIGN + 38, LINE*8, CNTRST-5); 
      }
      
      // Display Battery measurements
      if (myMeasurement == BATT || myMeasurement == SCANALL)
      {
        RIT128x96x4StringDraw(batteryPtr, R_ALLIGN, LINE*11, CNTRST-5);
        RIT128x96x4StringDraw("%", R_ALLIGN + 38, LINE*11, CNTRST-5); 
      }
      
      // Display EKG measurements
      if (myMeasurement == EKG || myMeasurement == SCANALL)
      {
        RIT128x96x4StringDraw(EKGPtr, L_ALLIGN, LINE*10, CNTRST-5);
        RIT128x96x4StringDraw("Hz", L_ALLIGN + 38, LINE*10, CNTRST-5); 
      }
      lastMeasure = myMeasurement;
    }
    lastMode = currentMode;
    *(displayDataPtr->measurementSelection) = (unsigned short) myMeasurement;
    
    //vTaskDelayUntil( &xLastWakeTime, xFrequency );
    vTaskSuspend(NULL);
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
