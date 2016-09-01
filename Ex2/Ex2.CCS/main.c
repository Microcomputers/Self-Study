#include <msp430fr5739.h>
#include "functions.h"

/*
 	main.c
	Switches and LEDs
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

// Switch presses causes this debounce ISR to fire
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void) {
	TA1CCTL0 = 0;
	TA1CTL = 0;
	EnableSwitches();
}

// Port 4 ISR to detect switch presses
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void) {
	// See link to get information on __even_in_range intrinsic
	// https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/238317
	switch(__even_in_range(P4IV, P4IV_P4IFG1)) {
		case P4IV_P4IFG0:
			DisableSwitches();
			P4IFG &= ~BIT0;					// Clear P4.0 IFG
			PJOUT ^= BIT0;					// Toggle PJ.0
			StartDebounceTimer(0);			// Reenable switches after debounce i.e. time elapsed
			break;
		case P4IV_P4IFG1:
			DisableSwitches();
			P4IFG &= ~BIT1;					// Clear P4.1 IFG
			PJOUT ^= BIT1;					// Toggle PJ.1
			StartDebounceTimer(0);			// Reenable switches after debounce i.e. time elapsed
			break;
	    default:
	    	break;
	}
}


