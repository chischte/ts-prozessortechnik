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
* htts://github.com/chischte/ts-prozessortechnik/tree/master/Calculator
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
#define displDigitsPerLine 20

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  // für die abs() funktion
#include <util/delay.h>

// NICHT GANZ KORREKTES VERWENDEN VON HEADER FILES ZUR STRUKTURIERUNG DES CODES:
// KOMPILIERT FEHLERFEI
#include "buttons.h"
#include "display.h"

enum operationStage {enterFirstOperand, enterOperation, enterSecondOperand, showResult};
enum selectedOperation {add, substract, multiply, divide};

// GLOBAL VARIABLES:

bool newDigitDefined=true;
bool storedNumberAvailable=false;
bool firstEnterPush=true;
bool operandSelected=false;
char currentProgramState=enterFirstOperand;
char currentOperand=multiply; // z.B.
char currentOperandChar;
char currentDigitNo=0;
int8_t currentDigitValue=0;
int16_t storedNumber=0;
int16_t firstOperand=0;
int16_t secondOperand=0;
int16_t result=0;

// FUNCTIONS:

// BERECHNEN DER ANZAHL STELLEN:
char CalculateNumberLength(int16_t number)
{
  char numberLength=1;
  // EINE STELLE FÜR DAS MINUS HINZUFÜGEN:
  if(number<0){
    numberLength++;
  }
  // FÜR JEDE DEZIMALSTELLE EINE DIGIT HINZUFÜGEN:
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
  for(int i=0;i<0x68;i++){
    WriteCharToLcd(i, charClear); // position,char
  }
}

void UpdateDisplay()
{
  int8_t position=0;
  if(currentProgramState==enterFirstOperand){
  position=displStartFirstLine;}
  if(currentProgramState==enterSecondOperand){
  position=displStartThirdLine;}
  
  // GESPEICHERTEN TEIL DER ZAHL PRINTEN:
  if(storedNumberAvailable){
    char numberLength=CalculateNumberLength(storedNumber);
    WriteNumberToLcd( position, storedNumber, numberLength); // (position,number,width)
    position = position+CalculateNumberLength(storedNumber);
  }
  // NEUESTE STELLE PRINTEN FALLS DEFINIERT:
  if(newDigitDefined)
  {
    char numberLength=CalculateNumberLength(currentDigitValue);
    WriteNumberToLcd( position, currentDigitValue, numberLength); // (position,number,width)
    position=position+numberLength;
    // BEI VORZEICHENWECHSEL NICHT VERWENDETE STELLE CLEAREN:
    WriteCharToLcd(position, charClear); // position,char
  }
}

