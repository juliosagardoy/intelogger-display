/* 
 * File: timeControl.h  
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM
 * Revision history: 1
 */

#ifndef TIMER_H
#define	TIMER_H

#include <time.h>
#include "Globals.h"

struct tm tt = {0}; /* Time struct from time.h */
struct tm* tp; /* Handle to Time struct */

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
void init_tmr6(void);

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
static void init_tmr4(void);

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
void init_tmr2(void);

/**
 * Init CCP4 module for PWM
 * 
 */
void init_ccp(void);

/** Increase uptime by s seconds
 *  @arg s seconds to be increased
 */
void incr_uptime(time_t);

/**
 * Reduces LED brightness by half. Wrap-around.
 * @return Current CCPR4L value
 */
byte Toggle_Brightess(void);

/**
 * Counter 0,1,2,3,then-wrap-around; used for digit selection.
 * Calling it increase value of active_digit
 */
static void incr_active_digit(void);

/**
 * Timer6 interruption routine
 * Controls time and LED display refresh rate
 * Occurs every 250*250ns*16*10 = 10ms
 * 
 */
void TMR6_ISR(void);

/**
 * 
 */
void TMR4_ISR(void);

/**
 *
 */
void TMR2_ISR(void);

/**
 *
 */
void CCP4_ISR(void);
#endif	/* TIMER_H */