void setup() {
  // put your setup code here, to run once:
  #define S0 22
#define S1 32
#define S2 28
#define S3 26
#define sensorOut 30

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Stores the red. green and blue colors
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
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

void loop() {
 // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK){
      handleCommand(&recvPacket);
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
