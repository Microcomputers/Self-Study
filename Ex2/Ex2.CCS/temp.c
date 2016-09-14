/*******************************************************************************
 *
 * FR_EXP.c
 * User Experience Code for the MSP-EXP430FR5739
 * C Functions File
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Created: Version 1.0 04/13/2011
 *          Version 1.1 05/11/2011
 *          Version 1.2 08/31/2011
 *  
 ******************************************************************************/
#include "temp.h"
#include "system.h"

//variables
extern volatile unsigned int ADCResult; 
volatile unsigned char ThreshRange[3]={0,0,0};
unsigned int CalValue = 0;
unsigned int ADCTemp =0;


unsigned int getThermisterVal()
{
	SetupThermistor();
	CalValue = CalibrateADC();
  	TakeADCMeas();
	if (ADCResult >= CalValue)
	{
	  //temp = DOWN;
	  ADCTemp = ADCResult - CalValue;
	}
	else
	{
	  //temp = UP;
	  ADCTemp = CalValue - ADCResult;
	}
	return ADCTemp;
}

/**********************************************************************//**
 * @brief  Setup thermistor
 * 
 * @param  none 
 *  
 * @return none
 *************************************************************************/
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
}

/**********************************************************************//**
 * @brief  ShutDownTherm
 * 
 * @param  none 
 *  
 * @return none
 *************************************************************************/
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

