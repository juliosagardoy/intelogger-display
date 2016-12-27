/* 
 * File: display.c
 * Author: Julio Sagardoy
 * Comments: 7-segment display controller
 * Revision history: 1
 */

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
 * Timer6 for display refresh events --> around 10ms/100Hz
 */
void
init_tmr6() {
    active_digit = 0;

    /* Configure overflow: Fcy=1MHz; Tcy=1us; incTMR=Tcy/2
     * 1us * 5 * 4 = 24us per tick */
    T6CONbits.T6CKPS = 0b01; // Prescaler @ 1:4
    T6CONbits.T6OUTPS = 0b0001; // Postscaler @ 1:2

    TMR6 = 0; // Then irq ea. 256*8us = 2048us (*4 char = 122Hz)
    PIE3bits.TMR6IE = 1;
    T6CONbits.TMR6ON = 1;
}

static void incr_active_digit() {
    if (active_digit < 3)
        active_digit++;
    else
        active_digit = 0;
}

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
    display_encode(0x00);
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

/* Overflow every 15 ms! 65Hz*/
void TMR6_ISR() {
    TMR6 = 6;

    /* At each iteration, switch active digit */
    incr_active_digit();

    if (display_mode == 1)
        display_digit(active_digit, &c_digits[active_digit]);

    else if (display_mode == 2) {
        /* Speed display will be aligned to the right of the display.  
         * The following takes the length of speed sentence into account */
        /* Speed between 0 and 9 kmh */
        if (strlen(nmea_get_gps_data().speed) == 1 && active_digit == 4)
            display_digit(active_digit, &nmea_get_gps_data().speed[0]);
            /* Speed between 10 and 99 kmh */
        else if (strlen(nmea_get_gps_data().speed) == 2 \
                && (active_digit == 3 || active_digit == 4))
            display_digit(active_digit, &nmea_get_gps_data().speed[active_digit - 3]);
            /* Speed between 100 kmh and 999 kmh :) */
        else if (strlen(nmea_get_gps_data().speed) == 3 \
                && (active_digit == 2 || active_digit == 3 || active_digit == 4))
            display_digit(active_digit, &nmea_get_gps_data().speed[active_digit - 2]);
        else
            display_digit(active_digit, "");
    }

    PIR3bits.TMR6IF = 0;
}