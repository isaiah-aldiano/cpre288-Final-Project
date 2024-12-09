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
void cliff(oi_t*sensor_data, int sum);

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
    oi_setWheels(100, 100); // move forward; full speed
    while (sum < centimeters*10) {
//        if(sensor_data->bumpLeft ||sensor_data->bumpRight){
//            bump(sensor_data, sum);
//            oi_setWheels(0, 0);
//
//            break;
//        }
//        if(sensor_data->cliffLeftSignal < 100 || sensor_data->cliffRightSignal < 100 || sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffRightSignal > 2700){
//            cliff(sensor_data, sum);
//            oi_setWheels(0, 0);
//            turn_clockwise(sensor_data, 90);
//            break;
//        }
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

//    while(flag == 0){
//        //command = uart_receive();
//        //oi_play_song(0);
//        uart_sendChar('\n');
//    }
//    flag = 0;

    move_forward(sensor_data, sum / 10);

}

void cliff(oi_t*sensor_data, int sum){
    oi_setWheels(0, 0); // stop

    //move_backwards(sensor_data, 10);
    unsigned char notes[4] = {67, 67, 72, 67};
    unsigned char duration[4] = {15, 15, 20, 15};
    oi_loadSong(0, 4, notes, duration);

//    oi_play_song(0);

//    distTrav = distTrav - 60 + (sum / 10);
//
    move_backwards(sensor_data, 10);
//    turn_clockwise(sensor_data, 90);
//    //TODO: scan here
//    move_forward(sensor_data, 50);
//    turn_counterclockwise(sensor_data, 90);
//    //TODO: scan here
//    move_forward(sensor_data, 80);
//
//    if(distTrav > 30){
//        turn_counterclockwise(sensor_data, 90);
//        move_forward(sensor_data, 50);
//        turn_clockwise(sensor_data, 90);
//    }



}

void helpSend(char data[]){
    char i;
    for(i = 0; i < strlen(data); i++){
        uart_sendChar(data[i]);
    }
    uart_sendChar('\n');
}

