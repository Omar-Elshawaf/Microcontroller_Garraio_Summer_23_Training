#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "driverlib/i2c.h"
#include "inc/hw_nvic.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

// Define the I2C slave address
#define SLAVE_ADDRESS 0x3C

// Function to initialize the UART console for printing
void InitConsole(void) {
    // Enable UART GPIO peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configure UART pins
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    // Enable UART module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Set UART clock source and configure pins
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Configure UART for console I/O
    UARTStdioConfig(0, 115200, 16000000);
}

// Function to initialize I2C
void I2C0_Init(void) {
    // Enable I2C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    // Enable GPIO peripheral for I2C pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configure I2C pins
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Set pin types for I2C
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    // Initialize I2C master with specified clock speed
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
}

// Function to initialize GPIO for the switch
void GPIO_Init(void) {
    // Enable GPIO peripheral for switch
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Configure switch pin as input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    // Configure pull-up resistor for switch pin
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

int main(void) {
    // Initialize system clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initialize I2C
    I2C0_Init();

    // Initialize GPIO for switch
    GPIO_Init();

    // Initialize UART console for printing
    InitConsole();

    // Initialize I2C master again (duplicate line, consider removing)
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // Enable global interrupts
    IntMasterEnable();
    // Set priority for I2C interrupt
    IntPrioritySet(INT_I2C0, 0);

    // Infinite loop
    while (1) {
        // Check if the switch is pressed
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0) {
            // Print message indicating switch press
            UARTprintf("Switch pressed, sending command to slave...\n");

            // Set the I2C slave address and indicate a write operation
            I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);
            // Put data to be sent in the data register
            I2CMasterDataPut(I2C0_BASE, 'B');
            // Send a single data byte
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

            // Wait until the I2C master is not busy
            while (I2CMasterBusy(I2C0_BASE)) {
            }

            // Delay to avoid multiple commands due to switch debounce
            SysCtlDelay(SysCtlClockGet() / 3);
        }
    }
}
