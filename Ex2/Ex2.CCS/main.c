#include <stdio.h>
#include <msp430fr5739.h>
#include "system.h"
#include "adc.h"
//#include "uart.h"
/*
 	main.c
	1. Switches and LEDs
	2.
	3. Temp sens
 */	
unsigned int ADCTResult_t;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	LEDs_INIT(); 				// Init leds
	SWITCHEs_INIT(); 			// Init Switches
    SetupThermistor();			// Init Temp sensor
	CalibrateADC();				// Calibrate analog to digital convertor
	//Start the first sample. If this is not done the ADC10 interupt will not trigger.
	ADC10CTL0 |= ADC10ENC | ADC10SC;

	while (1)
	{

		//__bis_SR_register(LPM4_bits + GIE);	// Enter LPM4 with interrupts enabled
		getThermisterVal();
		printf("%s%d\n", "Temp = ", (int)ADCTResult_t);
	}
	return 0;
}


