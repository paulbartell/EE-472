/****************************************** 
* File name: tasks.h
* File description: Defines task data structures and task function prototypes.
* author: Paul Bartell
******************************************/ 

enum _myBool { FALSE = 0, TRUE = 1 };
 typedef enum _myBool Bool;

typedef struct MeasureData
{
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
} MeasureData;

void measure(void* taskData);

typedef struct ComputeData
{
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned short* batteryState;
  unsigned char* tempCorrected;
  unsigned char* sysCorrected;
  unsigned char* diasCorrected;
  unsigned char* prCorrected;
  unsigned char* battCorrected;
} ComputeData;
void compute(void* taskData);

typedef struct DisplayData
{
  unsigned char* tempCorrected;
  unsigned char* sysCorrected;
  unsigned char* diasCorrected;
  unsigned char* prCorrected;
  unsigned char* battCorrected;
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
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned short* batteryState;
} WarningAlarmData;

void warningAlarm(void* taskData);
void warningAlarmSetup();





