#include "msp430fr5739.h"
#include "system.h"

volatile unsigned char ThreshRange[3]={0,0,0};
extern unsigned int CalValue_t, CalValue_x, CalValue_y, CalValue_z;

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

void switchesInit(uc_8 buttonNumber)
{
	// Enable on-board switches
	uc_8 temp = 1 << (buttonNumber - 1);
	uc_8 P4MASK = temp & 0x03;
	P4OUT |= P4MASK;			// Configure pullup resistor
	P4DIR &= ~(P4MASK);		// Direction = input
	P4REN |= P4MASK;			// Enable pullup resistor
/*
	P4IES &= ~(BIT0 + BIT1);		// P4.0 and P4.1 Lo/Hi edge interrupts
	P4IE = BIT0 + BIT1;				// P4.0 and P4.1 interrupts enabled
	P4IFG = 0;						// P4 IFG cleared
	*/
}

//reade switch values, (digitalRead())
bool switchesRead(uc_8 buttonNumber, uc_8 *value)
{
	uc_8 temp = 1 << (buttonNumber - 1);
	uc_8 P4MASK = temp & 0x03;

	if(((~P4IN) & P4MASK) != 0)
	{
		*value = 1;
	}
	else
	{
		*value = 0;
	}

	return TRUE;
}

void SetupThermistor(void)
{
    // ~16KHz sampling
    //Turn on Power
    P2DIR |= BIT7;
    P2OUT |= BIT7;
    // Configure ADC
    P1SEL1 |= BIT4;
    P1SEL0 |= BIT4;
    // Allow for settling delay
    __delay_cycles(50000);
    // Configure ADC
    ADC10CTL0 &= ~ADC10ENC;
    ADC10CTL0 = ADC10SHT_7 + ADC10ON;        // ADC10ON, S&H=192 ADC clks
    // ADCCLK = MODOSC = 5MHz
    ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10SSEL_0;
    ADC10CTL2 = ADC10RES;                    // 10-bit conversion results
    ADC10MCTL0 = ADC10INCH_4;                // A4 ADC input select; Vref=AVCC
    ADC10IE = ADC10IE0;                      // Enable ADC conv complete interrupt
    // Setup Thresholds for relative difference in Thermistor measurements
    ThreshRange[0]=15;
    ThreshRange[1]=25;
    ThreshRange[2]=45;
    __enable_interrupt();
}

void ShutDownTherm(void)
{
    // Turn off Vcc
    P2DIR &= ~BIT7;
    P2OUT &= ~BIT7;
    // Turn off ADC Channel
    P1SEL1 &= ~BIT4;
    P1SEL0 &= ~BIT4;
    // Turn off ADC
    ADC10CTL0 &= ~(ADC10ENC + ADC10ON);
    ADC10IE &= ~ADC10IE0;
    ADC10IFG = 0;
}

// Enable switches interfaced to P4.0 and P4.1
inline void EnableSwitches()
{
	P4IFG = 0;								// P4 IFG cleared
	P4IE = BIT0 + BIT1;						// P4.0 and P4.1 interrupts enabled
}

// Disable switches interfaced to P4.0 and P4.1
inline void DisableSwitches()
{
	P4IFG = 0;								// P4 IFG cleared
	P4IE &= ~(BIT0 + BIT1);					// P4.0 and P4.1 interrupts disabled
	P4IFG = 0;								// P4 IFG cleared
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

void CalibrateADC(void)
{
	unsigned char CalibCounter =0;
	unsigned int Value = 0;

	// Disable interrupts & user input during calibration
	DisableSwitches();
	__disable_interrupt();

	ADC10CTL0 &= ~ADC10ENC;		//Toggle ENC bit
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_4;	//Sample thermistor
	while(CalibCounter < 50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;    // Start conversion
		while (ADC10CTL1 & BUSY); 			// wait for sample to be done
		Value += ADC10MEM0;
	}
	CalValue_t = Value/50;
	//reset the counters
	CalibCounter = 0;
	Value = 0;

	ADC10CTL0 &= ~ADC10ENC;		//Toggle ENC bit
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;	//Sample thermistor
	while(CalibCounter < 50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;    // Start conversion
		while (ADC10CTL1 & BUSY); 			// wait for sample to be done
		Value += ADC10MEM0;
	}
	CalValue_x = Value/50;
	//reset the counters
	CalibCounter = 0;
	Value = 0;

	ADC10CTL0 &= ~ADC10ENC;		//Toggle ENC bit
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_13;	//Sample thermistor
	while(CalibCounter < 50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;    // Start conversion
		while (ADC10CTL1 & BUSY); 			// wait for sample to be done
		Value += ADC10MEM0;
	}
	CalValue_y = Value/50;
	//reset the counters
	CalibCounter = 0;
	Value = 0;

	ADC10CTL0 &= ~ADC10ENC;		//Toggle ENC bit
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_14;	//Sample thermistor
	while(CalibCounter < 50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;    // Start conversion
		while (ADC10CTL1 & BUSY); 			// wait for sample to be done
		Value += ADC10MEM0;
	}
	CalValue_z = Value/50;
	//reset the counters
	CalibCounter = 0;
	Value = 0;

	ADC10CTL0 &= ~ADC10ENC; // Toggle ENC bit. Need this to change the ADC10INCH_t value.
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_4 ; // WE NEED TO START AT THE X AXES FIRST DUE TO HOW THE INTERUPT ROUTINE WORKS.


	// Reenable switches and interrupts after calibration
	EnableSwitches();
	__enable_interrupt();
}

void acceInit()
{
	// P3.0,p3.1 and P3.2 are accelerometer inputs
	P3OUT &= ~(BIT0 + BIT1 + BIT2);
	P3DIR &= ~(BIT0 + BIT1 + BIT2);
	P3REN |= ~(BIT0 + BIT1 + BIT2);
	
}

void SetupAccel()
{

	//Setup  accelerometer
	// ~20KHz sampling
	//Configure GPIO
	ACC_PORT_SEL0 |= ACC_X_PIN + ACC_Y_PIN + ACC_Z_PIN;    //Enable A/D channel inputs
	ACC_PORT_SEL1 |= ACC_X_PIN + ACC_Y_PIN + ACC_Z_PIN;
  	ACC_PORT_DIR &= ~(ACC_X_PIN + ACC_Y_PIN + ACC_Z_PIN);
  	ACC_PWR_PORT_DIR |= ACC_PWR_PIN;              //Enable ACC_POWER
  	ACC_PWR_PORT_OUT |= ACC_PWR_PIN;

  	// Allow the accelerometer to settle before sampling any data 
  	__delay_cycles(200000);   
  
  	//Setting up the ADC stuff 
  	ADC10CTL0 &= ~ADC10ENC;                        // Ensure ENC is clear
  	ADC10CTL0 = ADC10ON + ADC10SHT_5;  
  	ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10CONSEQ_0 + ADC10SSEL_0;  
  	ADC10CTL2 = ADC10RES;    
  	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;
  	ADC10IV = 0x00;		//Clear all ADC12 channel int flags  
  	ADC10IE |= ADC10IE0;	//Enable ADC10 interrupts
  	
  	__enable_interrupt();
  	
}

/****
	Interrupt voids
*/

// Switch presses causes this debounce ISR to fire
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void)
{
	TA1CCTL0 = 0;
	TA1CTL = 0;
	EnableSwitches();
}

// Port 4 ISR to detect switch presses
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void)
{
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
