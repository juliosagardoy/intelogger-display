/* 
 * File: timeControl.c 
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM code
 * Revision history: 1
 */

#include <stdlib.h>
#include "timeControl.h"

static time_t uptime_s; /* time_t in seconds, from time.h */

/** 
 * Timer 2 for clock
 */
void
init_tmr2() {
    uptime_s = 0x00;

    /* Configure overflow: Fcy=1MHz; Tcy=1us; incTMR=Tcy/2 (each clock edge)
     * 0.5us * 16 * 1 = 32 us per tick */
    T2CONbits.T2CKPS = 0b10; // Prescaler @ 1:16
    T2CONbits.T2OUTPS = 0b0001; // Postscaler @ 1:1
    TMR2 = 6; // Preload at 6, then irq ea. (2^8-6)*32 us = 8ms
    PIE1bits.TMR2IE = 1;
    T2CONbits.TMR2ON = 1;
}

void incr_uptime(time_t s) {
    uptime_s += s;
}

/** Overflow every 8ms! */
void TMR2_ISR() {
    static byte tmr6_ovf1 = 0; /* Counter for Timer6 overflow */
    byte tmp[2];

    TMR2 = 6;

    /* Clock clocking */
    if (tmr6_ovf1++ >= 125) // 8ms*125 = 1 s have passed
    {
        tmr6_ovf1 = 0;
        uptime_s++; /* Increase clock by one second */

        tp = gmtime(&uptime_s); /* Re-populates tm time.h struct */

        /* DIRTY Workaround to fix itoa output when number is only one cypher. */
        if (tp->tm_sec < 10) {
            itoa(tmp, tp->tm_sec, 10);
            c_digits[3] = tmp[0];
            c_digits[2] = '0';
        } else {
            itoa(tmp, tp->tm_sec, 10);
            c_digits[3] = tmp[1];
            c_digits[2] = tmp[0];
        }
        if (tp->tm_hour < 10) {
            itoa(tmp, tp->tm_hour, 10);
            c_digits[1] = tmp[0];
            c_digits[0] = '0';
        } else
            itoa(c_digits, tp->tm_hour, 10);
        /* END of DIRTY */
    }

    PIR1bits.TMR2IF = 0;
}