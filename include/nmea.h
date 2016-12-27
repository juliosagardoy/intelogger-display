/* 
 * File: nmea.h  
 * Author: Julio Sagardoy
 * Comments: nmea sentence reader and parser methods
 * Revision history: 1
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef NMEA_H
#define	NMEA_H

#include <string.h>
#include "Globals.h"
#include "usart.h"

typedef struct {
    /**
     * <b>gps_speed</b>
     * <b><p>Summary:</b></p>Array size=4 for storing ASCII parsed speed in kmh
     */
    byte speed[4];

    /**
     * <b>gps_utc</b>
     * <b><p>Summary:</b></p>Array size=6 for storing ASCII parsed UTC time hhmmss
     */
    byte utc[6];

    /**
     * <b>gps_fix</b>
     * <b><p>Summary:</b></p>After parsing nmea, will equal 1 when GPS fix is valid
     * and 0 otherwise
     */
    byte fix;
} gps_t;

typedef gps_t gps;

/**
 * <p><b>nmea_parser</b></p>
 * <p><b>Summary:</b></p> Start a NMEA sentence read from EUSART.
 * <p><b>Precondition:</b></p>Call EUSART module init before using
 */
void nmea_parser(void);
void nmea_parse_gpgga(void);
void nmea_parse_gpvtg(void);
gps nmea_get_gps_data(void);

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* NMEA_H */

