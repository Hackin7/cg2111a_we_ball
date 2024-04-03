/*
 * Setup and start codes for external interrupts
 */

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.
ISR(INT2_vect)
{
  rightISR();
}

ISR(INT3_vect)
{
  leftISR();
}

ISR()
{
  colourSense();
}

// Set up the external interrupt pins INT2 and INT3
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  // Use bare-metal to configure pins 18 and 19 to be
  // falling edge triggered. Remember to enable
  // the INT2 and INT3 interrupts.
  // Hint: Check pages 110 and 111 in the ATmega2560 Datasheet.

  DDRD &= 0b11110011;
  EIMSK |= 0b00001100;
  EICRA = 0b10100000;
  PORTD |= 0b00001100;
}
