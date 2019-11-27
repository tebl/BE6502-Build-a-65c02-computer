* ---------------------------------------------------------
* Count up in binary with a bunch of nops so that we can
* actually read the LEDs (instead of showing up as all on).

        .CR     65c02
        .TF     count_mhz.hex,INT
        
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000   VIA Port B data
ORA     .EQ     $6001   VIA Port B data
DDRB    .EQ     $6002   VIA DDRB
DDRA    .EQ     $6003   VIA DDRA


START   LDA     #$FF
        STA     DDRB
        LDA     #$00

LOOP    STA     ORB     SET LEDS
        LDY     #100    WAIT SOME TIME
WAIT    NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        DEY
        BPL     WAIT    STILL WAITING?
DONE    INC             NEXT DIGIT
        JMP     LOOP

* ---------------------------------------------------------
* STORE VECTORS AT END OF EPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START   NMI VECTOR
        .DA     START   RESET VECTOR
        .DA     START   IRQ VECTOR