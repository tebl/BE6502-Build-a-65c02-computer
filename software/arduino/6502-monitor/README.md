# 6502 Bus Monitor
My first version of this sketch was essentially identical to the one Ben Eater provided for his *Build a 6502 computer* video series, so for the core features he's still the one to credit for all of this. Over time I've added quite a lot of features to the sketch in order to further enhance it, among the features added is a command-based serial interface. ability to have Arduino Mega replace the slow clock as well as using the special *BUS Enable* feature of the w65c02 processor to temporarily halt it and allow us to control the peripherals directly in its absence.

The sketch attempts to increase readability by using ANSI colour commands, but if this is not supported by your terminal software then you might end up with a lot of extra wierd characters in the output; if you do, try giving the command *ansi off* to turn them temporarily off. If this fixes your problem you may consider turning them off permanently by editing the ".ino"-file though I'd recommend that you instead install a better terminal program and use that instead (I recommend using Tera Term).

When you get so far that you want to move data to the computer by pasting the contents of your HEX-files into the terminal window, then you'll need to look into adding some time delays to the data sent out to the board - this is needed because the Arduino doesn't include any form of flow control, but your computer running at several Ghz will easily and quickly overrun the measly 16Mhz processor available to the Arduino. Tera Term makes that easy to do, which is why I recommend it.

## Arduino settings
I'm using a Funduino-branded Arduino Mega 2560 with the sketch, but for my own reference as there's starting to be a lot of boards and versions out there - these are the board settings I'm using within the Arduino software:

![Arduino settings](https://github.com/tebl/BE6502/raw/master/software/arduino/6502-monitor/arduino_settings.png)

## Using the sketch
The usage of the sketch is expertly explained in Ben Eaters video series, in particular [part 2](https://www.youtube.com/watch?v=yl8vPW5hydQ) of the series. As part of my own notes, though it may certainly be of interest to anyone else viewing this page is the image shown below - it attempts to correlate what the computer sees to what you'll see within the Arduino-console.

![Sketch results](https://github.com/tebl/BE6502/raw/master/software/arduino/6502-monitor/arduino_result.png)

Here the initial steps of the of the CPU initializing and then reading the initialization vector has been highlighted in yellow, then the three first instructions and the way it relates to the assembled version of my [count](https://github.com/tebl/BE6502/tree/master/software/examples/001%20-%20Blink)-assembly program can be seen (assembled using [SB-Assembler 3](https://www.sbprojects.net/sbasm/)). The remaining colours show how the various parts relate to each other.
