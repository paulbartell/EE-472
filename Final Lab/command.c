#include <stdio.h>
#include <string.h>

#include "schedule.h"
#include "tasks.h"
#include "semphr.h"
#include "rit128x96x4.h"

void command (void* taskDataPtr) {
  CommandData* commandDataPtr = (CommandData*) taskDataPtr;
  unsigned short i = 1;
  extern xTaskHandle taskList[];
  static short display = 1;
  static short started = 0;
  vSemaphoreCreateBinary(*(commandDataPtr->commandSemaphorePtr));
  xSemaphoreGive(*(commandDataPtr->commandSemaphorePtr));
  vTaskSuspend(NULL);
  
  while(1)
  {
    
    xSemaphoreTake(*(commandDataPtr->commandSemaphorePtr),300);
    
    strcpy(commandDataPtr->transmit, "");
    switch((commandDataPtr->recieve))
    {
      // indicates start mode
    case 'S':
      if (!started) {
        strcpy(commandDataPtr->transmit, "System Started");
        vTaskResume(taskList[TASK_MEASURE]);
        vTaskResume(taskList[TASK_COMPUTE]);
        vTaskResume(taskList[TASK_WARNINGALARM]);
        vTaskResume(taskList[TASK_STATUS]);
        vTaskResume(taskList[TASK_KEYPAD]);
        vTaskResume(taskList[TASK_COMMUNICATION]);
        vTaskResume(taskList[TASK_EKGCAPTURE]);
        vTaskResume(taskList[TASK_EKGPROCESS]);
        RIT128x96x4DisplayOn();
        started = 1;
      } else {
        strcpy(commandDataPtr->transmit, "System already started!");
      }
      break;
      // indicates stop mode
    case 'P':
      strcpy(commandDataPtr->transmit, "System Stopped");
      vTaskSuspend(taskList[TASK_MEASURE]);
      vTaskSuspend(taskList[TASK_COMPUTE]);
      vTaskSuspend(taskList[TASK_WARNINGALARM]);
      vTaskSuspend(taskList[TASK_STATUS]);
      vTaskSuspend(taskList[TASK_KEYPAD]);
      vTaskSuspend(taskList[TASK_COMMUNICATION]);
      vTaskSuspend(taskList[TASK_EKGCAPTURE]);
      vTaskSuspend(taskList[TASK_EKGPROCESS]);
      started = 0;
      break;
      // enables or disables OLED Display
    case 'D':
      display = !display;
      // Turn on display
      if(display)
      {
        RIT128x96x4DisplayOn();
        strcpy(commandDataPtr->transmit, "Display On");
        // Turn off display
      }
      else
      {
        RIT128x96x4DisplayOff();
        strcpy(commandDataPtr->transmit, "Display Off");
      }
      break;
      // requests the most recent values of the specified data
    case 'M':
      for(i = 0; i < 4; i++) {
        if (commandDataPtr->measure[i])  {
          switch (i) {
          case 0:
            strcat(commandDataPtr->transmit, "Temperature: ");
            strcat(commandDataPtr->transmit, commandDataPtr->displayData->tempCorrectedBuf->headPtr);
            strcat(commandDataPtr->transmit, " C");
            strcat(commandDataPtr->transmit, "<br />");
            break;
          case 1:
            strcat(commandDataPtr->transmit, "Systolic ");
            strcat(commandDataPtr->transmit, commandDataPtr->displayData->systolicPressCorrectedBuf->headPtr);
            strcat(commandDataPtr->transmit, " mmHg");
            strcat(commandDataPtr->transmit, "<br />");
            strcat(commandDataPtr->transmit, "Diastolic: ");
            strcat(commandDataPtr->transmit, commandDataPtr->displayData->diastolicPressCorrectedBuf->headPtr);
            strcat(commandDataPtr->transmit, " mmHg");
            strcat(commandDataPtr->transmit, "<br />");
            break;
          case 2:
            strcat(commandDataPtr->transmit, "Pulse Rate: ");
            strcat(commandDataPtr->transmit, commandDataPtr->displayData->prCorrectedBuf->headPtr);
            strcat(commandDataPtr->transmit, " BPM");
            strcat(commandDataPtr->transmit, "<br />");
            break;
          case 3:
            strcat(commandDataPtr->transmit, "EKG: ");
            strcat(commandDataPtr->transmit, commandDataPtr->displayData->ekgCorrectedBuf->headPtr);
            strcat(commandDataPtr->transmit, " Hz");
            strcat(commandDataPtr->transmit, "<br />");
            break;
          }
          
        }
      }
      if(strlen(commandDataPtr->transmit) == 0)
      {
        strcpy(commandDataPtr->transmit, " ");
      }
      break;
      // acknowledges the receipt of the identified command
    case 'A':
      strcpy(commandDataPtr->transmit, "acknowledged");
      break;
      //  error response is given for incorrect commands or non-existent commands
    case 'E':
      strcpy(commandDataPtr->transmit, "Error: Incorrect Usage, police on standby...");
      break;
    default:
      strcpy(commandDataPtr->transmit, " ");
    }
    xSemaphoreGive(*(commandDataPtr->commandSemaphorePtr));
    vTaskSuspend(NULL);
  }
  
}