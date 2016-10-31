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
	void intervalSet (int interval);	//
	void callBackSet(void (*funcPtr)()); //pointer to a function 
	void update();		//increment the counter
	uint16_t getoldtcnt() 	{return this->oldtcnt;} 	//returns oldtcnt
	uint16_t getcount() 	{return this->count;} 		//returns count
	uint16_t getmin() 		{return this->min;} 		//returns min
	uint16_t getmincount()  {return this->mincount;} 	//returns mincount
	uint16_t getmax() 		{return this->max;} 		//returns max
	uint16_t getmaxcount()  {return this->maxcount;} 	//returns maxcount
	uint16_t gettotaltime() {return this->totaltime;}	//returns totaltime
};

#endif
