/*
 * adc.c
 *
 *  Created on: Oct 23, 2024
 *      Author: lewisc65
 */

#include "adc.h"


void ADC_init(void)
{
//    //Module initialization
//    SYSCTL_RCGCADC_R |= 0x1;        //Set ADC Clock
//    timer_waitMillis(20);
//
//    SYSCTL_RCGCGPIO_R |= 0b011010;    //Set port clock
//    timer_waitMillis(20);
//
//    GPIO_PORTB_AFSEL_R |= 0x10;     //Set alternate function
//    timer_waitMillis(20);
//
//    GPIO_PORTB_DEN_R |= 0x10;        //Set pins as input
//    timer_waitMillis(20);
//
//    GPIO_PORTB_DIR_R |= 0x10;
//    timer_waitMillis(20);
//
//    GPIO_PORTB_AMSEL_R |= 0x10;    //Set analog
//    timer_waitMillis(20);
//
//    //Sample Sequencer
//    ADC0_ACTSS_R &= 0;
//    timer_waitMillis(20);
//
//    ADC0_EMUX_R |= 0xF;
//    timer_waitMillis(20);
//
//    ADC0_SSMUX0_R = 0xA;
//    timer_waitMillis(20);
//
//    ADC0_SSCTL0_R = 0b0110;
//    timer_waitMillis(20);
//
//    ADC0_ACTSS_R |= 0x1;
//    timer_waitMillis(20);
//
//    ADC0_PSSI_R |= 0x1;
//    timer_waitMillis(20);
//
//    ADC0_SAC_R = 0x4;

    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCADC_R |= 0x01;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= 0xEF;
    GPIO_PORTB_DIR_R &= 0xEF;
    GPIO_PORTB_AMSEL_R |= 0x10;
    GPIO_PORTB_ADCCTL_R = 0x00;


    ADC0_ACTSS_R &= 0xE;
    ADC0_EMUX_R &= 0xFFF0;
    ADC0_SSMUX0_R |= 0xA;
    ADC0_SAC_R |= 0x3;
    ADC0_SSCTL0_R |= 0x6;
    ADC0_ACTSS_R |= 0b00000001;

}


int ADC_read(void){
    ADC0_PSSI_R |= 0x01;
    ADC0_IM_R &= 0x1;


    while((ADC0_RIS_R & ADC0_IM_R) == 1) {

    }

    ADC0_ISC_R = 0x1;
    return ADC0_SSFIFO0_R;
}

float IR_SCAN() { // Returns ADC value as distance in CM
    /*
     * Bot 00 IR calibration y = 20302x^-.875
     * Y = ADC value
     * X = distance
     * sqrt(-.875, y/20302)
     */
    int adc_value;
    float divid = 20302;
    float exponent = -1/.875;
    float dist_cm;

    adc_value = ADC_read();
    dist_cm = adc_value / divid;
    dist_cm = pow(dist_cm, exponent);

    return dist_cm;
}

void ADC_calibrate(void) {
    char buffer[15];
    int i, j;
    int adc_value;
    for(j = 0; j < 8; j++) {
        uart_sendStr("------------------------------------------------------------------------\r\n");

        for(i = 0; i < 500; i++) {
            adc_value = ADC_read();
            sprintf(buffer, "%d\r\n", adc_value);
            uart_sendStr(buffer);
    //        uart_sendChar(buffer[0]);
            timer_waitMillis(20);
        }
    }

}
