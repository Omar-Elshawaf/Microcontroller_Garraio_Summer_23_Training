#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/tm4c123gh6pm.h"
#include "utils/uartstdio.h"

#define LED_RED_CAR_TRAFFIC1   (1U << 1)   // E
#define LED_YELLOW_CAR_TRAFFIC1  (1U << 2)
#define LED_GREEN_CAR_TRAFFIC1 (1U << 3)

#define LED_RED_CAR_TRAFFIC2   (1U << 1) // B
#define LED_YELLOW_CAR_TRAFFIC2  (1U << 2)
#define LED_GREEN_CAR_TRAFFIC2 (1U << 3)

#define PEDESTRIAN_RED1   (1U << 6)    // C
#define PEDESTRIAN_GREEN1  (1U << 7)

#define PEDESTRIAN_RED2   (1U << 6) //D
#define PEDESTRIAN_GREEN2  (1U << 2)

#define sw1 (1U << 0)
#define sw2 (1U << 4)

int counter = 0 ;
volatile uint32_t systickCounter = 0 ;
volatile uint32_t printing = 0 ;

void uart_print (void)
{
    // Set the system clock to 16MHz
        SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

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

void delayMs(int n)
{
    int32_t i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 670; j++)
            {}  /* do nothing for 1 ms */
}

void TimerInit(int TIME)
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
    TIMER0_TAILR_R = (TIME / (1.0/16000000)) -1 ;

    // Set timer count down
    TIMER0_TAMR_R &= ~(1 << 4);

    // Enable GPT0 to start count
    TIMER0_CTL_R |= (1 << 0);
}


void GPIOFinit ()
{
    SYSCTL_RCGCGPIO_R |= (1U << 5);  /* enable clock for GPIOF */
    GPIO_PORTF_LOCK_R |=0x4C4F434B;     //Unclock Code
    GPIO_PORTF_CR_R |= (sw1 | sw2);     //Commit Register
    GPIO_PORTF_DIR_R &= ~(sw1 | sw2);
    GPIO_PORTF_DEN_R |= (sw1 | sw2);
    GPIO_PORTF_PUR_R |= (sw1 | sw2);   //Configure Pull-up resistor on PF0 and PF4
    //  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    NVIC_EN0_R = 1 << 30;  // Enable IRQ30
}

void GPIOCinit()
{
    SYSCTL_RCGCGPIO_R |= (1U << 2);  /* enable clock for GPIOC */
    GPIO_PORTC_DIR_R |= (PEDESTRIAN_RED1 | PEDESTRIAN_GREEN1);
    GPIO_PORTC_DEN_R |= (PEDESTRIAN_RED1 | PEDESTRIAN_GREEN1);
}

void GPIODinit()
{

    SYSCTL_RCGCGPIO_R |= (1U << 3);  /* enable clock for GPIOD */
    GPIO_PORTD_DIR_R |= ( PEDESTRIAN_RED2 | PEDESTRIAN_GREEN2 );
    GPIO_PORTD_DEN_R |= ( PEDESTRIAN_RED2 | PEDESTRIAN_GREEN2);
}

void GPIOEinit()
{
    SYSCTL_RCGCGPIO_R |= (1U << 4);  /* enable clock for GPIOE */
    GPIO_PORTE_DIR_R |= (LED_RED_CAR_TRAFFIC1 | LED_YELLOW_CAR_TRAFFIC1 | LED_GREEN_CAR_TRAFFIC1);
    GPIO_PORTE_DEN_R |= (LED_RED_CAR_TRAFFIC1 | LED_YELLOW_CAR_TRAFFIC1 | LED_GREEN_CAR_TRAFFIC1);
}
void GPIOBinit()
{

    SYSCTL_RCGCGPIO_R |= (1U << 1);  /* enable clock for GPIOB */
    GPIO_PORTB_DIR_R |= (LED_RED_CAR_TRAFFIC2 | LED_YELLOW_CAR_TRAFFIC2 | LED_GREEN_CAR_TRAFFIC2  );
    GPIO_PORTB_DEN_R |= (LED_RED_CAR_TRAFFIC2 | LED_YELLOW_CAR_TRAFFIC2 | LED_GREEN_CAR_TRAFFIC2 );
}

