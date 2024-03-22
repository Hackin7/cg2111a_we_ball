
/*
 * Setup and start codes for external interrupts and 
 * pullup resistors.
 * 
 */
// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs. 
  DDRD &= 0b11110011;
  PORTD |= 0b00001100;
}

ISR(INT2_vect)
{
  rightISR();
}

ISR(INT3_vect)
{
  leftISR();
}

// Functions to be called by INT2 and INT3 ISRs.
void leftISR()
{
  if (dir == FORWARD){
    leftForwardTicks++;
    forwardDist = (unsigned long) ((float) leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
    dbprintf("Forwards: %d", forwardDist, "\n");
  } else if (dir == BACKWARD) {
    leftReverseTicks++;
    reverseDist = (unsigned long) ((float) leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
    dbprintf("Reverse: %d", reverseDist, "\n");
  } else if (dir == LEFT) {
    leftReverseTicks++;
    leftReverseTicksTurns = (unsigned long) ((float) leftReverseTicks / COUNTS_PER_REV);
    dbprintf("leftReverseTicks: %d, leftReverseTicksTurns: %d", leftReverseTicks, leftReverseTicksTurns, "\n");
  } else if (dir == RIGHT) {
    leftForwardTicks++;
    leftForwardTicksTurns = (unsigned long) ((float) leftForwardTicks / COUNTS_PER_REV);
    dbprintf("leftForwardTicks: %d, leftForwardTicksTurns:  %d", leftForwardTicks, leftForwardTicksTurns, "\n");
  }
}

void rightISR()
{
  if (dir == FORWARD){
    rightForwardTicks++;
  } else if (dir == BACKWARD) {
    rightReverseTicks++;
  } else if (dir == LEFT) {
    rightForwardTicks++;
    rightForwardTicksTurns = (unsigned long) ((float) rightForwardTicks / COUNTS_PER_REV);
  } else if (dir == RIGHT) {
    rightReverseTicks++;
    rightReverseTicksTurns = (unsigned long) ((float) rightReverseTicks / COUNTS_PER_REV);
    dbprintf("rightReverseTicks: %d, rightReverseTicksTurns: %d", rightReverseTicks, rightReverseTicksTurns, "\n");
  }
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

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.


// Implement INT2 and INT3 ISRs above.

/*
 * Setup and start codes for serial communications
 * 
 */
// Set up the serial connection. For now we are using 
// Arduino Wiring, you will replace this later
// with bare-metal code.
void setupSerial()
{
  // To replace later with bare-metal.
  Serial.begin(9600);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using the other UARTs
}

// Start the serial connection. For now we are using
// Arduino wiring and this function is empty. We will
// replace this later with bare-metal code.

void startSerial()
{
  // Empty for now. To be replaced with bare-metal code
  // later on.
  
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.

int readSerial(char *buffer)
{

  int count=0;

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs

  while(Serial.available())
    buffer[count++] = Serial.read();

  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  Serial.write(buffer, len);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}

/*
 * Alex's setup and run codes
 * 
 */

// Clears all our counters
void clearCounters()
{
  leftForwardTicks=0;
  rightForwardTicks=0;
  leftReverseTicks=0;
  rightReverseTicks=0;

  leftForwardTicksTurns=0;
  rightForwardTicksTurns=0;
  leftReverseTicksTurns=0;
  rightReverseTicksTurns=0;

  leftRevs=0;
  rightRevs=0;
  forwardDist=0;
  reverseDist=0; 
}

// Clears one particular counter
void clearOneCounter(int which)
{
  clearCounters();
}
// Intialize Alex's internal states

void initializeState()
{
  clearCounters();
}

void handleCommand(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        forward((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_REVERSE:
        backward((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_TURN_LEFT:
        left((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_TURN_RIGHT:
	    right((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_STOP:
        stop();
        sendOK();
      break;
    case COMMAND_GET_STATS:
        sendStatus();
        sendOK();
      break;
    case COMMAND_CLEAR_STATS:
        clearOneCounter(command->params[0]);
        sendOK();
      break;
    
    /*
     * Implement code for other commands here.
     * 
     */
        
    default:
      sendBadCommand();
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
     

        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}

void setup() {
  // put your setup code here, to run once:
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH *
    ALEX_BREADTH));
  alexCirc = PI * alexDiagonal;
  
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();
  sei();
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void loop() {
// Uncomment the code below for Step 2 of Activity 3 in Week 8 Studio 2

 //forward(0, 100);

// Uncomment the code below for Week 9 Studio 2


 // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK){
      handlePacket(&recvPacket);
  }else{
    if(result == PACKET_BAD)
    {
      sendBadPacket();
    }
    else if(result == PACKET_CHECKSUM_BAD)
    {
      sendBadChecksum();
    } 
  }

  if(deltaDist > 0)
  {
    if(dir==FORWARD)
    {
      if(forwardDist > newDist)
      {
        deltaDist=0;
        newDist=0;
        stop();
      }
    } else if (dir == BACKWARD)
    {
      if(reverseDist > newDist)
      {
      deltaDist=0;
      newDist=0;
      stop();
      }
    } else if(dir == STOP)
    {
      deltaDist=0;
      newDist=0;
      stop();
    }
  }
    
  if (deltaTicks > 0) {
    if (dir == LEFT) {
      if (leftReverseTicksTurns >= targetTicks){
        dbprintf("reset1: %d %d", leftReverseTicksTurns, targetTicks, "\n");
        deltaTicks = 0;
        targetTicks = 0;
        stop();
      }
    } else if (dir == RIGHT) {
      if (rightReverseTicksTurns >= targetTicks){
        dbprintf("reset2: %d %d", rightReverseTicksTurns, targetTicks, "\n");
        deltaTicks = 0;
        targetTicks = 0;
        stop();
      } 
    } else if (dir == STOP) {
        dbprintf("reset3: %d", targetTicks, "\n");
      deltaTicks = 0;
      targetTicks = 0;
      stop();
    } 
  }    
      
}

