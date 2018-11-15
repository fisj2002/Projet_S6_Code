/*
 * MicroServeur.c
 *
 * Created: 2018-09-27 12:18:41
 *  Author: dene2303
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <uart.h>
#include <stdio.h>

volatile uint8_t CTC_flag;


char copyBuffer[RX_BUFFER_SIZE];


ISR(TIMER1_COMPA_vect) {
	CTC_flag = 1;
}

// User-defined interrupt function
void RxEchoISR(char receivedChar)
{
	// Simply echoes what the user typed in
	SendnUart1(&receivedChar, 1);
	if (receivedChar == '\r')
		SendUart1("\n");
}


void Timer_Init(void) {
	
	TIMSK1 = 0x00; //Disable timer interrupts
	TCCR1A = 0x00; //Timer not connected to any pins
	TCCR1B = 0x09; //CTC mode; Timer_Rate = System_CLK = 1MHz
	// 1 tick = 1 us
}

void Delay(uint16_t us) {
	CTC_flag = 0; //Clear CTC Interrupt flag
	TCNT1 = 0x0000; //Clear timer
	OCR1A = us; //Load compare value with desired delay in us
	TIMSK1 = 0x02; //Enable OCIE1A Interrupt
	while(!CTC_flag); //wait for the designated number of us
}

int main(void)
{
	Timer_Init();
	
	InitUart_1(9600);
	SendUart1 ("Hello World!\n\r");
	SetRxInterruptUart1(&RxEchoISR);
	
    while(1)
    {
		// Wait 10 seconds
		for(int i=0; i<(200*16); i++)
		{
			Delay(50000); 
		}
		

		int numChar = GetRxCountUart1();
		
		int charCout = sprintf(copyBuffer,"\r\nSince the last 10 seconds, you sent %d bytes:\r\n", numChar);
		SendnUart1(copyBuffer, charCout);
		
		ExtractRxUart1(copyBuffer, numChar);
		SendnUart1(copyBuffer, numChar);
		SendUart1("\r\n");
		
		FlushRxUart1();
    }
}