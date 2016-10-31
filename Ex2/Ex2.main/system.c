#include "system.h"
#include <msp430fr5739.h> 

unsigned int ADC_counter = 0;
unsigned int ADCResult_X, ADCResult_Y, ADCResult_Z;
unsigned int CalValue_X, CalValue_Y, CalValue_Z;

void systemInit(void)
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
}

//Sets the "zero" point of the accelerometer
void calibrateADC(void) 
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

void setupAccel(void)
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

//this is copied from the Ti Fraunch Pad experience. 
//Only thing it does is make a fancy LED sequence on boot.
void startUpSequence(void)	
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
