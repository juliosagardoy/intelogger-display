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

void init_display() {
    memset(digits, '-', 4);
    init_tmr6();
}

/** 
 * Timer6 for display refresh events --> around 10ms/100Hz
 */
void
init_tmr6() {
    digit_en = 0;

    /* Configure overflow: Fcy=1MHz; Tcy=1us; incTMR=Tcy/2
     * 1us * 5 * 4 = 24us per tick */
    T6CONbits.T6CKPS = 0b01; // Prescaler @ 1:4
    T6CONbits.T6OUTPS = 0b0001; // Postscaler @ 1:2

    TMR6 = 0; // Then irq ea. 256*8us = 2048us (*4 char = 122Hz)
    PIE3bits.TMR6IE = 1;
    T6CONbits.TMR6ON = 1;
}

static void incr_digit_en() {
    if (digit_en < 3)
        digit_en++;
    else
        digit_en = 0;
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
 * @param n ASCII char. Only admits numbers, otherwise nothing will be put
 */
void
display_encode(const uint8_t* n) {
    switch (*n) {
        case '0':
            LATA |= 0b000001;
            LATC |= 0b01;
            break;
        case '1':
            LATA |= 0b001111;
            LATC |= 0b11;
            break;
        case '2':
            LATA |= 0b010010;
            LATC |= 0b01;
            break;
        case '3':
            LATA |= 0b000110;
            LATC |= 0b01;
            break;
        case '4':
            LATA |= 0b001100;
            LATC |= 0b11;
            break;
        case '5':
            LATA |= 0b100100;
            LATC |= 0b01;
            break;
        case '6':
            LATA |= 0b100000;
            LATC |= 0b01;
            break;
        case '7':
            LATA |= 0b001101;
            LATC |= 0b01;
            break;
        case '8':
            LATA |= 0b000000;
            LATC |= 0b01;
            break;
        case '9':
            LATA |= 0b000100;
            LATC |= 0b01;
            break;
        case '.':
            LATA |= 0b111111;
            LATC |= 0b11;
            break;
        case '-':
            LATA |= 0b111110;
            LATC |= 0b11;
            break;
        case 'e':
            LATA |= 0b110000;
            LATC |= 0b00;
            break;
        default:
            LATA |= 0b111111;
            LATC |= 0b11;
            break;
    }
}

/* Overflow every 15 ms! 65Hz*/
void TMR6_ISR() {
    TMR6 = 6;

    /* At each iteration, switch active digit */
    incr_digit_en();

    if (display_mode == 1)
        display_digit(digit_en, &digits[digit_en]);

    else if (display_mode == 2) {
        /* Speed display will be aligned to the right of the display.  
         * The following takes the length of speed sentence into account */
        /* Speed between 0 and 9 kmh */
        if (strlen(nmea_get_gps_data().speed) == 1 && digit_en == 4)
            display_digit(digit_en, &nmea_get_gps_data().speed[0]);
            /* Speed between 10 and 99 kmh */
        else if (strlen(nmea_get_gps_data().speed) == 2 \
                && (digit_en == 3 || digit_en == 4))
            display_digit(digit_en, &nmea_get_gps_data().speed[digit_en - 3]);
            /* Speed between 100 kmh and 999 kmh :) */
        else if (strlen(nmea_get_gps_data().speed) == 3 \
                && (digit_en == 2 || digit_en == 3 || digit_en == 4))
            display_digit(digit_en, &nmea_get_gps_data().speed[digit_en - 2]);
        else
            display_digit(digit_en, "");
    }

    PIR3bits.TMR6IF = 0;
}
