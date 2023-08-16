#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define LED_WHITE ((1U << 1) | (1U << 2) | (1U << 3))

volatile uint32_t systickCounter = 0;

void SysTick_Handler(void)
{
    systickCounter++; // Increment the SysTick counter

    if (systickCounter >= 10)
    {
        systickCounter = 0; // Reset the counter
        // Toggle LED_WHITE
        GPIO_PORTF_DATA_R ^= LED_WHITE;
    }
}

void GPIOinit()
{
    SYSCTL_RCGCGPIO_R |= (1U << 5);  /* enable clock for GPIOF */
    GPIO_PORTF_DIR_R |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_BLUE | LED_GREEN);
}

int main()
{
    GPIOinit();

    /* Configure SysTick */
    NVIC_ST_CTRL_R = 0x03; // enable it, interrupt, use system clock at 4 MHz
    NVIC_ST_RELOAD_R = 4000000 - 1; // Generate interrupt every 1 second

    while (1)
    {
        // this is an infinite loop until the interrupt happens
    }
}
