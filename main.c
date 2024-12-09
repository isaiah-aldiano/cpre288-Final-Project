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
    bool avoid;
    float avoidDist;
};

struct et {
    double forward[5];
    double backward[5];
    double left[5];
    double right[5];
    short directions[40];
};

//void Ascan(void);
void runScan(float scanData[], char printData[]);
void determineObjects(float scanData[], struct someObject obj[], char printData[]);
void objectsInit(struct someObject obj[]);
void sizeObjects(struct someObject obj[], float scanData[]);
void removeObjects(struct someObject obj[], char printData[]);
int moveToAvoid(struct someObject obj[], oi_t *sensor_data);

volatile int doSomething = 0;
float botSizeCm = 38;
float returnToPath;
int objectAtSide;

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

//    // Servo Calibration
//    button_init();
//    calibrateServo();

//    // ADC calibration
    servo_move_degrees(90);
//    timer_waitMillis(1000);
//    ADC_calibrate();

//    struct et phoneHome;
    volatile int continuing = -1;

    float scanData[180];
    char printData[40];
    struct someObject obj[5];
    objectsInit(obj);
//
//    runScan(scanData);
//    determineObjects(scanData, obj);
//    sizeObjects(obj);
//

//
//    bool adjustNeeded = false;
//    bool exitInput = false;

    while(1) {


    //    turn_counterclockwise(sensor_data, 90);
    //    timer_waitMillis(1200);

//        if(adjustNeeded) {
//            turn_clockwise(sensor_data, 3);
//            timer_waitMillis(300);
//            adjustNeeded = false;
//        } else {
//            adjustNeeded = true;
//        }

//        while(1) {
////            uart_sendChar(doSomething);
//            switch(doSomething) {
//            case 1:
//                exitInput = true;
//                break;
//            }
//
//            if(exitInput) {
//                exitInput = false;
//                break;
//            }
//        }
//        runScan(scanData);
//        determineObjects(scanData, obj);
//        sizeObjects(obj);
//        removeObjects(obj);
//        objectsInit(obj);

        while(1) {
            oi_setWheels(0, 0); // stop
            uart_init();

            runScan(scanData, printData);
            determineObjects(scanData, obj, printData);
            sizeObjects(obj, scanData);
            removeObjects(obj, printData);
            continuing = moveToAvoid(obj, sensor_data);
            objectsInit(obj);
            if(continuing == 2) {
//                int s;
                break;
            }
        }

//        while(1) {
//            /*
//             * 1. Run a scan
//             * 2. Determine if objects can be removed
//             *      a. If removable "remove" objects and wait for key press to continue
//             * 3. Determine if objects must be avoided
//             *      a. Determine how far to move away from object
//             * 4. Move cleaner
//             *      a. avoid immovable objects
//             *      b. return to original path
//             *      c. if bump sensor hits short object then remove short object
//             */
//
//
//            uart_sendChar(doSomething);
//            switch(doSomething) {
//            case 1:
//                break;
//            }
//        }

        move_forward(sensor_data, 20);
    }




    return 1;

}


void runScan(float scanData[], char printData[]) {
//    char data[20];

    int i, j,scans = 3;
    float dist_cm;

    char str[] = "\r\nDegrees   Distance(cm)\r\n";
    uart_sendStr(str);

    servo_move_degrees(0);
    timer_waitMillis(1000);

    IR_SCAN();

    for(i = 1; i < 180; i++) {
        dist_cm = 0;
        servo_move_degrees(i);

        for(j = 0; j < scans; j++) {
            dist_cm += IR_SCAN();
        }

        scanData[i] = dist_cm / scans;

        sprintf(printData,"%-10d %d\r\n", i, (int) dist_cm / scans);
        uart_sendStr(printData);


    }

    timer_waitMillis(1000);
    servo_move_degrees(1);

//    uart_sendStr("\r\n");
//    free(data);
}

void determineObjects(float scanData[], struct someObject obj[], char printData[]) {
//    char buffer[30];
    int i, points = 0, start = -1, end = -1, objCount = 0;
    int margin = 4;

    for(i = 1; i < 180; i++) {
//        while(scanData[i] - margin >= scanData[i - 1] || scanData[i] + margin <= scanData[i -1] && scanData[i] < 35) {
//            if(start == -1) {
//                start = i - 1;
//            }
//            i++;
//            points++;
//        }
        while(((scanData[i] - margin <= scanData[i - 1] && scanData[i - 1] <= scanData[i]) ||
                (scanData[i] + margin >= scanData[i - 1] && scanData[i - 1] >= scanData[i]))
                && scanData[i] < 35) {
            if(start == -1) {
                start = i - 1;
            }
            i++;
            points++;
        }

        if(points > 7) {
            end = i - 1;
            sprintf(printData, "object found S:%d F:%d SIZE:%d\r\n", start, end, end - start);
            uart_sendStr(printData);
            obj[objCount].start = start;
            obj[objCount].end = end;
            obj[objCount].angle = (start + end) / 2;
            obj[objCount].size = end - start;
            objCount++;

            if(objCount == 5) {
                break;
            }
        }

        start = -1;
        end = -1;
        points = 0;
    }
}

