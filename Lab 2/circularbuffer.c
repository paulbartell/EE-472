/****************************************** 
* file name: circularbuffer.c
* file description: Implements a circular buffer for storing measurements, when
      given a properly sized data array.
* author: Paul Bartell
******************************************/ 
#include "circularbuffer.h"
#include "string.h"

/**
*  Function cBuffInit initializes a CircularBuffer struct
*  @param cb       A pointer to the circular buffer struct
*  @param buffPtr  A pointer to the array to store data in
*  @param capacity The maximum number of items in the circular buffer.
*  @param itemSize The sizeof() each item in the buffer.
*/
void cBuffInit(CircularBuffer* cb, void* buffPtr, int capacity, int itemSize)
{
  cb->buffPtr = buffPtr;
  cb->capacity = capacity;
  cb->itemSize = itemSize;
  
  cb->headIndex = 0;
  cb->tailIndex = cb->capacity;
  cb->headPtr = cb->buffPtr;
  cb->tailPtr = cb->buffPtr + (cb->itemSize * cb->capacity);
}

/**
*  Function cBuffPut adds an item to the head of a CircularBuffer
*  @param cb       A pointer to the circular buffer struct
*  @param item     A pointer to the item to add to the buffer
*/
void cBuffPut(CircularBuffer* cb, void* item)   //Add Item to the front (head)
{
  // Adjust head index
  if(cb->headIndex == cb->capacity)
  {
    cb->headIndex = 0;
  }
  else
  {
    cb->headIndex++;
  }
  cb->headPtr = cb->buffPtr + (cb->itemSize * cb->headIndex);
  
  // Adjust tail index
  if(cb->tailIndex == cb->capacity)
  {
    cb->tailIndex = 0;
  }
  else
  {
    cb->tailIndex++;
  }
  cb->tailPtr = cb->buffPtr + (cb->itemSize * cb->tailIndex);
  
  memcpy(cb->headPtr, item, cb->itemSize);                      // copy the item
}

/**
*  Function cBuffGetAt returns a pointer to the item at the given index
*  @param cb       A pointer to the circular buffer struct
*  @param item     The item's index in the circular buffer
*        (an index of 0 returns the head, most recently added item)
*/
void* cBuffGetAt(CircularBuffer* cb, int index)
{
  return cb->buffPtr + cb->itemSize * ((index + cb->headIndex)%cb->capacity);
}

/**
*  Function cBuffGetHead returns a pointer to the head item in a circular buffer
*  @param cb       A pointer to the circular buffer struct
*/
void* cBuffGetHead(CircularBuffer* cb)
{
  return cb->headPtr;
}

/**
*  Function cBuffGetTail returns a pointer to the tail item in a circular buffer
*  @param cb       A pointer to the circular buffer struct
*/
void* cBuffGetTail(CircularBuffer* cb)
{
  return cb->tailPtr;
}