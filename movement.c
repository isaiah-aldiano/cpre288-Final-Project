/*
 * movement.c
 *
 *  Created on: Sep 11, 2024
 *      Author: lewisc65
 */


#include "movement.h"
#include "open_interface.h"
#include "uart_extra_help.h"

//void move_forward(oi_t*sensor_data, int centimeters);
//void move_backwards(oi_t *sensor_data, int centimeters);
//void turn_counter_clockwise(oi_t *sensor_data, double degrees);
//void turn_clockwise(oi_t *sensor_data, double degrees);
//void bump (oi_t*sensor_data, int sum);
//void helpSend(char data[]);

const double ERROR = .017;

void turn_clockwise(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_setWheels(-50, 50); // turn clockwise
    while (sum > (-1 * degrees) + (degrees * ERROR)) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->angle;
    }
    oi_setWheels(0, 0); // stop
}

void turn_counterclockwise(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_setWheels(50, -50); // turn counterclockwise
    while (sum < degrees - (degrees * ERROR)) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->angle;
    }
    oi_setWheels(0, 0); // stop
}

void move_backwards(oi_t *sensor_data, double centimeters){
    double sum = 0;
    oi_setWheels(-100, -100); // move backwards
    while (sum > -10 * centimeters) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void move_forward(oi_t*sensor_data, double centimeters){
    double sum = 0;
    oi_setWheels(96, 105); // move forward; full speed
    while (sum < centimeters*10) {
//        if(sensor_data->bumpLeft ||sensor_data->bumpRight){
//            bump(sensor_data, sum);
//            break;
//        }
         oi_update(sensor_data);
         sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void bump (oi_t*sensor_data, int sum){
    char command = 't';
    oi_setWheels(0, 0);

    char message[] = "Bump";
    helpSend(message);

    while(command != 'c'){
        command = uart_receive();
    }

    move_forward(sensor_data, sum / 10);
}

void helpSend(char data[]){
    char i;
    for(i = 0; i < strlen(data); i++){
        uart_sendChar(data[i]);
    }
    uart_sendChar('\n');
}

