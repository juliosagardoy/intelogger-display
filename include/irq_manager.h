/* 
 * File: irq_manager.h  
 * Author: Julio Sagardoy
 * Comments: Interruptions
 * Revision history: 1
 */

#ifndef IRQ_MANAGER_H
#define	IRQ_MANAGER_H

#include "Globals.h"
#include "usart.h"
#include "pwm.h"
#include "timeControl.h"
#include "display.h"
#include "switches.h"

inline void INTERRUPT_GlobalInterruptEnable(void);
inline void INTERRUPT_GlobalInterruptDisable(void);
inline void INTERRUPT_PeripheralInterruptEnable(void);
inline void INTERRUPT_PeripheralInterruptDisable(void);

void interrupt INTERRUPT_InterruptManager(void);

#endif	/* IRQ_MANAGER_H */

