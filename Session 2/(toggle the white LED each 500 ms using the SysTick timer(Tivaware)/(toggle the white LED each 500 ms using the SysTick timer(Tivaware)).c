#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"

#define LED_WHITE (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)

void SysTick_Handler(void)
{
    int status = GPIOPinRead(GPIO_PORTF_BASE, LED_WHITE);
    GPIOPinWrite(GPIO_PORTF_BASE, LED_WHITE, status^LED_WHITE); //toggling
}


int main(void)
{
    //volatile uint32_t count;

    // Set the system clock frequency
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_OSC_MAIN | SYSCTL_USE_OSC | SYSCTL_XTAL_16MHZ);

    //Enable Port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_WHITE);

    //Generate interrupt at 0.5 sec
    SysTickPeriodSet(8000000-1);

    //Enable the systic interrupt
    SysTickIntEnable();

    //Enables the SysTick counter
    SysTickEnable();
}
