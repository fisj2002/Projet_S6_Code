/*
 * fifo.c
 *
 * Created: 2018-10-26 19:21:19
 *  Author: dene2303
 */ 

#include "fifo.h"


/************************************************************************/
/* Private methods                                                      */
/************************************************************************/                                
char * NextAddr(volatile FifoBuffer_t* buffer, char * addr)
{
	++addr;
	
	if (addr >= buffer->startAddr + buffer->capacity)
		addr = buffer->startAddr;
		
	return addr;
}


/************************************************************************/
/* FiFo methods                                                         */
/************************************************************************/
FifoBuffer_t CreateFIFO(char * memoryLocation, size_t size)
{
	FifoBuffer_t structure =
	{
		.byteCount = 0,
		.capacity = size,
		.head = memoryLocation,
		.startAddr = memoryLocation,
		.tail = memoryLocation
	};
	
	return structure;
}

void ClearFIFO(volatile FifoBuffer_t* buffer)
{
	buffer->byteCount = 0;
	buffer->head = buffer->startAddr;
	buffer->tail = buffer->startAddr;
	
	/* Optionally reset data to zero */
}

/**
	Adds the char to the buffer
	returns false if the buffer is full
*/
bool AppendFIFO (volatile FifoBuffer_t* buffer, char newElement)
{
	if(buffer->byteCount >= buffer->capacity)
	return false;
	
	*(buffer->tail) = newElement;
	
	++ buffer->byteCount;
	buffer->tail = NextAddr(buffer, buffer->tail);
	
	return true;
}

/**
	Reads and element from the Fifo and puts it a the specified address
	Returns false if the fifo is empty
*/
bool ReadFIFO (volatile FifoBuffer_t* buffer, char * element)
{
	if (buffer->byteCount <= 0)
		return false;
		
	*element = *(buffer->head);
	
	-- buffer->byteCount;
	buffer->head = NextAddr(buffer, buffer->head);
	
	return true;
}

/** 
	Reads the next byte without removing it.
	Returns false if the fifo is empty.
*/
bool PeekFIFO (volatile FifoBuffer_t* buffer, char * element)
{
	if (buffer->byteCount <= 0)
		return false;
		
	*element = *(buffer->head);
	
	return true;
}
