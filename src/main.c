/*
 * File:   main.c
 * Author: julio
 *
 * Created on 8. Oktober 2016, 14:57
 */


//#include <xc.h>//in globals
#include <stdlib.h>

#include "Globals.h"
#include "timeControl.h"
#include "switches.h"
#include "display.h"
#include "irq_manager.h"

static void
init_osc() {
    OSCCON = 0x68; // PLL OFF,4MHz INTOSC, OSC defined in ConfBytes
    // OSTS intosc; HFIOFR disabled; HFIOFS not0.5percent_acc; PLLR disabled; T1OSCR disabled; MFIOFR disabled; HFIOFL not2percent_acc; LFIOFR disabled;
    // TUN 0x0;
    OSCTUNE = 0x00;
}

static void
init_mcu() {
    /* EUSART pins */
    //TRISCbits.TRISC7 = 1; // RX
    //TRISCbits.TRISC6 = 0; // TX

    /* I2C interface */
    // TODO

    /* 7-SEG ports direction config */
    TRISA = 0x00;
    ANSELA = 0x00; // All GPIO

    /* PWM generator */
    TRISBbits.TRISB0 = 0;
    ANSELBbits.ANSB0 = 1; // Select CCP

    /* MUX channel selector outputs */
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    ANSELBbits.ANSB4 = 0; // Select GPIO
    ANSELBbits.ANSB5 = 0; // Select GPIO

    /* Switch inputs */
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB1 = 0; // Select GPIO
    ANSELBbits.ANSB2 = 0; // Select GPIO
    ANSELBbits.ANSB3 = 0; // Select GPIO
    sw_Enable_IOC();

    // Pull-ups already enabled by default
}

void main(void) {
    char c_min[2] = {'0', '0'};
    char c_hour[2] = {'0', '0'};

    enum {
        hourH, hourL, minH, minL
    };
    init_osc();
    init_mcu();
    init_tmr6(); // Initialize timer with interrupt every 200ms
    init_ccp();

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    while (1) {
#ifdef SIM_ON
        inc_uptime(60); /* Force increase time by 1 min on simulator */
#endif
        tp = gmtime(get_uptime());
        
        /* DIRTY Workaround to fix itoa output when number is only one cypher. */
        if(tp->tm_min < 10) {
            itoa(c_min, tp->tm_min, 10);
            c_min[1] = c_min[0];
            c_min[0] = '0';
        }
        else
            itoa(c_min, tp->tm_min, 10);
        
        if(tp->tm_hour < 10) {
            itoa(c_hour, tp->tm_hour, 10);
            c_hour[1] = c_min[0];
            c_hour[0] = '0';
        }
        else
            itoa(c_hour, tp->tm_hour, 10);
        /* END of DIRTY */    
        
        Display_Show(hourH, &c_hour[0]);
        Display_Show(hourL, &c_hour[1]);
        Display_Show(minH, &c_min[0]);
        Display_Show(minL, &c_min[1]);
    }

    INTERRUPT_PeripheralInterruptDisable();
    INTERRUPT_GlobalInterruptDisable();
    return;
}

