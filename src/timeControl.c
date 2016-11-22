/* 
 * File: timeControl.c 
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM code
 * Revision history: 1
 */

#include <stdlib.h>
#include <pic16f1938.h>

#include "timeControl.h"
#include "display.h"
#include "nmea.h"


static time_t uptime_s; /* time_t in seconds, from time.h */
static volatile byte active_digit; /* Stores below enum 0,1,2,3 active 7-seg digit */
byte c_digits[4] = {'1', '2', '3', '4'};

/** 
 * Timer6 for display refresh events --> around 10ms/100Hz
 */
void
init_tmr6() {
    active_digit = 0;

    /* Configure overflow: Fcy=1MHz; Tcy=100ns * 10 * 64 = 76,8us per tick */
    T6CONbits.T6CKPS = 0b11; // Prescaler @ 1:64
    T6CONbits.T6OUTPS = 0b1011; // Postscaler @ 1:12

    TMR6 = 130; // Preload at 130, then irq ea. (130)*76,8 us = 9.984 ms = 100Hz
    PIE3bits.TMR6IE = 1;
    T6CONbits.TMR6ON = 1;
}

/**
 * Init timer4 for CCP4 module (PWM)
 */
static void
init_tmr4() {
    /* Configure overflow: Tcy=1/(Fosc/4)= 1us * 64presc = 64us/tick 
     * 64us/tick * 256 = 0,016384 s to overflow
     */
    T4CONbits.T4CKPS = 0b11; /* Prescaler @ 1:64 */
    PIE3bits.TMR4IE = 1;

    T4CONbits.TMR4ON = 1;
}

/** 
 * Timer 2 for clock
 */
void
init_tmr2() {
    uptime_s = 0x00;

    /* Configure overflow: Fcy=1MHz; Tcy=100ns * 1 * 10 = 1 us per tick */
    T2CONbits.T2CKPS = 0b00; // Prescaler @ 1:1
    T2CONbits.T2OUTPS = 0b1001; // Postscaler @ 1:10
    TMR2 = 6; // Preload at 6, then irq ea. (2^8-6)*1 us = 250 us
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
    PR4 = 255; /* PWM Period = 0.016384 s = 62Hz */

    /* DC will vary, refresh rate fixed to 62Hz */
    CCP4CON = 0b11; /* Select PWM mode and duty cycle 2-MSb */

    /* () Remember 2 LSb are unused but specified in CCP4CON */
    CCPR4L = 0b111111; /* duty cycle 8-MSb */

    init_tmr4();

    PIE3bits.CCP4IE = 1;
    TRISBbits.TRISB0 = 0;
}

void incr_uptime(time_t s) {
    uptime_s += s;
}

byte Toggle_Brightess(void) {
    if (CCPR4L < 0b100000)
        CCPR4L = 0b111111; /* Make bright again - DC=100% */
    else
        CCPR4L = 0b100000; /* Cut DC to 50% */
    return CCPR4L;
}

static void incr_active_digit() {
    if (active_digit < 4)
        active_digit++;
    else
        active_digit = 0;
}

/* Overflow every 9.98 ms! 100Hz*/
void TMR6_ISR() {
    TMR6 = 130;

    if (display_mode == 1)
        display_digit(active_digit, &c_digits[active_digit]);

    else if (display_mode == 2) {
        /* Speed display will be aligned to the right of the display.  
         The following takes the lenght of speed sentence into account */
        /* Speed between 0 and 9 kmh */
        if (strlen(gps_speed) == 1 && active_digit == 4)
            display_digit(active_digit, &gps_speed[0]);
            /* Speed between 10 and 99 kmh */
        else if (strlen(gps_speed) == 2 \
                && (active_digit == 3 || active_digit == 4))
            display_digit(active_digit, &gps_speed[active_digit - 3]);
            /* Speed between 100 kmh and 999 kmh :) */
        else if (strlen(gps_speed) == 3 \
                && (active_digit == 2) || active_digit == 3 || active_digit == 4)
            display_digit(active_digit, &gps_speed[active_digit - 2]);
        else
            display_digit(active_digit, "");
    }
    /* At each iteration, switch active digit */
    incr_active_digit();

    PIR3bits.TMR6IF = 0;
}

/** Overflow every 250 us! */
void TMR2_ISR() {
    volatile static word tmr6_ovf1 = 0; /* Counter for Timer6 overflow */
    volatile byte tmp[2];
    
    TMR2 = 6;

    /* Clock clocking */
    if (tmr6_ovf1++ >= 4000) // 250us*4000 = 1 ms have passed
    {
        tmr6_ovf1 = 0;
        uptime_s++; /* Increase clock by one second */

        tp = gmtime(&uptime_s); /* Re-populates tm time.h struct */
        
        /* DIRTY Workaround to fix itoa output when number is only one cypher. */
        if (tp->tm_min < 10) {
            itoa(tmp, tp->tm_min, 10);
            c_digits[3] = tmp[1];
            c_digits[2] = '0';
        } else
            itoa(tmp, tp->tm_min, 10);

        if (tp->tm_hour < 10) {
            itoa(tmp, tp->tm_hour, 10);
            c_digits[1] = tmp[1];
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