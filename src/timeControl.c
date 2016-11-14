/**
 * timer.c
 *
 */
#include <stdlib.h>

#include "timeControl.h"
#include "display.h"

static volatile byte tmr6_ovf; /* Counter for Timer6 overflow */
static volatile byte time_200ms; /* Counter for 200 ms periods */
static time_t uptime_s; /* time_t in seconds, from time.h */
static volatile byte active_digit; /* Stores below enum 0,1,2,3 active 7-seg digit */

static char c_min[2] = {'0', '0'};
static char c_hour[2] = {'0', '0'};

enum {
    hourH, hourL, minH, minL
};

void
init_tmr6() {
    tmr6_ovf = 0x00;
    time_200ms = 0x00;
    uptime_s = 0x00;
    active_digit = 0;

    /* Configure overflow: Fcy=4MHz; Tcy=250ns * 64 * 16 = 256us per tick */
    T6CONbits.T6CKPS = 0b11; // Prescaler @ 1:64
    T6CONbits.T6OUTPS = 0b1111; // Postscaler @ 1:16
    TMR6 = 6; // Preload at 6, then irq ea. (2^8-6)*250us*16*10 = 10ms
    PIE3bits.TMR6IE = 1;
    T6CONbits.TMR6ON = 1;
}

/**
 * Init timer4 for CCP4 module (PWM)
 */
void
init_tmr4() {
    /* Configure overflow: Tcy=250ns * 16 = 4us/tick 
     * 4us/tick * 256 = 1.024 ms to overflow
     */
    T4CONbits.T4CKPS = 0b11; /* Prescaler @ 1:64 */
    //  T4CONbits.T4OUTPS = 0b1111; /* Postscaler @ 1:16 */ /* not used on CCP */
    T4CONbits.TMR4ON = 1;
}

void
init_ccp() {
    TRISBbits.TRISB0 = 1;
    CCPTMRS0bits.C4TSEL = 0b01; /* CCP4 is based off Timer4 in PWM mode */
    PR4 = 64; /* PWM Period 100ms = 2^8/(PWMPeriodMAX/TMR4ovflow) = 64*/
    /* DC will vary, refresh rate fixed to 100Hz */
    /* DC LSb fixed to 00, we only vary Msb */
    CCP4CON = 0b00110000; /* Select PWM mode and duty cycle 2-MSb */
    /* With PR4=64, CCPR4L at 64=0b10000(00) will set DC=100%. 
     * () Remember 2 LSb are unused but specified in CCP4CON */
    CCPR4L = 0b10000; /* duty cycle 8-MSb */
    init_tmr4();
    TRISBbits.TRISB0 = 0;
}

inline void
reset_tmr6() {
    TMR6 = 6;
    tmr6_ovf = 0x00;
}

const time_t *get_uptime(void) {
    return &uptime_s;
}

void incr_uptime(time_t s) {
    uptime_s += s;
}

byte Toggle_Brightess(void) {
    if (CCPR4L < 0b100000)
        CCPR4L = 0b100000; /* Make bright again - DC=100% */
    else
        CCPR4L = 0b10000; /* Cut DC to 50% */
    return CCPR4L;
}

byte get_active_digit() {
    return active_digit;
}

void incr_active_digit() {
    if (active_digit < 4)
        active_digit++;
    else
        active_digit = 0;
}

void TMR6_ISR() {
    /* Display will refresh with this timer running at 100Hz (T=10ms).
     * Preload at 6 so that irq ea. (2^8-6)*250us*16*10 = 10ms
     */

    if (mode == 1) {
#ifndef SIM_ON
        if (active_digit == hourH)
#endif
            display_digit(hourH, &c_hour[0]);
#ifndef SIM_ON
        else if (active_digit == hourL)
#endif
            display_digit(hourL, &c_hour[1]);
#ifndef SIM_ON
        else if (active_digit == minH)
#endif
            display_digit(minH, &c_min[0]);
#ifndef SIM_ON
        else
#endif
            display_digit(minL, &c_min[1]);
    } else if (mode == 2) {
        
    }
    /* At each iteration, switch active digit */
    incr_active_digit();

    /* Clock clocking */
    if (tmr6_ovf++ >= 20) // 200ms have passed
    {
        reset_tmr6();
        if (time_200ms++ >= 5) // 200ms * 5  = 1 s
        {
            time_200ms = 0; /* Reset 200ms counter */
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

/**
 *
 */
void CCP4_ISR() {
    PIR3bits.CCP4IF = 0;

}
