#include "functions.h"

void LEDs_INIT()
{
	//we'll use LEDs PJ0.1 & PJ0.2
	PJDIR |= BIT0 + BIT1; //set pins PJ.0 and PJ.1 to outputs
	PJOUT &= ~(BIT0 + BIT1); //set pins to low
}


void SWITCHEs_INIT()
{
	// Enable on-board switches
	// P4.0 and P4.1 are interfaced to switches
	// Note: S1 when pressed makes P4.0 logic LOW (See schematic of MSP-EXP430FR5739)
	// Note: S2 when pressed makes P4.1 logic LOW (See schematic of MSP-EXP430FR5739)
	// Port 4 has only two pins
	P4OUT |= BIT0 +BIT1;			// Configure pullup resistor
	P4DIR &= ~(BIT0 + BIT1);		// Direction = input
	P4REN |= BIT0 + BIT1;			// Enable pullup resistor
	P4IES &= ~(BIT0 + BIT1);		// P4.0 and P4.1 Lo/Hi edge interrupts
	P4IE = BIT0 + BIT1;				// P4.0 and P4.1 interrupts enabled
	P4IFG = 0;						// P4 IFG cleared
}

// Disable switches interfaced to P4.0 and P4.1
inline void DisableSwitches() 
{
	P4IFG = 0;								// P4 IFG cleared
	P4IE &= ~(BIT0 + BIT1);					// P4.0 and P4.1 interrupts disabled
	P4IFG = 0;								// P4 IFG cleared
}

// Enable switches interfaced to P4.0 and P4.1
inline void EnableSwitches() 
{
	P4IFG = 0;								// P4 IFG cleared
	P4IE = BIT0 + BIT1;						// P4.0 and P4.1 interrupts enabled
}

// Simple software switch debounce model as a constant delay
inline void StartDebounceTimer(uc ucDelay) 
{
	// Default delay = 0
	TA1CCTL0 = CCIE;						// TACCR0 interrupt enabled
	if(ucDelay)
		TA1CCR0 = SW_HALF_DEB_CONST;
	else
		TA1CCR0 = SW_DEB_CONST;
	TA1CTL = TASSEL_1 + MC_1;				// ACLK, up mode
}