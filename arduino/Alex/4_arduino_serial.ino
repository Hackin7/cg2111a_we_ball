/*
 * Setup and start codes for serial communications
 * 
 */
 
void setupSerial()
{
  //setupSerialLib();
  setupUART();
}


void startSerial()
{
  //startSerialLib();
  startUART();
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.


volatile int counts = 0 ;

int readSerial(char *buffer)
{

  
  //int count = readUART(buffer); 
  int count = readSerialLib(buffer);
  
  // LCD Code
  if (count > 0) {
    lcd.clear();
    lcd.print("Received");
    lcd.setCursor(0,1);
    if (count >= 0) {
      lcd.print(buffer[0], HEX);
    }
    if (count >= 1) {
      lcd.print(buffer[1], HEX);
    }
    if (count >= 2) {
      lcd.print(buffer[2], HEX);
    }
    if (count >= 3) {
      lcd.print(buffer[3], HEX);
    }
  }
  
  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  //writeSerialLib(buffer, len);
  writeUART(buffer, len);
}

/* --- Serial Library Version below --------------------------------------------------- */
void setupSerialLib()
{
  // To replace later with bare-metal.
  Serial.begin(9600);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using the other UARTs
}


void startSerialLib()
{
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.


int readSerialLib(char *buffer)
{

  int count=0;

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs

  while(Serial.available())
    buffer[count++] = Serial.read();
    
  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerialLib(const char *buffer, int len)
{
  Serial.write(buffer, len);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}

/* --- USART Interrupt Version below -------------------------------------------------- */

// https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf


volatile char globalBuffer[1024];




void setupUART()
{
  // UCSR0C = 0b00000110; // 0b 00(Async UART)_000_110
  
  // // Set Baud Rate to 9600
  UBRR0H = 0;
  UBRR0L = 103;
  UCSR0A = 0;
  // UBRR0H = 0;
  // UBRR0L = 0b01100111;
  // UCSR0A = 0;

  //UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1<< UCSZ01) | (1 << UCSZ00);
}
void startUART()
{
  UCSR0B = 0b10111000;
  //UCSR0B = 0b10011000;
  //UCSR0B = 0b00011000; // Doesnt work
}

// Actual Interrupts //
// RECIEVE COMPLETE //
ISR(USART0_RXC){
  globalBuffer[counts++] = UDR0;
  if (counts >= 1024){ counts = 0; };
}

// TRANSFER COMPLETE //
ISR(USART0_TXC){

}

// UDR EMPTY HANDLER //
ISR(USART0_UDRE){

}

void writeUART( const char *buffer, int len){
  for ( int i = 0 ; i < len ; i ++ ){
    while ( !(UCSR0A & (1<< UDRE0))){}
    UDR0 = buffer[i];
  }
}

/*
int readUART(char *buffer)
{
  if (counts > 0) {
    buffer[0] = globalBuffer[0];
    return 1;
  }else{
    return 0;
  }
  
  int count = 0;

  for (count = 0; count < counts; count += 1) { //read in the data sequentially
    buffer[count] = globalBuffer[count];
  }

  counts = 0;
  return count;
}
*/


int readUART(char *buffer){
  int count=0;
  
  for (int i=0; i<128; i++){
    while((UCSR0A & (1 << RXC0)) == 0){}
    buffer[count++] = UDR0;
  } 
  //UDR0;
  
  /*lcd.setCursor(0,1);
  lcd.print(111);
  lcd.print(i);
  lcd.print(111);*/
  return count;
}

/*

/* --- Sending Data ------------------------------------*/
/*
#define UDRIEMASK   0b00100000
ISR(USART_UDRE_vect)
{
  char data;
  TResult result = readBuffer(&_xmitBuffer, &data);
  
  if (result == BUFFER_OK) {
    UDR0 = data  
  } else if (result == BUFFER_EMPTY) {
    
  }
  // UDR0 = _xmitBuffer;
  UCSR0B &= ~UDRIEMASK;
}



void writeUART(const char *buffer, int len){
  TResult result = BUFFER_OK;
  int i;
  for (i=1; i<size && result == BUFFER_OK; i++){
    result = writeBuffer(&_xmitBuffer, line[i]);
  }
  
  // Start sending the first byte
  UDR0 = line[0]
  
  // Enable the UDRE interrupt. The enable bit is bit 5 of UCSR0B
  UCSR0B |= 0b0010000;
    
}
*/