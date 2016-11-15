/* 
 * File:   switches.c
 * Author: julio
 *
 * Created on 
 */

#include "timeControl.h"

void SW1_ISR()	/* "H" sw pushed */
{
	/* When "H" is pushed, temporarily disable interrupts from the other sw */
	IOCBNbits.IOCBN2 = 0;
	IOCBNbits.IOCBN3 = 0;
	/* Check if "R" is also pushed, meaning "change brightness" */
	if(!PORTBbits.RB3)
		Toggle_Brightess();
	else
		incr_uptime(3600);	/* Increase time an hour */
		
	/* Reenable other sw interrupts */
	IOCBNbits.IOCBN2 = 1;
	IOCBNbits.IOCBN3 = 1;

	IOCBFbits.IOCBF1 = 0;
}

void SW2_ISR()	/* "M" sw pushed */
{
	/* When "M" is pushed,t emporarily disable interrupts from the other sw */
	IOCBNbits.IOCBN1 = 0;
	IOCBNbits.IOCBN3 = 0;
	incr_uptime(60);		/* Increase time a minute */
	/* Reenable other sw interrupts */
	IOCBNbits.IOCBN1 = 1;
	IOCBNbits.IOCBN3 = 1;
	
	IOCBFbits.IOCBF2 = 0;
}

void SW3_ISR()	/* "R" sw pushed */
{
	/* Change mode */
}