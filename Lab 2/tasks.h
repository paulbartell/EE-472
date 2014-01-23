typedef struct MeasureData
{
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
} MeasureData;

void measure(void* taskData);