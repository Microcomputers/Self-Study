#include "system.h"
volatile unsigned int ADCResult = 0;

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


/**********************************************************************//**
 * @brief  Calibrate thermistor or accelerometer
 * 
 * @param  none 
 *  
 * @return none
 *************************************************************************/
unsigned int CalibrateADC(void)
{
  unsigned char CalibCounter =0;
  unsigned int Value = 0;

  // Disable interrupts & user input during calibration
  DisableSwitches();            
  while(CalibCounter <50)
    {
      P3OUT ^= BIT4;
      CalibCounter++;
      while (ADC10CTL1 & BUSY); 
      ADC10CTL0 |= ADC10ENC | ADC10SC ;       // Start conversion 
      __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
      __no_operation(); 
      Value += ADCResult;
    }
    Value = Value/50;
    // Reenable switches after calibration
    EnableSwitches();
    return Value;
}

/**********************************************************************//**
 * @brief  Take ADC Measurement
 * 
 * @param  none 
 *  
 * @return none
 *************************************************************************/
void TakeADCMeas(void)
{  
  while (ADC10CTL1 & BUSY); 
  ADC10CTL0 |= ADC10ENC | ADC10SC ;       // Start conversion 
  __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
  __no_operation();                       // For debug only
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
