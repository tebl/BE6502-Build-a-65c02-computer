#define VERSION "0.1"
#define BAUD_RATE 115200
#define MAX_INPUT_SIZE 100


#define SBC_CLOCK 2
#define SBC_RW 3
#define SBC_IRQ 5
#define SBC_NMI 6
#define SBC_RDY 7
#define SBC_SYNC 8
#define SBC_RESET 23

#define SBC_A15 22
#define SBC_A14 24
#define SBC_A13 26
#define SBC_A12 28
#define SBC_A11 30
#define SBC_A10 32
#define SBC_A9 34
#define SBC_A8 36
#define SBC_A7 38
#define SBC_A6 40
#define SBC_A5 42
#define SBC_A4 44
#define SBC_A3 46
#define SBC_A2 48
#define SBC_A1 50
#define SBC_A0 52
const char SBC_ADDR[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};

#define SBC_D7 39
#define SBC_D6 41
#define SBC_D5 43
#define SBC_D4 45
#define SBC_D3 47
#define SBC_D2 49
#define SBC_D1 51
#define SBC_D0 53
const char SBC_DATA[] = {39, 41, 43, 45, 47, 49, 51, 53};

#define SBC_PIN21 4
#define SBC_PIN35 9
#define SBC_PIN36 10
#define SBC_PIN37 11
#define SBC_PIN38 12
#define SBC_PIN39 13

#define USER_LED A11
#define USER_SW1 A8
#define USER_SW2 A9
#define USER_SW3 A10