/*
 * Uart.c
 *
 * Created: 2018-09-27 13:11:53
 *  Author: dene2303
 */ 

#include "uart.h"

/************************************************************************/
/* Private prototypes                                                   */
/************************************************************************/

UART_System UART0;
UART_System UART1;

/************************************************************************/
/* Function declarations                                                */
/************************************************************************/

UART_System* GetUART0(void)
{
	return &UART0;
}

UART_System* GetUART1(void)
{
	return &UART1;
}

void InitUart_0 (uint16_t baudrate)
{
	// Setting UART ID
	UART0.ID = 0;
	
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
	UART0.rxBuffer = CreateFIFO(UART0.rxData, RX_BUFFER_SIZE);
	UART0.txBuffer = CreateFIFO(UART0.txData, TX_BUFFER_SIZE);
	
	//Initializing default value
	UART0.isSending = false;
	UART0.iVector = 0;
	
	// Enable interrupts
	UCSR0B |= 1 << RXCIE0;	// Receive complete interrupt
	sei();					// Global interrupt enable
	
	// Enable receiver and transmitter
	UCSR0B |= 1 << RXEN0;	// Enable receiver
	UCSR0B |= 1 << TXEN0;	// Enable transmitter
}

void InitUart_1 (uint16_t baudrate)
{
	// Setting UART ID
	UART1.ID = 1;
	
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
	UART1.rxBuffer = CreateFIFO(UART1.rxData, RX_BUFFER_SIZE);
	UART1.txBuffer = CreateFIFO(UART1.txData, TX_BUFFER_SIZE);	
		
	//Initializing default value
	UART1.isSending = false;
	UART1.iVector = 0;
	
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

void SetRxInterruptUart (UART_System* UART, void(*iVector)(char))
{
	UART->iVector = iVector;
}

/** 
	Sends a 0 ended String over the uart port using interrupts
	Returns the number of characters that were sent
*/

int SendUart(UART_System* UART, const char * message)
{
	int bytesQueued = 0;
	
	while(AppendFIFO(&(UART->txBuffer), *message))
	{
		++ bytesQueued;
		
		if(*message == 0)
			break;
			
		++message;
	}
	
	StartTxUart(UART);
	
	return bytesQueued;
}

/** 
	Sends an array of characters of fixed length.
	Returns the numbers of characters queued
*/

int SendnUart(UART_System* UART, const char * message, int length)
{
	int bytesQueued = 0;
	
	while(bytesQueued < length && AppendFIFO(&(UART->txBuffer), *message++))
		++ bytesQueued;
	
	StartTxUart(UART);
	
	return bytesQueued;
}

/** 
	Start sending bytes if the none were being sent.
	Returns false if uart is already sending, the buffer is empty or an invalid UART parameter is sent.
*/

int StartTxUart(UART_System* UART)
{
	switch (UART->ID)
	{
		case 0:
			return StartTxUart0(UART);
		case 1:
			return StartTxUart1(UART);
		default:
			return false;
	}
}

bool StartTxUart0()
{
	if (UART0.isSending)
		return false;
		
	char charToSend;
		
	if (!ReadFIFO(&(UART0.txBuffer), &charToSend))
		return false;
		
	UART0.isSending = true;	
	while ( !( UCSR0A & (1 << UDRE0)) );
	UDR0 = charToSend;
	
	UCSR0B |= 1 << UDRIE0; // enable UDR empty interrupt to keep sending the next ones

	return true;
}

bool StartTxUart1()
{
	if (UART1.isSending)
	return false;
	
	char charToSend;
	
	if (!ReadFIFO(&(UART1.txBuffer), &charToSend))
	return false;
	
	UART1.isSending = true;
	while ( !( UCSR1A & (1 << UDRE1)) );
	UDR1 = charToSend;
	
	UCSR1B |= 1 << UDRIE1; // enable UDR empty interrupt to keep sending the next ones

	return true;
}

/** 
	Retrieve the number of bytes in the reception buffer
*/
int GetRxCountUart(UART_System* UART)
{
	return UART->rxBuffer.byteCount;
}

/** 
	Read  and consume Rx buffer.
	The specified number of bytes is copied to the destination.
	Returns the number of bytes copied
*/

int ExtractRxUart(UART_System* UART, char * destination, int length)
{
	int byteCount = 0;
	
	while(byteCount++ < length && ReadFIFO(&(UART->rxBuffer), destination++));
	
	return byteCount;
}

/** 
	Resets the Rx Buffer
	Returns the number of characters removed
*/

int FlushRxUart(UART_System* UART)
{
	int flushed = UART->rxBuffer.byteCount;
	
	ClearFIFO(&(UART->rxBuffer));
	
	return flushed;
}

// The byte reception interrupt subroutine
ISR(USART0_RX_vect) 
{
	char in = UDR0;
	
	AppendFIFO(&(UART0.rxBuffer), in);
	
	// Call user-defined interrupt vector if any
	if (UART0.iVector)
		(*UART0.iVector)(in);
}

ISR(USART1_RX_vect)
{
	char in = UDR1;
	
	AppendFIFO(&(UART1.rxBuffer), in);
	
	if (UART1.iVector)
		(*UART1.iVector)(in);
}

// Keep sending bytes after until the buffer is empty 
ISR(USART0_UDRE_vect)
{
	char nextChar;
	
	if(ReadFIFO(&(UART0.txBuffer), &nextChar))
		UDR0 = nextChar;
	else
	{
		UART0.isSending = false;
		UCSR0B &= ~(1 << UDRIE0); //disable UDR empty interrupt
	}	
}

ISR(USART1_UDRE_vect)
{
	char nextChar;
	
	if(ReadFIFO(&(UART1.txBuffer), &nextChar))
		UDR1 = nextChar;
	else
	{
		UART1.isSending = false;
		UCSR1B &= ~(1 << UDRIE1); //disable UDR empty interrupt
	}
}