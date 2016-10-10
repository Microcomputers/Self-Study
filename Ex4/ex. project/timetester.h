#ifndef TIMETESTER_H
#define TIMETESTER_H

#include "clic3.h"

 typedef struct {ui_16 oldtcnt; ui_16 count; ui_16 min; ui_16 mincount; ui_16 max; ui_16 maxcount; ui_32 totaltime;} TimeTest ;

/* Structure must be initialised as follows
 * { 0, 0, 65535, 0, 0, 0, 0}
 */
/* Prototypes */

/* Initialise the timing for the system
NOTE this needs to be independent of other timer uses so don't overlap
*/
void TimeTestInit(void);
 
/* TimeTestStart initialises each measurement */
void TimeTestStart(TimeTest *fred);

/* TimeTestEnd completes each measurement and leaves results in the structure
 * Can appear in the code before TimeTestStart to assist measuring looping situations

 * The structure values returned:
 * Last starting TCNT. Not of value after execution stopped
 * Provides a count of the number of code executions .count
 * The minimum time execution in cycles and count: .min and .mincount
 * The maximum time execution in cycles and count: .max and .maxcount
 * The total time of all measurements: .totaltime
 *
 * SO average time is this .totaltime/.count
 */
void TimeTestEnd(TimeTest *fred);

ui_16 TimeTestCheck(ui_16 token) {
	ui_16 total = 0; 
	ui_16 counter = 0; 

	while (token == 1) {
		++counter;
		if (counter == 10) {
			++total; 
			counter = 0; 
		}
	}

	return total*10; 
} 




#endif
