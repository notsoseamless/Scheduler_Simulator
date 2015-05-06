/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :  SCHEDULER SIMULATOR                                          *
*                                                                             *
* File Name   :  task_set.h                                                   *
*                                                                             *
* Description :  task_set.c interface file                                    *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 12-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TASK_SET_H_
#define _TASK_SET_H_

void TASK_SET_init( void );
void TASK_SET_close( void );
void TASK_SET_add_task( const U8 task_num );

#endif /* _TASK_SET_H_ */
