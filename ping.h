#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "driverlib/interrupt.h"
#include <math.h>

/*
 * ping.h
 *
 *  Created on: Oct 30, 2024
 *      Author: lewisc65
 */

#ifndef PING_H_
#define PING_H_

volatile enum {LOW, HIGH, DONE} state; // set by ISR
volatile unsigned int rising_time; //Pulse start time: Set by ISR
volatile unsigned int falling_time; //Pulse end time: Set by ISR
volatile int overflow;


void ping_init(void);
int ping_read(void);
void send_pulse();
void TIMER3B_Handler(void);


#endif /* PING_H_ */
