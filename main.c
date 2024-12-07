#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include "Timer.h"
#include "uart_extra_help.h"
#include "lcd.h"
#include "adc.h"
#include "driverlib/interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "servo.h"
#include "ping.h"


//Object Struct
struct someObject{
    short start;
    short end;
    short size;
    short angle;
    int pingDist;
    float linearWidth;
};

//void Ascan(void);
void runScan(float scanData[]);
void determineObjects(float scanData[], struct someObject obj[]);
void objectsInit(struct someObject obj[]);
void sizeObjects(struct someObject obj[]);

volatile int doSomething;

/**
 * main.c
 */
int main(void)
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
//    timer_init();
    lcd_init();
    uart_init();
    uart_interrupt_init();
    ADC_init();
    servo_init();
    ping_init();

//     Servo Calibration
//    button_init();
//    calibrateServo();

//    // ADC calibration
//    servo_move_degrees(90);
//    timer_waitMillis(1000);
//    ADC_calibrate();



    float scanData[180];
    struct someObject obj[5];
    objectsInit(obj);
//
//    runScan(scanData);
//    determineObjects(scanData, obj);
//    sizeObjects(obj);
//


    bool adjustNeeded = false;
    bool exitInput = false;

    while(1) {
        move_forward(sensor_data, 30.5);
        timer_waitMillis(600);

        runScan(scanData);
        determineObjects(scanData, obj);
        sizeObjects(obj);

    //    turn_counterclockwise(sensor_data, 90);
    //    timer_waitMillis(1200);

        if(adjustNeeded) {
            turn_clockwise(sensor_data, 3);
            timer_waitMillis(300);
            adjustNeeded = false;
        } else {
            adjustNeeded = true;
        }

        while(1) {
//            uart_sendChar(doSomething);
            switch(doSomething) {
            case 1:
                exitInput = true;
                break;
            }

            if(exitInput) {
                exitInput = false;
                objectsInit(obj);
                break;
            }
        }

    }


//    while(1) {
//        /*
//         * 1. Run a scan
//         * 2. Determine if objects can be removed
//         *      a. If removable "remove" objects and wait for key press to continue
//         * 3. Determine if objects must be avoided
//         *      a. Determine how far to move away from object
//         * 4. Move cleaner
//         *      a. avoid immovable objects
//         *      b. return to original path
//         *      c. if bump sensor hits short object then remove short object
//         */
//
//
//        uart_sendChar(doSomething);
//        switch(doSomething) {
//        case 1:
//            break;
//        }
//    }

    return 1;

}


void runScan(float scanData[]) {
    char data[20];

    int i;
    float dist_cm;

    char str[] = "\r\nDegrees   Distance(cm)\r\n";
    uart_sendStr(str);

    servo_move_degrees(0);
    timer_waitMillis(1000);

    IR_SCAN();

    for(i = 0; i < 180; i++) {
        servo_move_degrees(i);
        dist_cm = IR_SCAN();
        scanData[i] = dist_cm;

        sprintf(data, "\r\n%-10d%.2f", i, dist_cm);
        uart_sendStr(data);
    }

    timer_waitMillis(1000);
    servo_move_degrees(1);

    uart_sendStr("\r\n");
    free(data);
}

void determineObjects(float scanData[], struct someObject obj[]) {
//    int n = sizeof(scanData) / sizeof(scanData[0]);
//    float copyData[n];
//    memcpy(copyData, scanData, n * sizeof(scanData[0]));


    int i, points = 0, start = -1, end = -1, objCount = 0;
    int margin = 3;

    for(i = 1; i < 180; i++) {
        while(scanData[i] <= scanData[i - 1] + margin && scanData[i] >= scanData[i -1] - margin && scanData[i] < 45) {
            if(start == -1) {
                start = i - 1;
            }
            i++;
            points++;
        }

        if(points >= 5) {
            end = i - 1;
            printf("object found S:%d F:%d SIZE:%d\n", start, end, end - start);
            obj[objCount].start = start;
            obj[objCount].end = end;
            obj[objCount].angle = (start + end) / 2;
            obj[objCount].size = end - start;
            objCount++;
        }

        start = -1;
        end = -1;
        points = 0;
    }
}

float pingObjects(int angle) {
    int i, scans = 1;
//    float cycles_in_milliseconds;
    float avg_dist = 0.0;

    servo_move_degrees(angle);
    timer_waitMillis(1000);

    IR_SCAN();

    for(i = 0; i < scans; i++) {

//        cycles = ping_read();
//
//        cycles_in_milliseconds = (float) cycles / 16000;
//
//        distance_cm = (float) cycles / 16000000.0; // cycles / clockMHz = seconds between pulse send and receive
//        distance_cm /= 2.0; // Only account for seconds to object
//        distance_cm *= 3400.0; // Seconds to object * Speed of sound (~340m/s) * 100(cm in m) = cm to object
//
        avg_dist += IR_SCAN();


    }

    return avg_dist / scans;

}

