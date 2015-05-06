/******************************************************************************
*                (c) John Oldman 2005                                         *
*                Chingford, London E4 7DT, England                            *
*                                                                             *
*                Project:                                                     *
*                                                                             *
*                An Investigation into an Adaptive Scheduler                  *
*                for Overloaded Real-Time Systems                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  rt_clock.c                                                   *
*                                                                             *
* Description :  Interface file for rt_clock.h                                *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 03-06-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _RT_CLOCK_H
#define _RT_CLOCK_H

void CLK_init( void );
U32 CLK_get_real_time( void );
U32 CLK_get_diff_time( const U32 t1, const U32 t2 );
void CLK_start_timer( const U32 pre_load );
U32 CLK_end_timer( void );
U32 CLK_clocks_per_second( void );


#endif
