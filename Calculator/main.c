/*
* *****************************************************************************
* CALCULATOR
* *****************************************************************************
* Program in C to add a calculator functionality to a prototyping
* board "EduBoard" as part of a school work.
* The code is written to be uploaded to an ATxmega128A3U AVR microcontroller.
* The User interface consists of 4 push buttons and a 4x20 digits display.
* *****************************************************************************
* Michael Wettstein
* November 2019, Zürich
* *****************************************************************************
*/

#define F_CPU 2000000
#define charPlus 0x2b
#define charMinus 0xb0
#define charMultiply 0x2a
#define charDivide 0x3a
#define charClear 0xa0
#define charFull 0xff
#define displStartFirstLine 0x00
#define displEndOfFirstLine 0x13
#define displStartSecondLine 0x40
#define displEndOfSecondLine 0x53
#define displStartThirdLine 0x14
#define displEndOfThirdline 0x27
#define displStartFourthLine 0x54
#define displEndOfFourthLine 0x67

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  // used for the abs() function
#include <util/delay.h>

#include "buttons.h"
#include "display.h"

enum operationStage {enterFirstOperand, enterOperation, enterSecondOperand, showResult};
enum selectedOperation {add, substract, multiply, divide};


// GLOBAL VARIABLES:
int16_t storedNumber=0;
int16_t firstOperand=0;
int16_t secondOperand=0;
int16_t result=0;
int8_t currentDigitValue=0;
char currentProgramState=enterFirstOperand;
char currentOperand=multiply; // for example
char currentOperandChar;
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

void ClearScreen()
{
  // CLEAR SCREEN:
  for(int i=0;i<0x68;i++){
    WriteCharToLcd(i, charClear); // (position,char)
  }
}

void UpdateDisplay()
{
  int8_t position=0;
  if(currentProgramState==enterFirstOperand){
  position=displStartFirstLine;}
  if(currentProgramState==enterSecondOperand){
  position=displStartThirdLine;}
  
  // PRINT STORED NUMBER IF AVAILABLE:
  if(storedNumberAvailable){
    char numberLength=CalculateNumberLength(storedNumber);
    WriteNumberToLcd( position, storedNumber, numberLength); // (position,number,width)
    position = position+CalculateNumberLength(storedNumber);
  }
  // PRINT CURRENT DIGIT IF USED:
  if(newDigitDefined)
  {
    char numberLength=CalculateNumberLength(currentDigitValue);
    WriteNumberToLcd( position, currentDigitValue, numberLength); // (position,number,width)
    position=position+numberLength;
    // CLEAR UNUSED DIGIT IF NUMBER OF DIGITS CHANGES:
    WriteCharToLcd(position, charClear); // (position,char)
  }
}

void FlipCurrentDigit(int step){ //steps are -1 or +1
  currentDigitValue+=step;
  
  if(currentDigitValue>9){
    currentDigitValue=0;
  }
  if (currentDigitNo==0){ //das erste Digit kann negativ sein
    if(currentDigitValue<(-9)){
      currentDigitValue=0;
    }
  }
  else{ // die folgenden Digits müssen mindestens 0 Sein
    if(currentDigitValue<0)
    currentDigitValue=9;
  }
  newDigitDefined=true;
  firstEnterPush=true;
  UpdateDisplay();
}

void FlipOperands()
{
  static char operand=0;
  if (operand==0) {
    currentOperandChar=charPlus;
    currentOperand=add;
  }
  if(operand==1)
  {
    currentOperandChar=charMinus;
    currentOperand=substract;
  }
  if(operand==2)
  {
    currentOperandChar=charMultiply;
    currentOperand=multiply;
  }
  if(operand==3)
  {
    currentOperandChar=charMultiply;
    currentOperand=divide;
  }
  WriteCharToLcd(40, currentOperandChar); // : (position,char)
  operand++;
  if(operand==4){
    operand=0;
  }
  operandSelected=true; // muss aber noch mit ENTER bestätigt werden
}

