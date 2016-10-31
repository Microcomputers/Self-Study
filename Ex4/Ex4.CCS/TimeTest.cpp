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
	this->oldtcnt = TB0R;
}

void TimeTest::end ()
{
	uint16_t newtcnt;
 //If called before TimeTestStart leave now!!!
 // If the tcnt value is zero and no previous count get out!
	if (!((this->oldtcnt == 0)&&(this->count == 0))) 
	{	
		newtcnt = TB0R;
		this->count += 1; // increment count
		newtcnt -= (this->oldtcnt + TimeTestOffset);
		this->totaltime += newtcnt;
		if (this->count == 1) 
		{ // If first time through
			this->min = newtcnt;
			this->max = newtcnt;
			this->mincount = 1;
			this->maxcount = 1;
		}
		else if( newtcnt < this->min)
		{ // New minimum
			this->min = newtcnt;
			this->mincount = 1;
		}
		else if( newtcnt == this->min )
		{
			this->mincount++;
		} //another at minimum
		else if( newtcnt > this->max)
		{ // New maximum
			this->max = newtcnt;
			this->maxcount = 1;
		}
		else if( newtcnt == this->max )
		{
			this->maxcount++; 
		} // another at maximum
	    else {}

	}
	
}

uint16_t TimeTest::getTime()
{
	return this->totaltime;
}

void TimeTest::intervalSet (int interval)
{
	this->interval = interval;
}

void TimeTest::callBackSet(void (*funcPtr)())
{
	this->callBack = funcPtr;
}

void TimeTest::update()
{
	if (this->interval >= this->timerCount)
	{
		this->timerCount = 0;
		this->callBack();
	}
	else
	{
		this->timerCount++;
	}
}
