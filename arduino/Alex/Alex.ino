#include "serialize.h"
#include "packet.h"
#include "constants.h"
#include <stdarg.h>
#include <math.h>

/* --------------------------------------------------------------------------------*/
/*
 * Alex's configuration constants
 */
 
#define ALEX_LENGTH 25.7
#define ALEX_BREADTH 15.6
#define PI 3.141592654

// Number of ticks per revolution from the 
// wheel encoder.
#define COUNTS_PER_REV      4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled 
// by taking revs * WHEEL_CIRC
#define WHEEL_CIRC          20.735

double tickDistance=5.34;

// to be declared on setup
float alexDiagonal = 0;
float alexCirc = 0;

/* --------------------------------------------------------------------------------*/
/*
 *    Alex's State Variables
 */

volatile TDirection dir;

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


// Variables to keep track of colour detected by colour sensor
volatile unsigned long redFrequency;
volatile unsigned long redColor;
volatile unsigned long blueFrequency;
volatile unsigned long blueColor;
volatile unsigned long greenFrequency;
volatile unsigned long greenColor;

char _recvBuffer, _xmitBuffer;


volatile long ultDuration = -1;
volatile long ultDist = -1;
volatile int colourEnum = 0;

/* --------------------------------------------------------------------------------*/
// This function needs to be here if not it'll throw an error
TResult readPacket(TPacket *packet) 
{
    // Reads in data from the serial port and
    // deserializes it.Returns deserialized
    // data in "packet".
    
    char buffer[PACKET_SIZE];
    int len;

    len = readSerial(buffer);

    if(len == 0)
      return PACKET_INCOMPLETE;
    else
      return deserialize(buffer, len, packet);
}

/* --- I2C LCD ---------------------------------------------------------------- */
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void lcdInit() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

/*
void test()
{
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      lcd.write(Serial.read());
    }
  }
}
*/