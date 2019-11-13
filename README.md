# BE6502 - Build a 6502 computer
[Ben Eater](https://eater.net/) has been a huge inspiration for many getting into building computers using basic electronic building blocks, his [videos](https://www.youtube.com/user/eaterbc) are truly excellent and it is no wonder he's often referred to as the Bob Ross of Breadboards! His latest project is [building a 6502 computer](https://eater.net/6502) on a breadboard, a video series that at this moment in time still has not concluded!

Anyway, I suck at getting things working on breadboards - so usually I just put it on a PCB instead, and hope it'll work as designed. I've designed some stuff with the standard 6502-processor before, but this'll be the first one to be tackled with the more modern Western Designs CPU (W65C02) at its heart. The video series depends on this specific brand and type of CPU, this is due to the way it is run using a slow and sometimes completely halted clock (other 6502 variants would loose their context and result in completely unexpected behavior). 

![PCB Preview](https://github.com/tebl/BE6502/raw/master/gallery/BE6502%20SBC.png)

## Project status
The BE6502 computer is still in the prototyping stage and though I have studied the videos up to this point, there may be stuff missing - some changes'll probably also show up when the last of Ben Eaters video in this series is released. See [TODO](https://github.com/tebl/BE6502/blob/master/TODO.md) for an overview of things I've yet to implement or want to change in the future!

## System modules
The BE6502 computer has a 39-pin backplane connector, like most of my other projects and these can be used to connect to an Arduino Mega as in the videos, a logic probe or even a backplane if you intend to build any additional expansions for use with the computer. The following listed modules are available for use with the computer, where available I've also added an order URL to a shared project on PCBway for ease of ordering - click the module name to get more information as well as a complete list of components needed to build it (usually referred to as a BOM).

| Name      |                 | Order  |
| --------- | --------------- | ------ |
| [BE6502 SBC](https://github.com/tebl/BE6502/tree/master/BE6502%20SBC) | required |
| [RC6502 Backplane](https://github.com/tebl/RC6502-Apple-1-Replica/tree/master/RC6502%20Backplane)  | optional | [PCBWay](https://www.pcbway.com/project/shareproject/RC6502_Apple_1_Replica__Backplane_module_revision_A_.html?inviteid=88707) |
| [RC6502 Prototyping module](https://github.com/tebl/RC6502-Apple-1-Replica/tree/master/RC6502%20Prototyping) | optional | [PCBWay](https://www.pcbway.com/project/shareproject/RC6502_Apple_1_Replica__Module_prototyping_board_.html?inviteid=88707) |

If you would like to have a copy of one or more modules (usually 5$ per module), consider using the order links in order to support the development of these boards while doing so. You can alternatively zip up the contents of the export directory for each module and give that to your favourite PCB fabrication plant (I use [PCBWay](https://www.pcbway.com/setinvite.aspx?inviteid=88707) instead, but not that these files may have changes not yet tested on my part.