/* --------------------------------------------------------------------------------*/
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

/* --------------------------------------------------------------------------------*/
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

/* --------------------------------------------------------------------------------*/
// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs. 
  DDRD &= 0b11110011;
  PORTD |= 0b00001100;
}