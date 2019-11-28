[Ben Eater](https://eater.net/) has been a huge inspiration for many getting into building computers using basic electronic building blocks, his [videos](https://www.youtube.com/user/eaterbc) are truly excellent and it is no wonder he's often referred to as the Bob Ross of Breadboards! His latest project is [building a 6502 computer](https://eater.net/6502) on a breadboard, a video series that I'm sure will also inspire a lot of new users to the art of building stuff with the 6502! Check out the main [README](https://github.com/tebl/BE6502) for an overview of all the modules available to the BE6502 computer.

## Clock Module
Ben Eater has used the clock module from his 8 bit computer build with this project as well, this module is my implementation with only a few tweaks to the design. The main change apart from the whole PCB-design is that it has a slightly reduced chip-count, this was done by replacing the single used OR-gate with three NAND-gates instead (a little NAND-logic exercise).

![Running light without overbyte](https://github.com/tebl/BE6502/raw/master/gallery/2019-11-27%2020.53.11.jpg)

Like the original breadboard version, you have a manual clock that allows you to clock in manual clock-pulses by pushing the button - the 555 in this case is used for debouncing (mechanical switches are not actually all that consistent). An adjustable slow clock is also available, here the 555 is used as a vibrator and will generate clock pulses with a frequency that is adjustable via the potentiometer. The final 555 allows for a clean transition between the two clocks so you can switch between the two without any erratic bahavior.

![Clock module](https://github.com/tebl/BE6502/raw/master/BE6502%20Clock/gallery/2019-11-27%2020.39.37.jpg)

# Schematic
The supplied KiCad files should be sufficient as both a schematic and as a starting point for ordering PCBs (basically you could just zip the contents of the export folder and upload that on a fabrication site), the schematic is also available in [PDF-format](https://github.com/tebl/BE6502/raw/master/BE6502%20Clock/export/BE6502%20Clock.pdf) so that you can refer to it while troubleshooting. You could even print it out and hang it on your office wall, excellent conversation starter at any technology-oriented business worth working at (or so I keep telling myself)!

# BOM
This is the part list as it stands now, most should be easy to get a hold of from your local electronic component shop though you might have to consider other sources depending on the quality of yourlocal vendor.

Some vendors will have the same ICs in different form factors, the ones you want will often be specified as being in the form of a DIP/PDIP package. Usually you'll want sockets for each of the ICs as well, a bag of assorted sockets should be easily available without setting you back more than a couple bucks. With the sockets in place, you don't need to dread having to remove an IC later, this leads to easier fault finding and you can easily "borrow" them for other projects later! For the pin headers; if you can't find the exact pin count, just buy the longer 40 pin versions and snip off the parts you don't need.

| Reference    | Item                                  | Count |
| ------------ | ------------------------------------- | ----- |
| PCB          | Fabricate yourself using Gerber files ([order](https://www.pcbway.com/project/shareproject/BE6502_Build_a_6502_computer__Clock_module_revision_A_.html?inviteid=88707)) |     1 |
| C1.C2,C5     | 10nF ceramic capacitor                |     3 |
| C3.C4,C7     | 100nF ceramic capacitor               |     3 |
| C6           | 1uF electrolytic capacitor (2.5mm)    |     1 |
| D1           | 5mm LED                               |     1 |
| R1-R3,R5-R6  | 1k ohm resistor                       |     5 |
| R4           | 1M ohm resistor                       |     1 |
| R7           | 220 ohm resistor                      |     1 |
| RV1          | Bourns 3296W 1M ohm potentiometer     |     1 |
| U1-U3        | LM555, NE555 or equivalent (DIP-8)    |     3 |
| U4           | 74HC00 (DIP-14) *                     |     1 |
| U5           | 74HC08 (DIP-14) *                     |     1 |
| SW1          | Momentary push button                 |     1 |
| SW2          | 6pin 8.5mm x 8.5mm latching switch    |     1 |
| J1           | 4p straight pin header **             |     1 |

\*) HC-series chips have been specified as that is what Ben Eater used in his Build a 6502 computer series, but can also be LS or HCT-series instead so use what you have available.

\**) You can use a female right-angle pin header and match it directly to the BE6502 SBC with some careful adjustments, but I recommend using some female to female dupont leads instead as the connector is a bit too loose when not mounting boards on a backing plate.