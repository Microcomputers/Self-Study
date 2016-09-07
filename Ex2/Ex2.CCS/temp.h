#ifndef TEMP_H
#define TEMP_H 

#define DOWN 0
#define UP 1

extern void SetupThermistor(void);
extern void ShutDownTherm(void);

unsigned int getThermisterVal();

#endif