// MANAGEN DES ÜBERLAUFS BEI AUSWAHL EINES DIGITS:
void FlipCurrentDigit(int step){ // step = -1 oder +1
  currentDigitValue+=step;
  
  if(currentDigitValue>9){
    currentDigitValue=0;
  }
  if (currentDigitNo==0){ // das erste Digit kann negativ sein
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

// ZWISCHEN DEN MÖGLICHEN OPERANDEN WECHSELN:
void FlipOperands()
{
  static char operand=0;
  if (operand==0) {
    currentOperand=add;
    currentOperandChar=charPlus;
  }
  if(operand==1)
  {
    currentOperand=substract;
    currentOperandChar=charMinus;
  }
  if(operand==2)
  {
    
    currentOperand=multiply;
    currentOperandChar=charMultiply;
  }
  if(operand==3)
  {
    currentOperand=divide;
    currentOperandChar=charDivide;
  }
  WriteCharToLcd(40, currentOperandChar); // position,char
  operand++;
  if(operand==4){
    operand=0;
  }
  operandSelected=true; // bestätigen mit Enter jezt möglich
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
  // EINGABEAUFFORDERUNG:
  WriteCharToLcd(0, charFull); // position,char
  // **************************************************************************
  // LOOP
  // **************************************************************************
  while(1)
  {
    // DRUCK AUF «PLUS»:
    if(DetectPlusButtonSwitch())
    {
      switch(currentProgramState){
        case enterFirstOperand:
        case enterSecondOperand:
        // ERHÖHT DEN WERT DER AKTUELLEN ZAHLENSTELLE UM 1:
        FlipCurrentDigit(+1);
        break;
        case enterOperation:
        // WECHSELT ZWISCHEN DEN OPERANDEN:
        FlipOperands();
      }
    }
    
    // DRUCK AUF «MINUS»:
    if(DetectMinusButtonSwitch())
    {
      switch(currentProgramState){
        case enterFirstOperand:
        case enterSecondOperand:
        // VERKLEINERT DEN WERT DER AKTUELLE ZAHLENSTELLE UM 1:
        FlipCurrentDigit(-1);
        break;
        case enterOperation:
        // WECHSELT ZWISCHEN DEN OPERANDEN:
        FlipOperands();
      }
    }
    
    // DRUCK AUF «ENTER»:
    if(DetectEnterButtonSwitch())
    {
      switch(currentProgramState){
        case enterFirstOperand:
        // SPRINGT ZUR NÄCHSTEN ZAHLENSTELLE:
        if(firstEnterPush)
        {
          // BESTÄTIGEN DES EINGEGEBEN DIGIT WERTS:
          storedNumber=UpdateStoredNumber();
          storedNumberAvailable=true;
          currentDigitValue=0;
          newDigitDefined=false;
          // ANZEIGEN DER EINGABEAUFFORDERUNG FÜR DIE NÄCHSTE STELLE:
          char numberLength=CalculateNumberLength(storedNumber);
          WriteCharToLcd(displStartFirstLine+numberLength, charFull); // position,char
          currentDigitNo++;
          UpdateDisplay();
          firstEnterPush=false;
        }
        else{ // SECOND ENTER PUSH:
          // BESTÄTIGEN UND ANZEIGEN DER GESPEICHERTEN ZAHL ALS ERSTEN OPERANDEN:
          firstOperand=storedNumber;
          WriteNumberToLcd( displStartFirstLine, firstOperand, displDigitsPerLine); //pos,num,width
          // VORBEREITEN FÜR OPERATION STAGE "enterOperand":
          storedNumber=0;
          storedNumberAvailable=false;
          currentDigitNo=0;
          newDigitDefined=false;
          firstEnterPush=true;
          currentProgramState++;
          // EINGEABEAUFFORDERUNG FÜR OPERATOR (1-*:) ANZEIGEN
          WriteCharToLcd(displStartSecondLine, charFull); // position,char
        }
        break;
        case enterOperation:
        if(operandSelected){
          // OPERATOR ANZEIGEN:
          WriteCharToLcd(displStartSecondLine, charClear); // position,char
          WriteCharToLcd(displEndOfSecondLine, currentOperandChar); // position,char
          // EINGABEAUFFORDERUNG FÜR ZWEITEN OPERANDEN:
          WriteCharToLcd(displStartThirdLine, charFull); // position,char
          currentProgramState++;
        }
        break;
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
          WriteCharToLcd(displStartThirdLine+numberLength, charFull); // position,char
          
          currentDigitNo++;
          UpdateDisplay();
          firstEnterPush=false;
        }
        else{ // SECOND ENTER PUSH:
          // BESTÄTIGEN DER GESPEICHERTEN ZAHL ALS ZWEITEN OPERANDEN:
          // ANZEIGEN DES ZWEITEN OPERANDEN:
          secondOperand=storedNumber;
          WriteNumberToLcd( displStartThirdLine, secondOperand, displDigitsPerLine); //pos,num,width
          
          firstEnterPush=true;
          currentProgramState++;
          // RESULTAT BERECHNEN UND ANZEIGEN:
          CalculateResult();
          WriteNumberToLcd( displStartFourthLine, result, displDigitsPerLine); // pos,num,width
          firstEnterPush=false;
          break;
          case showResult:
          // RESULTAT ALS ERSTEN OPERANDEN SETZEN UND VARIABLEN VORBEREITEN:
          ClearScreen();
          firstOperand=result;
          WriteNumberToLcd( displStartFirstLine,firstOperand, displDigitsPerLine); // pos,num,width
          WriteCharToLcd(displStartSecondLine, charFull); // position,char
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
      WriteCharToLcd(displStartFirstLine, charFull); // position,char
    }
  }// END OF MAIN LOOP
}// END OF MAIN