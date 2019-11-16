/*
* *****************************************************************************
* CALCULATOR
* *****************************************************************************
* A simple program in C to add a calculator functionality to a prototyping
* board "EduBoard" as part of a school work.
* The code is written to be uploaded to an ATxmega128A3U AVR microcontroller.
* The User interface consists of 4 push buttons and a 4x20 digits display.
* *****************************************************************************
* Michael Wettstein
* November 2019, Zürich
* *****************************************************************************
*/


#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#define F_CPU 2000000
#include <util/delay.h>

#include "buttons.h"
#include "display.h"

int main( void)
{
	// **************************************************************************
	// SETUP
	// **************************************************************************
	InitButtonPort();
	InitDisplayPort();
	InitLcd();
	
	// **************************************************************************
	// LOOP
	// **************************************************************************
	while(1)
	{
		// DRUCK AUF «PLUS» SCHREIBT EINE 1 IN FELD (0,0):
		if(DetectPlusButtonSwitch())
		{
			WriteNumberToLcd( 0, 1, 1);
		}
		
		// DRUCK AUF «MINUS» SCHREIBT EINE 2 IN FELD (1,0):
		if(DetectMinusButtonSwitch())
		{
			WriteNumberToLcd( 1, 2, 1);
		}
		
		// DRUCK AUD «ENTER» SCHREIBT EINE 3 IN FELD (2,0):
		if(DetectEnterButtonSwitch())
		{
			WriteNumberToLcd( 2, 3, 1);
		}
		
		// DRUCK AUF «CLEAR SCHREIBT EINE 4 IN FELD (3,0);
		if(DetectClearButtonSwitch())
		{
			WriteNumberToLcd( 3, 4, 1);
		}
		
	}//END OF LOOP
}// END OF MAIN