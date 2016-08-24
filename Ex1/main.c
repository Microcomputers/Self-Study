/*
 * main.c
 * Self study 1
 * author: Amir Rajabifar <amir_rajabifar@uahoo.com.au>
 * 
 * Pulse With Mudulation (PWM)
 * 
 * Objective: convert the provided energia code (*.ino)
 * to C code (*.c)
 */

#include <msp430fr5739.h> 

int dutyC = 90; //Duty cycle 10 to 90

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	//Stop watchdog timer
	
	P1DIR |= BIT2; 				//Set P1.2 to OUTPUT
	P1SEL0 |= BIT2;				//From P1.2 select TA1.1

	TA1CCR0 = 1000-1;	    	// PWM period
	TA1CCTL1 = OUTMOD_7;      	// CCR1 reset/set
	TA1CCR1 = dutyC * 10;	    // CCR1 PWM duty cycle
	TA1CTL = TASSEL_2 + MC_1; 	// SMCLK, up mode

	_BIS_SR(LPM0_bits);       	// Enter LPM0
	return 0;
}
