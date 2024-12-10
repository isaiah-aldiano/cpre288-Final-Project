


//#include "cyBot_Scan.h"
#include "uart_extra_help.h"
//#include "cyBot_uart.h"
#include "math.h"
#include "movement.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"

float distfromIR(int IR);
void helpSend(char data[]);
void Ascan(void);
void controls(char command, oi_t *sensor_data);


int angle = 0;
int dist = 0;
/**
 * main.c
 */
int main(void)
{

//    right_calibration_value = 337750; cybot 6
//    left_calibration_value = 1356250;

//    right_calibration_value = 337750; //cybot 8
//    left_calibration_value = 1335250;

    //CyBot 3
    //right_calibration_value = 269500;
    //left_calibration_value = 1319500;

    //Initialize oit
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    servo_init();
    ping_init();

    uart_init(115200);
    //cyBot_uart_init();

//    turn_clockwise(sensor_data, 45);
//    timer_waitMillis(400);
//    turn_counterclockwise(sensor_data, 45);

//    int i = 0;
//    int n = 0; //Number of objects
//    int objectStart[25];
//    int objectEnd[25];
//    float data[91];
//    char str[10];

    //cyBOT_init_Scan(0b0100);

    //cyBOT_init_Scan(0b0011);

    //cyBOT_Scan_t scan;

    ADC_init();

//    cyBOT_Scan((0), &scan); //First Scan
//    data[0] = distfromIR(scan.IR_raw_val);
//    data[1] = distfromIR(scan.IR_raw_val);
//    data[2] = distfromIR(scan.IR_raw_val);
//    int average = (data[0] + data[1] + data[2]) / 3;
//    if(average < 60){
//        objectStart[0] = 0;
//    }
//
//    //Scan for data
//    for(i = 1; i <= 90; i++){
//        cyBOT_Scan((i *2), &scan);
//
//        data[i] = distfromIR(scan.IR_raw_val);
//
//        //Send data to file
////        sprintf(str, "%.3f \r", data[i]);
////        helpSend(str);
//
//        if(data[i] < 60 && data[i-1] > 60){
//            objectStart[n] = i * 2;
//        }
//
//        if(data[i] > 60 && data[i-1] < 60){
//            if((i * 2) - objectStart[n] <= 4){
//                data[i] = 50;
//            }
//            objectEnd[n] = (i-1) * 2;
//            n++;
//        }
//
//    }

    int toggle = 1;
    int hCount = 0;
    while(1){
        char instruction = uart_receive();
        uart_sendChar('\n');
//        if(instruction == 't'){
//            toggle = toggle * -1;
//        }

        //Autonomous
        if(toggle < 0){
            if(instruction == 'h' && hCount == 0){
                Ascan();
                hCount ++;
            }
            else if(instruction == 'h' && hCount == 1){
                hCount = 0;

                move_forward(sensor_data, 10);

                if(angle < 90){
                    turn_clockwise(sensor_data, abs(angle - 90));
                } else if(angle > 90){
                    turn_counterclockwise(sensor_data, (angle - 90));
                }

                servo_move(90);
                timer_waitMillis(400);
                servo_move(90);
                move_forward(sensor_data, ping_read()/1.5);
            }
        }

        //Manual
        if(toggle > 0){
            controls(instruction, sensor_data);
        }

    }


//    //Movement of roomba
//    int angle;
//
//    if(midAngle[minObject] < 90 && n >= 1){
//        angle = abs(midAngle[minObject] - 90);
//        turn_clockwise(sensor_data, angle);
//        sprintf(str, "%d \r\n", angle);
//        helpSend(str);
//    } else if (n >= 1){
//        angle = midAngle[minObject] - 90;
//        turn_counterclockwise(sensor_data, angle);
//        sprintf(str, "%d \r\n", angle);
//        helpSend(str);
//    }
//
//    if(n >= 1){
//        move_forward(sensor_data, (objectDist[minObject]/6));
//        sprintf(str, "%d \r\n", objectDist[minObject]);
//        helpSend(str);
//    }

    //oi_free(sensor_data);


}




float distfromIR(int IR){
    //y = 2631.8e-0.022x
    //119.29e-0.001x
    float dist = exp(-0.001 * IR);
    dist *= 119.29;
    return dist;
}





