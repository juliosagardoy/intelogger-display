/* 
 * File: display.c
 * Author: Julio Sagardoy
 * Comments: 7-segment display controller
 * Revision history: 1
 */

#ifndef LED_DECODER_H
#define	LED_DECODER_H

#include <string.h>
#include "Globals.h"
#include "nmea.h"


static volatile byte active_digit; /* Active 7-seg digit: 0MSD - 3LSB */
byte c_digits[4] = {'-', '-', '-', '-'};

void display_encode(const byte*);
void display_digit(byte, const byte*);

/**
 * Init timer6 to overflow every 10ms
 * Fcy=4MHz
 */
void init_tmr6(void);

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

#endif	/* LED_DECODER_H */

