/****************************************** 
* File name: tasks.h
* File description: Defines task data structures and task function prototypes.
* author: Paul Bartell
******************************************/ 
#include "circularbuffer.h"

enum _myBool { FALSE = 0, TRUE = 1 };
 typedef enum _myBool Bool;

#define MEASURE_TASK 0
typedef struct MeasureData
{
  CircularBuffer* temperatureRawBuf;
  CircularBuffer* bloodPressRawBuf;
  CircularBuffer* pulseRateRawBuf;
  unsigned short* measurementSelection;
} MeasureData;

void measure(void* taskData);

typedef struct ComputeData
{
  CircularBuffer* temperatureRawBuf;
  CircularBuffer* bloodPressRawBuf;
  CircularBuffer* pulseRateRawBuf;

  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* bloodPressCorrectedBuf;
  CircularBuffer* prCorrectedBuf;
  CircularBuffer* battCorrected;
  
  unsigned short* batteryState;
} ComputeData;

void compute(void* taskData);

typedef struct DisplayData
{
  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* bloodPressCorrectedBuf;
  CircularBuffer* prCorrectedBuf;
  CircularBuffer* battCorrected;
  
  unsigned short* measurementSelection;
  unsigned short* mode;
  unsigned short* scroll;
  
} DisplayData;


void oledDisplay(void* taskData);
void oledDisplaySetup();

typedef struct StatusData
{
  unsigned short* batteryState;
} StatusData;

void status(void* taskData);

typedef struct WarningAlarmData
{
  CircularBuffer* temperatureRawBuf;
  CircularBuffer* bloodPressRawBuf;
  CircularBuffer* pulseRateRawBuf;
  unsigned short* batteryState;
} WarningAlarmData;

void warningAlarm(void* taskData);
void warningAlarmSetup();

typedef struct KeypadData
{
  unsigned short* mode;
  unsigned short* scroll;
  unsigned short* measurementSelection;
  unsigned short* alarmAcknowledge;
  unsigned short* select;
} KeypadData;

void keypad(void* taskData);

typedef struct CommunicationsData
{
  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* bloodPressCorrectedBuf;
  CircularBuffer* prCorrectedBuf;
} CommunicationsData;

void communications(void* taskData);