void helpSend(char data[]){
    char i;
    for(i = 0; i < strlen(data); i++){
        uart_sendChar(data[i]);
    }
    //uart_sendChar('\n');
}




void Ascan(void){
    int i = 0;
    int n = 0; //Number of objects
    int objectStart[25];
    int objectEnd[25];
    float data[91];
    char str[20];
    int s1 = 0;
    int s2 = 0;

    //Scan for data
    servo_move(0);
    timer_waitMillis(400);
    for(i = 0; i <= 90; i++){
        servo_move(i*2);
        s1 = ADC_read();
        servo_move(i*2);
        s2 = ADC_read();

        data[i] = (s1 + s2) / 2;
        sprintf(str, "%.3f", data[i]);
        helpSend(str);
    }

    data[90] = 70;
    data[0] = 70;

    //Clean data
    for(i = 1; i <=89; i++){
        float diffBelow = abs(data[i-1] - data[i]);
        float diffAbove = abs(data[i+1] - data[i]);

        //if difference of data above and below is too much, the data is set to the average of above and below
        if((diffBelow > 5) && (diffAbove > 5)){
            data[i] = (data[i+1] + data[i-1]) / 2;
        }
    }

    //Collect object data
    if(data[0] < 45 && data[1] < 45){
       objectStart[n] = 0;
    }

    for(i = 1; i <= 90; i++){
        if(data[i] < 45){
            if(data[i-1] > 45){
                //helpSend("Object detected \n\r");
                objectStart[n] = i * 2;
            } else if(data[i+1] > 45){
                objectEnd[n] = i * 2;
                n++;
            }
        }
    }

    int midAngle[25];
    int objectDist[25];
    float linearWidth [25];

    //Determine width of objects
    for(i = 0; i < n; i++){

        midAngle[i] = (objectEnd[i] + objectStart[i])/2; //Calculate the middle angle of each object

        servo_move(midAngle[i]);
        //cyBOT_Scan((midAngle[i]), &scan);
        timer_waitMillis(400);
        servo_move(midAngle[i]);

        objectDist[i] = ping_read();

        int width = objectEnd[i] - objectStart[i];
        linearWidth[i] = objectDist[i] * width * (3.1415926/ 180); //radius * angle (in radians)
        sprintf(str, "%.3f", linearWidth[i]);
        //helpSend(str);
    }

    //Determine smallest object
    float minLinWidth = 100;
    int minObject = 0;
    for(i = 0; i < n; i++){
        if(linearWidth[i] <= minLinWidth && linearWidth[i] > 2 && linearWidth[i] < 120){
            minLinWidth = linearWidth[i];
            minObject = i;

        }
    }

    //Point scanner at smallest object
//    cyBOT_Scan(midAngle[minObject], &scan);
//    timer_waitMillis(400);
//    cyBOT_Scan(midAngle[minObject], &scan);

    dist = objectDist[minObject]/1.5;
    angle = midAngle[minObject];

    sprintf(str, "END");
    helpSend(str);

    if(angle <= 90){
        //sprintf(str, "Turn right %d", abs(angle - 90));
    } else if (angle > 90){
        //sprintf(str, "Turn Left %d", (angle - 90));
    }
    //helpSend(str);
}





void controls(char command, oi_t *sensor_data){
    char data[300];
    char degrees[300];
    float irDist;
    int i;
    switch (command){
        case 'm':
            servo_move(0);
            timer_waitMillis(100);
            for(i = 0; i <= 180; i+= 2){
                timer_waitMillis(20);
                servo_move(i);

                timer_waitMillis(20);

                //Send current degrees
                sprintf(degrees, "%d           ", i);
                helpSend(degrees);

                irDist = ADC_read();

                //Copy float data to a string
                sprintf(data, "%.3f\n", irDist);
                helpSend(data);
            }
            sprintf(data, "END");
            helpSend(data);
            break;
        case 'w':
            move_forward(sensor_data, 20);
            break;
        case 'a':
            turn_counterclockwise(sensor_data, 15);
            break;
        case 's':
            move_backwards(sensor_data, 30);
            break;
        case 'd':
            turn_clockwise(sensor_data, 15);
            break;
        case 'f':
            oi_free(sensor_data);
            break;
        default:
//            lcd_printf("Ew");
//            uart_sendChar('E');
//            uart_sendChar('w');
            break;
    }
}

