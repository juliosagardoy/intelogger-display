/* 
 * File: display.c
 * Author: Julio Sagardoy
 * Comments: 7-segment display controller
 * Revision history: 1
 */

#include "Globals.h"
#include "display.h"

/* 7seg: a dp b c d e f g 
 * Common anode.
 * Values are already inverted considering current sinking through PIC!!!
 */
#define _0 0b01000001;
#define _1 0b11001111;
#define _2 0b01010010;
#define _3 0b01000110;
#define _4 0b11001100;
#define _5 0b01100100;
#define _6 0b01100000;
#define _7 0b01001101;
#define _8 0b01000000;
#define _9 0b01000100;
#define _E 0b00110000;
#define _dash 0b11111110;
#define _dp 0b10111111;
#define _null 0b11111111;

/**
 * Sends uint8_t n to designed 7-segment digit ch
 * @arg MUX channel, see enum
 * @arg uint8_t
 */
void
display_digit(byte ch, const byte *n) {
    /* MUX channel assign 
     * B    A   Channel
     * RB5  RB4
     * 0    0   DISP_C1
     * 0    1   DISP_C2
     * 1    0   DISP_C3
     * 1    1   DISP_C4
     */
    switch (ch) {
        case 0: /* Hour higher cypher */
            LATBbits.LATB4 = 0;
            LATBbits.LATB5 = 0;
            break;
        case 1: /* Hour lower cypher */
            LATBbits.LATB4 = 1;
            LATBbits.LATB5 = 0;
            break;
        case 2: /* Minutes higher cypher */
            LATBbits.LATB4 = 0;
            LATBbits.LATB5 = 1;
            break;
        case 3:
        default: /* Minutes lower cypher */
            LATBbits.LATB4 = 1;
            LATBbits.LATB5 = 1;
    }
    display_encode(n);
}

/**
 * Puts an ASCII char (byte) to its correspondent 7-segment equivalent and
 * sends it to the mux
 * @param n ASCII char. Only admits numbers, otherwise E. will be put
 */
void
display_encode(const uint8_t* n) {
    switch (*n) {
        case '0':
            LATA = _0;
            break;
        case '1':
            LATA = _1;
            break;
        case '2':
            LATA = _2;
            break;
        case '3':
            LATA = _3;
            break;
        case '4':
            LATA = _4;
            break;
        case '5':
            LATA = _5;
            break;
        case '6':
            LATA = _6;
            break;
        case '7':
            LATA = _7;
            break;
        case '8':
            LATA = _8;
            break;
        case '9':
            LATA = _9;
            break;
        case '.':
            LATA = _dp;
            break;
        case '-':
            LATA = _dash ;
            break;
        case 'e':
            LATA = _E;
            break;
        default:
            LATA = _null;
            break;
    }
}

