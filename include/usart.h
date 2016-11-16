/* 
 * File: usart.c  
 * Author: Julio Sagardoy
 * Comments: 
 * Revision history: 2
 */

#ifndef USART_H
#define	USART_H

#include "Globals.h"

/**
  Section: Macro Declarations
 */
#define EUSART_TX_BUFFER_SIZE 16
#define EUSART_RX_BUFFER_SIZE 8

/**
  Section: Global Variables
 */

volatile uint8_t eusartTxHead;
volatile uint8_t eusartTxTail;
volatile uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
volatile uint8_t eusartTxBufferRemaining;

volatile uint8_t eusartRxHead;
volatile uint8_t eusartRxTail;
volatile uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;

/* USART */
void init_eusart();
uint8_t EUSART_Read_1Byte(void);
uint16_t EUSART_Read_2Bytes(void);
void EUSART_Write_1Byte(uint8_t);
void EUSART_Write_2Bytes(word);
void EUSART_WriteCString(const char*);
void EUSART_WriteString(char*);
void EUSART_PurgeBuffer(void);
void EUSART_Receive_ISR(void);
void EUSART_Transmit_ISR(void);

/* I2C */

#endif	/* USART_H */

