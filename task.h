/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  task.h                                                       *
*                                                                             *
* Description :  task.c interface file                                        *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 12-03-05 |JRO| Created                                                      *
* 04-04-05 |JRO| Added pre-emption count to task_type                         *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
* 17-04-05 |JRO| Added screen control for development system                  *
* 18-06-05 |JRO| Added pre-caluclated utility * 1000 member                   *
* 26-06-05 |JRO| Added data for ALG_ADAP_03 & ALG_ADAP_04 scheduler           *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TASK_H_
#define _TASK_H_

/* macros to redefined constant data, using spare members */
#define MUF_CRITICAL(x)        tasks[(x)]->spare_1 /* MUF tasks: 37-40 */																  
#define MUF_URGENCY(x)         tasks[(x)]->spare_4																  
#define ELASTIC_COEFFICIENT(x) tasks[(x)]->spare_1																  
#define PERIOD_FLEXIBLE(x)     tasks[(x)]->spare_2													  
#define SKIP_GAP(x)            tasks[(x)]->spare_1 /* Skip tasks: 41-44 */													  
#define NOT_SKIPPED(x)         tasks[(x)]->spare_2 /* NOT YET SET UP */													  
#define TOTAL_SKIPS(x)         tasks[(x)]->spare_2 /* NOT YET SET UP */													  

extern U8 running_task_id;

#ifndef DEV_SYSTEM
/* stored task data array */
struct stored_task_data_type
{
    const U8   id;        /* task id                         */
    const U32  release;   /* task release time               */
    const U32  duration;  /* task execution time             */
    const U32  rel_dline; /* relative task deadline          */
    const U32  period;    /* task period (>0 if periodic)    */
    const U8   priority;  /* priority level, one is highest  */
    const U8   preempt;   /* 1 if task is pre-emptable       */
    const U32  spare_1;   /* spare constant data             */
    const U32  spare_2;   /* spare constant data             */
    const U32  spare_3;   /* spare constant data             */
    const U32  spare_4;   /* spare constant data             */
};
#endif /* DEV_SYSTEM */



/* task states */
typedef enum
{
   TSK_SLEEP,    /* not a scheduled task          */
   TSK_WAITING,  /* waiting load time             */
   TSK_REMOVED,  /* task removed by scheduler     */
   TSK_IDLE,     /* task is idle                  */
   TSK_SKIPPED,  /* task has been skipped         */
   TSK_PREMPT,   /* task has been per-empted      */
   TSK_READY,    /* task is ready to execute      */
   TSK_RUN       /* task is running               */
}task_state_type;


/* individual task data type */
struct task_type
{                                /* CONSTANTS                       */
                                 /* =========                       */
    U8          id;              /* task id                         */
    U32         release;         /* task release time               */
    U32         duration;        /* task execution time             */
    U32         rel_dline;       /* relative task deadline          */
    U32         period;          /* task period                     */
    U8          priority;        /* priority level, one is highest  */
    U8          preempt;         /* true if task can be pre-empted  */
    U32         spare_1;         /* spare constant data             */
    U32         spare_2;         /* spare constant data             */
    U32         spare_3;         /* spare constant data             */
    U32         spare_4;         /* spare constant data             */
                                       /*                                               */
                                       /* CALCULATED                                    */
                                       /* ==========                                    */
    U32         time_left;             /* duration remaining                            */
    U32         time_taken;            /* actual time taken by task to complete         */
    U32         abs_dline;             /* absolute task deadline                        */
    U32         laxity;                /* laxity of task                                */
    U32         c_duration;            /* calculated duration                           */
    U16         task_util;             /* calculated task set utility                   */
    U8          pre_count;             /* count of pre-emptions                         */
    U8          e_priority;            /* enhanced priority                             */
    U8          period_multiplyer;     /* number of times period multiplied             */
    task_state_type state;             /* state of task                                 */
    U8          q0_prev;               /* pointer to queue 0 previous task              */
    U8          q0_next;               /* pointer to queue 0 next task                  */
    U8          q1_prev;               /* pointer to queue 1 previous task              */
    U8          q1_next;               /* pointer to queue 1 next task                  */
    U8          q2_prev;               /* pointer to queue 2 previous task              */
    U8          q2_next;               /* pointer to queue 2 next task                  */
                                       /*                                               */
                                       /* REPORT PARAMETERS                             */
                                       /* =================                             */
    U32         per_doubles;           /* times period doubled                          */
    U32         dlines_missed;         /* deadlines missed by task                      */
    U32         dlines_met;            /* deadlines achieved by task                    */
    U32         net_value;             /* net value of task                             */
    U32         value;                 /* total value of task                           */
};



void TSK_init( void );
void TSK_close( void );
void TSK_soft_reset( void );
struct task_type *TSK_load_task( const U8 task_id, const U8 test_task_id );
void TSK_add_task( const U8 test_task_id, struct task_type* task );
void TSK_start_of_test( void );
void TSK_run_task_cycle( const U32 timeval );
void TSK_end_of_test( void );
void TSK_task_report( void );
void TSK_task_results( void );
void TSK_task_completed( const U8 task, const U32 timeval );

#endif /* _TASK_H_ */
