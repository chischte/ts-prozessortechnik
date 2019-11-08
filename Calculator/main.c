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
    if(DetectPlusButtonSwitch())
    {
      Counter++;
    }
    if(DetectMinusButtonSwitch())
    {
      Counter++;
    }
    if(DetectEnterButtonSwitch())
    {
      Counter++;
    }
    if(DetectClearButtonSwitch())
    {
      Counter++;
    }
    SetCounter();
    }
}