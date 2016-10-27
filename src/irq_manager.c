#include "irq_manager.h"
//#include "usart.h"
//#include "ccp.h"
#include "timeControl.h"
#include "display.h"
#include "switches.h"

inline void INTERRUPT_GlobalInterruptEnable() { INTCONbits.GIE = 1; }
inline void INTERRUPT_GlobalInterruptDisable() { INTCONbits.GIE = 0; }
inline void INTERRUPT_PeripheralInterruptEnable() { 
	INTCONbits.PEIE = 1;
	INTCONbits.IOCIE = 1; /* Interrupt-on-change */
}
inline void INTERRUPT_PeripheralInterruptDisable() { INTCONbits.PEIE = 0; INTCONbits.IOCIE = 1; }

void interrupt INTERRUPT_InterruptManager()
{
	// interrupt handler
	if (PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1) {
//		EUSART_Receive_ISR();
	} else if (PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1) {
//		EUSART_Transmit_ISR();
	} else if (PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF == 1) {
		//PIR1bits.TMR1IF = 0;
	} else if (PIE3bits.CCP4IE == 1 && PIR3bits.CCP4IF == 1) {
		CCP4_ISR();
	} else if (PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1) {
//		TMR2_ISR();
	} else if (PIE3bits.TMR6IE == 1 && PIR3bits.TMR6IF == 1) {
		TMR6_ISR();
	} else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF1 == 1) {
		SW1_ISR();
	} else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF2 == 1) {
		SW2_ISR();
	} else if (INTCONbits.IOCIE == 1 && IOCBFbits.IOCBF3 == 1) {
		SW3_ISR();
    }
	else {
		//Unhandled Interrupt
	}
}
