/**
 * timer.c
 *
 */

#include "timeControl.h"

static byte tmr6_ovf; /* Counter for Timer6 overflow */
static byte time_200ms; /* Counter for 200 ms periods */
static time_t uptime_s; /* time_t in seconds, from time.h */
static byte disp_slectd;

//struct tm tt = {0};	/* Time struct from time.h */
//struct tm* tp;	/* Handle to Time struct */
//time_t clock;		/* Clock */

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
void
<<<<<<< HEAD
init_tmr6()
{
=======
init_tmr6() {
>>>>>>> origin/master
    tmr6_ovf = 0x00;
    time_200ms = 0x00;
    uptime_s = 0x00;
    disp_slectd = 0x00;
    /* Configure overflow: Tcy=250ns * 64 * 16 = 256us per tick */
    T6CONbits.T6CKPS = 0b11; // Prescaler @ 1:64
    T6CONbits.T6OUTPS = 0b1111; // Postscaler @ 1:16
    TMR6 = 6; // Preload at 6, then irq ea. 250*250us*16*10 = 10ms
    PIE3bits.TMR6IE == 1;
    T6CONbits.TMR6ON = 1;
}

/**
 * Init timer4 for CCP4 module (PWM)
 */
void
init_tmr4()
{
    /* Configure overflow: Tcy=250ns * 64 * 16 = 256us/tick 
     * 256us/tick * 256 = 65 ms to overflow
     */
    T4CONbits.T4CKPS = 0b11; /* Prescaler @ 1:64 */
    T4CONbits.T4OUTPS = 0b1111; /* Postscaler @ 1:16 */
    T4CONbits.TMR4ON = 1;
}

/**
 * Init CCP4 module to PWM
 * 50Hz min (20 ms period): 256/65ms*20ms=78
 */
void
init_ccp()
{
    CCPTMRS0bits.C4TSEL = 0b01; /* CCP4 is based off Timer4 in PWM mode */
    PR4 = 80; /* PWM Period initial, equal to 50Hz*/
    /* Set duty cycle 50% */
    CCP4CON = 0b00111100; /* Select PWM mode and duty cycle 2-MSb */
    CCPR4L = 0x7F; /* duty cycle 8-MSb */
    init_tmr4();
}

/**
 * Manually resets Timer6 to initial state, which is preloaded to 6
 * This has to be called after each interrupt.
 */
inline void
reset_tmr6()
{
    TMR6 = 6;
    tmr6_ovf = 0x00;
}

/** Handle to uptime_s for time.h gmtime-type methods
 * @return pointer to uptime_s
 */
const time_t *get_uptime(void)
{
    return &uptime_s;
}

/** Increase uptime by s seconds
 *  @arg s seconds to be increased
 */
void inc_uptime(time_t s)
{
    uptime_s += s;
}

/**
 * Reduces brightness by one step. Wrap-around.
 * @return Current PR4 value
 */
byte Increase_Brightess(void)
{
    if (PR4 < 80)
        PR4 += 20;
    else
        PR4 = 0;
    return PR4;
}

/**
 * Return current value of the counter from 0 to 3 running every Timer6 irq
 * @return 
 */
byte get_ctr2four_val()
{
    return disp_slectd;
}

/**
 * Increase value of the counter from 0 to 3 running every Timer6 irq
 */
void inc_ctr2four_val()
{
    if (disp_slectd < 4)
        disp_slectd++;
    else
        disp_slectd = 0;
}

/**
 * Timer6 interruption routine
 * Occurs every 250*250ns*16*10 = 10ms
 * OLD--- Count to 100. 10ms * 100 = 1s. Then increase 1s, 1m or 1h.
 * Count to 25. 10ms * 20 = 200ms. Then increase 1m or 1h.
 */
void TMR6_ISR()
{
    /* Display will refresh with this timer running at 100Hz.
     * ctr2four increases a value of 1,
     * main code will do the rest when back from interrupt */
    inc_ctr2four_val();  
    /* Now continue with clock clocking */
    if (++tmr6_ovf >= 20) // 200ms have passed
    {
        reset_tmr6();
        if (++time_200ms >= 5) // 200ms * 5  = 1 s
        {
            uptime_s++; // Increase one second
        }
    }
    PIR3bits.TMR6IF = 0;
}

/**
 *
 */
void CCP4_ISR()
{
    PIR3bits.CCP4IF = 0;

}
