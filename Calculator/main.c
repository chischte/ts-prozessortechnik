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

#define F_CPU 2000000
#define plusChar 0x2b
#define minusChar 0xb0
#define multiplyChar 0x2a
#define divideChar 0x3a


#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  // used for the abs() function
#include <util/delay.h>

#include "buttons.h"
#include "display.h"

enum operationStage {enterFirstOperand, enterOperation, enterSecondOperand};

enum selectedOperation {add, substract, multiply, divide};


// GLOBAL VARIABLES:
int16_t storedNumber=0;
int16_t firstOperand=0;
int8_t currentDigitValue=0;
char currentProgramState=enterFirstOperand;
char currentOperand=multiply;
char noOfDigits=80;
char currentDigitNo=0;
bool newDigitDefined=true;
bool storedNumberAvailable=false;
bool firstEnterPush=true;
bool operandSelected=false;


// FUNCTIONS:
char CalculateNumberLength(int16_t number)
{
  char numberLength=1;
  // CALCULATE NUMBER OF DIGITS:
  // ADD A DIGIT FOR THE MINUS:
  if(number<0){
    numberLength++;
  }
  // ADD A DIGIT FOR EVERY POWER OF 10:
  while(abs(number)>=10){
    number/=10;
    numberLength++;
  }
  return numberLength;
}

int16_t UpdateStoredNumber(){
  int16_t newStoredNumber;
  if(storedNumber>=0)
  {
    newStoredNumber=(storedNumber*10)+currentDigitValue;
  }
  else{
    newStoredNumber=(storedNumber*10)-currentDigitValue;
  }
  return newStoredNumber;
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
  // PRINT CURRENT DIGIT IF USED:
  if(newDigitDefined)
  {
    char numberLength=CalculateNumberLength(currentDigitValue);
    WriteNumberToLcd( position, currentDigitValue, numberLength); // (position,number,width)
    position=position+numberLength;
    // CLEAR UNUSED DIGIT IF NUMBER OF DIGITS CHANGES:
    WriteCharToLcd(position, ' '); // (position,char)
  }
}


void FlipOperands()
{
  static char operand=0;
  if (operand==0) {
    WriteCharToLcd(40, plusChar); // + (position,char)
    currentOperand=add;
  }
  if(operand==1)
  {
    WriteCharToLcd(40, minusChar); // - (position,char)
    currentOperand=substract;
  }
  if(operand==2)
  {
    WriteCharToLcd(40, multiplyChar); // * (position,char)
    currentOperand=multiply;
  }
  if(operand==3)
  {
    WriteCharToLcd(40, divideChar); // : (position,char)
    currentOperand=divide;
  }
  operand++;
  if(operand==4){
    operand=0;
  }
  operandSelected=true; // muss aber noch mit ENTER bestätigt werden
}

int main( void)
{
  // **************************************************************************
  // SETUP
  // **************************************************************************
  
  InitButtonPort();
  InitDisplayPort();
  InitLcd();
  WriteCharToLcd(0, '_'); // (position,char)
  
  // **************************************************************************
  // LOOP
  // **************************************************************************
  while(1)
  {
    // DRUCK AUF «PLUS» ERHÖHT DEN WERT DER AKTUELLEN ZAHLENSTELLE UM 1:
    if(DetectPlusButtonSwitch())
    {
      switch(currentProgramState){
        
        case enterFirstOperand:
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
        break;
        case enterOperation:
        FlipOperands();
      }
    }
    
    // DRUCK AUF «MINUS» VERKLEINERT DEN WERT DER AKTUELLE ZAHLENSTELLE UM 1:
    if(DetectMinusButtonSwitch())
    {
      switch(currentProgramState){
        
        case enterFirstOperand:
        // BEI DER ERSTEN STELLE SIND WERTE VON -9 bis +9 ZULÄSSIG:
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
        break;
        
        case enterOperation:
        FlipOperands();
      }
    }
    
    // DRUCK AUF «ENTER» SPRINGT ZUR NÄCHSTEN ZAHLENSTELLE:
    if(DetectEnterButtonSwitch())
    {
      switch(currentProgramState){
        
        case enterFirstOperand:
        if(firstEnterPush)
        {
          storedNumber=UpdateStoredNumber();
          
          currentDigitValue=0;
          storedNumberAvailable=true;
          newDigitDefined=false;
          // ANZEIGEN DER EINGABEAUFFORDERUNG FÜR DIE NÄCHSTE STELLE:
          char numberLength=CalculateNumberLength(storedNumber);
          WriteCharToLcd(numberLength, '_'); // (position,char)
          currentDigitNo++;
          UpdateDisplay();
          firstEnterPush=false;
        }
        else{ // SECOND ENTER PUSH:
          // BESTÄTIGEN DER GESPEICHERTEN ZAHL ALS ERSTEN OPERANDEN:
          firstOperand=storedNumber;
          // ANZEIGEN DES ERSTEN OPERANDEN:
          WriteNumberToLcd( 0, firstOperand, 20); //pos,num,width
          // PREPARE FOR OPERATION STAGE "enterOperand":
          storedNumber=0;
          storedNumberAvailable=false;
          currentProgramState=enterOperation;
          // EINGEABEAUFFORDERUNG FÜR OPERATOR (1-*/) ANZEIGEN
          WriteCharToLcd(40, 0xff); // (position,char)
        }
        break;
        
        case enterOperation:
        break;
        
        case enterSecondOperand:
        if(firstEnterPush)
        {}else{ // SECOND ENTER PUSH:
        
      }
      break;
    }
  }

  // DRUCK AUF CLEAR LÖSCHT DIE GANZE EINGABE:
  if(DetectClearButtonSwitch())
  {
    storedNumber=0;
    currentDigitNo=0;
    currentDigitValue=0;
    storedNumberAvailable=false;
    firstEnterPush=true;
    for(int i=0;i<noOfDigits;i++){
      WriteCharToLcd(i, ' '); // (position,char)
    }
    WriteNumberToLcd( 0, 0, 0); // (position,number,width)
  }
}// END OF MAIN LOOP
}// END OF MAIN