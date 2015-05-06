/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  serial.h                                                     *
*                                                                             *
* Description :  Low Level Serial Routines interface file                     *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date    |Aut| Description                                                   *
* 10-03-05|JRO| Ported from Keil uVision/ARM development tools                *
*         |   |                                                               *
*                                                                             *
******************************************************************************/
#ifndef _SERIAL_H_
#define _SERIAL_H_

void SER_init( void );
U32 SER_putchar( const U32 ch );
U32 SER_getchar( void );

#endif /* _SERIAL_H_ */
