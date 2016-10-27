/**
 * 
 * 
 * 
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
#define _err 0b00110000;
#define _dp 0b10111111;

/**
 * Sends char to designed 7-segment position
 * @arg MUX channel
 * @arg char
 */
void
Display_Show(byte ch, const char *n)
{
    /* MUX channel assign 
     * B    A   Channel
     * RB5  RB4
     * 0    0   DISP_C1
     * 0    1   DISP_C2
     * 1    0   DISP_C3
     * 1    1   DISP_C4
    */
    switch (ch)
    {
    case 1: /* Hour higher cypher */
        LATBbits.LATB4 = 0;
        LATBbits.LATB5 = 0;
        break;
    case 2: /* Hour lower cypher */
        LATBbits.LATB4 = 1;
        LATBbits.LATB5 = 0;
        break;
    case 3: /* Minutes higher cypher */
        LATBbits.LATB4 = 0;
        LATBbits.LATB5 = 1;
        break;
    case 4: 
    default: /* Minutes lower cypher */
        LATBbits.LATB4 = 1;
        LATBbits.LATB5 = 1;
        break;
    }
    Display_Encode(n);
}

/**
 * Puts an ASCII char (byte) to its correspondent 7-segment equivalent and
 * sends it to the mux
 * @param n ASCII char. Only admits numbers, otherwise E. will be put
 */
void
Display_Encode(const char *n)
{
    switch (*n)
    {
    case '0':
        LATC = _0;
        break;
    case '1':
        LATC = _1;
        break;
    case '2':
        LATC = _2;
        break;
    case '3':
        LATC = _3;
        break;
    case '4':
        LATC = _4;
        break;
    case '5':
        LATC = _5;
        break;
    case '6':
        LATC = _6;
        break;
    case '7':
        LATC = _7;
        break;
    case '8':
        LATC = _8;
        break;
    case '9':
        LATC = _9;
        break;
    case '.':
        LATC = _dp;
        break;
    default:
        LATC = _err;
        break;
    }
}