void sizeObjects(struct someObject obj[]) {
    int i = 0;
    while(obj[i].angle != -1) {
        obj[i].pingDist = pingObjects(obj[i].start - 2);
        obj[i].linearWidth = obj[i].pingDist * obj[i].size * (3.1415926 / 180);
        i++;
        timer_waitMillis(1500);

    }
    //        linearWidth[i] = objectDist[i] * width * (3.1415926/ 180); //radius * angle (in radians)
}

void objectsInit(struct someObject obj[]) {
    int i;
    for(i = 0; i < 5; i++) {
        obj[i].start = -1;
        obj[i].end = -1;
        obj[i].size = -1;
        obj[i].angle = -1;
        obj[i].pingDist = -1;
        obj[i].linearWidth = -1;
    }
}

//while(1) {
////        move_forward(sensor_data, 65);
////        timer_waitMillis(200);
//
//        turn_counterclockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_counterclockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_counterclockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_counterclockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//
//        turn_clockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_clockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_clockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//        turn_clockwise(sensor_data, 90);
//        timer_waitMillis(1200);
//
//    }

////TODO: detect only larger objects to avoid
//void Ascan(void){
//    int i = 0;
//    int n = 0; //Number of objects
//    int objectStart[25];
//    int objectEnd[25];
//    float data[91];
//    int s1 = 0;
//    int s2 = 0;
//
//    //Scan for data
//    servo_move_degrees(0);
//    timer_waitMillis(400);
//    for(i = 0; i <= 90; i++){
//        servo_move_degrees(i*2);
//        s1 = ADC_read();
//        servo_move_degrees(i*2);
//        s2 = ADC_read();
//
//        data[i] = (s1 + s2) / 2;
////        sprintf(str, "%.3f \r\n", data[i]);
////        helpSend(str);
//    }
//
//    data[90] = 70;
//    data[0] = 70;
//
//    //Clean data
//    for(i = 1; i <= 90; i++){
//        float diffBelow = abs(data[i-1] - data[i]);
//        float diffAbove = abs(data[i+1] - data[i]);
//
//        //if difference of data above and below is too much, the data is set to the average of above and below
//        if((diffBelow > 5) && (diffAbove > 5)){
//            data[i] = (data[i+1] + data[i-1]) / 2;
//        }
//    }
//
//    //Collect object data
//    if(data[0] < 45 && data[1] < 45){
//       objectStart[n] = 0;
//    }
//
//    for(i = 1; i <= 60; i++){
//        if(data[i] < 45){
//            if(data[i-1] > 45){
//                //helpSend("Object detected \n\r");
//                objectStart[n] = i * 2;
//            } else if(data[i+1] > 45){
//                objectEnd[n] = i * 2;
//                n++;
//            }
//        }
//    }
//
//    int midAngle[25];
//    int objectDist[25];
//    float linearWidth [25];
//    int avoidObjects = 0;
//
//    //Determine width of objects
//    for(i = 0; i < n; i++){
//
//        midAngle[i] = (objectEnd[i] + objectStart[i])/2; //Calculate the middle angle of each object
//
//        servo_move_degrees(midAngle[i]);
//        timer_waitMillis(400);
//        servo_move_degrees(midAngle[i]);
//
//        objectDist[i] = ping_read();
//
//        int width = objectEnd[i] - objectStart[i];
//        linearWidth[i] = objectDist[i] * width * (3.1415926/ 180); //radius * angle (in radians)
//        ///sprintf(str, "%.3f \r\n", linearWidth[i]);
//        //helpSend(str);
//
//        //If object is to be avoided, put to front of array and new count of avoid objects created
//        if(linearWidth[i] >= 10 && linearWidth[i] < 100){ //TODO: figure out measurement of large objects
//            midAngle[avoidObjects] = midAngle[i];
//            objectDist[avoidObjects] = objectDist[i];
//            linearWidth[avoidObjects] = linearWidth[i];
//
//            avoidObjects++;
//        }
//    }
//
//    //TODO: avoidance of large objects
//
//}

//    timer_waitMillis(1000);
//    Ascan();

//    int i;
//    for(i = 0; i < 90; i++) {
//        servo_move_degrees(i * 2);
//    }
//    int roombaWidth = 32; //Roomba width in centimeters, used to determine turn distance
//    int fieldWidth = 64; //In cm
//    int fieldLength = 100; //In cm
//    int cycles = fieldWidth / roombaWidth;
//    int distTrav = 0;
//    char turnDir = 'r'; //Keeps track of which way to turn
//
//    for(i = 0; i < cycles; i++){
//        //Travel small distance and scan until reaching full field length
//        while(distTrav < fieldLength) {
//            distTrav += 40;
//            move_forward(sensor_data, 40);
//
//            //Scan field
//            Ascan();
//        }
//
//
//        move_forward(sensor_data, fieldLength - distTrav);
//        distTrav = 0;
//
//        if(turnDir == 'r'){
//            turnDir = 'l';
//            turn_clockwise(sensor_data, 90);
//            move_forward(sensor_data, roombaWidth);
//            turn_clockwise(sensor_data, 90);
//        } else {
//            turnDir = 'r';
//            turn_counterclockwise(sensor_data, 90);
//            move_forward(sensor_data, roombaWidth);
//            turn_counterclockwise(sensor_data, 90);
//        }
//    }
//
//
//    oi_free(sensor_data);