float pingObjects(int angle) {
    int i, scans = 10;
//    float cycles_in_milliseconds;
    float avg_dist = 0.0;
    int cycles; // Rising - Falling time of ping
    float cycles_in_milliseconds;
    float distance_cm; // distance to object in cm

    servo_move_degrees(angle);
    timer_waitMillis(1000);

//    IR_SCAN();

    for(i = 0; i < scans; i++) {
//        avg_dist += IR_SCAN();
        cycles = ping_read();

        cycles_in_milliseconds = (float) cycles / 16000.0;

        distance_cm = (float) cycles / 16000000.0; // cycles / clockMHz = seconds between pulse send and receive
        distance_cm /= 2.0; // Only account for seconds to object
        distance_cm *= 34000.0; // Seconds to object * Speed of sound (~340m/s) * 100(cm in m) = cm to object

//        sprintf(data, "CYC:%d\nMILLI:%.7f\nCM:%.2f\nOverflow:%d", cycles, cycles_in_milliseconds, distance_cm, overflow);
//
//
//
//        //        sprintf(data, "%d %d\n%d\n%d", rising_time, falling_time, rising_time - falling_time, falling_time - rising_time);
//        lcd_printf(data);

        avg_dist += distance_cm;

        timer_waitMillis(250);
    }

    return avg_dist / scans;

}

void sizeObjects(struct someObject obj[], float scanData[]) {
    int i = 0;
    int h;
    float theda;
    int angle;
    while(obj[i].angle != -1 && i < 5) {
//        h = scanData[obj[i].start] * 2;
//        angle = (obj[i].angle - obj[i].start) * 2;
//        theda = sin((angle * M_PI) / 180);
//        obj[i].pingDist = pingObjects(obj[i].angle);

        h = scanData[obj[i].start] * 2;
//        angle = (obj[i].angle - obj[i].start) * 2;
        theda = tan((obj[i].size * M_PI) / 360);
        obj[i].pingDist = pingObjects(obj[i].angle);

        obj[i].linearWidth =  h * theda;
        i++;
//        timer_waitMillis(300);

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
        obj[i].avoid = false;
        obj[i].avoidDist = -1;
    }
}

void removeObjects(struct someObject obj[], char printData[]) {
    int i = 0;
//    char buffer[30];

    uart_init();

    while(obj[i].start != -1 && i < 5) {
        if(obj[i].linearWidth <= 7.3) {
            sprintf(printData, "Object %d removed\r\n", i + 1);
            uart_sendStr(printData);
            while(1) {
                uart_sendChar(doSomething);
                if(doSomething) {
                    uart_sendStr("\r\n");
                    doSomething = 0;
                    break;
                }
            }

        } else {
            sprintf(printData, "Object %d immovable and will be avoided\r\n", i + 1);
            uart_sendStr(printData);

            if(obj[i].angle > 5 && obj[i].angle < 175) {
                obj[i].avoid = true;
            }
        }

        i++;
    }
}

int moveToAvoid(struct someObject obj[], oi_t *sensor_data) {
   int i = 0;
   float moveToAvoid = -1;
   bool turnDirection; // left = false, right = true;

   while(obj[i].start != -1 && i < 5) {
       if(obj[i].avoid) {
           // Determines of object is on right or left and which direction to turn to avoid it
           if(obj[i].angle <= 90) {
               moveToAvoid = (obj[i].pingDist * cos(obj[i].start));
               turnDirection = false;
           } else {
               moveToAvoid = (obj[i].pingDist * cos(180 - obj[i].start));
               turnDirection = true;
           }


           if(turnDirection) {
               turn_clockwise(sensor_data, 90);
           } else {
               turn_counterclockwise(sensor_data, 90);
           }
           // Moves bought to avoid object
//           if(moveToAvoid < botSizeCm / 2) {
//               moveToAvoid = (botSizeCm / 2) - moveToAvoid;
//               if(turnDirection) {
//                   turn_clockwise(sensor_data, 90);
//               } else {
//                   turn_counterclockwise(sensor_data, 90);
//               }

//               move_forward(sensor_data, moveToAvoid);
//
//               if(turnDirection) {
//                   turn_counterclockwise(sensor_data, 90);
//               } else {
//                   turn_clockwise(sensor_data, 90);
//               }
//           }
           return i;
       }
       i++;
   }

   return 2;
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
