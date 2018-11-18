/*
 * Uart.c
 *
 * Created: 2018-09-27 13:11:53
 *  Author: dene2303
 */ 

#include "uart.h"

// Flag to keep track of whether characters are queued to be sent
bool isSending0 = false;
bool isSending1 = false;

// Data buffers used to track packets about to be transfered
char rxData0[RX_BUFFER_SIZE];
char txData0[TX_BUFFER_SIZE];
char rxData1[RX_BUFFER_SIZE];
char txData1[TX_BUFFER_SIZE];

// Buffer parameters used to create queues
volatile FifoBuffer_t rxBuffer0;
volatile FifoBuffer_t txBuffer0;
volatile FifoBuffer_t rxBuffer1;
volatile FifoBuffer_t txBuffer1;

// Interrupt vectors to be called when packet is received
void (*iVector0)(char) = 0;
void (*iVector1)(char) = 0;


/************************************************************************/
/* Private prototypes                                                   */
/************************************************************************/
bool StartTxUart0();
bool StartTxUart1();


/************************************************************************/
/* Function declarations                                                */
/************************************************************************/


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
	Set a function to be executed everytime a new character is received
	The character is still added to the reception buffer.
	The character is added to the buffer before calling this interrupt vector
	Keep the functions short to avoid interrupt collision
	To disable the interrupt pass a 0 pointer
*/
void SetRxInterruptUart0 (void(*iVector)(char))
{
	iVector0 = iVector;
}
void SetRxInterruptUart1 (void(*iVector)(char))
{
	iVector1 = iVector;
}

/** 
	Sends a 0 ended String over the uart port using interrupts
	Returns the number of characters that were sent
*/
int SendUart0(const char * message)
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

int SendUart1(const char * message)
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
int SendnUart0(const char * message, int length)
{
	int bytesQueued = 0;
	
	while(bytesQueued < length && AppendFIFO(&txBuffer0, *message++))
		++ bytesQueued;
	
	StartTxUart0();
	
	return bytesQueued;
}

int SendnUart1(const char * message, int length)
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

/** 
	Retrieve the number of bytes in the reception buffer
*/
int GetRxCountUart0()
{
	return rxBuffer0.byteCount;
}

int GetRxCountUart1()
{
	return rxBuffer1.byteCount;
}

/** 
	Read  and consume Rx buffer.
	The specified number of bytes is copied to the destination.
	Returns the number of bytes copied
*/
int ExtractRxUart0(char * destination, int length)
{
	int byteCount = 0;
	
	while(byteCount++ < length && ReadFIFO(&rxBuffer0, destination++));
	
	return byteCount;
}

int ExtractRxUart1(char * destination, int length)
{
	int byteCount = 0;
	
	while(byteCount++ < length && ReadFIFO(&rxBuffer1, destination++));
	
	return byteCount;
}

/** 
	Resets the Rx Buffer
	Returns the number of characters removed
*/
int FlushRxUart0()
{
	int flushed = rxBuffer0.byteCount;
	
	ClearFIFO(&rxBuffer0);
	
	return flushed;
}
int FlushRxUart1()
{
	int flushed = rxBuffer1.byteCount;
	
	ClearFIFO(&rxBuffer1);
	
	return flushed;
}

// The byte reception interrupt subroutine
ISR(USART0_RX_vect) 
{
	char in = UDR0;
	
	AppendFIFO(&rxBuffer0, in);
	
	// Call user-defined interrupt vector if any
	if (iVector0)
		(*iVector0)(in);
}

ISR(USART1_RX_vect)
{
	char in = UDR1;
	
	AppendFIFO(&rxBuffer1, in);
	
	if (iVector1)
		(*iVector1)(in);
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