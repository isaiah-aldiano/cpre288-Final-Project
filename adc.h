/*
 * adc.h
 *
 *  Created on: Oct 23, 2024
 *      Author: lewisc65
 */

#ifndef ADC_H_
#define ADC_H_

#include "timer.h"
#include "math.h"
#include <stdio.h>
#include "uart_extra_help.h"

void ADC_init(void);
int ADC_read(void);
void ADC_calibrate(void);
float IR_SCAN(void);



#endif /* ADC_H_ */
