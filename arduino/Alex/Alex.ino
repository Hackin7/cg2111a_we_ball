#include <serialize.h>

#include "packet.h"
#include "constants.h"
volatile TDirection dir;
#include <stdarg.h>
#define ALEX_LENGTH 25.7
#define ALEX_BREADTH 15.6
#define PI 3.141592654
#include <math.h>

#define COUNTS_PER_REV 
#define WHEEL_CIRC 

float alexDiagonal = 0;
float alexCirc = 0;

/*
 * Alex's configuration constants
 */

// Number of ticks per revolution from the 
// wheel encoder.

#define COUNTS_PER_REV      4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled 
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC          20.735

/*
 *    Alex's State Variables
 */

// Store the ticks from Alex's left and
// right encoders.
volatile unsigned long leftForwardTicks; 
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks; 
volatile unsigned long rightReverseTicks;

volatile unsigned long leftForwardTicksTurns; 
volatile unsigned long rightForwardTicksTurns;
volatile unsigned long leftReverseTicksTurns; 
volatile unsigned long rightReverseTicksTurns;

// Store the revolutions on Alex's left
// and right wheels
volatile unsigned long leftRevs;
volatile unsigned long rightRevs;

// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;
volatile unsigned long deltaDist;
volatile unsigned long newDist;

volatile unsigned long deltaTicks=10;
volatile unsigned long targetTicks=10;

// Variables to keep track of whether we've moved a commanded distance

volatile double leftDistance;
volatile double rightDistance;

double tickDistance=5.34;

