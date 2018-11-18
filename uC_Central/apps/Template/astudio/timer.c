/*
 * timer.c
 *
 * Created: 2018-11-18 15:22:57
 *  Author: Chuck
 */ 

#include "timer.h"

void init_Timer(void)
{
	 TCCR0A = 0x00;
	 TCCR0B = TCCR0B && 0x05;
	 
	 TCNT0 = 0;
}