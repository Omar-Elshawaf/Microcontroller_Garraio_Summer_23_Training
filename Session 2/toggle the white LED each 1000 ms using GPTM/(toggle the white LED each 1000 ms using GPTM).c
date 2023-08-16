#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define LED_WHITE ((1U << 1) | (1U << 2) | (1U << 3))

void GPIOinit()
{
    SYSCTL_RCGCGPIO_R |= (1U << 5);  /* enable clock for GPIOF */
    GPIO_PORTF_DIR_R |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_BLUE | LED_GREEN);
}

void TimerInit(int time)
{
    // Enable GPT0 clock
    SYSCTL_RCGCTIMER_R |= (1 << 0);

    // Check for the clock signal is enabled
    while (SYSCTL_PRTIMER_R & 0x01 == 0x00){}

    // Disable GPT0
    TIMER0_CTL_R &= ~(1 << 0);

    // Set GPT0 to 32-bit mode
    TIMER0_CFG_R = 0x00;

    // Set GPT0 to periodic mode
    TIMER0_TAMR_R |= (0x2 << 0);

    // Generate timer to 1 sec
    TIMER0_TAILR_R = (time/(1.0/16000000)) -1 ;

    // Set timer count down
    TIMER0_TAMR_R &= ~(1 << 4);

    // Enable GPT0 to start count
    TIMER0_CTL_R |= (1 << 0);
}

int main()
{
    GPIOinit();
    TimerInit(1);

    while (1)
    {
        if ((TIMER0_RIS_R & 0x01) == 1) // If Timer A has timed out
        {
            TIMER0_ICR_R |= (1 << 0); // Clear Flag
            GPIO_PORTF_DATA_R ^= LED_WHITE;   // TOGGLE WHITE LED
        }
    }
}
