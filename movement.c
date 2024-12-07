/*
 * movement.c
 *
 *  Created on: Sep 11, 2024
 *      Author: lewisc65
 */


#include "movement.h"
#include "open_interface.h"
#include "uart_extra_help.h"

void move_forward(oi_t*sensor_data, int centimeters);
void move_backwards(oi_t *sensor_data, int centimeters);
void turn_counter_clockwise(oi_t *sensor_data, int degrees);
void turn_clockwise(oi_t *sensor_data, int degrees);
void bump (oi_t*sensor_data, int sum);
void helpSend(char data[]);
void cliff(oi_t*sensor_data, int sum);
void move_cliff(oi_t*sensor_data, int centimeters);

extern volatile  char uart_data;  // Your UART interupt code can place read data here
extern volatile  char flag;       // Your UART interupt can update this flag
extern volatile int distTrav;

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
            bump(sensor_data, sum);
            break;
        }
        if(sensor_data->cliffLeftSignal < 100|| sensor_data->cliffRightSignal < 100){
            cliff(sensor_data, sum);
            break;
        }
         oi_update(sensor_data);
         sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void move_cliff(oi_t*sensor_data, int centimeters){
    double sum = 0;
    oi_setWheels(200, 200); // move forward; full speed
    while (sum < centimeters*10) {
        if(sensor_data->bumpLeft ||sensor_data->bumpRight){
            bump(sensor_data, sum);
            break;
        }
         oi_update(sensor_data);
         sum += sensor_data->distance;
    }
    oi_setWheels(0, 0); // stop
}

void bump (oi_t*sensor_data, int sum){
    oi_setWheels(0, 0);

    //Send bump to GUI to alert user of object to remove
    char message[] = "Bump";
    helpSend(message);

    unsigned char notes[4] = {67, 67, 72, 67};
    unsigned char duration[4] = {15, 15, 20, 15};
    oi_loadSong(0, 4, notes, duration);

    while(flag == 0){
        //command = uart_receive();
        //oi_play_song(0);
        uart_sendChar('\n');
    }
    flag = 0;

    move_forward(sensor_data, sum / 10);
}

void cliff(oi_t*sensor_data, int sum){
    //move_backwards(sensor_data, 10);
    unsigned char notes[4] = {67, 67, 72, 67};
    unsigned char duration[4] = {15, 15, 20, 15};
    oi_loadSong(0, 4, notes, duration);

    oi_play_song(0);

    distTrav = distTrav - 60 + (sum / 10);

    move_backwards(sensor_data, 10);
    turn_clockwise(sensor_data, 90);
    //TODO: scan here
    move_forward(sensor_data, 50);
    turn_counterclockwise(sensor_data, 90);
    //TODO: scan here
    move_forward(sensor_data, 80);

    if(distTrav > 30){
        turn_counterclockwise(sensor_data, 90);
        move_forward(sensor_data, 50);
        turn_clockwise(sensor_data, 90);
    }



}

void helpSend(char data[]){
    uart_init(0);
    char i;
    for(i = 0; i < strlen(data); i++){
        uart_sendChar(data[i]);
    }
    uart_sendChar('\n');
}

