/* 
 * File: pwm.c 
 * Author: Julio Sagardoy
 * Comments: CCP PWM code
 * Revision history: 1
 */

#include "pwm.h"


/**
 * Init timer4 for CCP4 module (PWM)
 */
static void
init_tmr4() {
    /* Configure overflow:cy=1MHz; Tcy=1us; incTMR=Tcy/2
     * 1us * 4presc = 4us/tick 
     * 4us/tick * 256 = 1,024 ms to overflow
     */
    T4CONbits.T4CKPS = 0b01; /* Prescaler @ 1:4 */
    PIE3bits.TMR4IE = 1;
    TMR4 = 0;
    T4CONbits.TMR4ON = 1;
}

/**
 * CCP module
 */
void
init_ccp() {
    TRISBbits.TRISB0 = 1; /* Temporarily disable output to avoid glitches */
    CCPTMRS0bits.C4TSEL = 0b01; /* CCP4 is based off Timer4 in PWM mode */
    PR4 = 250; /* PWM Period = TMR4 * 250 = 0.001 s = 1kHz */

    /* DC table
     * 0b101100 0b111111 - min brightness (DC 191)
     * 0b001100 0b000000 - max brightness (DC 0)
     * */
    
    /* Select duty cycle 2-MSb (10) and PWM mode (11xx) */
    CCP4CON = 0b101100;
    
    /* 2 LSb in CCP4CON */
    CCPR4L = 0b111111; /* duty cycle 6-LSb */
    init_tmr4();
    
    //PIE3bits.CCP4IE = 1;
    TRISBbits.TRISB0 = 0;
}

/**
 * Adjust PWM DC (brightness)
 * @return current CCP DC setting
 */
byte Toggle_Brightess(void) {
    return 0;
}

void TMR4_ISR() {
    PIR3bits.TMR4IF = 0;

}

void CCP4_ISR() {
    PIR3bits.CCP4IF = 0;
}