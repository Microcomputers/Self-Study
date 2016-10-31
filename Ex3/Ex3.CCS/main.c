/*******************************************************************************
 *
 * main.c
 * Code for Self-Study Lab 3 using the MSP-EXP430FR5739
 *
 * Copyright (C) 2016 Curtin University
 *
 * Some code snippets/ideas from various sources from the internet have been
 * used in order to port to the MSP-EXP430FR5739 for educational purposes.
 * Majority of the code for the register setup stems from reading TI's
 * documentation. Every student is encouraged to read the documents provided
 * in Blackboard.
 *
 * Created: Version 1.0 04/09/2016 (Bugs Contact: Unit Coordinator)
 *
 ******************************************************************************/
//*****************************************************************************
//  Self-Study Lab 3 - Interrupt Latency
//
//  Description:
//  - Interrupt-based learning
//  - Remove RF-PWR jumper and place on P1.0 and P1.1.
//  - P1.0 generates PWM at ~10 Hz using timer TA0.1 fed from ACLK.
//  - P1.1 is set as input capture to detect on rising edge TA0.2.
//  - S1 presses toggles LED1.
//  - S1 presses also toggles TA0.2 interrupts which flashes LED2.
//  Note: ACLK = internal VLO, MCLK = SMCLK = DCO
//
//             MSP-EXP430FR5739
//             ----------------
//         /|\|                |
//          --|RST         PJ.1|---+--->LED2
//            |                |  [|] Means indirect through software
//            |      P1.1/TA0.2|<--+
//            |                |   |
//            |      P1.0/TA0.1|---+
//            |                |
//            |            PJ.0|--->LED1
//
//  Tested with CCS Version: 6.0.0.00190 on Win 7 Pro 64-bit
//*****************************************************************************
//*****************************************************************************
//
//  Task: Modify code measure the capture interrupt latency i.e. the interval
//        between the assertion of the interrupt signal and the starting of
//        the code to process that interrupt. You may use any unused port pins
//        to toggle signals to be measured on an oscilloscope.
//
//*****************************************************************************

#include <msp430fr5739.h>

// Type definitions
typedef unsigned char		uc;

// User definitions
#define CSKEY_H_LOCK		0x01							// Code to lock register
#define SW_DEB_CONST		1600							// Load into TA1CCR0 the value SW_DEB_CONST i.e. debounce time = 1600*1/8000 = 200 ms
#define SW_HALF_DEB_CONST	SW_DEB_CONST/2					// Shorter debounce time model assuming half
#define PWM_ACLK_TCNT_HIGH	854								// 8.3/10*1000 count (@ 8.3 kHz typ.) of ACLK to get ~10 Hz (May need to be tweaked for different boards)
															// A test board measured 8.542 kHz for VLO
#define PWM_ACLK_TCNT_LOW	426								// Is approximately PWM_ACLK_TCNT_HIGH/2 but again may need to be tweaked

// Startup initialisation code
void SystemInit(void) {
	// At startup ACLK is set to internal VLO which is tyically 8.3 kHz (min. 5 kHz to max. 13 kHz)
	// Measurements with a typical DMM reveal 8.542 kHz which is within specifications
	CSCTL0_H = CSKEY_H;										// Unlock register
	CSCTL1 = DCOFSEL_3;										// Set DCO to 8 MHz
	CSCTL2 = SELA__VLOCLK + SELS__DCOCLK + SELM__DCOCLK;	// Set ACLK = VLO; MCLK = DCO, MCLK = DCO
	CSCTL3 = DIVA__1 + DIVS__1 + DIVM__1;					// Set all dividers: f(ACLK)/1, f(SMCLK)/1, f(MCLK)/1
	CSCTL0_H = CSKEY_H_LOCK;								// Lock register

	// Enable on-board P4.0 switch
	// Note: S1 when pressed makes P4.0 logic LOW (See schematic of MSP-EXP430FR5739)
	P4OUT |= (BIT0);										// Configure pullup resistor
	P4DIR &= ~(BIT0);										// Direction = input
	P4REN |= (BIT0);										// Enable pullup resistor
	P4IES &= ~(BIT0);										// P4.0 Lo/Hi edge interrupts
	P4IE = (BIT0);											// P4.0 interrupts enabled
	P4IFG = 0;												// P4 IFG cleared

	// Enable Port J connected LEDs
	// PJ.0 will be tied to S1 interrupt
	// PJ.1 will be tied to TA0.2 (i.e. P1.1) interrupt configured to detect upon rising edges
	PJOUT &= ~(BIT0 + BIT1);								// Initialise PJ.0 and PJ.1 to OFF
	PJDIR |= (BIT0 + BIT1);									// Set PJ.0 and PJ.1 as outputs

	// Initialise unused port pins to eliminate wasted current consumption
	// Supress compiler message
	PAOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	PADIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	PAREN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	PBOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	PBDIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	PBREN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

	// Setup P1.0 as TA0.1 function and also as output
	P1DIR |= (BIT0);										// Set P1.0 as output
	P1SEL1 &= ~(BIT0);										// Set P1.1 as TA0.1
	P1SEL0 |= (BIT0);										// Set P1.1 as TA0.1

	// Setup P1.1 as TA0.2 function and also as input
	P1DIR &= ~(BIT1);										// Set P1.1 as input
	P1SEL1 &= ~(BIT1);										// Set P1.1 as TA0.2
	P1SEL0 |= (BIT1);										// Set P1.1 as TA0.2

	// Setup TA0.2 as capture input on rising edge
	TA0CCTL2 = CM_1 + CCIS_0 + SCS + CAP;

	// Setup Timer A to generate PWM signal at TA0.1
	TA0CCR0 = PWM_ACLK_TCNT_HIGH;
	TA0CCR1 = PWM_ACLK_TCNT_LOW;
	TA0CCTL1 = OUTMOD_3;									// PWM output mode 3: PWM Set/Reset
	TA0CTL = TASSEL__ACLK + MC__UP + TACLR;					// Set source as ACLK, continuous up mode, start timer

	// Set P1.2 as output (low)
	P1DIR |= (BIT2);
	P1OUT &= ~ (BIT2);
}

