/*
 *main.c
 *Version: 1.0
 *Parker Dillmann
 *The Longhorn Engineer (c) 2013 
 *www.longhornengineer.com
 *
 *Check bottom of file for License.
 *
*/

#include "msp430fr5739.h"
#include "FR_EXP.h"

#define SERVO_1 BIT4
#define SERVO_2 BIT5
#define scale 9

unsigned int servo_counter = 0;
unsigned int servo1pos = 1500;
unsigned int servo2pos = 1500;

unsigned int ADC_counter = 0;

unsigned int ADCResult_X, ADCResult_Y, ADCResult_Z;
unsigned int CalValue_X, CalValue_Y, CalValue_Z;

unsigned int temp;

void main(void)
{ 
	WDTCTL = WDTPW + WDTHOLD;	//Stop the dog
	SystemInit();	//Setup the pins				
	StartUpSequence();	//Copy Ti's fancy LED boot up trick      	   
    
	SetupAccel();	//Setup the ADC and Accel
	CalibrateADC(); //Find zero points of all 3 axis
	
	ADC10CTL0 |= ADC10ENC | ADC10SC; //Start the first sample. If this is not done the ADC10 interupt will not trigger.
 	
	while (1)
	{  
		if (ADCResult_X > CalValue_X + 10)        
        {
			servo1pos = 1500 + scale * (ADCResult_X - CalValue_X);
			if (servo1pos > 2500)
			{
				servo1pos = 2500;	
			}
		}
      	else if (ADCResult_X < CalValue_X - 10)
        {
        	servo1pos = 1500 - scale * (CalValue_X - ADCResult_X);
        	if (servo1pos < 500)
        	{
        		servo1pos = 500;
        	}
        }
        else
        {
        	servo1pos = 1500;
        }
        
        if (ADCResult_Y > CalValue_Y + 10)        
        {
			servo2pos = 1500 + scale * (ADCResult_Y - CalValue_Y);
			if (servo2pos > 2500)
			{
				servo2pos = 2500;	
			}
		}
      	else if (ADCResult_Y < CalValue_Y - 10)
        {
        	servo2pos = 1500 - scale * (CalValue_Y - ADCResult_Y);
        	if (servo2pos < 500)
        	{
        		servo2pos = 500;
        	}
        }
        else
        {
        	servo2pos = 1500;
        }	 	                                       
    }
}

void SystemInit(void) //Sets up the Fraunch board for Accel reads and LED tricks
{
	//Startup clock system in max. DCO setting ~8MHz
	// This value is closer to 10MHz on untrimmed parts  
	CSCTL0_H = 0xA5;                          // Unlock register
	CSCTL1 |= DCOFSEL0 + DCOFSEL1;            // Set max. DCO setting
	CSCTL2 = SELA_1 + SELS_3 + SELM_3;        // set ACLK = vlo; MCLK = DCO
	CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;        // set all dividers 
	CSCTL0_H = 0x01;                          // Lock Register
  
  	// Turn off temp. 
  	REFCTL0 |= REFTCOFF; 
  	REFCTL0 &= ~REFON;
  	
	// Enable LEDs
	P3OUT &= ~(BIT6+BIT7+BIT5+BIT4);
	P3DIR |= BIT6+BIT7+BIT5+BIT4;
	PJOUT &= ~(BIT0+BIT1+BIT2+BIT3);
	PJDIR |= BIT0 +BIT1+BIT2+BIT3;  

  	// P3.0,P3.1 and P3.2 are accelerometer inputs
  	P3OUT &= ~(BIT0 + BIT1 + BIT2);   
  	P3DIR &= ~(BIT0 + BIT1 + BIT2); 
  	P3REN |= BIT0 + BIT1 + BIT2;
  	
  	// Setup Servo ports
  	P1OUT &= ~(SERVO_1 + SERVO_2);
  	P1DIR |= SERVO_1 + SERVO_2;
  	P1SEL0 &= ~(SERVO_1 + SERVO_2);
  	P1SEL1 &= ~(SERVO_1 + SERVO_2);
  	
  	// Setup Servo Interrupt
  	TA0CCTL0 = CCIE;
  	TA0CCR0 = 1500;
  	TA0CTL = TASSEL_2 + MC_1 + ID_3;
}

