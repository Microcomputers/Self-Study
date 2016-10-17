#ifndef SYSTEM_H
#define SYSTEM_H
#include <msp430fr5739.h>

// Type definitions
typedef unsigned char uc;
typedef enum {FALSE, TRUE} bool;
typedef unsigned char uc_8;
typedef unsigned int uc_16;

//variables
//

// User definitions
#define LED_TOG_DLY_CONST	1000
#define SW_DEB_CONST		1600			// Load into TA1CCR0 the value SW_DEB_CONST i.e. debounce time = 1600*1/8000 = 200ms
#define SW_HALF_DEB_CONST	SW_DEB_CONST/2	// Shorter debounce time model assuming half

//Clock init
void clockInit();
//LEDs init
void ledsInit(uc_8 ledNumber);
//LEDs Write
bool ledsWrite(uc_8 ledNumber, uc_8 value);
//Switches init
void switchesInit(uc_8 buttonNumber);
//Switches Read
bool switchesRead(uc_8 buttonNumber, uc_8 *value);

void SetupThermistor(void);
void ShutDownTherm(void);

//Enable/Disable Switches
inline void EnableSwitches();
inline void DisableSwitches();
inline void StartDebounceTimer(uc ucDelay);

void CalibrateADC(void);


#endif
