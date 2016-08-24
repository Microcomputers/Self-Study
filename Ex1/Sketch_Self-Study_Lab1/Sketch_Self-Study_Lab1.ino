#include <msp430fr5739.h>

int dutyC = 90;             // Duty Cycle: Valid Range = 10 to 90

void setup() {
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT
  P1DIR |= BIT2;            // P1.2 to output
  P1SEL0 |= BIT2;	    // P1.2 to TA1.1
 
  TA1CCR0 = 1000-1;	    // PWM period
  TA1CCTL1 = OUTMOD_7;      // CCR1 reset/set
  TA1CCR1 = dutyC * 10;	    // CCR1 PWM duty cycle
  TA1CTL = TASSEL_2 + MC_1; // SMCLK, up mode
 
  _BIS_SR(LPM0_bits);       // Enter LPM0
}

void loop() {
}
