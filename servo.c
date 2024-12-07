/*
 * servo.c
 *
 *  Created on: Nov 6, 2024
 *      Author: lewisc65
 */
#include "lcd.h"
#include "timer.h"

int match_subtract;
int degrees = 90;
int position = 4000;
short switch_direction = 1;
int base_1_millisecond = 16000;
int right_cal = 9000;
int left_cal = 38700;
float difference;
float denom;

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0x2;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x00700000;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;


    SYSCTL_RCGCTIMER_R |= 0x2;
    TIMER1_CTL_R &= 0xEFF; // Disable that bih
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R |= 0x2; // Match interrupts needed? Why not
    TIMER1_TBMR_R |= 0xE; // Periodic, Edge-Time, PWM
    TIMER1_TBMR_R &= 0xEF; // Count Down
    TIMER1_TBILR_R |= 0xFFFF;

    TIMER1_ICR_R |= 0x800;
    TIMER1_IMR_R |= 0x800;

//    NVIC_EN0_R |= (1 << 22); //Nvic for 16/32-Bit Timer 1B (page 104)
//    IntRegister(INT_TIMER1B, timer1b_handler);
//    IntMasterEnable();

    TIMER1_CTL_R |= 0x100;

//    servo_move_clock_cycles(position);

}


//int servo_move(int degrees){
//
//    TIMER1_TBILR_R = 0x4E200 & 0xFFFF; //4E200 = 320000; //Timer start set
//    TIMER1_TBPR_R = 0x4E200 >> 16;
//
//    int slope = (285298 - 311710)/180;   //0x45B00
//    int match = (311710 + (slope*degrees));
//
//    TIMER1_TBMATCHR_R = match & 0xFFFF; //304000 = 0x4A380
//    TIMER1_TBPMR_R = match >> 16;
//
//    timer_waitMillis(20);
//
//    return match;
//}

int servo_move_clock_cycles(int clock_cycles) {

    match_subtract = clock_cycles; //Clock cycles + base 1 millisecond to register movement

    if(match_subtract < right_cal) {
        match_subtract = right_cal;
        degrees = 0;
        position = right_cal;
    } else if (match_subtract > left_cal) {
        match_subtract = left_cal;
        degrees = 180;
        position = left_cal;
    }

    TIMER1_TBMATCHR_R = TIMER1_TBILR_R - match_subtract;

    return match_subtract;
}

void servo_move_degrees(int degree) {
    difference = left_cal - right_cal;
    denom = 180 / difference;

    float clock_cycles  = (degree / denom) + right_cal;
    servo_move_clock_cycles((int) clock_cycles);
    timer_waitMillis(20);
}

void button_init() {
    static uint8_t initialized = 0;

    if(initialized){
        return;
    }

    SYSCTL_RCGCGPIO_R |= 0x10;
    GPIO_PORTE_DIR_R &= 0xC0;
    GPIO_PORTE_DEN_R |= 0x3F;

    initialized = 1;
}

int button_getButton(int curr_match) {


    if (!((GPIO_PORTE_DATA_R >> 2)&1)) {
        switch_direction *= -1;
    }

//    if (!((GPIO_PORTE_DATA_R >> 3)&1) ) { // button 4
//        if(switch_direction == 1) {
//            servo_move_clock_cycles(right_calibration_value);
//            degrees = 0;
//            position = right_calibration_value;
//
//        } else if (switch_direction == -1){
//            servo_move_clock_cycles(left_calibration_value);
//            degrees = 180;
//            position = left_calibration_value;
//
//        }
//    } else
//
    if (!((GPIO_PORTE_DATA_R >> 1)&1)) {
        curr_match += (775 * switch_direction);
        degrees += 5 * switch_direction;
//        servo_move_clock_cycles(position);
    }else if (!((GPIO_PORTE_DATA_R)&1)) {
        curr_match += (155 * switch_direction);
        degrees += 1 * switch_direction;
//        servo_move_clock_cycles(position);
    }

//    while(!((GPIO_PORTE_DATA_R)&1) || !((GPIO_PORTE_DATA_R >> 1)&1) || !((GPIO_PORTE_DATA_R >> 3)&1) || !((GPIO_PORTE_DATA_R >> 2)&1)) {
//
//    }

    return curr_match;
}

void calibrateServo() {
    match_subtract = base_1_millisecond;

    while(1) {
        match_subtract = button_getButton(match_subtract);
        timer_waitMillis(20);
        servo_move_clock_cycles(match_subtract);

        lcd_printf("%d", match_subtract);
    }
}




