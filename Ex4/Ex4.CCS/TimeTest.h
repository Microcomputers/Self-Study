#ifndef TIMETEST_H
#define TIMETEST_H 

#define TimeTestOffset (22)

class TimeTest
{
private:
	uint16_t oldtcnt;
	uint16_t count;	
	uint16_t min;	
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