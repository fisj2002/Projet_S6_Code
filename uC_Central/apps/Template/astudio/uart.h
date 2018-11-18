/*
 * Uart.h
 *
 * Created: 2018-09-27 13:12:16
 *  Author: dene2303
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdbool.h>
#include "config.h"
#include "fifo.h"
#include <avr/interrupt.h>

// Max number of characters
#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256
#define BAUD_RATE 9600

typedef struct{
	char ID;							// ID number of UART port
	char rxData[RX_BUFFER_SIZE];			// Data buffers used to track packets about to be transfered
	char txData[TX_BUFFER_SIZE];
	volatile FifoBuffer_t rxBuffer;			// Buffer parameters used to create queues
	volatile FifoBuffer_t txBuffer;
	bool isSending;					// Flag to keep track of whether characters are queued to be sent
	void (*iVector)(char);				// Interrupt vectors to be called when packet is received
} UART_System;

void InitUart_0(uint16_t baudrate);
void InitUart_1(uint16_t baudrate);

void SetRxInterruptUart(UART_System UART, void(*iVector)(char));

int SendUart(UART_System UART, const char * message);

int SendnUart(UART_System UART, const char * message, int length);

int StartTxUart(UART_System UART);

int GetRxCountUart(UART_System UART);

int ExtractRxUart(UART_System UART, char * destination, int length);

int FlushRxUart(UART_System UART);

#endif /* UART_H_ */