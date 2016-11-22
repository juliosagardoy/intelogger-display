/* 
 * File: nmea.h  
 * Author: Julio Sagardoy
 * Comments: nmea sentence reader and parser methods code
 * Revision history: 1
 */

#include "nmea.h"
#include "usart.h"

void nmea_parser() {
    /* Refresh GPS */
    if (EUSART_Read_1Byte_NONBL() != '$') /* Found GPS start bit */ {
        /* Read NMEA sentence header (5 chars string) */
        byte nmea_header[5];
        memset(nmea_header, 0x00, 5);

        for (int i = 0; i < 5; i++) {
            nmea_header[i] == EUSART_Read_1Byte();
        }
        if (strcmp(nmea_header, "GPGGA") == 0)
            nmea_parse_gpgga();

        /* Analyze header and extract data */
        if (strcmp(nmea_header, "GPVTG") == 0)
            nmea_parse_gpvtg();
    }
}

void nmea_parse_gpgga() {
    byte i = 0;
    byte c = 0;
    EUSART_Read_1Byte(); /* Discard ',' */
    /* Read next fixed 6 characters: hhmmss */
    for (i = 0; i < 6; i++)
        gps_utc[i] = EUSART_Read_1Byte();
    /* Discard uninteresting X parameters by counting 5 commas */
    do {
        if (EUSART_Read_1Byte() == ',')
            c++;
    } while (c < 5);
    /* Next param is GPS fix valid */
    if (EUSART_Read_1Byte() == '1')
        gps_fix = 1;
    else
        gps_fix = 0;
}

void nmea_parse_gpvtg() {
    byte i = 0;
    byte c = 0;
    EUSART_Read_1Byte(); /* Discard ',' */

    /* Discard uninteresting X parameters until speed_kmh 
     * by counting 7 commas */
    do {
        if (EUSART_Read_1Byte() == ',')
            c++;
    } while (c < 7);

    /* Read speed (km/h) param. Discard at decimal separator '.' */
    i = 0;
    while (EUSART_Read_1Byte() != '.')
        gps_speed[i++] = EUSART_Read_1Byte();
}