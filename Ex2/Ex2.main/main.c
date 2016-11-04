#include <msp430fr5739.h>
#include "system.h"
#include <stdio.h>
/*
 * main.c
 */

extern unsigned int ADCResult_X, ADCResult_Y, ADCResult_Z;



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	systemInit();				// setup pins
	startUpSequence();			// Copy Ti's fancy LED boot up trick
	setupAccel();				//Setup ADC nad Accel
	calibrateADC();
	//If this is not done the ADC10 interupt will not trigger.
	ADC10CTL0 |= ADC10ENC | ADC10SC; //Start the first sample.
	while (1)
	{
		printf("%s%d\t%s%d\t%s%d\n", "X = ", (int)ADCResult_X, "Y = ", (int)ADCResult_Y, "Z = ", (int)ADCResult_Z);
	}
	return 0;
}
