# BE6502 SBC
[Ben Eater](https://eater.net/) has been a huge inspiration for many getting into building computers using basic electronic building blocks, his [videos](https://www.youtube.com/user/eaterbc) are truly excellent and it is no wonder he's often referred to as the Bob Ross of Breadboards! His latest project is [building a 6502 computer](https://eater.net/6502) on a breadboard, a video series that at this moment in time still has not concluded!

Anyway, I suck at getting things working on breadboards - so usually I just put the components directly on a PCB instead, and hope it'll work as designed. The video series depends on this specific brand and type of CPU, this is due to the way it is run using a slow and sometimes completely halted clock (other 6502 variants would loose their context and result in completely unexpected behavior). This specific part of the project is the SBC-version of the computer, what this means is that it's essentially a complete computer on a single PCB.

![PCB Preview](https://github.com/tebl/BE6502/raw/master/gallery/BE6502%20SBC.png)


## Errata
I don't make mistakes, but when I do - I sometimes remember to add them to the *errata*. Keep checking this space, to see if I've found the first one yet!

# Schematic
The supplied KiCad files should be sufficient as both a schematic and as a starting point for ordering PCBs (basically you could just zip the contents of the export folder and upload that on a fabrication site), the schematic is also available in [PDF-format](https://github.com/tebl/BE6502/raw/master/BE6502%20SBC/export/BE6502%20SBC.pdf) so that you can refer to it while troubleshooting. You could even print it out and hang it on your office wall, excellent conversation starter at any technology-oriented business worth working at (or so I keep telling myself)!


# BOM
This is the part list as it stands now, most should be easy to get a hold of from your local electronic component shop though you might have to consider other sources depending on the quality of yourlocal vendor.

Some vendors will have the same ICs in different form factors, the ones you want will often be specified as being in the form of a DIP/PDIP package. Usually you'll want sockets for each of the ICs as well, a bag of assorted sockets should be easily available without setting you back more than a couple bucks. With the sockets in place, you don't need to dread having to remove an IC later, this leads to easier fault finding and you can easily "borrow" them for other projects later! For the pin headers; if you can't find the exact pin count, just buy the longer 40 pin versions and snip off the parts you don't need.

| Reference    | Item                                  | Count |
| ------------ | ------------------------------------- | ----- |
| PCB          | Fabricate yourself using Gerber files |     1 |
| C1-C6        | 100nF ceramic capacitor               |     6 |
| D1-D8        | 5mm LED diode, green or red color     |     8 |
| J1           | 39p right angle pin header            |     1 |
| J2           | 4p right angle pin header             |     1 |
| J3           | 2.1mm x 5.5mm barrel plug             |     1 |
| JP1          | Single row, 2p straight pin header    |     2 |
| R1-R3        | 3k3 (3300) ohm resistor               |     3 |
| R4-R9        | 1k ohm resistor                       |     6 |
| R10-R17      | 220 ohm resistor                      |     8 |
| U1           | W65C02 DIP-40                         |     1 |
| U2           | 62256 Static RAM DIP-28               |     1 |
| U3           | 28C64 or 28C256 DIP-28, EEPROM        |     1 |
| U4           | W65C22S DIP-40                        |     1 |
| U5           | 74HC00 DIP                            |     1 |
| U6           | RC1602A, 16x2 character LCD           |     1 |
| SW1-SW6      | Momentary push button                 |     6 |
| X1           | 1.000Mhz TTL Oscillator, (DIP 14)     |     1 |
|              | Jumpers for settings (CAP headers)    |     3 |

When constructing the board I recommend using 60/40 solder as the lead-free is more trouble than it's worth for hobby use, take your time with it and try to build up the board starting with the lowest components before moving on to the larger pieces. The LCD will be mounted on top of other components, so check that nothing shorts on the underside and unless you're 100% sure about the construction and my design - install some female header pins on it so that you can easily remove it while troubleshooting the circuit underneath!