#define S0 22
#define S1 32
#define S2 28
#define S3 26
#define sensorOut 30

#define TRIG 34
#define ECHO 36


void ultrasonicSensor(){
  // digitalWrite(TRIG, LOW);
  PORTC &= ~(0b00001000);
  // delayMicroseconds(2);
  _delay_ms(2);
  // digitalWrite(TRIG, HIGH);
  PORTC |= 0b00001000;
  // delayMicroseconds(10);
  _delay_ms(10);
  // digitalWrite(TRIG, LOW);
  PORTC &= ~(0b00001000);
  long duration = pulseIn(ECHO, HIGH);
  int distance = duration * 0.034/2;
  dbprintf("ultrasonic: %d %d\n", duration, distance);
}

void ultrasonicSetup() {
  // put your setup code here, to run once:
  // pinMode(TRIG, OUTPUT);
  DDRC |= 0b00001000;
  // pinMode(ECHO, INPUT);
  DDRC &= ~(0b00000010);
}

void setup() {
  // put your setup code here, to run once:
  ultrasonicSetup();

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
  
  lcdInit();
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();
  sei();
  stop();
}

long long int serialBufferClearTimer = 0;
void loop() {
  //colourSense();
  // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK){
    //lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("handle command"); 
    
    handleCommand(&recvPacket);
  }else{
    if(result == PACKET_BAD)
    {
    //lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("bad packet"); 
      sendBadPacket();
    }
    else if(result == PACKET_CHECKSUM_BAD)
    {
    //lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("bad checksum"); 
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