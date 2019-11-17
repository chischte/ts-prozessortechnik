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
//#include <math.h>    // used for the pow() function

#define F_CPU 2000000
#include <util/delay.h>

#include "buttons.h"
#include "display.h"

int16_t storedNumber=0;
int16_t upperLineNumber=0;
int8_t currentDigitValue=0;
char currentDigitNo=0;
bool newDigitDefined=true;
bool storedNumberAvailable=false;

char noOfDigitsPerLine=20;
bool firstEnterPush=true;

int32_t CalculateCurrentNumber()
{
  int32_t currentNumber=0;
  
  if (storedNumber>=0){
    currentNumber=storedNumber*10 + currentDigitValue;
  }
  else{
    currentNumber=storedNumber*10-currentDigitValue;
  }
  return currentNumber;
}

void UpdateDisplay()
{
  int8_t position=0;
  
  // PRINT STORED NUMBER IF AVAILABLE:
  if(storedNumberAvailable){
    char numberLength=CalculateNumberLength(storedNumber);
    WriteNumberToLcd( position, storedNumber, numberLength); // (position,number,width)
    position = CalculateNumberLength(storedNumber);
  }
  //PRINT CURRENT DIGIT IF USED:
  if(newDigitDefined)
  {
    char numberLength=CalculateNumberLength(currentDigitValue);
    WriteNumberToLcd( position, currentDigitValue, numberLength); // (position,number,width)
    position=position+numberLength;
    // CLEAR UNUSED DIGIT IF NUMBER OF DIGIT CHANGES:
    WriteCharToLcd(position, ' '); // (position,char)
  }
}


int main( void)
{
  // **************************************************************************
  // SETUP
  // **************************************************************************
  InitButtonPort();
  InitDisplayPort();
  InitLcd();
  UpdateDisplay();
  
  // **************************************************************************
  // LOOP
  // **************************************************************************
  while(1)
  {
    
    // DRUCK AUF «PLUS» ERHÖHT DEN WERT DER AKTUELLEN ZAHLENSTELLE UM 1:
    if(DetectPlusButtonSwitch())
    {
      // DRÜCKEN DER PLUS TASTE ERZEUGT WERTE VON 0 BIS 9
      if(currentDigitValue<9){
        currentDigitValue++;
      }
      else{
        currentDigitValue=0;
      }
      newDigitDefined=true;
      firstEnterPush=true;
      UpdateDisplay();
    }
    
    // DRUCK AUF «MINUS» VERKLEINERT DEN WERT DER AKTUELLE ZAHLENSTELLE UM 1:
    if(DetectMinusButtonSwitch())
    {
      // BEI DER ERSTEN STELLE SIND WERTE VON -9 bis +9 ZULÄSSIG
      if (currentDigitNo==0){
        if(currentDigitValue>(-9)){
          currentDigitValue--;
        }
        else{
          currentDigitValue=0;
        }
      }
      // BEI DEN NACHFOLGENDEN STELLEN SIND WERTE VON 0 BIS 9 ZULÄSSIG
      if (currentDigitNo>0){
        if(currentDigitValue>0){
          currentDigitValue--;
        }
        else{
          currentDigitValue=9;
        }
      }
      newDigitDefined=true;
      firstEnterPush=true;
      UpdateDisplay();
    }
    
    // DRUCK AUF «ENTER» SPRINGT ZUR NÄCHSTEN ZAHLENSTELLE:
    if(DetectEnterButtonSwitch())
    {
      if(firstEnterPush)
      {
        if(storedNumber>=0)
        {
          storedNumber=(storedNumber*10)+currentDigitValue;
          //storedNumber=564;
        }
        else{
          storedNumber=(storedNumber*10)-currentDigitValue;
        }
        currentDigitValue=0;
        storedNumberAvailable=true;
        newDigitDefined=false;
        char numberLength=CalculateNumberLength(storedNumber);
        WriteCharToLcd(numberLength, '_'); // (position,char)
        currentDigitNo++;
        UpdateDisplay();
        firstEnterPush=false;
      }
      if(!firstEnterPush)
      {
        //storedNumber=CalculateCurrentNumber();
      }
    }
    
    // DRUCK AUF CLEAR LÖSCHT DIE GANZE EINGABE:
    if(DetectClearButtonSwitch())
    {
      storedNumber=0;
      currentDigitNo=0;
      currentDigitValue=0;
      storedNumberAvailable=false;
      firstEnterPush=false;
      for(int i=0;i<noOfDigitsPerLine;i++){
        WriteCharToLcd(i, ' '); // (position,char)
      }
      WriteNumberToLcd( 0, 0, 0); // (position,number,width)
    }
    
  }//END OF LOOP
}// END OF MAIN