void Traffic_Light_only (void)
{
    while (1)
         {
         if ((TIMER0_RIS_R & 0x01) == 1 && counter ==0 ) // If Timer A has timed out
             {
               TIMER0_ICR_R |= (1 << 0); // Clear Flag
              GPIO_PORTE_DATA_R = LED_GREEN_CAR_TRAFFIC1 ;
              GPIO_PORTC_DATA_R=PEDESTRIAN_RED1;
              GPIO_PORTB_DATA_R =  LED_RED_CAR_TRAFFIC2;
              GPIO_PORTD_DATA_R = PEDESTRIAN_GREEN2 ;

          while (printing == 0)    // for printing only one time
          {
              UARTprintf("Traffic 1 is green & Traffic 2 is red "); UARTprintf("\n");

              UARTprintf("PEDESTRIAN Traffic 1 is red & PEDESTRIAN Traffic 2 is green  "); UARTprintf("\n");

              UARTprintf("------------------------------------------------ "); UARTprintf("\n");
              printing ++ ;
          }


              systickCounter ++ ;

              if (systickCounter >= 5)
                  {

                    systickCounter = 0; // Reset the counter
                    GPIO_PORTE_DATA_R = LED_YELLOW_CAR_TRAFFIC1;  //PE2 YELLOW
                    UARTprintf("Traffic 1 becomes yellow !! "); UARTprintf("\n");

                    UARTprintf("------------------------------------------------ "); UARTprintf("\n");

                    counter ++ ;
                    printing = 0 ;
                  }
             }
           else if ((TIMER0_RIS_R & 0x01) == 1 && counter ==1 ) // If Timer A has timed out
             {
               TIMER0_ICR_R |= (1 << 0); // Clear Flag
               systickCounter ++ ;
               if (systickCounter >= 2)
                  {
                   systickCounter = 0; // Reset the counter
                   GPIO_PORTE_DATA_R = LED_RED_CAR_TRAFFIC1; //PE1  WHITE
                   GPIO_PORTC_DATA_R=PEDESTRIAN_GREEN1;//on board LED on

                        counter ++ ;

                        UARTprintf("Traffic 1 is red & Traffic 2 is red "); UARTprintf("\n");

                        UARTprintf("PEDESTRIAN Traffic 1 is green & PEDESTRIAN Traffic 2 is green  "); UARTprintf("\n");

                        UARTprintf("------------------------------------------------ "); UARTprintf("\n");
                  }
            }

           else if ((TIMER0_RIS_R & 0x01) == 1 && counter ==2 ) // If Timer A has timed out
            {

            TIMER0_ICR_R |= (1 << 0); // Clear Flag
            counter ++ ;
            }
          /* -------------------------------------------------------------------------------*/
           else if ((TIMER0_RIS_R & 0x01) == 1 && counter ==3 ) // If Timer A has timed out
                 {
          //     GPIO_PORTE_DATA_R = 0x00 ;
                   TIMER0_ICR_R |= (1 << 0); // Clear Flag
               GPIO_PORTB_DATA_R = LED_GREEN_CAR_TRAFFIC2;
               GPIO_PORTD_DATA_R = PEDESTRIAN_RED2 ;
                   systickCounter ++ ;
                   while (printing == 0)
                   {
                   UARTprintf("Traffic 1 is red & Traffic 2 is green "); UARTprintf("\n");

                   UARTprintf("PEDESTRIAN Traffic 1 is green & PEDESTRIAN Traffic 2 is red  "); UARTprintf("\n");
                   UARTprintf("------------------------------------------------ "); UARTprintf("\n");
                   printing ++ ;
                   }
                   if (systickCounter >= 5)
                      {
                       systickCounter = 0; // Reset the counter
                       GPIO_PORTB_DATA_R = LED_YELLOW_CAR_TRAFFIC2; //PB2  YELLOW
                       UARTprintf("Traffic 2 becomes yellow !! "); UARTprintf("\n");
                       UARTprintf("------------------------------------------------ "); UARTprintf("\n");
                            counter ++ ;
                            printing = 0 ;
                      }}
           else if ((TIMER0_RIS_R & 0x01) == 1 && counter ==4 ) // If Timer A has timed out
                 {
                  TIMER0_ICR_R |= (1 << 0); // Clear Flag
                  systickCounter ++ ;
                  if (systickCounter >= 2)
                     {
                      systickCounter = 0; // Reset the counter
                      GPIO_PORTB_DATA_R = LED_RED_CAR_TRAFFIC2;
                     GPIO_PORTD_DATA_R = PEDESTRIAN_GREEN2 ;
                     UARTprintf("Traffic 1 is red & Traffic 2 is red "); UARTprintf("\n");

                    UARTprintf("PEDESTRIAN Traffic 1 is green & PEDESTRIAN Traffic 2 is green  "); UARTprintf("\n");
                    UARTprintf("------------------------------------------------ "); UARTprintf("\n");
                      counter ++ ;
                     }
                 }
           else if ((TIMER0_RIS_R & 0x01) == 1 && counter ==5 ) // If Timer A has timed out
                 {
                  TIMER0_ICR_R |= (1 << 0); // Clear Flag
                 counter =0 ;
                 }

     }
   }


