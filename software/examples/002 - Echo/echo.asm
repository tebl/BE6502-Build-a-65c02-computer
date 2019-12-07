* ---------------------------------------------------------
* READ SWITCHES AND OUTPUT READ VALUES TO LEDS.

        .CR     65C02
        .TF     echo.hex,INT
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000           VIA PORT B DATA
ORA     .EQ     $6001           VIA PORT A DATA 
IRA     .EQ     $6001           VIA PORT A DATA (SAME AS OUTPUT)
DDRB    .EQ     $6002           VIA DDRB
DDRA    .EQ     $6003           VIA DDRA

* ---------------------------------------------------------
* SUB-ROUTINES
INITVIA LDA     #%11111111      PORT B. DATA SIGNALS USED WITH
        STA     DDRB            LCD DISPLAY ONLY, ALSO LEDs.
        LDA     #%11100000      PORT A. PA0-PA4 INPUT FOR SWITCHES,
        STA     DDRA            PA5-PA7 USED FOR LCD CONTROL SIGNALS.
        RTS


* ---------------------------------------------------------
* MAIN PROGRAM STARTS HERE
START   JSR     INITVIA         INITIALIZE VIA
ECHO    LDA     IRA             READ SWITCHES
        AND     #%00011111      MASK OUT OTHER BITS
        STA     ORB             OUTPUT TO LEDS
        JMP     ECHO            DO IT FOREVER

* ---------------------------------------------------------
* STORE VECTORS AT END OF EPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START           NMI VECTOR
        .DA     START           RESET VECTOR
        .DA     START           IRQ VECTOR