#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Define the slave address
#define SLAVE_ADDRESS 0x3C

// Flag to indicate if the switch is pressed
static bool SwitchPressed = false;

// Initialize I2C0 peripheral for slave operation
void I2C0_Init(void) {
    // Enable the I2C0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    // Enable the GPIO peripheral for I2C0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin multiplexing for I2C0
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Configure the pin types for I2C0
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable I2C slave mode and initialize with the given address
    I2CSlaveEnable(I2C0_BASE);
    I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);
    I2CSlaveIntEnableEx(I2C0_BASE, I2C_SLAVE_INT_DATA);
}

// Initialize the UART console
void InitConsole(void) {
    // Enable the UART0 and GPIOA peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure the pin multiplexing for UART0
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Configure the pin types for UART0
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Set UART clock source and configure UART standard I/O
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

// Initialize GPIO for LED and switch
void GPIO_Init(void) {
    // Enable the GPIOF peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the LED pin as an output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Configure the switch pins with pull-up resistors
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// I2C0 slave interrupt handler
void I2C0SlaveIntHandler(void) {
    // Get the interrupt status
    uint32_t ui32Status = I2CSlaveIntStatus(I2C0_BASE, true);

    // Clear the GPIO interrupt flags
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

    // Check if the interrupt is for data reception
    if (ui32Status == I2C_SLAVE_INT_DATA) {
        // Clear the I2C slave interrupt
        I2CSlaveIntClear(I2C0_BASE);
        // Read the received data
        uint32_t ui32DataRx = I2CSlaveDataGet(I2C0_BASE);
        // Set the switch pressed flag
        SwitchPressed = true;
    }
}

int main(void) {
    // Initialize the system clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initialize I2C
    I2C0_Init();

    // Initialize LED and switch
    GPIO_Init();

    // Initialize UART console
    InitConsole();

    // Enable global interrupts
    IntMasterEnable();

    // Enable I2C slave interrupt
    IntEnable(INT_I2C0);
    I2CSlaveIntEnable(I2C0_BASE);

    while (1) {
        // Check if the switch is pressed
        if (SwitchPressed) {
            // Print a message and toggle the LED
            UARTprintf("Received command from master, blinking LED...\n");
            int status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, status ^ GPIO_PIN_2);
            // Clear the switch pressed flag
            SwitchPressed = false;
        }
    }
}
