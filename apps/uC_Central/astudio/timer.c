/*
 * timer.c
 *
 * Created: 2018-11-18 15:22:57
 *  Author: Chuck
 */ 

#include "timer.h"

void init_Timer(void)
{
	 TCCR0B |= (1 << CS02)|(1 << CS00);
	 
	 TCNT0 = 0;
}