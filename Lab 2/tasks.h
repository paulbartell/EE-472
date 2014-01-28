typedef struct MeasureData
{
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
} MeasureData;

void measure(void* taskData);

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
typedef struct DisplayData
{
  unsigned char* tempCorrected;
  unsigned char* sysCorrected;
  unsigned char* diasCorrected;
  unsigned char* prCorrected;
  unsigned char* battCorrected;
  unsigned short batteryState;
} DisplayData;
void oledDisplay(void* taskData);

typedef struct ComputeData
{
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned short batteryState;
  unsigned char* tempCorrected;
  unsigned char* sysCorrected;
  unsigned char* diasCorrected;
  unsigned char* prCorrected;
  unsigned char* battCorrected;
} ComputeData;
void compute(void* taskData);