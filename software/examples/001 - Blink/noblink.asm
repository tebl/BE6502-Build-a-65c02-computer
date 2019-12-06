* ---------------------------------------------------------
* PUT $55 PATTERN ON LEDS

        .CR     65C02
        .TF     noblink.hex,INT
        
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000   VIA Port B data
ORA     .EQ     $6001   VIA Port B data
DDRB    .EQ     $6002   VIA DDRB
DDRA    .EQ     $6003   VIA DDRA


START   LDA     #$FF
        STA     DDRB
        LDA     #$55
        STA     ORB
LOOP    NOP
        JMP     LOOP

* ---------------------------------------------------------
* STORE VECTORS AT END OF EPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START   NMI VECTOR
        .DA     START   RESET VECTOR
        .DA     START   IRQ VECTOR