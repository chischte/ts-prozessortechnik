/*
* buttons.h
*
* Created: 08/11/2019 23:11:56
*  Author: Michael Wettstein
*/

#ifndef BUTTONS_H_
#define BUTTONS_H_

// DEFINE BUTTON MASKS:
#define PLUS_BUTTON_MASK   0b00010000
#define MINUS_BUTTON_MASK  0b00100000
#define ENTER_BUTTON_MASK  0b01000000
#define CLEAR_BUTTON_MASK  0b10000000

// SwitchState Management
uint8_t plusState= 0;
uint8_t prevPlusState= 0;  // 0 = offen, 1 = gedrückt

uint8_t minusState= 0;
uint8_t prevMinusState= 0;  // 0 = offen, 1 = gedrückt

uint8_t enterState= 0;
uint8_t prevEnterState= 0;  // 0 = offen, 1 = gedrückt

uint8_t clearState= 0;
uint8_t prevClearState= 0;  // 0 = offen, 1 = gedrückt


void InitButtonPort()
{
  PORTF.DIRCLR= PLUS_BUTTON_MASK+MINUS_BUTTON_MASK+ENTER_BUTTON_MASK+CLEAR_BUTTON_MASK;
}


void GetPlusButton()
{
  prevPlusState= plusState;
  if(( PORTF.IN & PLUS_BUTTON_MASK)== 0)
  {
    plusState= 1;
  }
  else
  {
    plusState= 0;
  }
}

void GetMinusButton()
{
  prevMinusState= minusState;
  if(( PORTF.IN & MINUS_BUTTON_MASK)== 0)
  {
    minusState= 1;
  }
  else
  {
    minusState= 0;
  }
}
void GetEnterButton()
{
  prevEnterState= enterState;
  if(( PORTF.IN & ENTER_BUTTON_MASK)== 0)
  {
    enterState= 1;
  }
  else
  {
    enterState= 0;
  }
}
void GetClearButton()
{
  prevClearState= clearState;
  if(( PORTF.IN & CLEAR_BUTTON_MASK)== 0)
  {
    clearState= 1;
  }
  else
  {
    clearState= 0;
  }
}

// DETEKTIERE OB KNOPF «PLUS» GEDRÜCKT WURDE:
bool DetectPlusButtonSwitch()
{
  bool switchDetected=false;
  GetPlusButton();
  
  if( prevPlusState== 0)
  {
    if( plusState == 1)
    {
      switchDetected=true;
    }
    }
  return switchDetected;
}
// DETEKTIERE OB KNOPF «MINUS» GEDRÜCKT WURDE:
bool DetectMinusButtonSwitch()
{
  bool switchDetected=false;
  GetMinusButton();
  
  if( prevMinusState== 0)
  {
    if( minusState == 1)
    {
      switchDetected=true;
    }
    
  }
  return switchDetected;
}
// DETEKTIERE OB KNOPF «ENTER» GEDRÜCKT WURDE:
bool DetectEnterButtonSwitch()
{
  bool switchDetected=false;
  GetEnterButton();
  
  if( prevEnterState== 0)
  {
    if( enterState == 1)
    {
      switchDetected=true;
    }
    
  }
  return switchDetected;
}
// DETEKTIERE OB KNOPF «CLEAR» GEDRÜCKT WURDE:
bool DetectClearButtonSwitch()
{
  bool switchDetected=false;
  GetClearButton();
  
  if( prevClearState== 0)
  {
    if( clearState == 1)
    {
      switchDetected=true;
    }
    
  }
  return switchDetected;
}

#endif