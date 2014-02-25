/****************************************** 
* File name: tasks.h
* File description: Defines task data structures and task function prototypes.
* author: Paul Bartell
******************************************/ 
#include "circularbuffer.h"

#ifndef NULL
#define NULL (void *)0
#endif

enum _myBool { FALSE = 0, TRUE = 1 };
 typedef enum _myBool Bool;

#define TASK_MEASURE 0
#define TASK_COMPUTE 1
#define TASK_DISPLAY 2
#define TASK_WARNINGALARM 3
#define TASK_STATUS 4
#define TASK_KEYPAD 5
#define TASK_COMMUNICATION 6

typedef struct MeasureData
{
  CircularBuffer* temperatureRawBuf;
  CircularBuffer* systolicPressRawBuf;
  CircularBuffer* diastolicPressRawBuf;
  CircularBuffer* pulseRateRawBuf;
  unsigned short* measurementSelection;
} MeasureData;

void measure(void* taskData);


/*
typedef struct EKGData
{
	unsigned int* EKGRawBuf;
	unsigned int* EKGRawTempBuf;
	unsigned int* EKGFreqBuf;
} EKGData;

void EKGCapture (void* taskData);
void EKGProcess (void* taskData);
*/

typedef struct ComputeData
{
  CircularBuffer* temperatureRawBuf;
  CircularBuffer* systolicPressRawBuf;
  CircularBuffer* diastolicPressRawBuf;
  CircularBuffer* pulseRateRawBuf;

  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* systolicPressCorrectedBuf;
  CircularBuffer* diastolicPressCorrectedBuf;
  CircularBuffer* prCorrectedBuf;
  CircularBuffer* battCorrected;
  
  unsigned short* batteryState;
  unsigned short* measurementSelection;
} ComputeData;

void compute(void* taskData);

typedef struct DisplayData
{
  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* systolicPressCorrectedBuf;
  CircularBuffer* diastolicPressCorrectedBuf;
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
  CircularBuffer* systolicPressRawBuf;
  CircularBuffer* diastolicPressRawBuf;
  CircularBuffer* pulseRateRawBuf;
  unsigned short* batteryState;
  unsigned short* alarmAcknowledge;
} WarningAlarmData;

void warningAlarm(void* taskData);
void warningAlarmSetup();

typedef struct KeypadData
{
  unsigned short* mode;
  unsigned short* scroll;
  unsigned short* measurementSelection;
  unsigned short* alarmAcknowledge;
} KeypadData;

void keypad(void* taskData);
void keypadSetUp();

typedef struct CommunicationsData
{
  CircularBuffer* tempCorrectedBuf;
  CircularBuffer* systolicPressCorrectedBuf;
  CircularBuffer* diastolicPressCorrectedBuf;
  CircularBuffer* prCorrectedBuf;
  CircularBuffer* battCorrected;
  
  unsigned short measurementSelection;
} CommunicationsData;

void communication(void* taskData);