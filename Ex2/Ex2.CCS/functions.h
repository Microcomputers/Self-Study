#ifndef FUNCTION_H
#define FUNCTION_H 
#include <msp430fr5739.h>

// Type definitions
typedef unsigned char		uc;

// User definitions
#define LED_TOG_DLY_CONST	1000
#define SW_DEB_CONST		1600			// Load into TA1CCR0 the value SW_DEB_CONST i.e. debounce time = 1600*1/8000 = 200ms
#define SW_HALF_DEB_CONST	SW_DEB_CONST/2	// Shorter debounce time model assuming half

//Clock init
void CLOCK_INTI();
//LEDs init
void LEDs_INIT();
//Switches init
void SWITCHEs_INIT();

//Enable/Disable Switches
inline void EnableSwitches();
inline void DisableSwitches();
inline void StartDebounceTimer(uc ucDelay);

#endif