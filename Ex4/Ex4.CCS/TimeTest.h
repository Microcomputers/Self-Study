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
	int interval;
	int timerCount;
	void (*callBack)();

public:

	void init();		// test init
	void start();		// test start
	void end();			// test end
	uint16_t getTime();		// returns totaltime
	void intervalSet (int interval);	//
	void callBackSet(void (*funcPtr)()); //pointer to a function 
	void update();		//increment the counter
};

#endif
