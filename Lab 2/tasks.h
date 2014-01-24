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

void warningAlarm(vouid* taskData);