/*
 * usb_uart.c
 *
 * Created: 2018-10-26 17:03:28
 *  Author: dene2303
 */ 

#include <avr/interrupt.h>
#include <stdbool.h>
#include "uart.h"
#include "system.h"


// Max number of characters
#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256

// generic fifo struct
typedef struct
{
	char *   head;
	uint16_t  size;
	uint8_t   *data;
} FifoBuffer_t;


void InitUart (uint16_t baudrate)
{
	// Set baudrate for asynchronous double speed mode
	uint16_t baudRateSetting = (uint16_t) (CPU_CLK_HZ / 8) - 1;
	UBRR0H = baudRateSetting >> 4;
	UBRR0L = baudRateSetting;

	// Set byte format
	UCSR0A |= 1 << U2X0;	// Double speed mode : disabled
	UCSR0A &= ~(1 << MPCM0);	// Multi processor mode : disabled
	
	UCSR0B &= ~(1 << UCSZ02);	// 9 bit mode: disabled
	
	UCSR0C &= ~(0b11 << UMSEL00);	// asynchronous mode
	UCSR0C &= ~(0b11 << UPM00);		// No parity
	UCSR0C &= ~(1 << USBS0);		// 1 Stop bit
	UCSR0C |= 0b111 << UCSZ00;		// 8 bit word size
	
	// Enable interrupts
	UCSR0B |= 1 << RXCIE0;	// Receive complete interrupt
	sei();					// Global interrupt enable
	
	// Enable receiver and transmitter
	UCSR0B |= 1 << RXEN0;	// Enable receiver
	UCSR0B |= 1 << TXEN0;	// Enable transmitter
}


// The byte reception interrupt subroutine
ISR(USART0_RX_vect) 
{
	// Read incoming data and echo it on the line
	char in = UDR0;
	
	// Wait for empty buffer
	while( !(UCSR0A))
	UDR0 = in;
}