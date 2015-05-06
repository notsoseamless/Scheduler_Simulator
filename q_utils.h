/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :  SCHEDULER SIMULATOR    					     			  *
*                                                                             *
* File Name   :  q_util.h                                                     *
*                                                                             *
* Description :  Interface for q_utils.c                                      *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 27-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _Q_UTILS_H_
#define _Q_UTILS_H_


/* queue functions */
typedef enum
{
	Q_PRI,		/* priority          */
	Q_EDF,		/* earliest deadline */
	Q_LLF,		/* leat laxity first */
	Q_MAX
}q_order_type;	 


/* task queue type */
struct task_q_type
{
	q_order_type	q_order;	/* order of queue */
};


void QUTIL_init( void );
void QUTIL_close( void );
void QUTIL_push( void );

#endif
