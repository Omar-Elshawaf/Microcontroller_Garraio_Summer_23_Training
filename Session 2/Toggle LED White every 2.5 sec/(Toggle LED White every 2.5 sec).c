#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define LED_WHITE ((1U << 1) | (1U << 2) | (1U << 3))

void SysTick_Handler(void)
{
    // Toggle LED_WHITE
    GPIO_PORTF_DATA_R ^= LED_WHITE;
}

void GPIOinit ()
{
    SYSCTL_RCGCGPIO_R |= (1U << 5);  /* enable clock for GPIOF */
    GPIO_PORTF_DIR_R |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_BLUE | LED_GREEN);
}

int main() {

    GPIOinit();

    /* Configure SysTick */
    NVIC_ST_CTRL_R = 0x03;          // enable it, interrupt, use system clock at 4 MHZ
    NVIC_ST_RELOAD_R = 10000000-1;  //Generate interrupt at 2.5 sec --->LOAD=TIME_DELAY/(1/FREQUENCY)
                                    //LOAD=2.5/(1/4,000,000)=10,000,000

    while(1)
    {
          // this is an infinity loop till the interrupt happens
    }
}
