* ---------------------------------------------------------
* INITIALIZE LCD DISPLAY AND OUTPUT SOME DATA TO IT


        .CR     65C02
        .TF     hello_lcd.hex,INT
        
        .OR     $8000
        .TA     $0000

ORB     .EQ     $6000           VIA Port B data
ORA     .EQ     $6001           VIA Port A data
DDRB    .EQ     $6002           VIA DDRB
DDRA    .EQ     $6003           VIA DDRA

INITVIA LDA     #%11111111
        STA     DDRB
        LDA     #%11100000
        STA     DDRA
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
     
        LDA     #%00001111      DISPLAY ON, CURSOR EN.
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
        
LCD_CHR STA 	ORB             DATA IS IN ACCUMULATOR
        LDA     #%10100000      E=H R/W=L RS=H  
        STA     ORA
        LDA     #%00100000      E=L PULSE CLOCK LOW
        STA     ORA
        LDA     #%10100100      E=H
        STA     ORA
        RTS

WAITLCD NOP                     PLACEHOLDER
        RTS

START   JSR     INITVIA         INITIALIZE VIA
        JSR     INITLCD         INITIALIZE LCD
        
        LDA     #%01001000	    H
        JSR	    LCD_CHR

        LDA     #%01101001	    i
        JSR     LCD_CHR
LOOP    NOP
        JMP     LOOP

* ---------------------------------------------------------
* STORE VECTORS AT END OF EPROM.
        .OR     $FFFA
        .TA     $7FFA
        .DA     START   NMI VECTOR
        .DA     START   RESET VECTOR
        .DA     START   IRQ VECTOR