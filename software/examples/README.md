# Introduction
This directory contains a set of examples that've been made in order to test out the various parts of the BE6502-computer, the numbering scheme is mostly to put them into order from the simplest to the slightly less simple in terms of code. Along the way I've been attempting to learn some simple 6502 assembly programming myself, so there is every possibility that far more sophisticated solutions to the same problems exist somewhere out there.

The following examples of code developed for the system are available, they have been numbered according to complexity - hopefully they'll end up building towards something more substantial though they should be sufficient to show off the various features of the system. There may be more code examples hanging around in the various sub-directories, but either I didn't finish them - or I just forgot to add them to the list! Given that there is a slow clock available for the system, I've not taken the time or sought to increase the complexity of each one by adding the additional complexity needed to sufficiently slow down a 1Mhz system in order for the LEDs to function as expected.

| Number | Name        | Description                                     | Slow clock |   1 Mhz   |
| ------ | ----------- | ----------------------------------------------- | ---------- | --------- |
| [001](https://github.com/tebl/BE6502/tree/master/software/examples/001%20-%20Blink)    | noblink     | Blink an alternating pattern on LEDs            |      X     |           |
|        | count       | Count up in binary from $00 to $FF              |      X     |           |
|        | count_mhz   | Same as above, but can run at 1Mhz              |            |      X    |
| [002](https://github.com/tebl/BE6502/tree/master/software/examples/002%20-%20Echo)    | echo        | Show switch status on the LEDs directly                    |      X     |      X    |
| [003](https://github.com/tebl/BE6502/tree/master/software/examples/003%20-%20Debounce)    | debounce    | Read switches and perform code for each of them |      X     |           |
| [004](https://github.com/tebl/BE6502/tree/master/software/examples/004%20-%20Hello%20LCD)    | hello_lcd   | Initialize LCD and output a string to it        |      X     |           |
| [005](https://github.com/tebl/BE6502/tree/master/software/examples/005%20-%20Notepad)    | scratchpad   | Use keys to move around LCD and middle button to put an 'X' there|      X     |      X     |

For some of the examples I've included more than one version, usually they build upon the previous to make something slightly more impressive (or not, depending on familiarity with writing assembly code). Take a look at the README file that comes with the example, if I've included one, for more of the specifics. Files that belong together should have consistent naming according to what is described above, there are several files with the same name though with differing extensions - the following table is a short description of what you'll find inside:

| Extension | Description |
| --------- | ------------ |
| *.asm*    | Assembly code for the program, this is where you fix my bugs.
| *.hex*    | Intel HEX-file, this is what you burn onto an EEPROM using an EEPROM-burner
* *.list*   | This is the output from the assembler, very useful when debugging code as it shows the bytes the program was assembled into as well as the code (including comments). Read this when studying how a program works



## Assembling things
The 6502-CPU processes machine code and not assembly code, but while the latter may not be the easiest of languages to understand I assure you it's supposed to be more readable to us humans and not the machine. Generally though it's just a simple step above machine code, which is why each of the statements translate line by line to a single machine code instruction at a time. In order to take the *human-readable assembly code*  and translate it into something the computer can use, we'll need something called an *assembler*.

While Ben Eater opted for one called vasm in his videos, I chose [SB-Assembler 3](https://www.sbprojects.net/sbasm/) instead - it's bing actively updated and since I already had Python installed it was merely a matter of dropping some files into a folder somewhere.

One thing that confused the heck out of me at the start of all this, probably a couple of weeks ago (give or take) - is that there isn't a single defined *assembly-language* as compared to something like Python, Java etc. Mostly the assemblers seem to agree on the resulting machine code though some may include optimizations others do not, everything outside of that however - may have slight differences so code written for one assembler will often not work directly with a different assembler. Back in the day everyone had the same datasheets for the processor, but assemblers and such were usually expensive and proprietary stuff that you weren't even supposed to share the documentation for.

Sometime later, we of course got the Internet and somehow we started to agree on the basics of things thought the software and dialects remained. One common task when starting to learn a new assembler, therefore is to take some existing assembly code that you know work and start converting that to your favourite assemblers dialect. 

Each project comes with their own little batch-file to make things easier on myself, but there isn't really a need for them - if you do end up using them, then you need to ensure that SB-Assembler 3 has been included in the system PATH environment variable. A file can be manually assembled into machine code by running something like the following:
```
sbasm.py monitor.asm
```

## Burning things
If you do not wish to alter the code, then the Intel HEX file should be suitable for flashing onto a 28C256 EEPROM using your favourite brand of EEPROM burner (I use the MinoPRO TL866CS, it's the only one I've needed so far). Start by using *Select IC*, search for 28C256 and selecting the suitable type as according to the writing on the chips you ordered. Then use *Open file*, browse into the example you want to load onto the chip - look for the file with the *.hex*-extension and select that - just accept the standard option for the next. Finally, use the option *Program \(P\)* from the *Device \(D\)* menu.

Insert the chip into the EEPROM burner according to the illustration shown on screen, hit *Program* and watch it burn the cheap beyond recognition (just kidding). Watch for errors, if you get any - doublecheck that you selected the correct chip type, alternatively pop another one into the burner and see if that works instead (I buy them in 10-packs from China, there's usually one or two that's dead so it's always good to have extras).