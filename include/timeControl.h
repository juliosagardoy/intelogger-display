/* 
 * File:   timer.h
 * Author: julio
 *
 * Created on March 22, 2016, 8:25 AM
 */

#ifndef TIMER_H
#define	TIMER_H
#include <time.h>
#include "Globals.h"

struct tm tt = {0}; /* Time struct from time.h */
struct tm* tp; /* Handle to Time struct */


void init_tmr6(void);
void init_ccp(void);
inline void reset_tmr6(void);
const time_t *get_uptime(void);
void incr_uptime(time_t);
byte Increase_Brightess(void);
byte get_ctr2four_val(void);
void incr_active_digit(void);
void TMR6_ISR(void);
void CCP4_ISR(void);

#endif	/* TIMER_H */