// Interrupt driven main
void main(void) {

	WDTCTL = WDTPW | WDTHOLD;								// Stop watchdog timer
	SystemInit();											// System initialisation code

	// Forever loop
	while(1) {
		__bis_SR_register(LPM4_bits + GIE);					// Enter LPM4 with interrupts enabled
		__no_operation();			        				// For debugger

		// Students: Do your own stuff below
	}
}

// Disable switch interfaced to P4.0
inline void DisableSwitchP4_0(void) {
	P4IFG = 0;												// P4 IFG cleared
	P4IE &= ~(BIT0);										// P4.0 interrupt disabled
	P4IFG = 0;												// P4 IFG cleared
}

// Enable switch interfaced to P4.0
inline void EnableSwitchP4_0(void) {
	P4IFG = 0;												// P4 IFG cleared
	P4IE = (BIT0);											// P4.0 interrupt enabled
}

// Simple software switch debounce model as a constant delay
inline void StartDebounceTimer(uc ucDelay) {
	// Default delay = 0
	TA1CCTL0 = CCIE;										// TACCR0 interrupt enabled
	if(ucDelay)
		TA1CCR0 = SW_HALF_DEB_CONST;
	else
		TA1CCR0 = SW_DEB_CONST;
	TA1CTL = TASSEL__ACLK + MC__UP;							// Set source as ACLK, continuous up mode, start timer
}

// Switch presses causes this debounce ISR to fire
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void) {
	TA1CCTL0 = 0;
	TA1CTL = 0;
	EnableSwitchP4_0();
	TA0CCTL2 ^= CCIE;										// Toggle capture interrupt to sync with S1
	PJOUT &= ~(BIT1);										// Reinitialise PJ.1 to LOW output
}

// Capture interrupt
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A0_CCRx_ISR(void) {
	switch(__even_in_range(TA0IV, TA0IV_TAIFG)) {
		case TA0IV_NONE: break;								// Vector 0: No interrupts
		case TA0IV_TACCR1: break;							// Vector 2: TA0CCR1 CCIFG
		case TA0IV_TACCR2: {								// Vector 4: TA0CCR2 CCIFG
			// Toggle P1.2 on and off
			P1OUT ^= (BIT2);
			__delay_cycles(2000);
			P1OUT ^= (BIT2);
			// When S1 pressed, LED2 is toggled according to PWM frequency generated at P1.0
			PJOUT ^= (BIT1);								// Toggle PJ.1
			break;
		}
		case TA0IV_3: break;								// Vector 6: Reserved
		case TA0IV_4: break;								// Vector 8: Reserved
		case TA0IV_5: break;								// Vector 10: Reserved
		case TA0IV_6: break;								// Vector 12: Reserved
		case TA0IV_TAIFG: break;							// Vector 14: TA0IFG
		default: break;
	}
}

// Port 4 ISR to detect switch presses
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void) {
	switch(__even_in_range(P4IV, P4IV_P4IFG7)) {
		case P4IV_NONE: break;								// Vector 0: No interrupts
		case P4IV_P4IFG0: {									// Vector 2: P4IV P4IFG.0
			DisableSwitchP4_0();							// Stop any further switch interrupts until debounce completes
			P4IFG &= ~(BIT0);								// Clear P4.0 IFG
			PJOUT ^= (BIT0);								// Toggle PJ.0
			StartDebounceTimer(0);							// Re-enable switches after debounce i.e. time elapsed
			break;
		}
		case P4IV_P4IFG1: break;							// Vector 4: P4IV P4IFG.1
		case P4IV_P4IFG2: break;							// Vector 6: P4IV P4IFG.2
		case P4IV_P4IFG3: break;							// Vector 8: P4IV P4IFG.3
		case P4IV_P4IFG4: break;							// Vector 10: P4IV P4IFG.4
		case P4IV_P4IFG5: break;							// Vector 12: P4IV P4IFG.5
		case P4IV_P4IFG6: break;							// Vector 14: P4IV P4IFG.6
		case P4IV_P4IFG7: break;							// Vector 16: P4IV P4IFG.7
	    default: break;
	}
}
