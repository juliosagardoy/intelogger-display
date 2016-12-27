/* 
 * File: display.c
 * Author: Julio Sagardoy
 * Comments: 7-segment display controller
 * Revision history: 1
 */

#ifndef LED_DECODER_H
#define	LED_DECODER_H

#ifdef	__cplusplus
extern "C" {
#endif

    void display_encode(const byte*);
    void display_digit(byte, byte*);

#ifdef	__cplusplus
}
#endif

#endif	/* LED_DECODER_H */