void CalibrateADC(void) //Sets the "zero" point of the accelerometer
{
	unsigned char CalibCounter =0;
	unsigned int Value = 0;
	
    __disable_interrupt();	//Turn off any interupts just incase
    
    ADC10CTL0 &= ~ADC10ENC; //Toggle ENC bit. Need this to change the ADC10INCH_x value.
    ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;	//Sample the X-axis  
	while(CalibCounter <50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;	//Start sample.
		while (ADC10CTL1 & BUSY);	//Wait for sample to be done.
		Value += ADC10MEM0;
	}
	CalValue_X = Value/50;	//Average all samples to find the best zero.
	
	ADC10CTL0 &= ~ADC10ENC;	//Toggle ENC bit. Need this to change the ADC10INCH_x value.
	CalibCounter = 0;	//Reset the counters
	Value = 0;
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_13;	//Sample the Y-axis 
	while(CalibCounter <50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;	//Start sample.
		while (ADC10CTL1 & BUSY);	//Wait for sample to be done.
		Value += ADC10MEM0;
	}
	CalValue_Y = Value/50;	//Average all samples to find the best zero.
	
	ADC10CTL0 &= ~ADC10ENC;	//Toggle ENC bit. Need this to change the ADC10INCH_x value.
	CalibCounter = 0;	//Reset the counters
	Value = 0;
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_14;	//Sampel the Z-axis 
	while(CalibCounter <50)
	{
		P3OUT ^= BIT4;
		CalibCounter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;	//Start sample.
		while (ADC10CTL1 & BUSY);  	//Wait for sample to be done.
		Value += ADC10MEM0;
	}
	CalValue_Z = Value/50;	//Average all samples to find the best zero.
	
	ADC10CTL0 &= ~ADC10ENC;	//Toggle ENC bit. Need this to change the ADC10INCH_x value.
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;	//We need to start at the X-axis first due to how the interupt routine works.
	
	__enable_interrupt();	//enable interupts
}

void SetupAccel(void)
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

void StartUpSequence(void)	//this is copied from the Ti Fraunch Pad experience. Only thing it does is make a fancy LED sequence on boot.
{
	unsigned char flag=4,up=1,counter = 0; 
	unsigned char LED_ArrayPJ[] = {0x01,0x02,0x04,0x08};
	unsigned char LED_ArrayP3[] = {0x80,0x40,0x20,0x10};

	while (counter <10) 
	{    
		counter++;
		PJOUT &= ~(BIT0 +BIT1+BIT2+BIT3);
		P3OUT &= ~(BIT4 +BIT5+BIT6+BIT7);    
		if(up)
		{
			while(flag)
			{
				P3OUT = LED_ArrayP3[flag-1];
				PJOUT = LED_ArrayPJ[flag-1];
				LongDelay();               
				flag--;
			}
			up=0;
		}
		else
		{
			while(flag<4)
			{
				P3OUT = LED_ArrayP3[flag];
				PJOUT = LED_ArrayPJ[flag];
				LongDelay();        
				flag++;
			}
			up = 1;
		}
	}   
	PJOUT &= ~(BIT0 +BIT1+BIT2+BIT3);
	P3OUT &= ~(BIT4 +BIT5+BIT6+BIT7);    
}

void LongDelay()
{
  __delay_cycles(250000);
}

//Servo Interrupt
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{  
  	if(servo_counter == 0)
  	{
  		P1OUT |= SERVO_1;
  		P1OUT |= SERVO_2;
  		if (servo1pos >= servo2pos)
  		{
  			TA0CCR0 = servo2pos;	
  		}
  		else
  		{
  			TA0CCR0 = servo1pos;
  		}
  		servo_counter++;
  	}
  	else if(servo_counter == 1)
  	{
  		if (servo1pos >= servo2pos)
  		{
  			P1OUT &= ~(SERVO_2);
  			TA0CCR0 = servo1pos - servo2pos;	
  		}
  		else
  		{
  			P1OUT &= ~(SERVO_1);
  			TA0CCR0 = servo2pos - servo1pos;
  		}
  		servo_counter++;
  	}
  	else
  	{
		TA0CCR0 = 20000 - servo1pos;
		P1OUT &= ~(SERVO_1);
		P1OUT &= ~(SERVO_2);
		servo_counter = 0;	
  	}
}

//ADC10 interupt routine
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{   
	if (ADC_counter == 0) //X-axis
	{
		ADC10CTL0 &= ~ADC10ENC;	
		ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_13;	//Next channel is the Y-axis
		ADCResult_X = ADC10MEM0;
		ADC_counter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;
	}
	else if (ADC_counter == 1)	//Y-axis
	{
		ADC10CTL0 &= ~ADC10ENC;
		ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_14;	//Next channel is the Z-axis
		ADCResult_Y = ADC10MEM0;
		ADC_counter++;
		ADC10CTL0 |= ADC10ENC | ADC10SC;
	}
	else	//Z-axis
	{
		ADC10CTL0 &= ~ADC10ENC;
		ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;	//Next channel is the X-axis
		ADCResult_Z = ADC10MEM0;
		ADC_counter = 0;
		ADC10CTL0 |= ADC10ENC | ADC10SC;
	}
}

/*
┌──────────────────────────────────────────────────────────────────────────────────────┐
│                           TERMS OF USE: MIT License                                  │
├──────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this  │
│software and associated documentation files (the "Software"), to deal in the Software │
│without restriction, including without limitation the rights to use, copy, modify,    │
│merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    │
│permit persons to whom the Software is furnished to do so, subject to the following   │
│conditions:                                                                           │
│                                                                                      │
│The above copyright notice and this permission notice shall be included in all copies │
│or substantial portions of the Software.                                              │
│                                                                                      │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   │
│INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         │
│PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    │
│HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     │
│OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        │
│SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                │
└──────────────────────────────────────────────────────────────────────────────────────┘
*/
