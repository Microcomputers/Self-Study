#include "uart.h"
//Serial buffer
volatile unsigned char TX_Buffer[7] = {0,1,2,3,4,5,6};

//9600 baud
void serialBegin()
{
  // Configure UART 0
  UCA0CTL1 |= UCSWRST; 
  UCA0CTL1 = UCSSEL_2;              // Set SMCLK as UCLk 
  UCA0BR0 = 52 ;                    // 9600 baud
  // 8000000/(9600*16) - INT(8000000/(9600*16))=0.083
  UCA0BR1 = 0; 
  // UCBRFx = 1, UCBRSx = 0x49, UCOS16 = 1 (Refer User Guide)
  UCA0MCTLW = 0x4911;
  UCA0CTL1 &= ~UCSWRST;             // release from reset
}

void serialWrite()
{
	int counter = 0;
	while (counter < 7)
	{
	    while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
	    UCA0TXBUF = TX_Buffer[counter];
	    counter++;		
	}	
	counter = 0;
}
