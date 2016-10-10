#ifndef TIMETEST_H
#define TIMETEST_H 

#include <inttypes.h>

class TimeTest
{
private:
	uint16_t oldtcnt;
	uint16_t count;	
	uint16_t main;	
	uint16_t mincount;	
	uint16_t max;
	uint16_t maxcount;
	uint16_t totaltime;
		
public:
	void init();		// test init
	void start();		// test start
	void end();			// test end
	
};

#endif