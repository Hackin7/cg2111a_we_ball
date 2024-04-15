#include <AFMotor.h>

// Motor control
#define FRONT_LEFT   4 // M4 on the driver shield
#define FRONT_RIGHT  1 // M1 on the driver shield
#define BACK_LEFT    3 // M3 on the driver shield
#define BACK_RIGHT   2 // M2 on the driver shield

AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

unsigned long computeDeltaTicks(float ang)
{
  unsigned long ticks = (unsigned long) ((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
}

void move(float speed, int direction)
{
  int speed_scaled = (speed/100.0) * 255;
  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);

  switch(direction)
    {
      case BACK:
        motorFL.run(BACKWARD);
        motorFR.run(BACKWARD);
        motorBL.run(FORWARD);
        motorBR.run(FORWARD); 
      break;
      case GO:
        motorFL.run(FORWARD);
        motorFR.run(FORWARD);
        motorBL.run(BACKWARD);
        motorBR.run(BACKWARD); 
      break;
      case CW:
        motorFL.run(BACKWARD);
        motorFR.run(FORWARD);
        motorBL.run(FORWARD);
        motorBR.run(BACKWARD); 
      break;
      case CCW:
        motorFL.run(FORWARD);
        motorFR.run(BACKWARD);
        motorBL.run(BACKWARD);
        motorBR.run(FORWARD); 
      break;
      case STOP:
      default:
        motorFL.run(STOP);
        motorFR.run(STOP);
        motorBL.run(STOP);
        motorBR.run(STOP); 
    }
}

/* --- Timer Logic -------------------------------------------------------- */



void movementTimerSetup(){
    // Set CTC (Clear Timer on Compare Match) mode
    TCCR5A = 0;
    TCCR5B = 0;
    TCCR5A &= ~(1 << WGM51 | 1 << WGM50);
    TCCR5B |= (1 << WGM52);
    // Set prescaler to 1024
    //TCCR5B |= (1 << CS52) | (1 << CS50);
    
    // Set prescaler to 1 - Max accuracy
    TCCR5B |= (1 << CS50);
    
    // Load the compare value for 1 ms interval
    //OCR5A = 15; 
    //OCR5A = 15624;
    //// Enable Timer5 compare match interrupt
    //TIMSK5 |= (1 << OCIE5A);
    // Enable global interrupts - done later
    //sei();
}

void movementTimerEnable(){
    TCNT5 = 0;
    OCR5A = 14974; //time_val; //15624 - 1 second;
    TIMSK5 |= (1 << OCIE5A);
}
void movementTimerDisable(){
    TIMSK5 &= ~(1 << OCIE5A);
}


volatile int32_t timeDiff = -1; 
volatile int32_t milliseconds = 0;
ISR(TIMER5_COMPA_vect) {
  milliseconds++;
}

void movementTimerForceStop(){
    timeDiff = -1;
    movementTimerDisable();
}
void movementTimerCheck(){
  if (timeDiff != -1 && milliseconds > timeDiff) {
    stop();
    //movementTimerForceStop();
  }
}

void movementTimerTrigger(int time_val){
  timeDiff = time_val;
  milliseconds = 0;
  movementTimerEnable();
}

void timersFailsave(TDirection setDir, int moveDir, int time, float speed){
    dir = (TDirection) setDir;
    move(speed, moveDir);
    movementTimerTrigger(time);
    //delay(time);
    //stop();
}
 

/* --- Movement Functions -------------------------------------------------------- */
void forward(float dist, float speed)
{
  // failsafe - Timers
  if (dist > 1000) {
    timersFailsave(FORWARD, FORWARD, dist-1000, speed);
      deltaDist = dist;
    return;
  }
  movementTimerForceStop();
  
  if(dist > 0) {
      deltaDist = dist;
  } else {
      deltaDist=9999999;
  }
  newDist=forwardDist + deltaDist;
  
  dir = (TDirection) FORWARD;
  move(speed, FORWARD);
}

void backward(float dist, float speed)
{
  // failsafe - Timers
  if (dist > 1000) {
    timersFailsave(BACKWARD, BACKWARD, dist-1000, speed);
    return;
  }
  movementTimerForceStop();
  
  if(dist > 0) {
      deltaDist = dist;
  } else {
      deltaDist=9999999;
  }
  newDist=reverseDist + deltaDist;
  
  dir = (TDirection) BACKWARD;
  move(speed, BACKWARD);
}



void left(float ang, float speed) {
  // failsafe - Timers
  if (ang > 1000) {
    timersFailsave(LEFT, CCW, ang-1000, speed);
    return;
  }
  movementTimerForceStop();
  
  if(ang == 0) {
      deltaTicks=99999999;
  } else {
      deltaTicks=computeDeltaTicks(ang);
  }
  targetTicks = leftReverseTicksTurns + deltaTicks;
  dbprintf("left targetTicks: %d %d", deltaTicks, targetTicks, "\n");
  
  ccw(ang, speed);
}

void right(float ang, float speed) {
  // failsafe - Timers
  if (ang > 1000) {
    timersFailsave(RIGHT, CW, ang-1000, speed);
    return;
  }
  movementTimerForceStop();
  
  if(ang == 0) {
      deltaTicks=99999999;
  } else {
      deltaTicks=computeDeltaTicks(ang);
  }
  //targetTicks = rightReverseTicksTurns + deltaTicks;
  targetTicks = leftForwardTicksTurns + deltaTicks;
  dbprintf("right targetTicks: %d %d", deltaTicks, targetTicks, "\n");
  
  cw(ang, speed);
}


void ccw(float dist, float speed)
{
  dir = (TDirection) LEFT;
  move(speed, CCW);
}

void cw(float dist, float speed)
{
  dir = (TDirection) RIGHT;
  move(speed, CW);
}

void stop()
{
  deltaDist = 0;
  movementTimerForceStop();
  move(0, STOP);
}
