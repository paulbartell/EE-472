/****************************************** 
* file name: circularbuffer.h
* file description: Contains structures and function protoypes for a circular
        buffer data type.
* author: Paul Bartell
******************************************/ 
typedef struct CircularBuffer
{
  char *buffPtr;
  char *headPtr;                // Pointer to the current head item
  char *tailPtr;                // Pointer to the current tail item
  unsigned int capacity;        // Capacity of the CB
  unsigned int itemSize;        // Sizeof each item
  unsigned int count;           // Number of items currently in CB
  unsigned int headIndex;       // Current index of head of CB
  unsigned int tailIndex;
} CircularBuffer;

void cBuffInit(CircularBuffer* cb, void* buffPtr, int capacity, int itemSize);
void cBuffPut(CircularBuffer* cb, void* item);
void* cBuffGetAt(CircularBuffer* cb, int index);
void* cBuffGetHead(CircularBuffer* cb);
void* cBuffGetTail(CircularBuffer* cb);
