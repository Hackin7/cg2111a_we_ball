
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
