* ---------------------------------------------------------
* A SIMPLE APPLICATION THAT LETS YOU MOVE THE CURSOR AROUND
* ON THE SCREEN, CLICKING THE MIDDLE POSITION PUTS AN X INTO
* THAT SPOT.

        .CR     65C02
        .TF     scratchpad.hex,INT
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000           VIA PORT B DATA
ORA     .EQ     $6001           VIA PORT A DATA 
IRA     .EQ     $6001           VIA PORT A DATA (SAME AS OUTPUT)
DDRB    .EQ     $6002           VIA DDRB
DDRA    .EQ     $6003           VIA DDRA

SWITCH  .EQ     $FF             HOLDS SWITCH CODE PUSHED
LCD_X   .EQ     $FE             LCD X position
LCD_Y   .EQ     $FD             LCD Y position

* ---------------------------------------------------------
* SUB-ROUTINES

INITVIA LDA     #%11111111      PORT B. DATA SIGNALS USED WITH
        STA     DDRB            LCD DISPLAY ONLY, ALSO LEDs.
        LDA     #%11100000      PORT A. PA0-PA4 INPUT FOR SWITCHES,
        STA     DDRA            PA5-PA7 USED FOR LCD CONTROL SIGNALS.
        RTS

INITLCD LDA     #%00110000      FUNCTION SET (INITIALIZE)
        JSR     LCD_CMD
        JSR     WAITLCD
        
        LDA     #%00110000      
        JSR     LCD_CMD
        JSR     WAITLCD
     
        LDA     #%00110000      
        JSR     LCD_CMD
        JSR     WAITLCD
        
        LDA     #%00111000      FUNCTION SET (8-BIT TRANSFER)
        JSR     LCD_CMD
        JSR     WAITLCD
     
        LDA     #%00001101      DISPLAY ON, CURSOR EN.
        JSR     LCD_CMD
        JSR     WAITLCD
     
        LDA     #%00000001      CLEAR DISPLAY
        JSR     LCD_CMD
        JSR     WAITLCD
        
        LDA     #%00000110      SET ENTRY MODE
        JSR     LCD_CMD
        RTS

LCD_CMD STA 	ORB             DATA IS IN ACCUMULATOR
        LDA     #%10000000      E=H R/W=L RS=L  
        STA     ORA
        LDA     #%00000000      E=L PULSE CLOCK LOW
        STA     ORA
        LDA     #%10000000      E=H
        STA     ORA
        RTS

WAITLCD NOP                     PLACEHOLDER FOR BETTER THINGS
        RTS
        
LCD_CHR STA 	ORB             DATA IS IN ACCUMULATOR
        LDA     #%10100000      E=H R/W=L RS=H  
        STA     ORA
        LDA     #%00100000      E=L PULSE CLOCK LOW
        STA     ORA
        LDA     #%10100100      E=H
        STA     ORA
        RTS

SET_POS LDY     #0
        LDA     #$80            ROW 0 DDRAM ADDRESS
        CPY     LCD_Y
        BEQ     ADD_X
        LDA     #$C0            ROW 1 DDRAM ADDRESS
ADD_X   CLC                     CLEAR CARRY
        ADC     LCD_X           ADD X POSITION
        JSR     LCD_CMD         SEND COMMAND
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
        JSR     INITLCD         INITIALIZE LCD
        LDA     #0
        STA     LCD_X           CLEAR LCD X POSITION
        STA     LCD_Y           CLEAR LCD Y POSITION

FUNCN   JSR     GETKEY
MIDDLE  LDA     SWITCH
        CMP     #%00011110      BIT 0 LOW?
        BNE     UP               ... TRY UP INSTEAD.
        LDA     #%01011000      SET TO 'X'
        JSR     LCD_CHR         OUTPUT TO LCD
        JSR     SET_POS         RESET POSITION TO WHERE WE WERE
        JMP     FUNCN

UP      LDA     SWITCH
        CMP     #%00011101      BIT 1 LOW?
        BNE     DOWN             ... TRY DOWN INSTEAD.
        LDA     #$00            MOVE TO UPPER LINE
        STA     LCD_Y
        JSR     SET_POS         UPDATE DISPLAY
        JMP     FUNCN

DOWN    LDA     SWITCH
        CMP     #%00011011      BIT 2 LOW?    
        BNE     LEFT             ... TRY LEFT INSTEAD.
        LDA     #$01            MOVE TO LOWER LINE
        STA     LCD_Y
        JSR     SET_POS         UPDATE DISPLAY
        JMP     FUNCN

LEFT    LDA     SWITCH
        CMP     #%00010111      BIT 3 LOW?
        BNE     RIGHT            ... TRY RIGHT INSTEAD.
        LDA     LCD_X
        CMP     #0
        BEQ     FUNCN           DONE IF ALL THE WAY TO THE LEFT
        DEC     LCD_X           DECREMENT X POSITION
        JSR     SET_POS         UPDATE DISPLAY
        JMP     FUNCN

RIGHT   LDA     SWITCH
        CMP     #%00001111      BIT 4 LOW? 
        BNE     NOKEY            GOTO NOKEY IF NOT
        LDA     LCD_X
        CMP     #15             LAST POSITION
        BEQ     FUNCN           DONE IF ALL THE WAY TO THE RIGHT
        INC     LCD_X           INCREMENT X POSITION
        JSR     SET_POS         UPDATE DISPLAY
        JMP     FUNCN

NOKEY   NOP                     WAIT A BIT
        JMP     FUNCN            THEN RETURN TO SCANNING KEYPAD

* ---------------------------------------------------------
* STORE VECTORS AT END OF EEPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START           NMI VECTOR
        .DA     START           RESET VECTOR
        .DA     START           IRQ VECTOR