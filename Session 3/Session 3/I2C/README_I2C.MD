There was a problem during working on ths ccode t every switch press we need to press reset button to make the second Tiva blink led and that was wrong, it must be automatic. the error was in Not configuring of pull up resistor for connected pins to slave tiva c board PORTB .

IN SLAVE CODE:
 // Configure the switch pins with pull-up resistors
  //  GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
   // GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
