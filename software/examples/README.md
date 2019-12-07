# Introduction
This directory contains a set of examples that've been made in order to test out the various parts of the BE6502-computer, the numbering scheme is mostly to put them into order from the simplest to the slightly less simple in terms of code. Along the way I've been attempting to learn some simple 6502 assembly programming myself, so there is every possibility that far more sophisticated solutions to the same problems exist somewhere out there.

## Assembling things
The 6502-CPU processes machine code and not assembly code, but while the latter may not be the easiest of languages to understand I assure you it's supposed to be more readable to us humans and not the machine. Generally though it's just a simple step above machine code, which is why each of the statements translate line by line to a single machine code instruction at a time. In order to take the *human-readable assembly code*  and translate it into something the computer can use, we need something called an *assembler*.

While Ben Eater opted for one called vasm, I chose [SB-Assembler 3](https://www.sbprojects.net/sbasm/) instead - it's available for all major platforms and it seems to have been updated somewhere within the last decade. One thing that confused me at the start of all this, a couple of weeks ago (give or take), is that there isn't a single defined *assembly-language* as compared to something like Python, Java etc. Mostly the assemblers agree on the resulting machine code, but everything outside of that may have slight differences so code written for one assembler might not work directly with a different assembler. Probably why many start out by converting existing assembly code to their favourite assembler as their first project, it's a way of getting accustomed to different variants and ensuring that the nuances between them has been understood.

Each project comes with their own little batch-file to make things easier on myself, but there isn't really a need for them - if you do end up using them, then you need to ensure that SB-Assembler 3 has been included in the system PATH environment variable. A file can be manually assembled into machine code by running something like the following:
```
sbasm.py monitor.asm
```

## Burning things
If you do not wish to alter the code, then the Intel HEX file should be suitable for flashing onto a 28C256 EEPROM using your favourite brand of EEPROM burner (I use the MinoPRO TL866CS, it's the only one I've needed so far). An additional file with the extension *.list* is usually included, referring to it makes reading the code easier when debugging the software as you can see the bytes each instruction was assembled into.

## Show me the examples!
The following examples have been finished, mostly for my own pleasure as I had fun doing them. Any improvements are left as student-assignments, any unintentional bugs as well. The following examples are available, they have been numbered in sequence of rising complexity and hopefully they'll end up building towards something resembling a program - there may be more, either I didn't finish them or I forgot to add them to the list. In order to keep things understandable at a basic level I have not added any delays to most of the code pieces, which means that they'd run too fast at 1Mhz - therefore the slow clock module has been specified for them.

| Number | Name        | Description                                     | Slow clock |   1 Mhz   |
| ------ | ----------- | ----------------------------------------------- | ---------- | --------- |
| [001](https://github.com/tebl/BE6502/tree/master/software/examples/001%20-%20Blink)    | noblink     | Blink an alternating pattern on LEDs            |      X     |           |
|        | count       | Count up in binary from $00 to $FF              |      X     |           |
|        | count_mhz   | Same as above, but can run at 1Mhz              |            |      X    |
| [002](https://github.com/tebl/BE6502/tree/master/software/examples/002%20-%20Echo)    | echo        | Mirrors switches on the LEDs                    |      X     |      X    |
| [003](https://github.com/tebl/BE6502/tree/master/software/examples/003%20-%20Debounce)    | debounce    | Read switches and perform code for each of them |      X     |           |
| [004](https://github.com/tebl/BE6502/tree/master/software/examples/004%20-%20Hello%20LCD)    | hello_lcd   | Initialize LCD and output a string to it        |      X     |           |