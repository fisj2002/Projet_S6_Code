/*
 * Uart.h
 *
 * Created: 2018-09-27 13:12:16
 *  Author: dene2303
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdbool.h>


// Max number of characters
#define TX_BUFFER_SIZE 1024
#define RX_BUFFER_SIZE 1024



void InitUart_0 (uint16_t baudrate);
void InitUart_1 (uint16_t baudrate);

void SetRxInterruptUart0 (void(*iVector)(char));
void SetRxInterruptUart1 (void(*iVector)(char));

int SendUart0 (const char * message);
int SendUart1 (const char * message);

int SendnUart0(const char * message, int length);
int SendnUart1(const char * message, int length);

int GetRxCountUart0();
int GetRxCountUart1();

int ExtractRxUart0(char * destination, int length);
int ExtractRxUart1(char * destination, int length);

int FlushRxUart0();
int FlushRxUart1();

#endif /* UART_H_ */