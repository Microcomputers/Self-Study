#include <msp430fr5739.h>
#include "system.h"
#include "temp.h"
#include "uart.h"
/*
 	main.c
	1. Switches and LEDs
	2.
	3. Temp sens
 */	

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
	unsigned int tempVal = 0;   // temp readings

	LEDs_INIT(); 				// Init leds
	SWITCHEs_INIT(); 			// Init Switches	
	serialBegin();				// Begin Serial communication at 9600 buad
	
	while (1)
	{
		__bis_SR_register(LPM4_bits + GIE);	// Enter LPM4 with interrupts enabled
		tempVal = getThermisterVal();
		serialWrite();
	}
	return 0;
}



