/*
 * Uart.h
 *
 * Created: 2018-09-27 13:12:16
 *  Author: dene2303
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdbool.h>

void InitUart_0 (uint16_t baudrate);
void InitUart_1 (uint16_t baudrate);

int SendUart0 (char * message);
int SendUart1 (char * message);

int SendnUart0(char * message, int length);
int SendnUart1(char * message, int length);

bool StartTxUart0();
bool StartTxUart1();



#endif /* UART_H_ */