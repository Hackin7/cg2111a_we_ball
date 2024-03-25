# cg2111a_we_ball

our robot will move as smoothly as a ball

## Arduino - Firmware Code

Code can be found in `/arduino/Alex`

For the `.ino` files, the compiler first loads the principal file (the one with the same name as the folder)
and then loads the others in alphabetical order. Concept behind multiple files can be found here: 
https://forum.arduino.cc/t/correct-way-to-include-multiple-files/507705/3 

Order of imports would be as such (think of it as combining into 1 big `.ino` file in this order)

```
Alex.ino
0_motors.ino
...
6_robotlib.ino
```

To get it to compile make sure to import `serialize.zip` as an arduino library first.

## RPi - Server Code


## PC - Client Code


