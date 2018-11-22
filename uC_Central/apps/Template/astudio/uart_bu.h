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

UART_System Get_UART0(void);
UART_System Get_UART1(void);

void InitUart_0(uint16_t baudrate);
void InitUart_1(uint16_t baudrate);

void SetRxInterruptUart0(void(*iVector)(char));

int SendUart0(const char * message);

int SendnUart0(const char * message, int length);

int StartTxUart0();

int GetRxCountUart0();

int ExtractRxUart0(char * destination, int length);

int FlushRxUart0();

void SetRxInterruptUart1(void(*iVector)(char));

int SendUart1(const char * message);

int SendnUart1(const char * message, int length);

int StartTxUart1();

int GetRxCountUart1();

int ExtractRxUart1(char * destination, int length);

int FlushRxUart1();

#endif /* UART_H_ */