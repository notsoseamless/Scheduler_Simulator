/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  timers.h                                                     *
*                                                                             *
* Description :  header file for timers.c                                     *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 11-03-05 |JRO| Created                                                      *
* 17-04-05 |JRO| Added screen control for development system                  *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TIMERS_H_
#define _TIMERS_H_

void TIM_init( void );
void TIM_start_simulation( void );
void TIM_continue_simulation( void );

#ifdef DEV_SYSTEM    /* CONDITIONAL COMPILE - Keil Development System */
void TIM_start( void );
#endif  /* DEV_SYSTEM */

#endif /* _TIMERS_H_ */
