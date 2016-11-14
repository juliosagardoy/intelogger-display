/* 
 * File:   led_decoder.h
 * Author: julio
 *
 * Created on 9. Oktober 2016, 18:28
 */

#ifndef LED_DECODER_H
#define	LED_DECODER_H

#ifdef	__cplusplus
extern "C" {
#endif

    void Display_encode(const char*);
    void display_digit(byte, const char*);

#ifdef	__cplusplus
}
#endif

#endif	/* LED_DECODER_H */

