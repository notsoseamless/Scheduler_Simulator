/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  date.h                                                       *
*                                                                             *
* Description :  date.c interface file                                        *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 23-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _DATE_H_
#define _DATE_H_

void DATE_get_date( S8* date_str );
void DATE_get_time( S8* time_str );
void DATE_stamp_start( void );
U32 DATE_get_duration( void );

#endif /* _DATE_H_ */
