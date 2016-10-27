 /* 
 * File:   Globals.h
 * Author: julio
 *
 * Created on February 7, 2015, 9:32 AM
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

/*--------------------------------------------------------------------------------------------------
                                         Prototypes
--------------------------------------------------------------------------------------------------*/
//inline void activityIndicatorOff( void );
//inline void activityIndicatorOn( void );     //Indicates that IEBus packets are being processed
//inline void usartIndicatorOff(void);
//inline void usartIndicatorOn(void);            //Indicates that spi communication is being used

#endif	/* GLOBALS_H */

