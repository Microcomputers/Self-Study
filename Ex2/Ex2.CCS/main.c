#include <msp430fr5739.h>
#include "system.h"
//#include "temp.h"

/*
 	main.c
	1. Switches and LEDs
	2.
	3. Temp sens
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
	LEDs_INIT(); 				// Init leds
	SWITCHEs_INIT(); 			// Init Switches

	while (1)
	{
		__bis_SR_register(LPM4_bits + GIE);	// Enter LPM4 with interrupts enabled
	}
	return 0;
}



