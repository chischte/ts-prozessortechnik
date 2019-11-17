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
#include <stdlib.h>  // used for the abs() function

#define F_CPU 2000000
#include <util/delay.h>

#include "buttons.h"
#include "display.h"

int16_t firstLineNumber=0;
int16_t previousFirstLineNumber= 1;
char displayDigitLength=19;

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
    // DRUCK AUF «PLUS» ERHÖHT DIE ZAHLENSTELLE UM 1:
    if(DetectPlusButtonSwitch())
    {
      firstLineNumber++;
    }
    
    // DRUCK AUF «MINUS» VERKLEINERT DIE ZAHLENSTELLE UM 1:
    if(DetectMinusButtonSwitch())
    {
      firstLineNumber--;
    }
    
    // DRUCK AUF «ENTER» SPRINGT ZUR NÄCHSTEN ZAHLENSTELLE:
    if(DetectEnterButtonSwitch())
    {
      
    }
    
    // UPDATE DISPLAY:
    char numberLength=CalculateNumberLength(firstLineNumber);
    if(firstLineNumber!=previousFirstLineNumber){
      WriteNumberToLcd( 0, firstLineNumber, numberLength); // (position,number,width)
      previousFirstLineNumber=firstLineNumber;
      
      // CLEAR UNUSED DIGIT IF NUMBER OF DIGIT CHANGES:
       WriteCharToLcd(numberLength, ' '); // (position,char)
    }
    
  }//END OF LOOP
}// END OF MAIN