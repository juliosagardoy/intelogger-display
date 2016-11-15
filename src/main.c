/*
 * File:   main.c
 * Author: julio
 *
 * Created on 8. Oktober 2016, 14:57
 */


//#include <xc.h>//in globals

#include <string.h>
#include "Globals.h"
#include "timeControl.h"
#include "switches.h"
#include "display.h"
#include "irq_manager.h"
#include "usart.h"

static void
init_osc() {
    OSCCON = 0x68; /* 01101000 PLL OFF,4MHz INTOSC, OSC defined in ConfBytes
    * OSTS intosc; HFIOFR disabled; HFIOFS not0.5percent_acc; PLLR disabled;
    * T1OSCR disabled; MFIOFR disabled; HFIOFL not2percent_acc; LFIOFR disabled;
    * TUN 0x0;
    */
    OSCTUNE = 0x00;
}

static void
init_mcu() {
    /* EUSART pins for GPS */
    TRISCbits.TRISC7 = 1; // RX
    TRISCbits.TRISC6 = 0; // TX

    /* I2C interface */
    // TODO

    /* 7-SEG ports direction config */
    TRISA = 0x00;
    ANSELA = 0x00; // All GPIO

    /* PWM generator */
    //  TRISBbits.TRISB0 = 0;
    ANSELBbits.ANSB0 = 1; // Select CCP

    /* MUX channel selector outputs */
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    ANSELBbits.ANSB4 = 0; // Select GPIO
    ANSELBbits.ANSB5 = 0; // Select GPIO

    /* Switch inputs */
    OPTION_REGbits.nWPUEN = 0; /* Enable individual weak pull-ups */
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB1 = 0; // Select GPIO
    ANSELBbits.ANSB2 = 0; // Select GPIO
    ANSELBbits.ANSB3 = 0; // Select GPIO
    IOCBNbits.IOCBN1 = 1; /* Enable interrupt-on-change negative edge */
    IOCBNbits.IOCBN2 = 1;
    IOCBNbits.IOCBN3 = 1;
}

void main(void) {

    init_osc();
    init_mcu();
    init_tmr6();
    init_ccp();
    init_eusart();

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    memset(gps_heading, NULL, 5);
    memset(gps_speed, NULL, 4);

    while (1) {
#ifdef SIM_ON
        TMR6_ISR();
#endif
#ifndef SIM_ON
        /* Refresh GPS */
        if (EUSART_Read_1Byte() != '$') /* Found GPS start bit */ {
            /* Read NMEA sentence header (5 chars string) */
            char nmea_header[5];
            memset(nmea_header, 0x00, 5);

            for (int i = 0; i < 5; i++) {
                nmea_header[i] == EUSART_Read_1Byte();
            }
            if (strcmp(nmea_header, "GPGGA") == 0) {
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

            /* Analyze header and extract data */
            if (strcmp(nmea_header, "GPVTG") == 0) {
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
        }
#endif
    }

    INTERRUPT_PeripheralInterruptDisable();
    INTERRUPT_GlobalInterruptDisable();
    return;
}
