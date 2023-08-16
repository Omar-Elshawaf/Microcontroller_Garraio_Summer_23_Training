#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


void UART_INIT(void);
void ADC_INIT(void);
void PORTF_INIT(void);
unsigned int ADC_VALEU; // Variable to store the ADC result
uint8_t voltage;


int main()
{
    UART_INIT();
    ADC_INIT();
    PORTF_INIT();

    while (1)
    {
        ADC0_PSSI_R |= (1 << 3); // Start sampling by setting bit 3 of the ADCPSSI register
        while ((ADC0_RIS_R & 0x8) == 0); // Wait until the raw interrupt status bit for SS3 (bit 3) is set

        ADC_VALEU = ADC0_SSFIFO3_R; // Read the ADC result from the FIFO buffer
        ADC0_ISC_R |= (1 << 3);     // Clear the interrupt status for SS3 to acknowledge the interrupt

        // Check the ADC result and control the LED accordingly
        if (ADC_VALEU > 2048)
        {
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2) | (1 << 3 ) ; // Turn on the LED connected to PF1

        }
        else
        {
            GPIO_PORTF_DATA_R &= ~((1 << 1) | (1 << 2) | (1 << 3 )); // Turn off the LED connected to PF1
        }

       // Convert digital value back into voltage
       voltage = (uint8_t)(ADC_VALEU * 0.0008);

       UARTprintf("Voltage = %d", voltage);
       UARTprintf("\n");
    }
}

void UART_INIT(void)
{
     // Set the system clock to 50MHz
      SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

      // Enable UART0 and GPIOA
      SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

      // Configure GPIO Pins for UART mode
      GPIOPinConfigure(GPIO_PA0_U0RX);
      GPIOPinConfigure(GPIO_PA1_U0TX);
      GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

      // Initialize UART
      UARTStdioConfig(0, 115200, SysCtlClockGet());
}
void ADC_INIT(void)
{
       // Configure ADC0
       SYSCTL_RCGCADC_R |= (1 << 0);    // Enable the ADC module 0
       SYSCTL_RCGCGPIO_R |= (1 << 4);   // Enable the clock to Port E
       GPIO_PORTE_AFSEL_R |= (1 << 3);  // Enable alternate function for PE3 (AIN0)
       GPIO_PORTE_DEN_R &= ~(1 << 3);  // Clearing bit 3 to disable digital function for PE3
       GPIO_PORTE_AMSEL_R |= (1 << 3); // Enable analog function for PE3 (AIN0)

       // Configuration of Sample Sequencer for ADC Channel
       ADC0_ACTSS_R &= ~(1 << 3); // Disable SS3 during configuration
       ADC0_EMUX_R &= ~0xF000;   // Clear bits 15-12 to select software trigger
       ADC0_SSMUX3_R = 0x0;      // Set SS3 to sample from AIN0 (PE3)
       ADC0_SSCTL3_R |= (1 << 1) | (1 << 2); // Set bits 1 and 2 for the 1st sample and enable interrupt
       ADC0_ACTSS_R |= (1 << 3); // Enable ADC0 sequencer 3
}
void PORTF_INIT(void)
{
        // Configure Port F for LED control
        SYSCTL_RCGCGPIO_R |= 0x20; // Enable the clock to PORTF since LEDs are connected to PF1, PF2, and PF3
        GPIO_PORTF_DIR_R |= 0xE;   // Configure LED pins (PF1, PF2, PF3) as outputs
        GPIO_PORTF_DEN_R |= 0xE;   // Enable digital functions for LED pins
        GPIO_PORTF_DATA_R &= ~0xE; // Turn off the LEDs initially
}
