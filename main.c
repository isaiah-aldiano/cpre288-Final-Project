#include "uart_extra_help.h"
#include "math.h"
#include "movement.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"

/**
 * main.c
 */
int main(void)
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    servo_init();
    ping_init();
    int i = 0;

    int roombaWidth = 3;
    int fieldWidth = 30;
    int fieldLength = 50;
    int cycles = fieldWidth / roombaWidth;

    for(i = 0; i < cycles; i++){
        move_forward(sensor_data, fieldLength);
    }


    oi_free(sensor_data);
}
