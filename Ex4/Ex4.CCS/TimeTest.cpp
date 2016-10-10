#include <msp430fr5739.h>
#include <inttypes.h>
#include "TimeTest.h"

void TimeTest::init ()
{
	//continuous mode!!!!
	TB0CTL = MC_2 | ID_0 | TASSEL_2;
}

//initialises each measurement
void TimeTest::start ()
{
	oldtcnt = TB0R;
}

void TimeTest::end ()
{
	uint16_t newtcnt;
 //If called before TimeTestStart leave now!!!
 // If the tcnt value is zero and no previous count get out!
	if (!((oldtcnt == 0)&&(count == 0))) 
	{	
		newtcnt = TB0R;
		count += 1; // increment count
		newtcnt -= (oldtcnt + TimeTestOffset);
		totaltime += newtcnt;
		if (count == 1) 
		{ // If first time through
			min = newtcnt;
			max = newtcnt;
			mincount = 1;
			maxcount = 1;
		}
		else if( newtcnt < min)
		{ // New minimum
			min = newtcnt;
			mincount = 1;
		}
		else if( newtcnt == min )
		{
			mincount++;
		} //another at minimum
		else if( newtcnt > max)
		{ // New maximum
			max = newtcnt;
			maxcount = 1;
		}
		else if( newtcnt == max )
		{
			maxcount++; 
		} // another at maximum
	    else {}
	}
	
}
