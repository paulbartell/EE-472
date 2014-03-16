#include <stdio.h>
#include <string.h>

#include "schedule.h"
#include "tasks.h"
#include "rit128x96x4.h"

void command (void* taskDataPtr) {
  CommandData* commandDataPtr = (CommandData*) taskDataPtr;
  unsigned short i = 1;
  static short display = 1;
  
  while(1)
  {
    commandDataPtr->transmit = "";
    switch((commandDataPtr->recieve))
    {
      // indicates start mode
    case 'S':
      commandDataPtr->transmit = "System Started";
      break;
      // indicates stop mode
    case 'P':
      commandDataPtr->transmit = "System Stopped";
      break;
      // enables or disables OLED Display
    case 'D':
      display = !display;
      // Turn on display
      if(display)
      {
        RIT128x96x4DisplayOn();
        commandDataPtr->transmit = "Display On";
        // Turn off display
      }
      else
      {
        RIT128x96x4DisplayOff();
        commandDataPtr->transmit = "Display Off";
      }
      break;
      // requests the most recent values of the specified data
    case 'M':
      for(i = 0; i < 4; i++) {
        if (commandDataPtr->measure[i])  {
          if (strlen(commandDataPtr->transmit) > 0) {
            strcat(commandDataPtr->transmit, "Bam");
          } else {
            commandDataPtr->transmit = "Bam";
          }
        }
      }
      break;
      // acknowledges the receipt of the identified command
    case 'A':
      commandDataPtr->transmit = "acknowledged";
      break;
      //  error response is given for incorrect commands or non-existent commands
    case 'E':
      commandDataPtr->transmit = "Error";
      break;
    default:
      commandDataPtr->transmit = " ";
    }
    vTaskSuspend(NULL);
  }
  
}