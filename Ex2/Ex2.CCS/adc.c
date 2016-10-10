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
 *          Version 2.0 10/08/2016
 *          
 ******************************************************************************/
#include "adc.h"
#include "system.h"

//variables
volatile unsigned int ADCResult; 
unsigned int CalValue_t;
extern unsigned int ADCTResult_t;


void getThermisterVal()
{
    //map 0 - 1023 to 0 - 5V, This is the voltage read 
    //ADCResult = (ADCResult * 0.004882814);
    //CalValue_t = (CalValue_t * 0.004882814);
    if (ADCResult >= CalValue_t)
    {
        ADCTResult_t = ADCResult - CalValue_t;
    }
    else
    {
        ADCTResult_t = CalValue_t - ADCResult; 
    }
    //Convert the voltage to degrees Celsius
    //The correct formula should be extracted from the temp sens data sheet
    //ADCTResult_t = (ADCTResult_t * 0.004882814);
    //ADCTResult_t = (ADCTResult_t - 0.5) * 100.0;
    ADCTResult_t = ((ADCTResult_t * 27069) - 18169625) >> 16;
    //ADCTResult_t = ((ADCTResult_t * 27069) - 18169625) ;
}



//ADC ISR
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    ADC10CTL0 &= ~ADC10ENC;
    ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_4;
    ADCResult = ADC10MEM0;
    ADC10CTL0 |= ADC10ENC | ADC10SC;
}

