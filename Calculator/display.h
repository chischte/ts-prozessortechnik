/*
* display.h
*
* Created: 08/11/2019 23:11:56
*  Author: Michael Wettstein
*/

#ifndef DISPLAY_H_
#define DISPLAY_H_

// DEFINE DISPLAY MASKS:
#define DISP_EN_MASK   0b00000100
#define DISP_RW_MASK   0b00000010
#define DISP_RS_MASK   0b00000001
#define DISP_DATA_MASK 0b11110000

// GLOBAL VARIABLE:
uint8_t DispPos= 0;

// FUNCTIONS:
void InitDisplayPort()
{
  PORTD.DIRSET= DISP_RS_MASK + DISP_RW_MASK + DISP_EN_MASK;
  PORTA.DIRSET= DISP_DATA_MASK;
}

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

void WriteCharToLcd( int8_t position, char charToWrite)
{
  DispPos= position;
  WriteChar( charToWrite);
}

#endif /* DISPLAY_H_ */