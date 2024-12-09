/*
*
*   uart_extra_help.c
* Description: This is file is meant for those that would like a little
*              extra help with formatting their code, and following the Datasheet.
*/

#include "uart_extra_help.h"

#define REPLACE_ME 0x00

volatile char uart_data;
volatile char flag = 0;

void uart_init()
{
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCUART_R |= 0x02;
    GPIO_PORTB_AFSEL_R |= 0b00000011;
    GPIO_PORTB_PCTL_R |= 0x00000011;
    GPIO_PORTB_DEN_R |= 0x03;
    GPIO_PORTB_DIR_R |= 0b00000010;

    double fbrd;
    int ibrd;

    fbrd = 16000000.0 / (16.0 * 115200.0);
    ibrd = floor(fbrd); // 8
    fbrd = (fbrd - ibrd) * 64 + .5;

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b01100000;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R = 0x0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x1;        // enable UART1
}

void uart_sendChar(char data)
{
   while((UART1_FR_R & 0x20) != 0){};

   UART1_DR_R = data;
}

char uart_receive(void)
{
   char rdata;

   while((UART1_FR_R & 0x10)){};
   rdata = (char) (UART1_DR_R & 0xFF);

   return rdata;
 
}

void uart_sendStr(const char *data)
{
    while(*data != '\0'){
        uart_sendChar(*data);
        data++;
    }
    timer_waitMillis(20);
}

// _PART3


void uart_interrupt_init()
{
    UART1_CTL_R &= 0xFFFFFFFE;
    UART1_ICR_R = 0xFFFF;
    UART1_ICR_R = 0x0000;
    UART1_IM_R |= 0x00000010;
    NVIC_PRI1_R |= 0x00200000;
    NVIC_EN0_R |= 0b01000000;
    IntRegister(INT_UART1, UART1_Handler);
    IntMasterEnable();
    UART1_CTL_R |= 0x1;

}

void UART1_Handler(void) {

    if(UART1_MIS_R & 0x10) {
        char input;
        input = UART1_DR_R & 0xFF;
        uart_sendChar(input);

        if(input == 'r') {
            doSomething = 1;
        }

        UART1_ICR_R |= 0x10;
    }

//    doSomething = input;
}
