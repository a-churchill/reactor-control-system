# Reactor Control System

This code drives a biomass reactor, and enables its operation and monitoring by inexperienced personnel. The C++ was originally written by another student, and I refactored and added to it extensively; the monitoring script was written by me completely.

I have added the version of the control system code that I started working on at the beginning to the previous_code file.

## Equipment Used

The display is a 4-line by 20-character display made by Smraza Electronics; the keypad was a standard, 12-key keypad; the motor driver was a XP02-115AC-Q DC motor driver by Minarik Drives; the board used was an Arduino Mega 2560. To convert the Arduino's digital output to an analog signal for the motor drivers and flow controllers, we used a low-pass filter. The system is described by the schematic below, which I made as well.

![System Schematic](system_schematic.png?raw=true "System Schematic")


