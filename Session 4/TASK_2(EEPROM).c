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
#include "driverlib/pin_map.h"
#include "driverlib/eeprom.h"
#include "driverlib/flash.h"

int main()
{
    uint32_t Data[2]; // Array to hold data for writing to EEPROM
    uint32_t Read[2]; // Array to hold data read from EEPROM

    Data[0] = 0x1;    // Initialize data to be written
    Data[1] = 0x2;

    // Configure system clock settings
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ);
    SysCtlDelay(20000000); // Introduce a delay

    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0); // Enable EEPROM peripheral

    // Initialize EEPROM module to prepare it for use
    EEPROMInit();

    while (1)
    {
    // Perform a mass erase operation on EEPROM memory
    EEPROMMassErase();

    // Read data from EEPROM memory and store in Read array
    EEPROMRead(Read, 0x0, sizeof(Read));

    // Write data from Data array to EEPROM memory
    EEPROMProgram(Data, 0x0, sizeof(Data));

    // Read data from EEPROM memory again to verify write operation
    EEPROMRead(Read, 0x0, sizeof(Read));

    // Erase the Read array by filling it with zeros
    int i;
    for ( i = 0; i < sizeof(Read) / sizeof(Read[0]); i++)
        {
        Read[i] = 0;
        }
    }
}
