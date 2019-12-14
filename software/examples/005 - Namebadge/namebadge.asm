* ---------------------------------------------------------
* ALLOWS YOU TO MOVE AROUND ON THE SCREEN AND CHANGE THE
* CONTENTS OF EACH POSITION, IN EFFECT MAKING THE SBC AN
* ELECTRONIC NAMEBADGE.

        .CR     65C02
        .TF     namebadge.hex,INT
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000           VIA PORT B DATA
IRB     .EQ     $6000           VIA PORT A DATA (SAME AS OUTPUT)
ORA     .EQ     $6001           VIA PORT A DATA 
IRA     .EQ     $6001           VIA PORT A DATA (SAME AS OUTPUT)
DDRB    .EQ     $6002           VIA DDRB
DDRA    .EQ     $6003           VIA DDRA

SPACE   .EQ     %00100000       BLANK SPACE
A_CHR   .EQ     %01000001       'A'
MODE_MV .EQ     $00             MOVE CURSOR MODE
MODE_LD .EQ     $01             DATA LOAD MODE

SWITCH  .EQ     $FF             HOLDS SWITCH CODE PUSHED
LCD_X   .EQ     $FE             LCD X position
LCD_Y   .EQ     $FD             LCD Y position
MODE    .EQ     $FC             BUTTON MODE
LAST    .EQ     $FB             LAST CHARACTER USED
MEMORY  .EQ     $0200           MEMORY SET ASIDE FOR CHARACTERS

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

WAITLCD LDA     #%00000000      WE'LL NEED TO READ FROM THE DISPLAY, SO
        STA     DDRB             PORT B WILL NOW BE INPUTS.
        LDA     #%11000000      E=H R/W=H RS=L
        STA     ORA
        LDA     #%01000000      E=L PULSE CLOCK LOW
        STA     ORA
        LDA     #%11000000      E=H
        STA     ORA
        LDA     IRB             READ LCD STATUS REGISTER
        AND     #%10000000      MASK OUT BUSY FLAG
        BNE     WAITLCD
        LDA     #%10000000      LEAVE LCD CONTROL SIGNALS IN WRITE
        STA     ORA
        LDA     #%11111111      RESET VIA PORT B DATA DIRECTION SO THAT
        STA     DDRB              WE'RE READY TO SEND THE NEXT COMMAND.
        RTS
    
INITMEM LDY     #$FF            NUM BYTES TO CLEAR (80 CHARACTER MEMORY)
        LDA     #$00            ZERO-FILL MEMORY
NXT_MEM STA     MEMORY,Y        CLEAR BUFFER    
        DEY                      ONE BYTE
        BNE     NXT_MEM          AT A TIME.
        RTS
    
LCD_CHR STA 	ORB             DATA IS IN ACCUMULATOR
        LDA     #%10100000      E=H R/W=L RS=H  
        STA     ORA
        LDA     #%00100000      E=L PULSE CLOCK LOW
        STA     ORA
        LDA     #%10100100      E=H
        STA     ORA
        RTS

LCD_POS JSR     LCD_ADR         GET CURRENT POSITION IN LCD DRAM
        ORA     #$80            ADD IN ORDER TO GET THE LCD COMMAND
        JSR     LCD_CMD         UPDATE POSITION ON LCD
        RTS

LCD_ADR LDA     LCD_Y           LOAD Y POSITION (LINE NUMBER)
        BEQ     ADD_X            LCD_Y POSITION IS 0?
        LDA     #$40             SET BASE TO $40 FOR LINE 1
ADD_X   CLC                     CLEAR CARRY BIT (JUST IN CASE)
        ADC     LCD_X           ADD X POSITION
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

START   CLD                     CLEAR DECIMAL MODE (JUST IN CASE)
        JSR     INITVIA         INITIALIZE VIA
        JSR     INITLCD         INITIALIZE LCD
        JSR     INITMEM         INITIALIZE MEMORY
        LDA     #0
        STA     LCD_X           CLEAR LCD X POSITION
        STA     LCD_Y           CLEAR LCD Y POSITION
        STA     MODE            CLEAR MODE SELECT
        LDA     #A_CHR          SET LAST CHARACTER TO BE AN
        STA     LAST             'A' TO BEGIN WITH.

FUNCN   JSR     GETKEY
MIDDLE  LDA     SWITCH
        CMP     #%00011110      BIT 0 LOW?
        BNE     UP               ... TRY UP INSTEAD.
        LDA     MODE            CHECK MODE TO SEE WHERE WE ARE
        BEQ     M_SET            ... 0, SWITCH TO ENTRY MODE
        JMP     M_UNSET          ... 1, SWITCH TO CARET MODE
M_SET   LDA     #%00001100      TEMPORARILY DISABLE CURSOR WHILE
        JSR     LCD_CMD          CHANGING THINGS AROUND.
        LDA     #MODE_LD        SET MODE TO DATA LOAD, SWITCHES WILL NOW
        STA     MODE             CHANGE THE CHARACTER SHOWN INSTEAD.
        JSR     LCD_ADR         LCD ADDRESS OFFSET TO ACCUMULATOR
        TAY                     TRANSFER ACCUMULATOR TO X
        LDA     MEMORY,Y        GET CURRENT CHARACTER FROM RAM,
        BNE     M_SETN0          USE LAST CHARACTER ONLY IF 0
        LDA     LAST            SET EQUAL TO LAST CHARACTER IF IT WAS,
        STA     MEMORY,Y         UPDATE MEMORY AS WELL.
