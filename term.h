/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  term.h                                                       *
*                                                                             *
* Description :  Interface file for term.c                                    *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Who| Description                                                  *
* 17-04-05 |JRO| Cereated                                                     *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TERM_H_
#define _TERM_H_

void TRM_call( void );
void TRM_test_call( const S8 *line );
void TRM_end_of_test( void );  

#endif