void Interrupt (void)
{

    delayMs(300) ;  // for Debouncing
      if (GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_PIN_4) // Check if interrupt is caused by PF4/SW1
        {
            GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); // Clear the interrupt flag
            if (counter == 0 )
            {
            GPIO_PORTC_DATA_R=PEDESTRIAN_GREEN1;
            GPIO_PORTE_DATA_R = LED_RED_CAR_TRAFFIC1;
            UARTprintf("PEDESTRIAN Traffic 1 becomes green & Traffic 1 becomes red "); UARTprintf("\n");
            UARTprintf("************************************************************ "); UARTprintf("\n");
            delayMs(4000) ;
            }
            else if (counter == 1)
            {
                UARTprintf("Wait 2 seconds !!!  "); UARTprintf("\n");
                UARTprintf("************************************************************ "); UARTprintf("\n");
            }
            else if ( counter == 2 || counter == 3 || counter == 4 )
            {
                UARTprintf("You can already pass !!  "); UARTprintf("\n");
                UARTprintf("************************************************************ "); UARTprintf("\n");
            }
         }

       else if (GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_PIN_0) // Check if interrupt is caused by PF0/SW2
        {
           GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0); // Clear the interrupt flag
           if (counter == 3)
           {
                     GPIO_PORTD_DATA_R=PEDESTRIAN_GREEN2;
                     GPIO_PORTB_DATA_R = LED_RED_CAR_TRAFFIC2;
                     UARTprintf("PEDESTRIAN Traffic 2 becomes green & Traffic 2 becomes red "); UARTprintf("\n");
                     UARTprintf("************************************************************ "); UARTprintf("\n");
                     delayMs(4000) ;
           }
           else if (counter == 4)
           {
               UARTprintf("Wait 2 seconds !!!  "); UARTprintf("\n");
               UARTprintf("************************************************************ "); UARTprintf("\n");
           }

           else  if ( counter == 0 || counter == 1 || counter == 5 )
           {
                 UARTprintf("You can already pass !!  "); UARTprintf("\n");
                 UARTprintf("************************************************************ "); UARTprintf("\n");
           }
        }

       else if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_PIN_4) && (GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_PIN_0) )
       {
           GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); // Clear the interrupt flag
           GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0); // Clear the interrupt flag
                     if (counter == 0 )
                     {
                     GPIO_PORTC_DATA_R=PEDESTRIAN_GREEN1;
                     GPIO_PORTE_DATA_R = LED_RED_CAR_TRAFFIC1;
                     UARTprintf("PEDESTRIAN Traffic 1 becomes green & Traffic 1 becomes red "); UARTprintf("\n");
                     }
                     else if (counter == 1)
                     {
                         UARTprintf("Wait 2 seconds !!!  "); UARTprintf("\n");
                     }

                     else if ( counter == 2 || counter == 3 || counter == 4 )
                     {
                         UARTprintf("You can already pass !!  "); UARTprintf("\n");
                     }

                     else if (counter == 3)
                                 {
                                           GPIO_PORTD_DATA_R=PEDESTRIAN_GREEN2;
                                           GPIO_PORTB_DATA_R = LED_RED_CAR_TRAFFIC2;
                                           UARTprintf("PEDESTRIAN Traffic 2 becomes green & Traffic 2 becomes red "); UARTprintf("\n");


                                 }
                     else if (counter == 4)
                     {
                                     UARTprintf("Wait 2 seconds !!!  "); UARTprintf("\n");
                     }


                     else if ( counter == 0 || counter == 1 || counter == 5 )
                     {
                                       UARTprintf("You can already pass !!  "); UARTprintf("\n");
                     }
                                       delayMs(4000) ;
       }



}



void GPIOPortF_Handler(void)
{


    Interrupt() ;

}
int main()
{
    uart_print ();
    GPIOCinit();
    GPIOEinit();
    GPIOBinit();
    GPIOFinit();
    GPIODinit();
    TimerInit(1);
    Traffic_Light_only() ;
}
