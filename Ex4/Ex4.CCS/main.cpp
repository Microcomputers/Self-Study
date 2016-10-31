#include <msp430fr5739.h>
#include <inttypes.h>
#include <stdio.h>
#include "TimeTest.h"


TimeTest timer1; //Declare Timer1 of TimeTest
void ledToggle(); // user defined function

int main()
{
	//setup timer
	TA0CTL = MC__UP | TASSEL__SMCLK | TAIE;
	TA0CCR0 = 1000;
	TA0CTL &= ~TAIFG;
	//init LEDs
	P3OUT &= ~0xF0;
	P3DIR |= 0xF0;
	__enable_interrupt();
	timer1.init();
	timer1.intervalSet(1000);
	timer1.callBackSet(&ledToggle);
	while (1)
	{
		timer1.start();
		for (int i = 0; i < 50; ++i)
		{
			
		}
		timer1.end();
		printf("%d\n",(int)timer1.getTime());
	}
	return 0;
}

void ledToggle()
{
	P3OUT ^= 0xF0;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
	timer1.update();
	TA0CTL &= ~TAIFG;
}
