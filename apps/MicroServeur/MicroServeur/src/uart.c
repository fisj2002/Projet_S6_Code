/*
 * Uart.c
 *
 * Created: 2018-09-27 13:11:53
 *  Author: dene2303
 */ 

#include <avr/interrupt.h>
#include "uart.h"
#include "system.h"
#include "fifo.h"


// Max number of characters
#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256


bool isSending0 = false;
bool isSending1 = false;

char rxData0[RX_BUFFER_SIZE];
char txData0[TX_BUFFER_SIZE];
char rxData1[RX_BUFFER_SIZE];
char txData1[TX_BUFFER_SIZE];

volatile FifoBuffer_t rxBuffer0;
volatile FifoBuffer_t txBuffer0;
volatile FifoBuffer_t rxBuffer1;
volatile FifoBuffer_t txBuffer1;



void InitUart_0 (uint16_t baudrate)
{
	// Set baudrate for asynchronous double speed mode
	uint16_t baudRateSetting = (uint16_t) (CPU_CLK_HZ / 8 / baudrate) - 1;
	UBRR0H = (baudRateSetting >> 8) && 0xff;
	UBRR0L = baudRateSetting & 0xff;

	// Set byte format
	UCSR0A |= 1 << U2X0;	// Double speed mode : disabled
	UCSR0A &= ~(1 << MPCM0);	// Multi processor mode : disabled
	
	UCSR0B &= ~(1 << UCSZ02);	// 9 bit mode: disabled
	
	UCSR0C &= ~(0b11 << UMSEL00);	// asynchronous mode
	UCSR0C &= ~(0b11 << UPM00);		// No parity
	UCSR0C &= ~(1 << USBS0);		// 1 Stop bit
	UCSR0C |= 3 << UCSZ00;		// 8 bit word size
	
	// Setting up buffers
	rxBuffer0 = CreateFIFO(rxData0, RX_BUFFER_SIZE);
	txBuffer0 = CreateFIFO(txData0, TX_BUFFER_SIZE);
	
	// Enable interrupts
	UCSR0B |= 1 << RXCIE0;	// Receive complete interrupt
	sei();					// Global interrupt enable
	
	// Enable receiver and transmitter
	UCSR0B |= 1 << RXEN0;	// Enable receiver
	UCSR0B |= 1 << TXEN0;	// Enable transmitter
}

void InitUart_1 (uint16_t baudrate)
{
	// Set baudrate for asynchronous double speed mode
	uint16_t baudRateSetting = (uint16_t) (CPU_CLK_HZ / 8 / baudrate) - 1;
	UBRR1H = (baudRateSetting >> 8) && 0xff;
	UBRR1L = baudRateSetting & 0xff;
		
	// Set byte format
	UCSR1A |= 1 << U2X1;	// Double speed mode : enabled
	UCSR1A &= ~(1 << MPCM1);	// Multi processor mode : disabled
	
	UCSR1B &= ~(1 << UCSZ12);	// 9 bit mode: disabled
	
	UCSR1C &= ~(0b11 << UMSEL10);	// asynchronous mode
	UCSR1C &= ~(0b11 << UPM10);		// No parity
	UCSR1C &= ~(1 << USBS1);		// 1 Stop bit
	UCSR1C |= 3 << UCSZ10;		// 8 bit word size
	
	// Setting up buffers
	rxBuffer1 = CreateFIFO(rxData1, RX_BUFFER_SIZE);
	txBuffer1 = CreateFIFO(txData1, TX_BUFFER_SIZE);	
	
	// Enable interrupts
	UCSR1B |= 1 << RXCIE1;	// Receive complete interrupt
	sei();					// Global interrupt enable
	
	// Enable receiver and transmitter
	UCSR1B |= 1 << RXEN1;	// Enable receiver
	UCSR1B |= 1 << TXEN1;	// Enable transmitter
}

/** 
	Sends a 0 ended String over the uart port using interrupts
	Returns the number of characters that were sent
*/
int SendUart0(char * message)
{
	int bytesQueued = 0;
	
	while(AppendFIFO(&txBuffer0, *message))
	{
		++ bytesQueued;
		
		if(*message == 0)
			break;
			
		++message;
	}
	
	StartTxUart0();
	
	return bytesQueued;
}

int SendUart1(char * message)
{
	int bytesQueued = 0;
	
	while(AppendFIFO(&txBuffer1, *message))
	{
		++ bytesQueued;
		
		if(*message == 0)
			break;
		
		++message;
	}
	
	StartTxUart1();
	
	return bytesQueued;
}

/** 
	Sends an array of characters of fixed length.
	Returns the numbers of characters queued
*/
int SendnUart0(char * message, int length)
{
	int bytesQueued = 0;
	
	while(bytesQueued < length && AppendFIFO(&txBuffer0, *message++))
		++ bytesQueued;
	
	StartTxUart0();
	
	return bytesQueued;
}

int SendnUart1(char * message, int length)
{
	int bytesQueued = 0;
	
	while(bytesQueued < length && AppendFIFO(&txBuffer1, *message++))
		++ bytesQueued;
	
	StartTxUart1();
	
	return bytesQueued;
}


/** 
	Start sending bytes if the none were being sent.
	Returns false if uart is already sending or the buffer is empty.
*/
bool StartTxUart0()
{
	if (isSending0)
		return false;
		
	char charToSend;
		
	if (!ReadFIFO(&txBuffer0, &charToSend))
		return false;
		
	isSending0 = true;	
	while ( !( UCSR0A & (1 << UDRE0)) );
	UDR0 = charToSend;
	
	UCSR0B |= 1 << UDRIE0; // enable UDR empty interrupt to keep sending the next ones

	return true;
}

bool StartTxUart1()
{
	if (isSending1)
	return false;
	
	char charToSend;
	
	if (!ReadFIFO(&txBuffer1, &charToSend))
	return false;
	
	isSending1 = true;
	while ( !( UCSR1A & (1 << UDRE1)) );
	UDR1 = charToSend;
	
	UCSR1B |= 1 << UDRIE1; // enable UDR empty interrupt to keep sending the next ones

	return true;
}


// The byte reception interrupt subroutine
ISR(USART0_RX_vect) 
{
	char in = UDR0;
	
	AppendFIFO(&rxBuffer0, in);
}

ISR(USART1_RX_vect)
{
	char in = UDR1;
	
	AppendFIFO(&rxBuffer1, in);
}

// Keep sending bytes after until the buffer is empty 
ISR(USART0_UDRE_vect)
{
	char nextChar;
	
	if(ReadFIFO(&txBuffer0, &nextChar))
		UDR0 = nextChar;
	else
	{
		isSending0 = false;
		UCSR0B &= ~(1 << UDRIE0); //disable UDR empty interrupt
	}	
}

ISR(USART1_UDRE_vect)
{
	char nextChar;
	
	if(ReadFIFO(&txBuffer1, &nextChar))
		UDR1 = nextChar;
	else
	{
		isSending1 = false;
		UCSR1B &= ~(1 << UDRIE1); //disable UDR empty interrupt
	}
}