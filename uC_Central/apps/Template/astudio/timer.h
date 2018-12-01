/*
 * timer.h
 *
 * Created: 2018-11-18 15:23:42
 *  Author: Chuck
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#include "config.h"
#include "math.h"
#include <avr/interrupt.h>

#define DELAY 20 // Timer delay in ms
#define TIMER_COUNT_LOOP (DELAY*16000000L/1024000 - 1) // 8000000 = CPU_CLK_HZ mais le include ne fonctionne pas; hotfix -Chuck

void init_Timer(void);
#endif /* TIMER_H_ */