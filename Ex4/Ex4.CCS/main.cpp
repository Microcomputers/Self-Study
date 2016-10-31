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
		printf("%s%d\t", "oldtcnt: ",(int)timer1.getoldtcnt());
		printf("%s%d\t", "count: ",(int)timer1.getcount());
		printf("%s%d\t", "min: ",(int)timer1.getmin());
		printf("%s%d\t", "mincount: ",(int)timer1.getmincount());
		printf("%s%d\t", "max: ",(int)timer1.getmax());
		printf("%s%d\t", "maxcount: ",(int)timer1.getmaxcount());
		printf("%s%d\n", "totaltime: ",(int)timer1.gettotaltime());
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
