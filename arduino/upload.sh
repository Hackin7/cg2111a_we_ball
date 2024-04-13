#!/bin/sh

#PATH_TO_PROJ=testing/ultrasonic_test/
#FILENAME=ultrasonic_test.ino

PATH_TO_PROJ=Alex/
FILENAME=Alex.ino

rm -rf /tmp/arduino_build
arduino --pref build.path=/tmp/arduino_build --verify $PATH_TO_PROJ$FILENAME
#Alex/Alex.ino 

# Force Reset
raspi-gpio set 23 op
raspi-gpio set 23 dh
raspi-gpio set 23 dl

/home/pi/arduino-1.8.19/hardware/tools/avr/bin/avrdude -C/home/pi/arduino-1.8.19/hardware/tools/avr/etc/avrdude.conf -v -patmega2560 -cwiring -P/dev/ttyS0 -b115200 -D -Uflash:w:/tmp/arduino_build/$FILENAME.hex:i
