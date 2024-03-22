

unsigned long computeDeltaTicks(float ang)
{
  unsigned long ticks = (unsigned long) ((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
}

void left(float ang, float speed) {
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
  if(ang == 0) {
      deltaTicks=99999999;
  } else {
      deltaTicks=computeDeltaTicks(ang);
  }
  targetTicks = rightReverseTicksTurns + deltaTicks;
  dbprintf("right targetTicks: %d %d", deltaTicks, targetTicks, "\n");
  
  cw(ang, speed);
}
