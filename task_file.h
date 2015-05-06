/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  task_file.h                                                  *
*                                                                             *
* Description :  task_file.c interface file                                   *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 13-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TASK_FILE_H_
#define _TASK_FILE_H_

void TASK_FILE_init( void );
void TASK_FILE_dump_tasks( void );
void TASK_FILE_get_task( const U8 task_id, const U8* const task_data );
void TASK_FILE_close( void );

#endif /* _TASK_SET_H_ */
