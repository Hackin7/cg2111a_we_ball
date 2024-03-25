
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