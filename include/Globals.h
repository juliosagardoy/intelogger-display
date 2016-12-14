/* 
 * File: Globals.h  
 * Author: Julio Sagardoy
 * Comments: Globals
 * Revision history: 1
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <xc.h>
#include <stddef.h>
#include <stdint.h>        /* For uintX_t definition */
#include <string.h>
#include <stdio.h>

/*--------------------------------------------------------------------------------------------------
                                          Constants
--------------------------------------------------------------------------------------------------*/
#define _XTAL_FREQ 4000000

#define bit_is_set(port,pin)     ((port) & (1<<(pin)))
#define bit_is_clear(port,pin)   ~((port) & (1<<(pin)))

#define sbi(port, bit)           (port) |= (1 << (bit))
#define cbi(port, bit)           (port) &= ~(1 << (bit))

#define _BV(bit)                 (1 << (bit))

#define toggle(b)                b^=0x01

#define INPUT_IS_SET             ( bit_is_set( DATAIN_PIN, DATAIN ) )
#define INPUT_IS_CLEAR           ( bit_is_clear( DATAIN_PIN, DATAIN ) )

/*--------------------------------------------------------------------------------------------------
                                       Type definitions
--------------------------------------------------------------------------------------------------*/
typedef uint8_t     byte;
typedef uint16_t    word;

/**
 * 
 */
volatile byte display_mode = 2; /* Display mode: 1-clock 2-speed */

#endif	/* GLOBALS_H */

