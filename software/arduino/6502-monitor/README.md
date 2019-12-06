# 6502 Bus Monitor

This is essentially identical to the Arduino Sketch provided by Ben Eater for his *Build a 6502 computer* video series, the only thing I've done is put it in a folder and try not to call it my own. I don't know if it'll be featured in any of his future videos, but for reference this was the file made available after the third video in the series. His original files along with the accompanying videos can be found at his web page at https://eater.net/6502.

## Arduino settings
I'm using a Funduino-branded Arduino Mega 2560 with the sketch, but for my own reference as there's starting to be a lot of boards and versions out there - these are the board settings I'm using within the Arduino software:

![Arduino settings](https://github.com/tebl/BE6502/raw/master/software/arduino/6502-monitor/arduino_settings.png)

## Using the sketch
The usage of the sketch is expertly explained in Ben Eaters video series, in particular [part 2](https://www.youtube.com/watch?v=yl8vPW5hydQ) of the series. As part of my own notes, though it may certainly be of interest to anyone else viewing this page is the image shown below - it attempts to correlate what the computer sees to what you'll see within the Arduino-console.

![Sketch results](https://github.com/tebl/BE6502/raw/master/software/arduino/6502-monitor/arduino_result.png)

Here the initial steps of the of the CPU initializing and then reading the initialization vector has been highlighted in yellow, then the three first instructions and the way it relates to the assembled version of my [count](https://github.com/tebl/BE6502/tree/master/software/examples/001%20-%20Blink)-assembly program can be seen (assembled using [SB-Assembler 3](https://www.sbprojects.net/sbasm/)). The remaining colours show how the various parts relate to each other.
