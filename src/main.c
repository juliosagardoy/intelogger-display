/* 
 * File: main.c  
 * Author: Julio Sagardoy
 * Comments: 
 * Revision history: 1
 */

#include "configBytes.h"
#include "Globals.h"
#include "irq_manager.h"
#include "timeControl.h"
#include "switches.h"
#include "pwm.h"
#include "display.h"
#include "usart.h"
#include "nmea.h"

static void
init_osc() {
    OSCCON = 0b01101000; /* 01101000 PLL OFF,4MHz INTOSC, OSC defined in ConfBytes */
 
    OSCTUNE = 0x00;
}

static void
init_mcu() {
    /* EUSART pins for GPS */
    TRISCbits.TRISC7 = 1; // RX
    TRISCbits.TRISC6 = 0; // TX

    /* I2C interface */
    //TODO SET both TRIS as INPUT yes as input

    /* 7-SEG ports direction config */
    TRISA = 0x00;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    LATA |= 0b111111;    // All on = all led off
    LATC |= 0b11;
    ANSELA = 0x00;  // All Digital
    
    /* PWM generator */
    ANSELBbits.ANSB0 = 0;   // Select Digital
    /* ...Done at init_ccp... */

    /* MUX channel selector outputs */
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    ANSELBbits.ANSB4 = 0; // Select Digital
    ANSELBbits.ANSB5 = 0; // Select Digital

    /* Switch inputs */
    OPTION_REGbits.nWPUEN = 0; /* Enable individual weak pull-ups */
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB1 = 0; // Select Digital
    ANSELBbits.ANSB2 = 0; // Select Digital
    ANSELBbits.ANSB3 = 0; // Select Digital
//    IOCBNbits.IOCBN1 = 1; /* Enable interrupt-on-change negative edge */
//    IOCBNbits.IOCBN2 = 1;
//    IOCBNbits.IOCBN3 = 1;
}

void main(void) {
    display_mode = 1;
    init_osc();
    init_mcu();
    init_display();
    init_tmr2();
    init_ccp();
    init_eusart();

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
  
    while (1) {
#ifdef SIM_ON
        TMR2_ISR();
#endif
#ifndef SIM_ON
        nmea_parser();
#endif
    }

    INTERRUPT_PeripheralInterruptDisable();
    INTERRUPT_GlobalInterruptDisable();
    return;
}
