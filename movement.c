/*
 * movement.c
 *
 *  Created on: Sep 11, 2024
 *      Author: lewisc65
 */


#include "movement.h"
#include "open_interface.h"
void move_forward(oi_t*sensor_data, int centimeters);
void move_backwards(oi_t *sensor_data, int centimeters);
void turn_counter_clockwise(oi_t *sensor_data, int degrees);
void turn_clockwise(oi_t *sensor_data, int degrees);
void bump (oi_t*sensor_data);

void turn_clockwise(oi_t *sensor_data, int degrees){
    double sum = 0;
    oi_setWheels(-200, 200); // turn clockwise
    while (sum > -1 * (degrees - 10)) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->angle;
    }
    oi_setWheels(0, 0); // stop
}

void turn_counterclockwise(oi_t *sensor_data, int degrees){
    double sum = 0;
    oi_setWheels(200, -200); // turn counterclockwise
    while (sum < (degrees - 20)) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->angle;
    }
    oi_setWheels(0, 0); // stop
}

void move_backwards(oi_t *sensor_data, int centimeters){
    double sum = 0;
    oi_setWheels(-200, -200); // move backwards
    while (sum > -10 * centimeters) { //turn set degrees
             oi_update(sensor_data);
             sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void move_forward(oi_t*sensor_data, int centimeters){
    double sum = 0;
    oi_setWheels(200, 200); // move forward; full speed
    while (sum < centimeters*10) {
        if(sensor_data->bumpLeft ||sensor_data->bumpRight){
            bump(sensor_data);
            break;
        }
         oi_update(sensor_data);
         sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void bump (oi_t*sensor_data){
    if(sensor_data->bumpLeft){

        move_backwards(sensor_data, 5);

        turn_clockwise(sensor_data, 45);

        move_forward(sensor_data, 15);

        turn_counterclockwise(sensor_data, 90);
    }
    else {
        move_backwards(sensor_data, 5);

        turn_counterclockwise(sensor_data, 45);

        move_forward(sensor_data, 15);

        turn_clockwise(sensor_data, 90);
    }
    oi_setWheels(0, 0);
    //move_forward(sensor_data, (sum/10)+15); //returns remaining distance to go forward plus 15cm for the distance backwards
}


