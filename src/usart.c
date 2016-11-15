/**
 * usart.c
 * Ver:    2
 *
*/

#include "usart.h"

void init_eusart()
{
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    PIE1bits.TXIE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDEN disabled; WUE disabled; RCIDL idle; ABDOVF no_overflow; SCKP async_noninverted_sync_fallingedge; BRG16 16bit_generator;
    BAUDCON = 0x48;

    // ADDEN disabled; RX9 8-bit; RX9D 0x0; FERR no_error; CREN enabled; SPEN enabled; SREN disabled; OERR no_error;
    RCSTA = 0x90;

    // CSRC slave_mode; TRMT TSR_empty; TXEN enabled; BRGH hi_speed; SYNC asynchronous; SENDB sync_break_complete; TX9D 0x0; TX9 8-bit;
    TXSTA = 0x26;

    // Baud Rate = 115200; SPBRGL 34;
    //SPBRGL = 0x22;
    // Baud Rate = 57600; SPBRGL 16;
    SPBRGL = 16;
    
    // Baud Rate = 115200; SPBRGH 0;
    SPBRGH = 0x00;
    

    // initializing the driver state
    eusartTxHead = 0;
    eusartTxTail = 0;
    eusartTxBufferRemaining = sizeof(eusartTxBuffer);

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;

    EUSART_PurgeBuffer();
            
    // enable receive interrupt
    PIE1bits.RCIE = 1;
}

/*
 * Reads 1 Byte from buffer and advances
*/
uint8_t EUSART_Read_1Byte(void)
{
    uint8_t readValue  = 0;

    while(0 == eusartRxCount)
    {
    }

    PIE1bits.RCIE = 0;

    readValue = eusartRxBuffer[eusartRxTail++];
    if(sizeof(eusartRxBuffer) <= eusartRxTail)
    {
        eusartRxTail = 0;
    }
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

uint16_t EUSART_Read_2Bytes(void)
{
	uint16_t readValue = 0;
	readValue |= EUSART_Read_1Byte();
	readValue << 8;
	readValue |= EUSART_Read_1Byte();
	
	return readValue;
}

void EUSART_Write_1Byte(byte txData)
{
    while(0 == eusartTxBufferRemaining)
    {
    }

    if(0 == PIE1bits.TXIE)
    {
        TXREG = txData;
    }
    else
    {
        PIE1bits.TXIE = 0;
        eusartTxBuffer[eusartTxHead++] = txData;
        if(sizeof(eusartTxBuffer) <= eusartTxHead)
        {
            eusartTxHead = 0;
        }
        eusartTxBufferRemaining--;
    }
    PIE1bits.TXIE = 1;
}

void EUSART_Write_2Bytes(word txData)
{
    EUSART_Write_1Byte((byte)txData);
    EUSART_Write_1Byte((byte)txData << 8);
}

void EUSART_WriteCString(const char* str)
{
    while ( *str )
    {   
        EUSART_Write_1Byte( *str++ );
    }
}

void EUSART_WriteString(char* str)
{
    while ( *str )
    {
        EUSART_Write_1Byte( *str++ );
    }
}

void EUSART_PurgeBuffer(void)
{
//    memset(eusartTxBuffer, 0, sizeof (eusartTxBuffer));
 //   memset(eusartRxBuffer, 0, sizeof (eusartRxBuffer));
}

/**
 *  ISRs
 */
void EUSART_Receive_ISR(void)
{
    // Check for EUSART error - restart
    if(1 == RCSTAbits.OERR)
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    // buffer overruns are ignored
    eusartRxBuffer[eusartRxHead++] = RCREG;

    if(sizeof(eusartRxBuffer) <= eusartRxHead)
    {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}

void EUSART_Transmit_ISR(void)
{

    // add your EUSART interrupt custom code
    if(sizeof(eusartTxBuffer) > eusartTxBufferRemaining)
    {
        TXREG = eusartTxBuffer[eusartTxTail++];
        if(sizeof(eusartTxBuffer) <= eusartTxTail)
        {
            eusartTxTail = 0;
        }
        eusartTxBufferRemaining++;
    }
    else
    {
        PIE1bits.TXIE = 0;
    }
}