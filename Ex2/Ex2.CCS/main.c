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
#define led1Pin 4
#define led2Pin 3
#define button1Pin 1
#define button2Pin 2

unsigned int ADCTResult_t;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	ledsInit(led1Pin); 				// Init leds
	ledsInit(led2Pin);
	switchesInit(button1Pin); 			// Init Switches
	switchesInit(button2Pin);
	SetupThermistor();			// Init Temp sensor
	CalibrateADC();				// Calibrate analog to digital convertor
	//Start the first sample. If this is not done the ADC10 interupt will not trigger.
	ADC10CTL0 |= ADC10ENC | ADC10SC;
	uc_8 buttonState = 0; //buton status
	while (1)
	{
		switchesRead(button1Pin, &buttonState);
		ledsWrite(led1Pin, buttonState);
		switchesRead(button2Pin, &buttonState);
		ledsWrite(led2Pin, buttonState);
		getThermisterVal();
		printf("%s%d\n", "Temp = ", (int)ADCTResult_t);
	}
	return 0;
}
