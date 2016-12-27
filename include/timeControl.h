/*
 * File: timeControl.h
 * Author: Julio Sagardoy
 * Comments: Timers and CCP PWM
 * Revision history: 1
 */

#ifndef TIMER_H
#define	TIMER_H

#include <time.h>

#include "display.h"
//#include "nmea.h"

struct tm tt = {0}; /* Time struct from time.h */
struct tm* tp; /* Handle to Time struct */

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
void init_tmr2(void);

/** Increase uptime by s seconds
 *  @arg s seconds to be increased
 */
void incr_uptime(time_t);

/**
 *
 */
void TMR2_ISR(void);

#endif	/* TIMER_H */