#include <msp430.h>
#include <stdio.h>

typedef enum {FALSE, TRUE} bool;
typedef unsigned char uc_8;
typedef unsigned int uc_16;

void ledsInit(uc_8 ledNumber);
bool ledsWrite(uc_8 ledNumber, uc_8 value);
void buttonsInit(uc_8 buttonNumber);
bool buttonsRead(uc_8 buttonNumber, uc_8 *value);
void accInit(void);
bool accRead(uc_16*);
void tempInit(void);
bool tempRead(uc_16*);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;				// Stop watchdog timer

    ledsInit(4);							// Initialise LEDs
    buttonsInit(1);

    uc_8 temp = 0;
    ledsWrite(4,1);							// Turn on LED 4


    uc_16 accReadings[3] = {0};
    uc_16 tempReading = 0;

    while(1){
    	buttonsRead(1, &temp);					// Read from buttons
    	printf("Button State: %d\n", temp);
    	accInit();
		accRead(accReadings);
		printf("Accelerometer: %d %d %d\n", accReadings[0], accReadings[1], accReadings[2]);
		tempInit();
		tempRead(&tempReading);
		printf("Temperature: %d\n\n", tempReading);
    }
}

void ledsInit(uc_8 ledNumber)
{
	uc_8 temp = 1 << (ledNumber - 1);

	uc_8 PJMASK = (uc_16)temp & 0x000F;
	uc_8 P3MASK = temp & 0xF0;

	PJOUT &= ~PJMASK;
	P3OUT &= ~P3MASK;

	PJDIR |= PJMASK;
	P3DIR |= P3MASK;
}

bool ledsWrite(uc_8 ledNumber, uc_8 value)
{
	uc_8 temp = 1 << (ledNumber - 1);

	uc_8 PJMASK = (uc_16)temp & 0x000F;
	uc_8 P3MASK = temp & 0xF0;

	if (value != 0){
		PJOUT |= PJMASK;
		P3OUT |= P3MASK;
	} else {
		PJOUT &= ~PJMASK;
		P3OUT &= ~P3MASK;
	}

	return TRUE;
}


void buttonsInit(uc_8 buttonNumber)
{
	uc_8 temp = 1 << (buttonNumber - 1);
	uc_8 P4MASK = temp & 0x03;

	P4OUT |= P4MASK;						// Select pullup resistor
	P4DIR &= ~P4MASK;						// Direction = input
	P4REN |= P4MASK;						// Enable pullup resistor
}

bool buttonsRead(uc_8 buttonNumber, uc_8 *value)
{
	uc_8 temp = 1 << (buttonNumber - 1);
	uc_8 P4MASK = temp & 0x03;

	if(((~P4IN) & P4MASK) != 0){
		*value = 1;
	} else {
		*value = 0;
	}

	return TRUE;
}

void accInit(void)
{
	P3SEL0 |= 0x07;							// Connect ACC pins to ADC
	P3SEL1 |= 0x07;
	P3DIR &= ~(0x07);						// Set pins as input

	P2DIR |= 0x80;							// Enable power for ACC
	P2OUT |= 0x80;

	// Allow the accelerometer to settle before sampling any data
	__delay_cycles(200000);

	// Ensure ENC (enable conversion) bit is clear
	ADC10CTL0 &= ~ADC10ENC;

	// Turn ADC on, set sample-and-hold time to 96 clk cycles
	ADC10CTL0 = ADC10ON + ADC10SHT_5;

	// sample-and-hold source = ADC10SC bit
	// pulse mode select = time of sample from sampling length in CTL0
	// conversion sequence mode = single channel, single conversion
	// clock source = MODCLK
	ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10CONSEQ_0 + ADC10SSEL_0;

	// resolution = 10 bit
	ADC10CTL2 = ADC10RES;

	// select reference = AVCC to AVSS
	// input channel = A12
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;

	// clear all flags
	ADC10IFG = 0x00;
}

bool accRead(uc_16 *values)
{
	ADC10CTL0 &= ~ADC10ENC;
	ADC10MCTL0 = (ADC10MCTL0 & 0xF0) | (ADC10INCH_12);
	ADC10IFG = 0x00;
	ADC10CTL0 |= ADC10ENC + ADC10SC;		// Start conversion
	while((ADC10IFG & ADC10IFG0) == 0);		// Wait for conversion to complete
	*values = ADC10MEM0;					// Store result

	values++;

	ADC10CTL0 &= ~ADC10ENC;
	ADC10MCTL0 = (ADC10MCTL0 & 0xF0) | (ADC10INCH_13);
	ADC10IFG = 0x00;
	ADC10CTL0 |= ADC10ENC + ADC10SC;		// Start conversion
	while((ADC10IFG & ADC10IFG0) == 0);	// Wait for conversion to complete
	*values = ADC10MEM0;					// Store result

	values++;

	ADC10CTL0 &= ~ADC10ENC;
	ADC10MCTL0 = (ADC10MCTL0 & 0xF0) | (ADC10INCH_14);
	ADC10IFG = 0x00;
	ADC10CTL0 |= ADC10ENC + ADC10SC;		// Start conversion
	while((ADC10IFG & ADC10IFG0) == 0);		// Wait for conversion to complete
	*values = ADC10MEM0;					// Store result

	return TRUE;
}

void tempInit(void){
	P1SEL0 |= 0x08;							// Connect TEMP pins to ADC
	P1SEL1 |= 0x08;
	P1DIR &= ~(0x08);						// Set pins as input

	P2DIR |= 0x80;							// Enable power for TEMP
	P2OUT |= 0x80;

	__delay_cycles(50000);

	// Ensure ENC (enable conversion) bit is clear
	ADC10CTL0 &= ~ADC10ENC;

	// Turn ADC on, set sample-and-hold time to 192 clk cycles
	ADC10CTL0 = ADC10ON + ADC10SHT_7;

	// sample-and-hold source = ADC10SC bit
	// pulse mode select = time of sample from sampling length in CTL0
	// conversion sequence mode = single channel, single conversion
	// clock source = MODCLK
	ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10CONSEQ_0 + ADC10SSEL_0;

	// resolution = 10 bit
	ADC10CTL2 = ADC10RES;

	// select reference = AVCC to AVSS
	// input channel = A12
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_4;

	// clear all flags
	ADC10IFG = 0x00;
}

bool tempRead(uc_16* value){
	ADC10IFG = 0x00;
	ADC10CTL0 |= ADC10ENC + ADC10SC;		// Start conversion
	while((ADC10IFG & ADC10IFG0) == 0);		// Wait for conversion to complete
	*value = ADC10MEM0;						// Store result

	return TRUE;
}
