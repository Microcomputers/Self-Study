#include <msp430fr5739.h>
#include <inttypes.h>
#include <stdio.h>
#include "TimeTest.h"




int main()
{
	TimeTest timer1; //Declare Timer1 of TimeTest

	timer1.init();
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
