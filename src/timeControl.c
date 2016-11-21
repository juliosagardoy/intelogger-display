/* 
 * File: timeControl.c 
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM code
 * Revision history: 1
 */

#include <stdlib.h>

#include "timeControl.h"
#include "display.h"
#include "nmea.h"


time_t uptime_s; /* time_t in seconds, from time.h */
volatile byte active_digit; /* Stores below enum 0,1,2,3 active 7-seg digit */

byte c_min[2] = {'0', '0'};
byte c_hour[2] = {'0', '0'};

void
init_tmr6() {
    uptime_s = 0x00;
    active_digit = 0x01;

    /* Configure overflow: Fcy=1MHz; Tcy=100ns * 1 * 10 = 1 us per tick */
    //T6CONbits.T6CKPS = 0b11; // Prescaler @ 1:64
    T6CONbits.T6CKPS = 0b00;    // Prescaler @ 1:1
    //T6CONbits.T6OUTPS = 0b1111; // Postscaler @ 1:16
    T6CONbits.T6OUTPS = 0b1001; // Postscaler @ 1:10
    TMR6 = 6; // Preload at 6, then irq ea. (2^8-6)*1 us = 250 us
    PIE3bits.TMR6IE = 1;
    T6CONbits.TMR6ON = 1;
}

/**
 * Init timer4 for CCP4 module (PWM)
 */
void
init_tmr4() {
    /* Configure overflow: Tcy=1/(Fosc/4)= 1us * 64presc = 64us/tick 
     * 64us/tick * 256 = 0,016384 s to overflow
     */
    T4CONbits.T4CKPS = 0b11; /* Prescaler @ 1:64 */
    PIE3bits.TMR4IE = 1;
    PIR3bits.TMR4IF = 0;
    //  T4CONbits.T4OUTPS = 0b1111; /* Postscaler @ 1:16 */ /* not used on CCP */
    T4CONbits.TMR4ON = 1;
}

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

static const time_t *get_uptime(void) {
    return &uptime_s;
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

/* Overflow every 250us! */
void TMR6_ISR() {
    volatile static byte tmr6_ovf1 = 0; /* Counter for Timer6 overflow */
    volatile static byte tmr6_ovf2 = 0; /* Counter at 100Hz for display updates */
    volatile static byte tmr6_ovf3 = 0; /* Counter at 4Hz for clock */
    
    TMR6 = 6;
    
    /* Display will refresh with this timer running at 100Hz (T=10ms).
     * Preload at 6 so that irq ea. (2^8-6)*1us = 250us
     */
    if (tmr6_ovf2++ >= 40) /* 250us * 40 = 10ms */ {
        tmr6_ovf2 = 0;
        if (mode == 1) {
            if (active_digit == 1)
                display_digit(1, &c_hour[0]);

            else if (active_digit == 2)
                display_digit(2, &c_hour[1]);

            else if (active_digit == 3)
                display_digit(3, &c_min[0]);
            else
                display_digit(4, &c_min[1]);
        } else if (mode == 2) {
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
    }
    
    /* Clock clocking */
    if (tmr6_ovf1++ >= 250) // 250us*250 = 62.5 ms have passed
    {
        tmr6_ovf1 = 0;

        if (tmr6_ovf3++ >= 16) // 62.5ms * 16  = 1 s
        {
            tmr6_ovf3 = 0; /* Reset 200ms counter */
            uptime_s++; /* Increase clock by one second */

            tp = gmtime(get_uptime()); /* Re-populates tm time.h struct */

            /* DIRTY Workaround to fix itoa output when number is only one cypher. */
            if (tp->tm_min < 10) {
                itoa(c_min, tp->tm_min, 10);
                c_min[1] = c_min[0];
                c_min[0] = '0';
            } else
                itoa(c_min, tp->tm_min, 10);

            if (tp->tm_hour < 10) {
                itoa(c_hour, tp->tm_hour, 10);
                c_hour[1] = c_min[0];
                c_hour[0] = '0';
            } else
                itoa(c_hour, tp->tm_hour, 10);
            /* END of DIRTY */
        }
    }
    PIR3bits.TMR6IF = 0;
}

void TMR4_ISR() {
    PIR3bits.TMR4IF = 0;

}

void CCP4_ISR() {
    PIR3bits.CCP4IF = 0;
}