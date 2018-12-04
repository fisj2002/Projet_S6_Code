/*
 * fifo.h
 *
 * Created: 2018-10-26 19:21:31
 *  Author: dene2303
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include <stdbool.h>
#include <stdio.h>

// generic fifo struct
typedef struct
{
	char * startAddr;
	char * head;
	char * tail;
	unsigned byteCount;
	unsigned capacity;

} FifoBuffer_t;


FifoBuffer_t CreateFIFO(const char * memoryLocation, size_t size);

void ClearFIFO(volatile FifoBuffer_t* buffer);

bool AppendFIFO (volatile FifoBuffer_t* buffer, char newElement);

bool ReadFIFO (volatile FifoBuffer_t* buffer, char * element);

bool PeekFIFO (const volatile FifoBuffer_t* buffer, char * element);


#endif /* FIFO_H_ */