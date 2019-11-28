[Ben Eater](https://eater.net/) has been a huge inspiration for many getting into building computers using basic electronic building blocks, his [videos](https://www.youtube.com/user/eaterbc) are truly excellent and it is no wonder he's often referred to as the Bob Ross of Breadboards! His latest project is [building a 6502 computer](https://eater.net/6502) on a breadboard, a video series that I'm sure will also inspire a lot of new users to the art of building stuff with the 6502! Check out the main [README](https://github.com/tebl/BE6502) for an overview of all the modules available to the BE6502 computer.

## Arduino Mega 2560-shield and Backplane
During the initial videos, Ben Eater shows how an Arduino Mega 2560 can be used when debugging or even seeking to understand how a 6502 computer works. This is done by connecting up the address/data busses as well as some of the control signals (R/W and the clock), the values are then printed every clock pulse.

![Arduino console](https://github.com/tebl/BE6502/raw/master/BE6502%20Mega%20Adapter/gallery/2019-11-27%2021.32.10.jpg)

That's a lot of wires and I don't have that kind of patience, so I designed it as an Arduino Mega 2560-shield that doubles up as a two (or three) slot backplane. You plug the SBC-module into one of them, then you can use the other for whatever expansion module you want to try out with the system. The third and outer set of pins can be installed as slot in the same manner, but I opted to install straight header pins here instead - that way I can use some female to male dupont wires and easily run them as needed over to a breadboard.

![Backplane](https://github.com/tebl/BE6502/raw/master/BE6502%20Mega%20Adapter/gallery/2019-11-27%2020.24.50.jpg)
![Arduino attached](https://github.com/tebl/BE6502/raw/master/BE6502%20Mega%20Adapter/gallery/2019-11-27%2020.24.57.jpg)

I've adapted the Arduino sketch created by Ben Eater, mostly I've just changed a few minor things to have them just like I would like them to be - I've also added all of the other BE6502-backplane signals available since they have all been mapped over to the Arduino Mega 2560. What's particularly awesome about that is that you can program the Arduino Mega to manually control the signalling without the SBC-module present, meaning that you now have the added ability to test out expansion module logic step by step.

For more information about using the sketch to analyze the computer while it is running as well as a few annotations that should help make the data streaming into the console more understandable, see [6502-monitor](https://github.com/tebl/BE6502/tree/master/software/arduino/6502-monitor) within the software-section of the project. Arduino console with the reset vector highlighted in yellow can be seen below as an example of output.

![Arduino console](https://github.com/tebl/BE6502/raw/master/software/arduino/6502-monitor/arduino_console.png)

## Jumpers
| Reference | Position | Description                                        |
| --------- | -------- | -------------------------------------------------- |
| USB_PWR   | o-o *    | Link to power from/to Arduino Mega 2560            |

*) This is the default position and will allow the Arduino Mega 2560 to be powered at all times, so do not be alarmed if the module wakes up when power is applied to the BE6502 SBC barrel connector - remove jumper to keep it off if you're not running any sketches that work without a computer connected to the module.

# Schematic
The supplied KiCad files should be sufficient as both a schematic and as a starting point for ordering PCBs (basically you could just zip the contents of the export folder and upload that on a fabrication site), the schematic is also available in [PDF-format](https://github.com/tebl/BE6502/raw/master/BE6502%20Mega%20Adapter/export/BE6502%20Mega%20Adapter.pdf) so that you can refer to it while troubleshooting. You could even print it out and hang it on your office wall, excellent conversation starter at any technology-oriented business worth working at (or so I keep telling myself)!

# BOM
This is the part list as it stands now, most should be easy to get a hold of from your local electronic component shop though you might have to consider other sources depending on the quality of yourlocal vendor.

Some vendors will have the same ICs in different form factors, the ones you want will often be specified as being in the form of a DIP/PDIP package. Usually you'll want sockets for each of the ICs as well, a bag of assorted sockets should be easily available without setting you back more than a couple bucks. With the sockets in place, you don't need to dread having to remove an IC later, this leads to easier fault finding and you can easily "borrow" them for other projects later!

| Reference    | Item                                  | Count |
| ------------ | ------------------------------------- | ----- |
| PCB          | Fabricate yourself using Gerber files ([order](https://www.pcbway.com/project/shareproject/BE6502_Build_a_6502_computer__Arduino__and_backplane_module_.html?inviteid=88707)) |     1 |
| C1.C2,C3     | 100nF ceramic capacitor               |     3 |
| J1           | 39p straight pin header *             |     1 |
| J2-J3        | 39p female pin header *               |     2 |
| JP1          | 2p straight pin header                |     1 |
|              | Jumpers for settings (CAP headers)    |     1 |
| XA1          | Arduino pin headers **                |       |

*) You won't find the exact pin count for these, so the easiest is to use the 40 pin versions instead - either leave the last pin hangig across where pin 40 should have been or snip it off!

**) I'm not going to list these separatately, but you'll need some single and double row pin headers that you can cut down to size and solder to the board. Usually I insert them into the Mega itself, position the board on top and then tack each one in place temporarily to get the alignment correct. Remove the Arduino Mega 2560 board, then finally solder the pin headers into place.