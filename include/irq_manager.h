/* 
 * File:   irq_manager.h
 * Author: julio
 *
 * Created on February 7, 2015, 10:02 AM
 */

#ifndef IRQ_MANAGER_H
#define	IRQ_MANAGER_H

#include "Globals.h"

inline void INTERRUPT_GlobalInterruptEnable(void);
inline void INTERRUPT_GlobalInterruptDisable(void);
inline void INTERRUPT_PeripheralInterruptEnable(void);
inline void INTERRUPT_PeripheralInterruptDisable(void);

void interrupt INTERRUPT_InterruptManager(void);

#endif	/* IRQ_MANAGER_H */