M_SETN0 JSR     LCD_CHR         OUTPUT CHARACTER TO THE CURRENT POSITION
        JSR     LCD_POS         RESET POSITION TO WHERE WE WERE
        LDA     #%00001110      RE-ENABLE THE CURSOR, BUT HAVE IT
        JSR     LCD_CMD          IN UNDERLINE MODE TO SHOW MODE.
        JMP     FUNCN           DONE              
M_UNSET LDA     #%00001101      CHANGE CURSOR BACK
        JSR     LCD_CMD          TO BLOCK MODE
        LDA     #MODE_MV        SET MODE TO MOVE, SWITCHES WILL NOW
        STA     MODE             MOVE CARET POINTER INSTEAD.
        JMP     FUNCN           DONE

UP      LDA     SWITCH
        CMP     #%00011101      BIT 1 LOW?
        BNE     DOWN             ... TRY DOWN INSTEAD.        
        LDA     MODE            CHECK MODE TO SEE WHERE WE ARE,
        BNE     U_NXT            NEXT CHARACTER IF MODE 1
        LDA     #$00            MOVE TO UPPER LINE
        STA     LCD_Y
        JSR     LCD_POS         UPDATE CARET POSITION
        JMP     FUNCN           DONE MOVING CARET
U_NXT   LDA     #%00001100      TEMPORARILY DISABLE CURSOR
        JSR     LCD_CMD
        JSR     LCD_ADR         LCD ADDRESS OFFSET TO ACCUMULATOR
        TAY                     TRANSFER ACCUMULATOR TO X
        LDA     MEMORY,Y        GET NEW CHARACTER FOR OUTPUT
        CLC                     CLEAR CARRY BIT (JUST IN CASE)
        ADC     #$01            ADD 1 TO GET THE NEXT VALUE
        STA     MEMORY,Y        SAVE NEW CHARACTER IN RAM,
        STA     LAST             ALSO SET IT AS THE LAST CHARACTER
        JSR     LCD_CHR         OUTPUT TO LCD
        JSR     LCD_POS         RESET POSITION TO WHERE WE WERE
        LDA     #%00001110      RE-ENABLE THE CURSOR
        JSR     LCD_CMD
        JMP     FUNCN           DONE CHANGE CHARACTER

DOWN    LDA     SWITCH
        CMP     #%00011011      BIT 2 LOW?    
        BNE     LEFT             ... TRY LEFT INSTEAD.
        LDA     MODE            CHECK MODE TO SEE WHERE WE ARE,
        BNE     D_PRV            PREVIOUS CHARACTER IF MODE 1
        LDA     #$01            MOVE TO UPPER LINE
        STA     LCD_Y
        JSR     LCD_POS         UPDATE CARET POSITION
        JMP     FUNCN           DONE MOVING CARET
D_PRV   LDA     #%00001100      TEMPORARILY DISABLE CURSOR
        JSR     LCD_CMD
        JSR     LCD_ADR         LCD ADDRESS OFFSET TO ACCUMULATOR
        TAY                     TRANSFER ACCUMULATOR TO X
        LDA     MEMORY,Y        GET NEW CHARACTER FOR OUTPUT
        SEC                     SET CARRY-BIT BEFORE SUBTRACTION    
        SBC     #$01            SUBTRACT 1 TO GET THE NEXT VALUE
        STA     MEMORY,Y        SAVE NEW CHARACTER IN RAM,
        STA     LAST             AND SET IT AS THE LAST CHARACTER
        JSR     LCD_CHR         OUTPUT TO LCD
        JSR     LCD_POS         RESET POSITION TO WHERE WE WERE
        LDA     #%00001110      RE-ENABLE THE CURSOR
        JSR     LCD_CMD
        JMP     FUNCN           DONE CHANGE CHARACTER

LEFT    LDA     SWITCH
        CMP     #%00010111      BIT 3 LOW?
        BNE     RIGHT            ... TRY RIGHT INSTEAD.
        LDA     MODE            CHECK IF IN CARET MODE,
        BEQ     L_MOVE          IF NOT THEN EXIT MODE FIRST.
        LDA     #%00001101      CHANGE CURSOR BACK
        JSR     LCD_CMD          TO BLOCK MODE
L_MOVE  LDA     LCD_X
        CMP     #0
        BEQ     L_DONE          DONE IF ALL THE WAY TO THE LEFT
        DEC     LCD_X           DECREMENT X POSITION
        JSR     LCD_POS         UPDATE DISPLAY
L_DONE  JMP     FUNCN

RIGHT   LDA     SWITCH
        CMP     #%00001111      BIT 4 LOW? 
        BNE     NOKEY            GOTO NOKEY IF NOT
        LDA     MODE            CHECK IF IN CARET MODE,
        BEQ     R_MOVE          IF NOT THEN EXIT MODE FIRST.
        LDA     #%00001101      CHANGE CURSOR BACK
        JSR     LCD_CMD          TO BLOCK MODE
R_MOVE  LDA     LCD_X
        CMP     #15             LAST POSITION
        BEQ     R_DONE          DONE IF ALL THE WAY TO THE RIGHT
        INC     LCD_X           INCREMENT X POSITION
        JSR     LCD_POS         UPDATE DISPLAY
R_DONE  JMP     FUNCN

NOKEY   NOP                     WAIT A BIT
        JMP     FUNCN            THEN RETURN TO SCANNING KEYPAD

* ---------------------------------------------------------
* STORE VECTORS AT END OF EEPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START           NMI VECTOR
        .DA     START           RESET VECTOR
        .DA     START           IRQ VECTOR