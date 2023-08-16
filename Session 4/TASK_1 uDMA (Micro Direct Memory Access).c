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
#include "driverlib/udma.h"

int main()
{
    // Create a control table in memory to manage DMA transfers
    uint8_t DMA_Control_Table[1024];

    // Define source and destination buffers for DMA transfer
    uint8_t Source_Buffer[256] = "DMA Test";
    uint8_t Destination_Buffer[256];

    // Enable uDMA (Micro Direct Memory Access) peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);

    // Wait until uDMA peripheral is ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA));

    // Enable the uDMA controller
    uDMAEnable();

    // Set the base address of the control table for uDMA transfers
    uDMAControlBaseSet(&DMA_Control_Table[0]);

    // Disable attributes for the uDMA software channel
    uDMAChannelAttributeDisable(UDMA_CHANNEL_SW, UDMA_ATTR_ALL);

    // Configure uDMA channel control for software transfer
    uDMAChannelControlSet(UDMA_CHANNEL_SW | UDMA_PRI_SELECT,
                          UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_8 | UDMA_ARB_8);

    // Set up uDMA channel transfer parameters
    uDMAChannelTransferSet(UDMA_CHANNEL_SW | UDMA_PRI_SELECT,
                           UDMA_MODE_AUTO, Source_Buffer, Destination_Buffer, sizeof(Destination_Buffer));

    // Enable and initiate the uDMA software channel
    uDMAChannelEnable(UDMA_CHANNEL_SW);
    uDMAChannelRequest(UDMA_CHANNEL_SW);

    // Main loop
    while (1)
    {
        // Code execution will remain in this loop
        // as the DMA transfer is being performed
    }
}
