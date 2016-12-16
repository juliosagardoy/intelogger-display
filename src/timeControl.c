/* 
 * File: timeControl.c 
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM code
 * Revision history: 1
 */

#include <stdlib.h>
#include "Globals.h"
#include "timeControl.h"
#include "display.h"
#include "nmea.h"

static time_t uptime_s; /* time_t in seconds, from time.h */
static volatile byte active_digit; /* Active 7-seg digit: 0MSD - 3LSB */
byte c_digits[4] = {'-', '-', '-', '-'};

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

/**
 * Init timer4 for CCP4 module (PWM)
 */
static void
init_tmr4() {
    /* Configure overflow:cy=1MHz; Tcy=1us; incTMR=Tcy/2
     * 1us * 4presc = 4us/tick 
     * 4us/tick * 256 = 1,024 ms to overflow
     */
    T4CONbits.T4CKPS = 0b01; /* Prescaler @ 1:2 */
    PIE3bits.TMR4IE = 1;

    T4CONbits.TMR4ON = 1;
}

/** 
 * Timer 2 for clock
 */
void
init_tmr2() {
    uptime_s = 0x00;

    /* Configure overflow: Fcy=1MHz; Tcy=1us; incTMR=Tcy
     * 0.5us * 64 * 10 = 20 us per tick */
    T2CONbits.T2CKPS = 0b11; // Prescaler @ 1:64
    T2CONbits.T2OUTPS = 0b1001; // Postscaler @ 1:10
    TMR2 = 6; // Preload at 6, then irq ea. (2^8-6)*20 us = 5ms
    PIE1bits.TMR2IE = 1;
    T2CONbits.TMR2ON = 1;
}

/**
 * CCP module
 */
void
init_ccp() {
    TRISBbits.TRISB0 = 1; /* Temporarily disable output to avoid glitches */
    CCPTMRS0bits.C4TSEL = 0b01; /* CCP4 is based off Timer4 in PWM mode */
    PR4 = 256; /* PWM Period = TMR4 till ovf = 0.001024 s */

    /* Select duty cycle 2-MSb (10) and PWM mode (11xx) */
    //CCP4CON = 0b101100;
    CCP4CON = 0b111100;
    /* 2 LSb in CCP4CON */
    //CCPR4L = 0b000000; /* duty cycle 8-MSb */
    CCPR4L = 0b111111; /* duty cycle 8-MSb */
    init_tmr4();
    
    TMR4 = 0;
    PIE3bits.CCP4IE = 1;
    TRISBbits.TRISB0 = 0;
}

void incr_uptime(time_t s) {
    uptime_s += s;
}

byte Toggle_Brightess(void) {
    if (CCPR4L < 0b100000) {
        CCP4CON = 0b111100;
        CCPR4L = 0b111111; /* Make bright again - DC=100% */
        return 1;
    } else {
        CCP4CON = 0b101100;
        CCPR4L = 0b000000; /* Cut DC to 50% */
        return 0;
    }
}

static void incr_active_digit() {
    if (active_digit < 3)
        active_digit++;
    else
        active_digit = 0;
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

/** Overflow every 5ms! */
void TMR2_ISR() {
    static byte tmr6_ovf1 = 0; /* Counter for Timer6 overflow */
    byte tmp[2];

#ifdef SIM_ON       /* Simulate call to TMR6_ISR after 15ms */
    static byte isr2_qt = 1;
    if (isr2_qt++ == 3) /* 3*5ms=15ms have passed*/
        TMR6_ISR();
#endif

    TMR2 = 6;

    /* Clock clocking */
    if (tmr6_ovf1++ >= 200) // 5ms*200 = 1 s have passed
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

void TMR4_ISR() {
    PIR3bits.TMR4IF = 0;

}

void CCP4_ISR() {
    PIR3bits.CCP4IF = 0;
}
