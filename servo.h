
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "driverlib/interrupt.h"
/*
 * servo.h
 *
 *  Created on: Nov 6, 2024
 *      Author: lewisc65
 */

#ifndef SERVO_H_
#define SERVO_H_


void servo_init(void);
//int servo_move(float degrees);
int servo_move_clock_cycles(int clock_cycles);
void servo_move_degrees(int degree);
int get_match(void);
int get_match_subtract(void);
void button_init(void);
int button_getButton(void);
void calibrateServo(void);


#endif /* SERVO_H_ */
