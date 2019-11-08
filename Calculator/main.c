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
* TODO:

*/
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#define F_CPU 2000000
#include <util/delay.h>

// DEFINE BUTTON MASKS:
#define PLUS_BUTTON_MASK   0b00010000
#define MINUS_BUTTON_MASK  0b00100000
#define ENTER_BUTTON_MASK  0b01000000
#define CLEAR_BUTTON_MASK  0b10000000
// DEFINE DISPLAY MASKS:
#define DISP_EN_MASK   0b00000100
#define DISP_RW_MASK   0b00000010
#define DISP_RS_MASK   0b00000001
#define DISP_DATA_MASK 0b11110000

void InitPorts()
{
  PORTF.DIRCLR= PLUS_BUTTON_MASK+MINUS_BUTTON_MASK+ENTER_BUTTON_MASK+CLEAR_BUTTON_MASK;
  PORTD.DIRSET= DISP_RS_MASK + DISP_RW_MASK + DISP_EN_MASK;
  PORTA.DIRSET= DISP_DATA_MASK;
}

// S1 Management
uint8_t S1= 0;
uint8_t S1Before= 0;  // 0 = offen, 1 = gedrückt
// S2 Management
uint8_t S2= 0;
uint8_t S2Before= 0;  // 0 = offen, 1 = gedrückt

void GetMinusButton()
{
  S2Before= S2;
  if(( PORTF.IN & MINUS_BUTTON_MASK)== 0)
  {
    S2= 1;
  }
  else
  {
    S2= 0;
  }
}


void GetPlusButton()
{
  S1Before= S1;
  if(( PORTF.IN & PLUS_BUTTON_MASK)== 0)
  {
    S1= 1;
  }
  else
  {
    S1= 0;
  }
}


//////// LCD Funktionen - Start
void Nibble()
{
  PORTD.OUTSET= DISP_EN_MASK;
  _delay_us( 20);
  PORTD.OUTCLR= DISP_EN_MASK;
  _delay_us( 20);
}

void SetData( uint8_t data)
{
  PORTA.OUTSET= data& DISP_DATA_MASK;
  PORTA.OUTCLR= ~data& DISP_DATA_MASK;
}

void SendValue( uint8_t value, bool command)
{
  if( command)
  {
    PORTD.OUTCLR= DISP_RS_MASK;
  }
  else
  {
    PORTD.OUTSET= DISP_RS_MASK;
  }
  PORTD.OUTCLR= DISP_RW_MASK;

  SetData( value);
  Nibble();
  value <<= 4;
  SetData( value);
  Nibble();
}

void SendCommand( uint8_t command)
{
  SendValue( command, true);
}

void Send_Data( uint8_t data)
{
  SendValue( data, false);
}

void InitLcd()
{
  SetData( 0);
  _delay_ms( 100);
  SetData( 0x30);
  _delay_ms( 30);
  Nibble();
  _delay_ms( 10);
  Nibble();
  _delay_ms( 10);
  Nibble();
  _delay_ms( 10);
  SetData( 0x20);
  Nibble();
  SendCommand( 0x28);
  SendCommand( 0x10);
  SendCommand( 0x0C);
  SendCommand( 0x04);
  SendCommand( 0x1);
  SendCommand( 0x2);
}

uint8_t DispPos= 0;
void MoveAndInc()
{
  SendCommand(0x80+ DispPos);
  if( DispPos== 0x13)
  {
    DispPos= 0x3F;
  }
  if( DispPos== 0x53)
  {
    DispPos= 0x13;
  }
  if( DispPos== 0x27)
  {
    DispPos= 0x53;
  }
  if( DispPos== 0x67)
  {
    DispPos= 0xFF;
  }
  DispPos+= 1;
}

void WriteChar( uint8_t c)
{
  MoveAndInc();
  Send_Data( c);
}

void WriteNumberToLcd( int8_t position, int16_t number, int8_t width)
{
  if( width< 0)
  {
    width= 0;
  }
  if( width> 20)
  {
    width= 20;
  }
  
  char numberText[32];
  sprintf( numberText, "%20d", number);
  DispPos= position;
  char* numberTextPtr= numberText+ 20- width;
  while( *numberTextPtr!= '\0')
  {
    WriteChar( *numberTextPtr);
    numberTextPtr++;
  }
}
//////// LCD Funktionen - Ende

// Counter Management
int16_t Counter= 0;
int16_t OldCounter= -1; // Sonst wird bei Start die 0 nicht ausgegeben
void SetCounter()
{
  // Die Zahl auf dem Display nur aktualisieren,
  // wenn sie sich geändert hat.
  if( Counter!= OldCounter)
  {
    WriteNumberToLcd( 0xE, Counter, 6);
    OldCounter= Counter;
  }
}


#define FOREVER 1
int main( void)
{
  InitPorts();
  InitLcd();
  
  while( FOREVER)
  {
   
    GetMinusButton();
    
    if( S2Before== 0)
    {
      if( S2 == 1)
      {
        Counter= Counter+ 1;
      }
    }
 
  
    GetPlusButton();
    
    if( S1Before== 0)
    {
      if( S1 == 1)
      {
        Counter= Counter+ 1;
      }
    }
 
    
    SetCounter();
    // DETEKTIERE OB KNOPF «PLUS» GEDRÜCKT WURDE:
    // DETEKTIERE OB KNOPF «MINUS» GEDRÜCKT WURDE:
    // DETEKTIERE OB KNOPF «ENTER» GEDRÜCKT WURDE:
    // DETEKTIERE OB KNOPF «CLEAR» GEDRÜCKT WURDE:

  }
}