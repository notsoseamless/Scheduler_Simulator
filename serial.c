
/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*  NOTE: This file is adapted from part of the uVision/ARM development tools  *
*        Supplied with the uVision IDE                                        *
*        (c) KEIL ELEKTRONIK GmbH 2002-2004                                   *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  serial.c                                                     *
*                                                                             *
* Description :  Physical Layer Functionality                                 *
*                Low Level Serial Routines                                    *
*                Keil Development System, or PC (see in globals.h).           *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 10-03-05 |JRO| Ported from Keil uVision/ARM development tools               *
* 11-03-05 |JRO| changed C++ comments to please pclint.                       *
* 17-04-05 |JRO| Added screen control for development system                  *
* 20-04-05 |JRO| Added interrupt UART1 handler                                *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "serial.h"

#ifdef DEV_SYSTEM
/***************************************/
/* compile for Keil development system */
/***************************************/
#include "term.h"
#include "lpc21xx.h"

#define CR  0x0D                                /* carridge return    */

void ser1( void ) __attribute__ ((interrupt));  /* Generate Interrupt */


/******************************************************************************
*                              PUBLIC FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : SER_init
*  Description : Initialise serial interface
*  Parameters  : task id
*  Returns     : none
******************************************************************************/
void SER_init( void )
{
    volatile char dummy;

    PINSEL0 = 0x00050000;             /* Enable RxD1 and TxD1              */
    U1LCR   = 0x83;                   /* 8 bits, no Parity, 1 Stop bit     */
    U1DLL   = 97;                     /* 9600 Baud Rate @ 15MHz VPB Clock  */
    U1LCR   = 0x03;                   /* DLAB = 0                          */

    U1IER = 0;                        /* Disable UART1 Interrupts          */

    VICVectAddr1 = (U32)ser1;         /* define Interrupt handler          */

    VICVectCntl1 = 0x20 | 7;          /* UART1 Interrupt                   */
    VICIntEnable = 1 << 7;            /* Enable UART1 Interrupt            */

    dummy = U1IIR;      /* Read IrqID - Required to Get Interrupts Started */
    U1IER = 1;                        /* Enable UART1 RX Interrupt         */

} /* SER_init() */


/******************************************************************************
*  Function    : SER_putchar
*  Description : Write character to serial port
*  Notes       : Adapted from uVision/ARM development tools
*  Parameters  : character to write
*  Returns     : ch
******************************************************************************/
U32 SER_putchar( const U32 ch )
{
    if( ch is '\n' )
    {
        while (!(U1LSR & 0x20));
        U1THR = CR;                          /* output CR */
    }

    while (!(U1LSR & 0x20));

    return (U1THR = ch);

} /* SER_putchar() */

/******************************************************************************
*  Function    : SER_getchar
*  Description : Read character from Serial Port
*  Notes       : Adapted from uVision/ARM development tools
*  Parameters  : none
*  Returns     : none
******************************************************************************/
U32 SER_getchar( void )
{
    while( !(U1LSR & 0x01) );

    return( U1RBR );

} /* SER_getchar() */


/******************************************************************************
*  Function    : ser1
*  Description : UART1 Interrupt handler
*  Notes       : Adapted from uVision/ARM development tools
*  Parameters  : None
*  Returns     : None
******************************************************************************/
void ser1( void )
{
    volatile char dummy;

    TRM_call();          /* call the terminal                               */

    VICVectAddr = 0;     /* Acknowledge Interrupt                           */

    dummy       = U1IIR; /* Read IrqID - Required to Get Interrupts Started */

} /* ser1() */



#else
/********************************/
/* compile for PC based  system */
/********************************/


/******************************************************************************
*  Function    : SER_getchar
*  Description : dummy
******************************************************************************/
U32 SER_getchar( void ){return(0);}

/******************************************************************************
*  Function    : SER_putchar
*  Description : dummy
******************************************************************************/
U32 SER_putchar( const U32 ch ){return(0);}


#endif