void CalculateResult(){
  if(currentOperand==add){
    result=firstOperand+secondOperand;
  }
  if(currentOperand==substract){
    result=firstOperand-secondOperand;
  }
  if(currentOperand==multiply){
    result=firstOperand*secondOperand;
  }
  if(currentOperand==divide){
    result=firstOperand/secondOperand;
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
  WriteCharToLcd(0, charFull); // (position,char)
  
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
        case enterSecondOperand: // gleiches Verhalten in beiden Cases!
        // DRÜCKEN DER PLUS TASTE ERZEUGT WERTE VON 0 BIS 9
        FlipCurrentDigit(+1);
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
        case enterSecondOperand:
        // BEI DER ERSTEN STELLE SIND WERTE VON -9 bis +9 ZULÄSSIG:
        FlipCurrentDigit(-1);
        break;
        
        case enterOperation:
        FlipOperands();
      }
    }
    
    // DRUCK AUF «ENTER»
    if(DetectEnterButtonSwitch())
    {
      switch(currentProgramState){
        
        case enterFirstOperand:
        case enterSecondOperand:
        // SPRINGT ZUR NÄCHSTEN ZAHLENSTELLE:
        if(firstEnterPush)
        {
          storedNumber=UpdateStoredNumber();
          currentDigitValue=0;
          storedNumberAvailable=true;
          newDigitDefined=false;
          // ANZEIGEN DER EINGABEAUFFORDERUNG FÜR DIE NÄCHSTE STELLE:
          char numberLength=CalculateNumberLength(storedNumber);
          if(currentProgramState==enterFirstOperand){
            WriteCharToLcd(displStartFirstLine+numberLength, '_'); // (position,char)
          }
          if(currentProgramState==enterSecondOperand)
          {
            WriteCharToLcd(displStartThirdLine+numberLength, '_'); // (position,char)
          }
          currentDigitNo++;
          UpdateDisplay();
          firstEnterPush=false;
        }
        else{ // SECOND ENTER PUSH:
          // BESTÄTIGEN DER GESPEICHERTEN ZAHL ALS ERSTEN OPERANDEN:
          // ANZEIGEN DES ERSTEN OPERANDEN:
          if(currentProgramState==enterFirstOperand)
          {
            firstOperand=storedNumber;
            WriteNumberToLcd( displStartFirstLine, firstOperand, 20); //pos,num,width
          }
          if(currentProgramState==enterSecondOperand)
          {
            secondOperand=storedNumber;
            WriteNumberToLcd( displStartThirdLine, secondOperand, 20); //pos,num,width
          }
          
          // PREPARE FOR OPERATION STAGE "enterOperand":
          storedNumber=0;
          storedNumberAvailable=false;
          currentDigitNo=0;
          newDigitDefined=false;
          firstEnterPush=true;
          currentProgramState++;
          // EINGEABEAUFFORDERUNG FÜR OPERATOR (1-*/) ANZEIGEN
          if(currentProgramState==enterOperation)
          {
            WriteCharToLcd(displStartSecondLine, charFull); // (position,char)
          }
        }
        break;
        
        case enterOperation:
        if(operandSelected){
          WriteCharToLcd(displStartSecondLine, charClear); // : (position,char)
          WriteCharToLcd(displEndOfSecondLine, currentOperandChar); // : (position,char)
          currentProgramState++;
        }
        break;
        
        case showResult:
        if(firstEnterPush){
          CalculateResult();
          WriteNumberToLcd( displStartFourthLine, result, 20); //pos,num,width
          firstEnterPush=false;
        }
        else{ // SECOND ENTER PUSH:
          // SET RESULT AS FIRST OPERAND AND PREPARE VARIABLES:
          ClearScreen();
          firstOperand=result;
          WriteNumberToLcd( displStartFirstLine,firstOperand, 20); //pos,num,width
          WriteCharToLcd(displStartSecondLine, charFull); // (position,char)
          secondOperand=0;
          firstEnterPush=true;
          storedNumber=0;
          currentDigitNo=0;
          currentDigitValue=0;
          storedNumberAvailable=false;
          newDigitDefined=false;
          firstEnterPush=true;
          operandSelected=false;
          currentProgramState=enterOperation;
        }
        break;
      }
    }

    // DRUCK AUF CLEAR LÖSCHT DIE GANZE EINGABE:
    if(DetectClearButtonSwitch())
    {
      // RESET ALL RELEVANT VARIABLES:
      storedNumber=0;
      currentDigitNo=0;
      currentDigitValue=0;
      storedNumberAvailable=false;
      newDigitDefined=true;
      firstEnterPush=true;
      operandSelected=false;
      currentProgramState=enterFirstOperand;
      ClearScreen();
      WriteCharToLcd(displStartFirstLine, charFull); // (position,char)
      //WriteNumberToLcd( 0, 0, 0); // (position,number,width)
    }
  }// END OF MAIN LOOP
}// END OF MAIN