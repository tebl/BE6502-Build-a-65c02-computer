# BE6502 - Build a 6502 computer
[Ben Eater](https://eater.net/) has been a huge inspiration for many getting into building computers using basic electronic building blocks, his [videos](https://www.youtube.com/user/eaterbc) are truly excellent and it is no wonder he's often referred to as the Bob Ross of Breadboards! His latest project is [building a 6502 computer](https://eater.net/6502) on a breadboard, a video series that at this moment in time still has not concluded so this may not match whatever he does after part 3!

![SBC and clock module](https://github.com/tebl/BE6502/raw/master/gallery/2019-11-24%2013.24.12.jpg)

Personally, I am somewhat awful at getting things to work on breadboards - even when buying the good stuff from reputable electronics shops, so I usually just go directly to PCB instead. I've designed a few other amateur projects using the trusty old 6502-processor before, but this'll be the first one to be tackled with the more modern Western Designs CPU (W65C02) instead. His video series depends this specific brand of CPU, this is due to the way it is run using a slow and sometimes completely halted clock that wouldn't work the same on other brands. The older 6502 variants will more than likely loose their context, with the end result being a CPU doing unexpected things instead of running as the machine code would normally dictate. 

![LCD mounter](https://github.com/tebl/BE6502/raw/master/gallery/2019-12-06%2019.39.01.jpg
)
![Computer with backplane](https://github.com/tebl/BE6502/raw/master/gallery/2019-11-27%2021.33.14.jpg)

The BE6502 computer has a 39-pin backplane connector, like most of my other projects and these can be used to connect to an Arduino Mega as in the videos, a logic probe or even a separate larger backplane if you intend to build any more additional expansions for use with the computer. The picture above shows the SBC along with clock and Arduino Mega Shield. See *System modules* below for a listing of available modules, which are required at the various steps in the video series and any optional extras I felt like listing that would be compatible with the supplied boards.

Over time I may even add some short videos to show off the computer in action, but given that I have a voice suited for silent movies they'll probably just be videos of the computer blinking away by itself. As they become available, you'll find those on this [Youtube-playlist](https://www.youtube.com/playlist?list=PLOJSxpeHB9KTk9ptKCbEZnNEy9QRU02SS).

## Project status
The BE6502 computer is still in the prototyping stage and though I have studied the videos up to this point, at the time of writing that'd be everything up to and including part 3. See [TODO](https://github.com/tebl/BE6502/blob/master/TODO.md) for an overview of things I've yet to implement or want to change in the future!

Some of these details are likely to change as his original design by itself does not seem to reset reliably when using the 1 Mhz clock. At the moment I've got it fixed by using one of my own RC6502-modules in one of the backplane slots, details for this module can be found at https://github.com/tebl/RC6502-Apple-1-Replica/tree/master/RC6502%20Reset%20Circuit. If you'd like to build it up on a breadboard you can easily do that, I've installed pin headers as the front slot of my backplane to make it easier to do things like that. Alternatively you could program the Arduino Mega to do it for you!

## System modules
The following listed modules are available for use with the computer, where available I've also added an order URL to a shared project on PCBway for ease of ordering - click the module name to get more information as well as a complete list of components needed to build it (usually referred to as a BOM). Some modules have been marked with other computer names, these are other modules that I've found works directly and can be of interest when playing around with the BE6502-computer.

| Name      | Video series, parts 1-3 | Future expansions | Order  |
| --------- | ----------------------- | ----------------- | ------ |
| [Computer (SBC)](https://github.com/tebl/BE6502/tree/master/BE6502%20SBC) | required | | [PCBway](https://www.pcbway.com/project/shareproject/BE6502_Build_a_6502_computer__SBC_module_revision_A_.html?inviteid=88707) |
| [Adjustable and manual clock](https://github.com/tebl/BE6502/tree/master/BE6502%20Clock) | required | | [PCBway](https://www.pcbway.com/project/shareproject/BE6502_Build_a_6502_computer__Clock_module_revision_A_.html?inviteid=88707) |
| [Arduino Mega Shield and Backplane ](https://github.com/tebl/BE6502/tree/master/BE6502%20Mega%20Adapter) | required | | [PCBway](https://www.pcbway.com/project/shareproject/BE6502_Build_a_6502_computer__Arduino__and_backplane_module_.html?inviteid=88707) |
| [RC6502 Backplane](https://github.com/tebl/RC6502-Apple-1-Replica/tree/master/RC6502%20Backplane) |  | optional | [PCBWay](https://www.pcbway.com/project/shareproject/RC6502_Apple_1_Replica__Backplane_module_revision_A_.html?inviteid=88707) |
| [RC6502 Prototyping module](https://github.com/tebl/RC6502-Apple-1-Replica/tree/master/RC6502%20Prototyping) | | optional | [PCBWay](https://www.pcbway.com/project/shareproject/RC6502_Apple_1_Replica__Module_prototyping_board_.html?inviteid=88707) |

If you would like to have a copy of one or more modules (usually 5$ per module), consider using the order links in order to support the development of these boards while doing so. You can alternatively zip up the contents of the export directory for each module and give that to your favourite PCB fabrication plant instead if you want to, but note that the files online here may be newer and have changes made to them that may not have been tested yet (the order link above will point to my last tested version).