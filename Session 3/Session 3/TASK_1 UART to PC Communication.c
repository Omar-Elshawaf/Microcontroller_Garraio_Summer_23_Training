#include "inc/tm4c123gh6pm.h"
#include <stdlib.h>
#include <stdint.h>


void UART_INIT (void) ;
void PORTF_INIT (void) ;
void printString(char *s);
void uartTransmit(char c);
char uartReceive(void);
void delay(void);


int main()
{
    UART_INIT () ;
    PORTF_INIT() ;

    while (1)
    {
        printString("If you want to blink the LEDs? choose what to blink from the following : [R,B,G,W]\n\r");
    char answer = uartReceive();


     if (answer == 'R' || answer == 'r' )
        GPIO_PORTF_DATA_R = 0x2; // turn on the red LED
     else if (answer == 'B' || answer == 'b' )
         GPIO_PORTF_DATA_R = 0x4; // turn on the blue LED
     else if (answer == 'G' || answer == 'g' )
         GPIO_PORTF_DATA_R = 0x8; // turn on the green LED
     else if (answer == 'W' || answer == 'w' )
         GPIO_PORTF_DATA_R = 0xE; // turn on the white LED
     else
         GPIO_PORTF_DATA_R &= ~ 0xE; // Turn off the LEDs


    printString(&answer);
    printString("\n\r");

    }
}

void UART_INIT (void)
{
    //Configure Port A for UART
       SYSCTL_RCGCUART_R |= 0x1; // Enable the UART0 Module
       SYSCTL_RCGCGPIO_R |= 0x1; // Enable the clock to Port A since UART0 uses PA0 for RX and PA1 for TX
       GPIO_PORTA_DEN_R |= 0x3; // Enable digital functions for PA0 and PA1
       GPIO_PORTA_AFSEL_R |= 0x3; // Enable alternate function for PA0 and PA1
       GPIO_PORTA_PCTL_R |= 0x11; // Assign UART signals to PA0 and PA1
       UART0_CTL_R = 0x0; // Disable the UART
       //BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
       //BRD=16,000,000/16 * 9600 = 104.16666666
       //UARTFBRD=0.166667 * 64 + 0.5 = 11
       UART0_IBRD_R = 104; // Write the integer portion for 9600 baud rate
       UART0_FBRD_R = 11; // Write the fractional portion for 9600 baud rate
       UART0_LCRH_R = 0x60; // Configure UART for data length of 8 bits, no parity bit, one stop bit.
       UART0_CC_R = 0x0; // Select the UART Baud Clock Source as system clock 16 MHZ
       UART0_CTL_R = 0x301; // Enable the UART, RX and TX sections 0x301=Enable bit 0,8 for TX,9 for RX
}
void PORTF_INIT (void)
{
    //Configure Port F
       SYSCTL_RCGCGPIO_R |= 0x20; // Enable the clock to the PORTF since LEDs are connected to PF1, PF2 and PF3
       GPIO_PORTF_DIR_R |= 0xE; // Configure LED pins to be an output
       GPIO_PORTF_DEN_R |= 0xE; // Enable digital functions for LED pins
       GPIO_PORTF_DATA_R &= ~ 0xE; // Turn off the LEDs
}


void printString(char *s)
{
    while (*s != NULL)
    {
        uartTransmit(*(s++));
    }
}

void uartTransmit(char c)
{
    while (UART0_FR_R & (1 << 5)); // if TX holding register is full, wait until it becomes empty
    UART0_DR_R = c;
}

char uartReceive()
{
    while (UART0_FR_R & (1 << 4)); // if RX holding register is empty, wait until it becomes full
    char data = UART0_DR_R;
    return data;
}

