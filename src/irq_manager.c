/* 
 * File: irq_manager.c  
 * Author: Julio Sagardoy
 * Comments: Interruptions
 * Revision history: 1
 */

#include "irq_manager.h"


inline void INTERRUPT_GlobalInterruptEnable()
{
    INTCONbits.GIE = 1;
}

inline void INTERRUPT_GlobalInterruptDisable()
{
    INTCONbits.GIE = 0;
}

inline void INTERRUPT_PeripheralInterruptEnable()
{
    IOCBFbits.IOCBF1 = 0;
    IOCBFbits.IOCBF2 = 0;
    IOCBFbits.IOCBF3 = 0;
    INTCONbits.IOCIE = 1; /* Interrupt-on-change */
    INTCONbits.PEIE = 1;   
}

inline void INTERRUPT_PeripheralInterruptDisable()
{
    INTCONbits.PEIE = 0;
    INTCONbits.IOCIE = 0;
}

void interrupt INTERRUPT_InterruptManager()
{
    // interrupt handler
    if (PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
    {
        EUSART_Receive_ISR();
    }
    else if (PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1)
    {
        EUSART_Transmit_ISR();
    }
    else if (PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF == 1)
    {
        PIR1bits.TMR1IF = 0;
    }
    else if (PIE3bits.CCP4IE == 1 && PIR3bits.CCP4IF == 1)
    {
        CCP4_ISR();
    }
    else if (PIE3bits.TMR6IE == 1 && PIR3bits.TMR6IF == 1)
    {
        TMR6_ISR();
    }
    else if (PIE3bits.TMR4IE == 1 && PIR3bits.TMR4IF == 1)
    {
        TMR4_ISR();
    }
    else if (PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1)
    {
        TMR2_ISR();
    }
    else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF1 == 1)
    {
        SW1_ISR();
    }
    else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF2 == 1)
    {
        SW2_ISR();
    }
    else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF3 == 1)
    {
        SW3_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}
