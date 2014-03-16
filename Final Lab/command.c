#include <stdio.h>
#include <string.h>

#include "schedule.h"
#include "tasks.h"

void command (void* taskDataPtr) {
	 CommandData* commandDataPtr = (CommandData*) taskDataPtr;
	 unsigned short i == 1;
	 static short display = 1;
	 
	 while(1)
	 {
		switch((commandDataPtr->recieve))
		{
		// indicates start mode
		case 'S':
			if(strlen(commandDataPtr->recieve) == 1)
			{
				commandDataPtr->transmit = "START";
			}
			else
			{
				commandDataPtr->transmit = "Unrecognized command.";
			}
			break;
		// indicates stop mode
		case 'P':
			if(strlen(commandDataPtr->recieve) == 1)
			{
				commandDataPtr->transmit = "STOP";
			}
			else
			{
				commandDataPtr->transmit = "Unrecognized command.";
			}
			break;
		// enables or disables OLED Display
		case 'D':
			if(strlen(commandDataPtr->recieve) == 1)
			{
				display = !display;
				// Turn on display
				if(display)
				{
					RIT128x96x4DisplayOn();
				// Turn off display
				}
				else
				{
					RIT128x96x4DisplayOff();
				}
			}
			else
			{
				commandDataPtr->transmit = "Unrecognized command.";
			}
			break;
		// requests the most recent values of the specified data
		case 'M':
			for(i = 0; i < 4; i++) {
				if (strlen(transmit) > 0) {
					strcat(commandDataPtr->transmit, "Bam");
				} else {
					commandDataPtr->transmit = "Bam";
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
		case default:
			commandDataPtr->transmit = "Unrecognized command.";
	 }
	 
}