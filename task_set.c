/******************************************************************************
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  task_set.c                                                   *
*                                                                             *
* Description :  This module is responsible for all task set operations       *                                                                             *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 12-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#if(0)
#include "globals.h"
#ifndef DEV_SYSTEM
/*lint -e85 has dimension 0 */
#include <stdio.h>
/*lint +e85 has dimension 0 */
#include <string.h>
#include <io.h>
#endif
#include "messages_out.h"
#include "task.h"
#include "task_set.h"





/******************************************************************************
*                            PRIVATE DATA TYPES								  *
******************************************************************************/


struct tcb_type
{                                 /* CONSTANT DATA                                 */
                                  /* =============                                 */
    const U8     id;              /* task id                                       */
    const U32    release;         /* task release time                             */
    const U32    duration;        /* task execution time                           */
    const U32    rel_dline;       /* relative task deadline                        */
    const U32    period;          /* task period                                   */
    const U8     priority;        /* priority level, one is highest                */
    const U8     preempt;         /* 1 if task can be pre-empted                   */
    const U8     spare_1;         /* spare constant data                           */
    const U8     spare_2;         /* spare constant data                           */
    const U8     spare_3;         /* spare constant data                           */
    const U8     spare_4;         /* spare constant data                           */
                                  /*                                               */
                                  /* DYNAMIC DATA                                  */
                                  /* ============                                  */
    task_state_type state;        /* state of task                                 */
    U32         urgency;          /* urgency value                                 */
    U32         time_left;             /* duration remaining                            */
    U32         time_taken;            /* actual time taken by task to complete         */
    U32         abs_dline;             /* absolute task deadline                        */
    U32         laxity;                /* laxity of task                                */
    U32         c_duration;            /* calculated duration                           */
    U16         task_util;             /* calculated task set utility                   */
    U8          pre_count;             /* count of pre-emptions                         */
    U8          e_priority;            /* enhanced priority                             */
    U8          period_multiplyer;     /* number of times period multiplied             */
    U32         per_doubles;           /* times period doubled                          */
    U32         dlines_missed;         /* deadlines missed by task                      */
    U32         dlines_met;            /* deadlines achieved by task                    */
    U32         net_value;             /* net value of task                             */
    U32         value;                 /* total value of task                           */
};


/* linked list of task control blocks (TCBs) */
struct tcb_list_type
{
	 struct tcb_list_type*		p;			/* pointer to previous tcb */
	 struct tcb_list_type*		n;			/* pointer to next tcb */
	 struct tcb*				tcb;	    /* this tcb */
};

#if(0)
static struct tcb_list_type task_set;

static U32 max_num_test_tasks;
static U32 num_test_tasks;



static FILE* task_data_file_ptr;

static const S8 *task_data_file_name = (S8*)"task_data_01.txt";





/******************************************************************************
*                        PRIVATE FUNCTIONS PROTOTYPES						  *
******************************************************************************/
static void open_task_data_file( const S8* file_name );



/******************************************************************************
*                              PUBLIC FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Function    : TASK_SET_init
*  Description : initialises task set object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TASK_SET_init( void )
{
    /* initialise task object      */


} /* TASK_SET_init() */


/******************************************************************************
*  Function    : TASK_SET_close
*  Description : nicely closed down the task set object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TASK_SET_close( void )
{

} /* TASK_SET_close() */


/******************************************************************************
*  Function    : TASK_SET_add_task
*  Description : adds a task to the task set
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TASK_SET_add_task( const U8 task_num )
{


} /* TASK_SET_add_task() */


/******************************************************************************
*                            PRIVATE FUNCTIONS								  *
******************************************************************************/



/******************************************************************************
*  Function    : get_task
*  Description : get task data from data file 
*  Parameters  : task_num - number of requested task
*  Returns     : none
******************************************************************************/
static void get_task( const U8 task_num )
{


}
#endif

#endif
