* ---------------------------------------------------------
* READ SWITCHES, DEBOUNCE AND DO SOMETHING WITH THEM.

        .CR     65C02
        .TF     debounce.hex,INT
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000           VIA PORT B DATA
ORA     .EQ     $6001           VIA PORT A DATA 
IRA     .EQ     $6001           VIA PORT A DATA (SAME AS OUTPUT)
DDRB    .EQ     $6002           VIA DDRB
DDRA    .EQ     $6003           VIA DDRA

SWITCH  .EQ     $FF             HOLDS SWITCH CODE PUSHED

* ---------------------------------------------------------
* SUB-ROUTINES

INITVIA LDA     #%11111111      PORT B. DATA SIGNALS USED WITH
        STA     DDRB            LCD DISPLAY ONLY, ALSO LEDs.
        LDA     #%11100000      PORT A. PA0-PA4 INPUT FOR SWITCHES,
        STA     DDRA            PA5-PA7 USED FOR LCD CONTROL SIGNALS.
        RTS

GETKEY  LDA     IRA             READ SWITCHES
        AND     #%00011111      MASK OUT OTHER BITS
        CMP     #%00011111      NO KEYS PUSHED?
        BEQ     GETKEY          CHECK AGAIN...
        STA     SWITCH          STORE VALUE READ
AGAIN   LDA     IRA             WAIT FOR KEY UP
        AND     #%00011111      MASK OUT OTHER BITS
        CMP     #%00011111      NO KEYS PUSHED?
        BNE     AGAIN           CHECK AGAIN...
        RTS

* ---------------------------------------------------------
* MAIN PROGRAM STARTS HERE

START   JSR     INITVIA         INITIALIZE VIA

FUNCN   JSR     GETKEY
MIDDLE  LDA     SWITCH
        CMP     #%00011110
        BNE     UP
        STA     ORB
        JMP     FUNCN

UP      LDA     SWITCH
        CMP     #%00011101
        BNE     DOWN
        STA     ORB
        JMP     FUNCN

DOWN    LDA     SWITCH
        CMP     #%00011011
        BNE     LEFT
        STA     ORB
        JMP     FUNCN

LEFT    LDA     SWITCH
        CMP     #%00010111
        BNE     RIGHT
        STA     ORB
        JMP     FUNCN

RIGHT   LDA     SWITCH
        CMP     #%00001111
        BNE     UNKNOWN
        STA     ORB
        JMP     FUNCN

UNKNOWN LDA     SWITCH
        ORA     #%11100000      LIGHT TOP 3 LEDS TO SHOW
        STA     ORB              UNKNOWN VALUE DETECTED.
        JMP     FUNCN

* ---------------------------------------------------------
* STORE VECTORS AT END OF EEPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START           NMI VECTOR
        .DA     START           RESET VECTOR
        .DA     START           IRQ VECTOR