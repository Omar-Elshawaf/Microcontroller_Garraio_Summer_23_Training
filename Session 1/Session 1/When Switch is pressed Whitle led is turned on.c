#include <stdint.h>
#include "tm4c123gh6pm.h"

#define sw1 (1U << 0)
#define sw2 (1U << 4)
#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define LED_WHITE ((1U << 1) | (1U << 2) | (1U << 3))

void GPIOinit ()
{
   SYSCTL_RCGCGPIO_R |= (1U << 5);  /* enable clock for GPIOF */
    GPIO_PORTF_LOCK_R |=0x4C4F434B;     //Unclock Code
    GPIO_PORTF_CR_R |= (sw1 | sw2);     //Commit Register
    GPIO_PORTF_DIR_R |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_DIR_R &= ~(sw1 | sw2);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_BLUE | LED_GREEN | sw1 | sw2);
    GPIO_PORTF_PUR_R |= (sw1 | sw2);   //Configure Pull-up resistor on PF0 and PF4
}

int main() {
  
    GPIOinit();

    while(1)
	{
	 int value1=GPIO_PORTF_DATA_R & sw1;  //read the status of switch1
          int value2=GPIO_PORTF_DATA_R & sw2;  //read the status of switch2
  	  if(value1==0 || value2==0)
  	  {
	  	  GPIO_PORTF_DATA_R=LED_WHITE;//on board LED on
   	   }
  	  else
    	{
	  	  GPIO_PORTF_DATA_R=0;//on board LED off
    	}
	}
}
