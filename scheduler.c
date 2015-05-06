/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  scheduler.c                                                  *
*                                                                             *
* Description :  scheduler                                                    *
*                conditionally compiles for either Keil Development           *
*                System, or PC.                                               *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 12-03-05 |JRO| Created                                                      *
* 25-03-05 |JRO| Added idle_q_insert() because idle queue needed different    *
*          |   | algorithm to ready queue.                                    *
* 29-03-05 |JRO| Removed idle_q_insert(), by adding functionality to          *
*          |   | q_insert().                                                  *
* 04-04-05 |JRO| Added pre-emption counting to schedule_tasks()               *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
* 17-04-05 |JRO| Added screen control for development system                  *
* 20-04-05 |JRO| Added RR & CYC scheduler                                     *
* 22-05-05 |JRO| Added MMUF scheduler                                         *
* 25-06-05 |JRO| Added ALG_ADAP_01 & ALG_ADAP_02 scheduler                    *
* 26-06-05 |JRO| Added ALG_ADAP_03 & ALG_ADAP_04 scheduler                    *
* 15-07-05 |JRO| Added ALG_ADAP_05 & ALG_ADAP_06 scheduler                    *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "messages_out.h"
#include "task.h"
#include "test.h"
#include "utils.h"		   /* general utilities        */
#include "rt_clock.h"	   /* real time clock timer    */
#include "scheduler.h"     /* interface file           */

/* maximum EDF task set utility x 1000 (Liu and Layland 1973) */
static const U16 MAX_EDF_UTIL = (U16)1000;

/* scheduling algorithm */
static algorithm_type algorithm;

/* local array of pointers to tasks, defined in task.c */
extern struct task_type *tasks[NUM_OF_TASKS];

/* local pointer to ready header */
static struct task_type readys;
static struct task_type *ready_tasks;

/* local pointer to idle header */
static struct task_type idles;
static struct task_type *idle_tasks;

/* local pointer to waiting header */
static struct task_type waits;
static struct task_type *waiting_tasks;

/* local pointer to skipped header */
static struct task_type skippeds;
static struct task_type *skipped_tasks;

/* local pointer to removed header */
static struct task_type removeds;
static struct task_type *removed_tasks;

/* local pointer to doubled periods header */
static struct task_type doubleds;
static struct task_type *doubled_periods;

/* local pointer to LST header */
static struct task_type lsts;
static struct task_type *lst_tasks;

/* private function prototypes */
static void process_waiting_tasks( const U32 timeval );
static void process_completed_tasks( const U32 timeval );
static void process_removed_tasks( const U32 timeval );
static void process_doubled_periods( void );
static U8 doubled_queue_has_task( const U16 utility );
static void process_skipped_tasks( const U32 timeval );
static void process_idle_tasks( const U32 timeval );
static void process_ready_tasks( const U32 timeval );
static void schedule_adapive_one_algorithm( const U32 timeval );
static void schedule_adapive_three_algorithm( void );
static void schedule_adapive_five_algorithm( const U32 timeval );
static void schedule_adapive_six_algorithm( const U32 timeval );
static void schedule_adapive_seven_algorithm( const U32 timeval );
static void schedule_rr_algorithm( void );
static void schedule_cyc_algorithm( void );
static void schedule_rm_algorithm( void );
static void schedule_drm_algorithm( const U32 timeval );
static void schedule_irm_algorithm( void );
static void schedule_muf_algorithm( const U32 timeval );
static void schedule_mmuf_algorithm( const U32 timeval );
static void schedule_mmmuf_algorithm( const U32 timeval );
static void schedule_edf_algorithm( void );
static void schedule_spt_algorithm( void );
static void schedule_llf_algorithm( const U32 timeval );
static void schedule_mllf_algorithm( const U32 timeval );
static void schedule_skip_rto_algorithm( const U32 timeval );
#if(0)
static void schedule_d_star_algorithm( const U32 timeval );
#endif
static void preempt_task( const U8 task );
static void dispatch_next_task( void );
static void skip_first_task( const U32 timeval );

static void q0_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm );

static void q1_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm );

static void q2_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm );

static U8 q_extract_first( struct task_type *header );
static void q0_extract( const U8 task );
static void q1_extract( const U8 task, struct task_type *header );
static void q2_extract( const U8 task, struct task_type *header );

static boolean q_empty( const U8 queue, const struct task_type *header );
static U8 q_first_id( const U8 queue, const struct task_type *header );
static U8 q_second_id( const U8 queue, const struct task_type *header );
static void calculate_all_laxities( const U32 timeval );
static U32 calculate_laxity( const U8 task, const U32 timeval );
static void calculate_all_muf_urgencies( void );
static void calculate_muf_urgency( const U8 task );
static U8 get_lowest_priority_active_task( void );
static U8 get_active_task_priority( void );
static void calculate_active_task_priority( const U8 task );
static U8 task_skippable( const U8 task );
static boolean can_meet_deadline( const U8 task, const U32 timeval );
static void decrement_not_skipped_count( const U8 task );
static U16 calculate_all_task_utilities( void );
static U16 calculate_task_utility( const U8 task );
static U16 get_task_set_utility( void );
static U8 removed_queue_has_task_with_lower_utility( const U16 util );
static U8 double_task_period( const U8 task );
static void half_task_period( const U8 task );
#if(0)
static void q_dump( const struct task_type *header );
static void q1_dump( const struct task_type *header );
#ifdef DEBUG
    static void task_params( void );
#endif
#endif

/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/



/******************************************************************************
*  Description : initialises scheduler object
******************************************************************************/
void SCH_init( void )
{
    U8 index;

    /* initialise local tasks array */
    for( index=ZERO_CPV; index<NUM_OF_TASKS; index++ )
    {
/*        tasks[index] = TSK_load_task( index, ZERO_CPV );  */
    }

    /* dereference queue pointers */
    ready_tasks     = &readys;
    idle_tasks      = &idles;
    skipped_tasks   = &skippeds;
    lst_tasks       = &lsts;
    removed_tasks   = &removeds;
    waiting_tasks   = &waits;
    doubled_periods = &doubleds;
} 

/******************************************************************************
*  Description : performs a soft initialisation of scheduler object
******************************************************************************/
void SCH_soft_reset( void )
{
    U8 index;

    MESS_OUT_message( (S8*)"SCH_soft_reset()", MESSAGE_OP_DEBUG );

    /* initialise local tasks array */
    for( index=ZERO_CPV; index<NUM_OF_TASKS; index++ )
    {
 /*       tasks[index] = TSK_load_task( index, ZERO_CPV );	*/
    }


/*    MESS_OUT_message_1( (S8*)"SCH_soft_reset(): task 1 time_taken = ",
                        tasks[ONE_CPV]->time_taken, 
                        MESSAGE_OP_DEBUG );	  */
    
    /* reset the queue pointers */
    SCH_reset_queues();
} 

/******************************************************************************
*  Description : Resets all queues
******************************************************************************/
void SCH_reset_queues( void )
{
    U8 index;

    MESS_OUT_message( (S8*)"SCH_reset_queues()", MESSAGE_OP_DEBUG );

    /* reset task pointers */
    for( index=1; index<NUM_OF_TASKS; index++ )
    {
        tasks[index]->q0_prev = ZERO_CPV;
        tasks[index]->q1_prev = ZERO_CPV;
        tasks[index]->q2_prev = ZERO_CPV;
        tasks[index]->q0_next = ZERO_CPV;
        tasks[index]->q1_next = ZERO_CPV;
        tasks[index]->q2_next = ZERO_CPV;
    }
    /* reset header pointers */
    ready_tasks->q0_prev            = ZERO_CPV;
    ready_tasks->q0_next            = ZERO_CPV;
    ready_tasks->q1_prev            = ZERO_CPV;
    ready_tasks->q1_next            = ZERO_CPV;
    ready_tasks->q2_prev            = ZERO_CPV;
    ready_tasks->q2_next            = ZERO_CPV;

    idle_tasks->q0_prev             = ZERO_CPV;
    idle_tasks->q0_next             = ZERO_CPV;
    idle_tasks->q1_prev             = ZERO_CPV;
    idle_tasks->q1_next             = ZERO_CPV;
    idle_tasks->q2_prev             = ZERO_CPV;
    idle_tasks->q2_next             = ZERO_CPV;

    skipped_tasks->q0_prev          = ZERO_CPV;
    skipped_tasks->q0_next          = ZERO_CPV;
    skipped_tasks->q1_prev          = ZERO_CPV;
    skipped_tasks->q1_next          = ZERO_CPV;
    skipped_tasks->q2_prev          = ZERO_CPV;
    skipped_tasks->q2_next          = ZERO_CPV;

    lst_tasks->q0_prev              = ZERO_CPV;
    lst_tasks->q0_next              = ZERO_CPV;
    lst_tasks->q1_prev              = ZERO_CPV;
    lst_tasks->q1_next              = ZERO_CPV;
    lst_tasks->q2_prev              = ZERO_CPV;
    lst_tasks->q2_next              = ZERO_CPV;

    removed_tasks->q0_prev          = ZERO_CPV;
    removed_tasks->q0_next          = ZERO_CPV;
    removed_tasks->q1_prev          = ZERO_CPV;
    removed_tasks->q1_next          = ZERO_CPV;
    removed_tasks->q2_prev          = ZERO_CPV;
    removed_tasks->q2_next          = ZERO_CPV;

    waiting_tasks->q0_prev          = ZERO_CPV;
    waiting_tasks->q0_next          = ZERO_CPV;
    waiting_tasks->q1_prev          = ZERO_CPV;
    waiting_tasks->q1_next          = ZERO_CPV;
    waiting_tasks->q2_prev          = ZERO_CPV;
    waiting_tasks->q2_next          = ZERO_CPV;

    doubled_periods->q0_prev        = ZERO_CPV;
    doubled_periods->q0_next        = ZERO_CPV;
    doubled_periods->q1_prev        = ZERO_CPV;
    doubled_periods->q1_next        = ZERO_CPV;
    doubled_periods->q2_prev        = ZERO_CPV;
    doubled_periods->q2_next        = ZERO_CPV;
} 

/******************************************************************************
*  Description : Inserts task_id pointer in tasks[] in idle queue in deadline
*                order.
*                Tasks are ready for process_idle_tasks() for transfer to the
*                ready queue unless 'release' has a value.
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
void SCH_add_task( const U8 task_id, const U8 test_task_id )
{
    MESS_OUT_message_2( (S8*)"SCH_add_task(): Task ",
                        task_id,
                        (S8*)" loaded with id = ",
                        tasks[task_id]->id,
                        MESSAGE_OP_DEBUG );
    
    /* load test_task[] pointer into task[] */
    #if(0)
    tasks[task_id] = TSK_load_task( task_id, test_task_id );  
	#else
   	TSK_add_task( test_task_id, tasks[task_id] );	 
	#endif

    if( tasks[task_id]->id > ZERO_CPV )
    {
        /* initialise calculated utility with estimated */
        tasks[task_id]->task_util = tasks[task_id]->task_util;

        /* initialised calculated duration with estimated */
        tasks[task_id]->c_duration = tasks[task_id]->duration;

        if( tasks[task_id]->release > 0 )
        {
            /* set state */
            tasks[task_id]->state = TSK_WAITING;

            /* set deadline for time in wait queue */
            tasks[task_id]->abs_dline = tasks[task_id]->release;

            /* queue new idle task in deadline order */
            q0_insert( task_id, waiting_tasks, ALG_EDF );
        }
        else
        {
            /* set state */
            tasks[task_id]->state = TSK_IDLE;

            /* queue new idle task in deadline order */
            q0_insert( task_id, idle_tasks, ALG_EDF );
        }

        /* set not-skipped count */
        NOT_SKIPPED(task_id) = (U8)SKIP_GAP(task_id);
    }

    #ifdef DEBUG
        OPR_0_message( (S8*)"Ready tasks\n" );
        q_dump( ready_tasks );
        OPR_0_message( (S8*)"Idle tasks\n" );
        q_dump( idle_tasks );
        task_params();
    #endif
}

/******************************************************************************
*  Description : Called by timing interrupt - heart of scheduler
*                - checks for tasks with zero time_left
*                - process idle queue transferring any tasks to ready queue
*                - if there are ready tasks then schedule
*                - calls some algorithm specific functions
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : timeval - system time
******************************************************************************/
void SCH_start( const U32 timeval )
{
    CLK_start_timer( (U32)ZERO_CPV );         /* start the timer           */
    calculate_all_laxities( timeval );        /* calculate laxity          */

    if(     (algorithm is ALG_MUF)
          or(algorithm is ALG_MMUF)
          or(algorithm is ALG_MMMUF)   )
    {
        calculate_all_muf_urgencies();        /* calculate muf urgency     */
    }

    process_waiting_tasks( timeval );         /* process waiting tasks     */
    process_completed_tasks( timeval );       /* process completed tasks   */
    process_removed_tasks( timeval);          /* process any removed tasks */

    if(     (algorithm is ALG_ADAP_02)
          or(algorithm is ALG_ADAP_03)
          or(algorithm is ALG_ADAP_04)
          or(algorithm is ALG_ADAP_05)
          or(algorithm is ALG_ADAP_06)
          or(algorithm is ALG_ADAP_07)   )
    {
        process_doubled_periods();            /* process any doubled tasks */
    }


    if(     (algorithm is ALG_RM_RTO)
          or(algorithm is ALG_EDF_RTO)
          or(algorithm is ALG_EDF_BWP)   )
    {
        process_skipped_tasks( timeval );     /* process any skipped tasks */
    }

    process_idle_tasks( timeval );            /* process any idle tasks    */
    process_ready_tasks( timeval );           /* process any ready tasks   */
} 

/******************************************************************************
*  Description : Setter function - sets scheduler algorithm to alg
******************************************************************************/
void SCH_set_algorithm( const algorithm_type alg )
{
    algorithm = alg;
}

/******************************************************************************
*  Description : make scheduler report - calls out_res.OPR_0_message() to report
******************************************************************************/
void SCH_report_algorithm( const algorithm_type a, const message_class class )
{
    switch ( a )
    {
        case ALG_CYC:
            MESS_OUT_message( (S8*)"(01) Cyclic Executive\n", class );
            break;
        case ALG_RR:
            MESS_OUT_message( (S8*)"(02) Round Robin\n", class );
            break;
        case ALG_RM:
            MESS_OUT_message( (S8*)"(03) Rate Monotonic\n", class );
            break;
        case ALG_DRM:
            MESS_OUT_message( (S8*)"(04) Delayed Rate Monotonic\n", class );
            break;
        case ALG_IRM:
            MESS_OUT_message( (S8*)"(05) Intelligent Rate Monotonic\n", class );
            break;
        case ALG_EDF:
            MESS_OUT_message( (S8*)"(06) Earliest Deadline First\n", class );
            break;
        case ALG_SPT:
            MESS_OUT_message( (S8*)"(07) Shortest Processing Time\n", class );
            break;
        case ALG_LLF:
            MESS_OUT_message( (S8*)"(08) Least Laxity First\n", class );
            break;
        case ALG_MLLF:
            MESS_OUT_message( (S8*)"(09) Modified Least Laxity First\n", class );
            break;
        case ALG_MUF:
            MESS_OUT_message( (S8*)"(10) Maximum Urgency First\n", class );
            break;
        case ALG_MMUF:
            MESS_OUT_message( (S8*)"(11) Modified Maximum Urgency First\n", class );
            break;
        case ALG_MMMUF:
            MESS_OUT_message( (S8*)"(12) Modified Maximum Urgency First\n", class );
            break;
        case ALG_D_STAR:
            MESS_OUT_message( (S8*)"(13) D-Star\n", class );
            break;
        case ALG_DD_STAR:
            MESS_OUT_message( (S8*)"(14) Double D-Star\n", class );
            break;
        case ALG_D_OVER:
            MESS_OUT_message( (S8*)"(15) D over\n", class );
            break;
        case ALG_EDF_RTO:
            MESS_OUT_message( (S8*)"(16) EDF Red Task Only - Task Skip\n", class );
            break;
        case ALG_RM_RTO:
            MESS_OUT_message( (S8*)"(17) RM Red Task Only - Task Skip\n", class );
            break;
        case ALG_EDF_BWP:
            MESS_OUT_message( (S8*)"(18) RM Blue When Possible - Task Skip\n", class );
            break;
        case ALG_ADAP_01:
            MESS_OUT_message( (S8*)"(19) Adaptive No.1 - Removal\n", class );
            break;
        case ALG_ADAP_02:
            MESS_OUT_message( (S8*)"(20) Adaptive No.2 - Removal & recovery\n", class );
            break;
        case ALG_ADAP_03:
            MESS_OUT_message( (S8*)"(21) Adaptive No.3 - Period doubler using utility\n", class );
            break;
        case ALG_ADAP_04:
            MESS_OUT_message( (S8*)"(22) Adaptive No.4 - Period doubler & halver\n", class );
            break;
        case ALG_ADAP_05:
            MESS_OUT_message( (S8*)"(23) Adaptive No.5 - Period doubler using laxity & utility\n", class );
            break;
        case ALG_ADAP_06:
            MESS_OUT_message( (S8*)"(24) Adaptive No.6 - Period doubler/halver using laxity & utility \n", class );
            break;
        case ALG_ADAP_07:
            MESS_OUT_message( (S8*)"(25) Adaptive No.7 - Progressive period doubler/halver using laxity & utility \n", class );
            break;
        case ALG_MAX_ALG:
        default:
            MESS_OUT_message( (S8*)"RUN ERROR: default switch in SCH_report_algorithm()\n", MESSAGE_OP_ERROR );
            break;
    }
}

/******************************************************************************
*  Description : Calls local function - tells what algorithm is loaded
******************************************************************************/
void SCH_what_algorithm( const message_class class )
{
    SCH_report_algorithm( algorithm, class );
}

/******************************************************************************
*  Description : Access function returns the loaded algorithm type
******************************************************************************/
algorithm_type SCH_get_algorithm( void )
{
	return algorithm;
} 

/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/


   
/******************************************************************************
*  Description : Private helper used by SCH_start() to process waiting tasks.
*                A task is transferred from waiting to idle when its abs_dline
*                has been been reached
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void process_waiting_tasks( const U32 timeval  )
{
    U8 task;

    while(  /* the waiting queue is NOT empty */
            ( !q_empty(ZERO_CPV,waiting_tasks) )
            and /* AND the last deadline is less than or equal to time now */
            ( tasks[ task = q_first_id(ZERO_CPV,waiting_tasks) ]
                                -> abs_dline <= timeval )
         )
    {
        /* extract first waiting task - removes from queue */
        task = q_extract_first( waiting_tasks );

        /* change state to idle */
        tasks[task]->state = TSK_IDLE;

        calculate_all_laxities( timeval );        /* calculate laxity      */
        calculate_all_muf_urgencies();            /* calculate muf urgency */

        /* add to idle queue */
        q0_insert( task, idle_tasks, ALG_EDF );


        #ifdef DEBUG
            MESS_OUT_message( (S8*)"Waiting tasks", MESSAGE_OP_DEBUG );
            q_dump( waiting_tasks );
            task_params();
        #endif
    }
} 

/******************************************************************************
*  Description : Private helper used by SCH_start() to process complete tasks.
*                A task is considered complete when its time_left has reached
*                zero.
*  Notes       : (1) Adapted from code in Buttazzo's book (Buttazzo 2005)
*                (2) This scheduler is called "frequently". In a real world
*                    situation placing a completed task onto the idle queue
*                    might not be enough and decision made whether to transfer
*                    it to the ready queue made.
******************************************************************************/
static void process_completed_tasks( const U32 timeval  )
{
    if( /* is there a running task? */
        (running_task_id > ZERO_CPV)
        and /* AND has its time_left expired? */
        (tasks[running_task_id]->time_left is ZERO_CPV)  )
    {
        /* inform task object */
        TSK_task_completed( running_task_id, timeval );

        if( tasks[running_task_id]->period > ZERO_CPV )
        {
            /* periodic - change state to idle */
            tasks[running_task_id]->state = TSK_IDLE;

            /* place in idle queue in EDF order */
            q0_insert( running_task_id, idle_tasks, ALG_EDF );
        }
        else
        {
            /* non-periodic - change state to sleep */
            tasks[running_task_id]->state = TSK_SLEEP;
        }

        /* clear running task id */
        running_task_id = ZERO_CPV;
    }
} 

/******************************************************************************
*  Description : Private helper used by SCH_start() to process removed tasks.
*                Only runs for Adaptive-Two algorithm
******************************************************************************/
static void process_removed_tasks( const U32 timeval )
{
    U8  task;
    U16 old_utility;
    U16 task_set_utility;
    U16 spare_utility;

    if( algorithm is ALG_ADAP_02 )
    {
        /* get task set utility */
        task_set_utility = get_task_set_utility();

        spare_utility = MAX_EDF_UTIL - task_set_utility;

        /* get task with utility that is smaller than utility */
        task = removed_queue_has_task_with_lower_utility( spare_utility );

        while(  ( task_set_utility < MAX_EDF_UTIL ) /* there is spare utility */
                and /* removed queue NOT empty & has a task that will fit */
                ( task > ZERO_CPV )  )
        {
            /* extract removed task from queue */
            q0_extract( task );

            /* change state to idle */
            tasks[task]->state = TSK_IDLE;

            /* reset the task deadline */
            tasks[task]->abs_dline = timeval;

            /* add to idle queue in deadline order */
            q0_insert( task, idle_tasks, ALG_EDF );

            /* report task replacement */
            MESS_OUT_message_1( (S8*)"Replacing task ", 
                                task, 
                                MESSAGE_OP_VERBOSE );

            /* recalculate spare task set utility */
            old_utility = MAX_EDF_UTIL - spare_utility;

            task_set_utility = get_task_set_utility();

            /* find if there is any other tasks in removed queue */
            task = removed_queue_has_task_with_lower_utility( spare_utility );

            MESS_OUT_message_2( (S8*)"Utility changed from ", old_utility,
                                (S8*)" to ", task_set_utility, 
                                MESSAGE_OP_VERBOSE );
        }
    }
} 

/******************************************************************************
*  Description : Private helper used by SCH_start()
*                processes doubled tasks, restoring when there is sufficient
*                task utility
******************************************************************************/
static void process_doubled_periods( void )
{
    U8  task;
    U16 old_utility;
    U16 task_set_utility;

    if( algorithm is ALG_ADAP_04 )
    {
        /* get task set utility */
        task_set_utility = get_task_set_utility();

        if( MAX_EDF_UTIL > task_set_utility )
        {
            task = doubled_queue_has_task( MAX_EDF_UTIL - task_set_utility );

            /* get task with utility that is smaller than utility */

            while(  ( task_set_utility < MAX_EDF_UTIL ) /* there is spare utility */
                    and /* the doubled task queue is NOT empty & has a task that will fit */
                    ( task is_not ZERO_CPV )  )
            {
                /* remember previous utility for reporting purposes */
                old_utility = task_set_utility;

                /* extract removed task from doubled_periods under queue-1 */
                q1_extract( task, doubled_periods );

                /* half the task period */
                half_task_period( task );

                /* recalculate spare task set utility */
                task_set_utility = get_task_set_utility();

                if( MAX_EDF_UTIL > task_set_utility )
                {
                    /* find if there is any other tasks in removed queue */
                    task = doubled_queue_has_task( MAX_EDF_UTIL - task_set_utility );
                }

                MESS_OUT_message_2( (S8*)"Task set utility changed from ",
                                    old_utility,
                                    (S8*)" to ",
                                    task_set_utility, 
                                    MESSAGE_OP_VERBOSE );
            }
        }
    }
    else if( (algorithm is ALG_ADAP_06)or(algorithm is ALG_ADAP_07) )
    {
        /* get task set utility */
        task_set_utility = calculate_all_task_utilities();

        if( MAX_EDF_UTIL > task_set_utility )
        {
            task = doubled_queue_has_task( MAX_EDF_UTIL - task_set_utility );

            /* get task with utility that is smaller than utility */
            while(  ( task_set_utility < MAX_EDF_UTIL ) /* there is spare utility */
                    and /* the doubled task queue is NOT empty & has a task that will fit */
                    ( task is_not ZERO_CPV )  )
            {
                /* remember previous utility for reporting purposes */
                old_utility = task_set_utility;

                /* extract removed task from doubled_periods under queue-1 */
                q1_extract( task, doubled_periods );

                /* half the task period */
                half_task_period( task );

                if( tasks[task]->period_multiplyer > ZERO_CPV )
                {
                    /* re-calculate e_priority */
                    calculate_active_task_priority( task );

                    /* re-insert into doubled periods queue */
                    q1_insert( task, doubled_periods, ALG_ADAP_07 );
                }

                /* recalculate spare task set utility */
                task_set_utility = calculate_all_task_utilities();

                if( MAX_EDF_UTIL > task_set_utility )
                {
                    /* find if there is any other tasks in removed queue */
                    task = doubled_queue_has_task( MAX_EDF_UTIL - task_set_utility );
                }

                MESS_OUT_message_2( (S8*)"Task set utility changed from ",
                                    old_utility,
                                    (S8*)" to ",
                                    task_set_utility,
                                    MESSAGE_OP_VERBOSE );
            }
        }
    }
    else
    {
        /* do nothing */
    }
}

/******************************************************************************
*  Description : Private helper used by  process_doubled_periods scans
*                doubled queue and returns a suitable task, or zero
*                Queue-1 only contains tasks that are doubled
******************************************************************************/
static U8 doubled_queue_has_task( const U16 utility )
{
    U8 task = ZERO_CPV;
    U8 multiplyer;
    U8 n;   /* auxiliary pointer to next task */
    n = doubled_periods->q1_next;

    multiplyer = tasks[n]->period_multiplyer;

    while( (n is_not ZERO_CPV) and (tasks[n]->task_util > (utility << multiplyer) ) )
    {
        n = tasks[n]->q1_next;
        multiplyer = tasks[n]->period_multiplyer;
    }

    if( tasks[n]->task_util <= (utility << multiplyer) )
    {
        task = n;
    }

    return( task );
}

/******************************************************************************
*  Description : private helper used by SCH_start() to process skipped tasks
*                A task is skipped until its q0_next deadline
*  Notes       : Based on the "Skip-Over" paper by Koren and Shasha (1995).
******************************************************************************/
static void process_skipped_tasks( const U32 timeval )
{
    U8 task;

    while(   ( !q_empty(ZERO_CPV, skipped_tasks) )
             and
             ( timeval >= tasks[task=q_first_id( ZERO_CPV, skipped_tasks )]->abs_dline ) )
    {
        /* get first idle task */
        task = q_extract_first( skipped_tasks );

        /* set absolute deadline */
        tasks[task]->abs_dline = tasks[task]->rel_dline + timeval;

        /* set time left to duration value */
        tasks[task]->time_left = tasks[task]->c_duration;

        /* reset not-skipped count */
        NOT_SKIPPED(task) = (U8)SKIP_GAP(task);

        if( tasks[task]->laxity is ZERO_CPV )
        {
            /* put in ready queue */
            q0_insert( task, ready_tasks, algorithm ); /* add to idle queue */
            tasks[task]->state = TSK_READY;      /* change state to idle */
        }
        else
        {
            /* put in idle queue */
            q0_insert( task, idle_tasks, algorithm );  /* add to ready queue */
            tasks[task]->state = TSK_IDLE;       /* change state to ready */
        }
    }
}

/******************************************************************************
*  Description : private helper used by SCH_start() to process idle tasks
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void process_idle_tasks( const U32 timeval )
{
    U8 task;

    while(  /* the idle queue is NOT empty */
            ( !q_empty(ZERO_CPV,idle_tasks) )
            and /* AND the last deadline is less than or equal to time now */
            ( tasks[ task = q_first_id(ZERO_CPV, idle_tasks) ]
                                ->abs_dline <= timeval )
         )
    {
        /* extract first idle task - removes from queue */
        task = q_extract_first( idle_tasks );

        /* set absolute deadline - also double check multiplier */
        if( PERIOD_FLEXIBLE(task)is ONE_CPV )
        {
            tasks[task]->abs_dline +=
               (tasks[task]->rel_dline << tasks[task]->period_multiplyer);
        }
        else
        {
            tasks[task]->abs_dline += (tasks[task]->rel_dline);
        }

        /* set time left to duration value */
        tasks[task]->time_left = tasks[task]->c_duration;

        /* change state to ready */
        tasks[task]->state = TSK_READY;

        calculate_all_laxities( timeval );        /* calculate laxity      */
        calculate_all_muf_urgencies();            /* calculate muf urgency */

        /* add to ready queue */
        q0_insert( task, ready_tasks, algorithm );

        /* if in adaptive algorithm 05 then we are using laxity to */
        /* detect overload, using a second queue:                  */
        if( (algorithm is ALG_ADAP_05) or (algorithm is ALG_ADAP_06) )
        {
            q2_insert( task, lst_tasks, ALG_LLF );
        }

        #ifdef DEBUG
            MESS_OUT_message( (S8*)"lst tasks\n", MESSAGE_OP_debug );
            q_dump( lst_tasks );
            task_params();
        #endif
    }

} /* process_idle_tasks() */


/******************************************************************************
*  Description : private helper used by SCH_start() to process ready tasks
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void process_ready_tasks( const U32 timeval )
{
    if( !q_empty(ZERO_CPV, ready_tasks) )
    {
        switch ( algorithm )
        {
            case ALG_ADAP_01:
            case ALG_ADAP_02:
                schedule_adapive_one_algorithm( timeval );
                break;

            case ALG_ADAP_03:
            case ALG_ADAP_04:
                schedule_adapive_three_algorithm();
                break;

            case ALG_ADAP_05:
                schedule_adapive_five_algorithm( timeval );
                break;

            case ALG_ADAP_06:
                schedule_adapive_six_algorithm( timeval );
                break;

            case ALG_ADAP_07:
                schedule_adapive_seven_algorithm( timeval );
                break;

            case ALG_CYC:
                /* cyclic executive */
                schedule_cyc_algorithm();
                break;

            case ALG_RR:
                /* round robin */
                schedule_rr_algorithm();
                break;

            case ALG_RM:
                schedule_rm_algorithm();
                break;

            case ALG_DRM:
                schedule_drm_algorithm( timeval );
                break;

            case ALG_IRM:
                schedule_irm_algorithm();
                break;

            case ALG_MUF:
                schedule_muf_algorithm( timeval );
                break;

            case ALG_MMUF:
                schedule_mmuf_algorithm( timeval );
                break;

            case ALG_MMMUF:
                schedule_mmmuf_algorithm( timeval );
                break;

            case ALG_EDF:
                schedule_edf_algorithm();
                break;

            case ALG_SPT:
                schedule_spt_algorithm();
                break;

            case ALG_LLF:
                schedule_llf_algorithm( timeval );
                break;

            case ALG_MLLF:
                schedule_mllf_algorithm( timeval );
                break;

            case ALG_EDF_RTO:
                schedule_skip_rto_algorithm( timeval );
                break;

            case ALG_D_STAR:
                #if(0)
                schedule_d_star_algorithm( timeval );
                #endif
                break; 

            case ALG_RM_RTO:
            case ALG_EDF_BWP:
            case ALG_DD_STAR:
            case ALG_D_OVER:
            case ALG_MAX_ALG:
            default:
                /* do nothing */
                MESS_OUT_message( (S8*)"default switch in process_ready_tasks()\n", 
                                  MESSAGE_OP_ERROR );
                break;
        } /* end switch */

    } /* end if */

} /* process_ready_tasks() */


/******************************************************************************
*  Description : private helper implements first adaptive design.
*                In normal conditions, operates EDF
*                Detects overload when utility is more than one
*                Pre-empts tasks with lower priority, until utility is within
*                bounds by placing them onto removed queue.
*  Notes       : Also used for adaptive two algorithm
******************************************************************************/
static void schedule_adapive_one_algorithm( const U32 timeval )
{
    U8  lowest_priority;
    U16 utility;
    U16 new_utility;

	/* get the utility of the task set */
	utility = calculate_all_task_utilities();

    /* check that task set utility is less than one (1000) */
    while( utility > MAX_EDF_UTIL )
    {
        /* scheduler is in OVERLOAD */
        /* ======================== */

        /* identify active task with lowest priority */
        lowest_priority = get_lowest_priority_active_task();

        /* and remove it */
        MESS_OUT_message_1( (S8*)"Removing task ", 
                            lowest_priority, 
                            MESSAGE_OP_VERBOSE );

        /* if the lowest priority task also happens to be the running */
		/* task, then pre-empt it                                     */
        if( lowest_priority is running_task_id )
        {
            /* lowest priority task is running */
            preempt_task( lowest_priority );
        }

        /* extract lowest task from queue */
        q0_extract( lowest_priority );

		/* record this activity to report storage */
		TST_incr_test_report_value( TST_REMOVALS );
		TOTAL_SKIPS(lowest_priority)++;

        /* place extracted task onto 'removed' queue, in priority order */
        q0_insert( lowest_priority, removed_tasks, ALG_RM );

        /* change state of extracted task to removed */
        tasks[lowest_priority]->state = TSK_REMOVED;

        /* get the new task set utility */
        new_utility = get_task_set_utility();

        /* and report it */
        MESS_OUT_message_2( (S8*)"Utility changed from ", utility, 
                            (S8*)" to ", new_utility,
                            MESSAGE_OP_VERBOSE );

        /* reset utility value ready for next while loop */
        utility = new_utility;

    } /* end while loop */

    /* attempt to restore removed tasks */
    process_removed_tasks( timeval );

    /* now perform normal EDF scheduling */
    schedule_edf_algorithm();

} /* schedule_adapive_one_algorithm() */


/******************************************************************************
*  Description : private helper implements third adaptive design.
*                In normal conditions, operates EDF
*                Detects overload when utility is more than one
*                Doubles the periods tasks with lower priority, until utility
*                is within bounds.
******************************************************************************/
static void schedule_adapive_three_algorithm( void )
{
    U8  lowest_priority = ZERO_CPV;
    U32 utility;
    U32 new_utility;

    /* get the task set utility */
	utility = calculate_all_task_utilities();

    /* check that task set utility is less than one */
    while( utility > MAX_EDF_UTIL )
    {
        /* scheduler is in OVERLOAD */
        /* ======================== */

        /* identify active task with lowest priority */
        lowest_priority = get_lowest_priority_active_task();

        (void)double_task_period( lowest_priority );

        /* en queue task into second list, doubled_periods, priority order */
        q1_insert( lowest_priority, doubled_periods, ALG_RM );

        /* get new utility */
        new_utility = get_task_set_utility();

        MESS_OUT_message_2( (S8*)"Utility changed from ", utility, 
                            (S8*)" to ", new_utility,
                            MESSAGE_OP_VERBOSE );

        /* set for next while loop */
        utility = new_utility;

    } /* end while loop */

    /* now perform normal EDF scheduling */
    schedule_edf_algorithm();
} 

/******************************************************************************
*  Description : private helper implements fifth adaptive design.
*                In normal conditions, operates EDF
*                Detects overload when utility is more than one
*                Doubles the periods tasks with lower priority, until utility
*                is within bounds.
******************************************************************************/
static void schedule_adapive_five_algorithm( const U32 timeval )
{
    U8  first_id;
    U8  lowest_priority = ZERO_CPV;
    U32 laxity;

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    /* get laxity for task at front of ready queue */
    laxity = tasks[first_id]->laxity;

    while( laxity < tasks[running_task_id]->time_left )
    {
        MESS_OUT_message_1( (S8*)"OVERLOAD: Laxity is zero for task ", 
                            first_id, 
                            MESSAGE_OP_VERBOSE );

        /* scheduler is in OVERLOAD */
        /* ======================== */

        /* identify active task with lowest priority */
        lowest_priority = get_lowest_priority_active_task();

        /* double period of lowest_priority task */
        (void)double_task_period( lowest_priority );

        /* en queue task in second list, doubled_periods, priority order */
        q1_insert( lowest_priority, doubled_periods, ALG_RM );

        /* recalculate laxities */
        calculate_all_laxities( timeval );

        /* get index of first task in ready queue */
        first_id = q_first_id( ZERO_CPV, ready_tasks );

        /* get laxity for task at front of ready queue */
        laxity = tasks[first_id]->laxity;

        if( laxity > 0 )
        {
            MESS_OUT_message_1( (S8*)"Laxity is now ", 
                                laxity, MESSAGE_OP_VERBOSE );
        }

    } /* end while loop */

    /* now perform normal EDF scheduling */
    schedule_edf_algorithm();
}

/******************************************************************************
*  Description : private helper implements adaptive design #6.
*                Features:
*                - monitors task execution times
*                - uses calculated task set utility
*                - doubles tasks
******************************************************************************/
static void schedule_adapive_six_algorithm( const U32 timeval )
{
	const U8 max_loop_count = 5;
	U8  first_id;
	U8  lowest_priority = ZERO_CPV;
	U32 laxity;
	U32 utility;
	U32 new_utility;
	U16 time_frigg = 906;
	U8  task_frigg = FIVE_CPV;
	U8  loop_count;

	/* set up a timed forced fault event */
	if( timeval is time_frigg )
	{
		MESS_OUT_message_1( (S8*)"Running task duration incremented by ", 
		                    task_frigg, 
		                    MESSAGE_OP_VERBOSE );

		/* mangle task parameter */
		tasks[running_task_id]->time_taken += task_frigg;
	}

	/* check that calculated task set utility is less than 100% */
	utility = calculate_all_task_utilities();

	/* get index of first task in ready queue */
	first_id = q_first_id( ZERO_CPV, ready_tasks );

	/* get laxity for task at front of ready queue */
	laxity = calculate_laxity( first_id, timeval );

	loop_count = ZERO_CPV;
	while( ((utility > MAX_EDF_UTIL) or (laxity < tasks[running_task_id]->time_left))
		and (loop_count < max_loop_count) )
	{
		/* report why we are here */
		if(utility > MAX_EDF_UTIL)
		{
			MESS_OUT_message_1( (S8*)"OVERLOAD: utility = ", 
			                     utility, MESSAGE_OP_VERBOSE );
		}
		else
		{
			MESS_OUT_message_2( (S8*)"OVERLOAD: task ", first_id, 
				                (S8*)" laxity = ", laxity,
				                MESSAGE_OP_VERBOSE );
		}

		/* scheduler is in OVERLOAD */
		/* ======================== */

		/* identify active task with lowest priority */
		lowest_priority = get_lowest_priority_active_task();

		(void)double_task_period( lowest_priority );

		/* en queue task in second list, doubled_periods, priority order */
		q1_insert( lowest_priority, doubled_periods, ALG_RM );

		/* get new utility */
		new_utility = calculate_all_task_utilities();

		MESS_OUT_message_2( (S8*)"Task set utility changed from ", utility,
			                (S8*)" to ", new_utility,
			                MESSAGE_OP_VERBOSE );

		/* set for next while loop */
		utility = new_utility;

		/* get laxity for task at front of ready queue */
		laxity = calculate_laxity( first_id, timeval );

		loop_count++;

	} /* end while loop */

	if( loop_count >= max_loop_count )
	{
		MESS_OUT_message( (S8*)"GIVING UP: INSUFFICIENT SPARE UTILITY\n", 
		                  MESSAGE_OP_VERBOSE );
	}

	/* now perform normal EDF scheduling */
	schedule_edf_algorithm();
}

/******************************************************************************
*  Description : private helper implements adaptive design #7.
*                Features:
*                - monitors task execution times
*                - uses task set utility
*                - progressive doubling of tasks  
*                - loop counter to prevent lock-up
******************************************************************************/
static void schedule_adapive_seven_algorithm( const U32 timeval )
{
	const U8 max_loop_count = 10;
	U8  first_id;
	U8  candidate_task = ZERO_CPV;
	U32 laxity;
	U32 utility;
	U32 new_utility;
	U16 time_frigg     = 906;
	U8  task_frigg_1   = FIVE_CPV;
	U8  task_frigg_2   = 100;
	U8  loop_count;

	/* set up a timed forced fault event */
	if( (timeval is time_frigg)and(test_num is 57) )
	{
	    MESS_OUT_message_1( (S8*)"Running task duration incremented by ", 
	                        task_frigg_1, 
	                        MESSAGE_OP_VERBOSE );

		/* mangle task parameter */
		tasks[running_task_id]->time_taken += task_frigg_1;
	}
	else if( (timeval is time_frigg)and(test_num is 60) )
	{
		MESS_OUT_message_1( (S8*)"Running task duration incremented by ", 
		                    task_frigg_2, 
		                    MESSAGE_OP_VERBOSE );

		/* mangle task parameter */
		tasks[running_task_id]->time_taken += task_frigg_2;
	}
	else
	{
		/* do nothing */
	}


	/* get task set utility */
	utility = calculate_all_task_utilities();

	/* get index of first task in ready queue */
	first_id = q_first_id( ZERO_CPV, ready_tasks );

	/* get laxity for task at front of ready queue */
	laxity = calculate_laxity( first_id, timeval );

	loop_count = ZERO_CPV;
	while( (utility > MAX_EDF_UTIL)and(loop_count < max_loop_count) )
	{
		/* scheduler is in OVERLOAD */
		/* ======================== */

		/* report why we are here */
		if(utility > MAX_EDF_UTIL)
		{
			/* overload due to utility */
			MESS_OUT_message_1( (S8*)"OVERLOAD: utility = ", 
			                    utility, 
			                    MESSAGE_OP_VERBOSE );
		}
		else
		{
			/* overload due to laxity */
			MESS_OUT_message_2( (S8*)"OVERLOAD: task ", first_id,
				                (S8*)" laxity = ", laxity,
				                MESSAGE_OP_VERBOSE );
		}

		/* identify candidate task for period doubling */
		candidate_task = get_active_task_priority();

		/* remove task from doubled queue */
		q1_extract( candidate_task, doubled_periods );

		/* double the candidate task */
		(void)double_task_period( candidate_task );

		/* en queue task in second list, doubled_periods, e_priority order */
		q1_insert( candidate_task, doubled_periods, ALG_ADAP_07 );

		/* get new utility */
		new_utility = calculate_all_task_utilities();

		MESS_OUT_message_2( (S8*)"Task set utility changed from ", utility,
			                (S8*)" to ", new_utility, 
			                MESSAGE_OP_VERBOSE );

		/* set for next while loop */
		utility = new_utility;

		/* get laxity for task at front of ready queue */
		laxity = calculate_laxity( first_id, timeval );

		loop_count++;

	} /* end while loop */

	if( loop_count >= max_loop_count )
	{
		MESS_OUT_message( (S8*)"GIVING UP: INSUFFICIENT SPARE UTILITY\n", 
		                  MESSAGE_OP_INFO );
	}

	/* ok, now we perform normal EDF scheduling */
	schedule_edf_algorithm();
}

/******************************************************************************
*  Description : private helper implements round robin algorithm
******************************************************************************/
static void schedule_rr_algorithm( void )
{


} 

/******************************************************************************
*  Description : private helper implements cyclic executive algorithm
******************************************************************************/
static void schedule_cyc_algorithm( void )
{


} 

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using priority algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void schedule_rm_algorithm( void )
{
    U8 first_id;

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    if(
          (tasks[first_id]->priority < tasks[running_task_id]->priority)
          or /* OR there are no running tasks */
          (running_task_id is ZERO_CPV)
      )
    {
        /* why are we here ? */
        if( running_task_id is_not ZERO_CPV )
        {
            MESS_OUT_message_4( (S8*)"RM: running task #",
                            running_task_id,
                           (S8*)" priority = ",
                           tasks[running_task_id]->priority,
                           (S8*)" - next task #",
                            first_id,
                           (S8*)" priority = ",
                            tasks[first_id]->priority, 
                            MESSAGE_OP_VERBOSE );
        }
        else
        {
            MESS_OUT_message( (S8*)"RM: No running task", 
                              MESSAGE_OP_VERBOSE );
        }

        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }

        /* dispatch next task in ready queue */
        dispatch_next_task();                 /* dispatch new task     */
    }
}

/******************************************************************************
*  Description : private helper user the delayed rate monotonic scheduling
*                algorithm as described by Naghibzadeh (2002)
*  Parameters  : Tasks are delayed by (T - c). In other words, all laxity
*                is removed.
******************************************************************************/
static void schedule_drm_algorithm( const U32 timeval )
{
    U8 first_id;
    U32 laxity;

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    /* get laxity of first task in queue */
    laxity = calculate_laxity( first_id, timeval );

	/* if( laxity is 0) */
	MESS_OUT_message_2( (S8*)"Laxity of task ",
	                    first_id,
	                    (S8*)" is ",
	                    laxity, MESSAGE_OP_VERBOSE );

    if(
        (
            ((tasks[first_id]->priority < tasks[running_task_id]->priority) or (running_task_id is ZERO_CPV))
            and /* the next task has no laxity */
            (laxity is ZERO_CPV )
        )
       /* or  OR there are no running tasks 
        (running_task_id is ZERO_CPV) */
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }

        /* dispatch next task in ready queue */
        dispatch_next_task();                 /* dispatch new task     */
    }
} 

/******************************************************************************
*  Description : private helper user the intelligent rate monotonic scheduling
*                algorithm as described by Naghibzadeh and Fathi (2003)
*  Parameters  : Tasks are delayed by (T - c). In other words, all laxity
*                is removed.
******************************************************************************/
static void schedule_irm_algorithm( void )
{
    U8 first_id;

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    if(
          (tasks[first_id]->priority < tasks[running_task_id]->priority)
          or /* OR there are no running tasks */
          (running_task_id is ZERO_CPV)
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }

        /* dispatch next task in ready queue */
        dispatch_next_task();                 /* dispatch new task     */
    }
} 

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using priority algorithm
*  Notes       : Adapted from Stewart and Khosla's paper that introduces
*                the Maximum Urgency First (MUF) algorithm
*                (Stewart and Khosla 1991).
******************************************************************************/
static void schedule_muf_algorithm( const U32 timeval )
{
    U8 first_id;

    calculate_all_laxities( timeval );        /* calculate the laxities */
    calculate_all_muf_urgencies();            /* calculate the muf urgencies */

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    if(   /* next task muf urgency is less than running task muf urgency */
          (MUF_URGENCY(first_id) > MUF_URGENCY(running_task_id))
          or /* OR there are no running tasks */
          (running_task_id is ZERO_CPV)
      )
    {
        /* why are we here ? */
        if( running_task_id is_not ZERO_CPV )
        {
            MESS_OUT_message_4( (S8*)"MUF: running task #",
                            running_task_id,
                           (S8*)" urgency = ",
                           MUF_URGENCY(running_task_id),
                           (S8*)" - next task #",
                            first_id,
                           (S8*)" urgency = ",
                            MUF_URGENCY(first_id), 
                            MESSAGE_OP_VERBOSE );
        }
        else
        {
            MESS_OUT_message( (S8*)"MUF: No running task", 
                               MESSAGE_OP_VERBOSE );
        }

        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            /* report task replacement */
            MESS_OUT_message_1( (S8*)"Laxity Zero: removing task ", 
                                running_task_id, 
                                MESSAGE_OP_VERBOSE );

            preempt_task( running_task_id );  /* pre-empt running task */
        }
        dispatch_next_task();                 /* dispatch new task     */
    }
}


/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using a modified muf
*                algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void schedule_mmuf_algorithm( const U32 timeval )
{
    U32 laxity;
    U8 first_id;

    calculate_all_laxities( timeval );        /* calculate the laxities      */
    calculate_all_muf_urgencies();            /* calculate the muf urgencies */

    first_id = q_first_id(ZERO_CPV, ready_tasks);
    laxity = tasks[first_id]->laxity;

    if(      /* next task laxity = zero */
          ( laxity is ZERO_CPV )
          or /* OR there are no running tasks */
          ( running_task_id is ZERO_CPV )
      )
    {
        /* why are we here ? */
        if( running_task_id is_not ZERO_CPV )
        {
            MESS_OUT_message_2( (S8*)"MMUF: running task #",
                                running_task_id,
                                (S8*)" laxity = ",
                                laxity, 
                                MESSAGE_OP_VERBOSE );
        }
        else
        {
            MESS_OUT_message( (S8*)"MMUF: No running task", 
                               MESSAGE_OP_VERBOSE );
        }
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            /* report task replacement */
            MESS_OUT_message_1( (S8*)"Laxity Zero: removing task ", running_task_id, MESSAGE_OP_VERBOSE );

            preempt_task( running_task_id );  /* pre-empt running task */
        }

        dispatch_next_task();                 /* dispatch next task    */
    }
} 

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using a modified muf
*                algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void schedule_mmmuf_algorithm( const U32 timeval )
{
    U8 first_id;

    calculate_all_laxities( timeval );        /* calculate the laxities      */
    calculate_all_muf_urgencies();            /* calculate the muf urgencies */

    first_id = q_first_id(ZERO_CPV, ready_tasks);

    if(   /* next task laxity = zero */
          ( tasks[first_id]->laxity is ZERO_CPV )
          or /* OR next task muf urgency is less than running task muf urgency */
          (
			  (MUF_CRITICAL(running_task_id) is ZERO_CPV)
              and
              (MUF_URGENCY(first_id) > MUF_URGENCY(running_task_id))
          )
          or /* OR there are no running tasks */
          ( running_task_id is ZERO_CPV )
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            /* report task replacement */
            MESS_OUT_message_1( (S8*)"Laxity Zero: removing task ", 
                                running_task_id, 
                                MESSAGE_OP_VERBOSE );

            preempt_task( running_task_id );  /* pre-empt running task */
        }

        dispatch_next_task();                 /* dispatch next task    */
    }
}

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using priority algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void schedule_edf_algorithm( void )
{
    U8 first_id;

    /* get index of first task in ready queue */
    first_id  = q_first_id( ZERO_CPV, ready_tasks );

    if(    /* next task in queue has a deadline before the running task */
          ( tasks[first_id]->abs_dline < tasks[running_task_id]->abs_dline )
          or /* OR there are no running tasks */
          (running_task_id is ZERO_CPV)
      )
    {
        /* why are we here ? */
        if( running_task_id is_not ZERO_CPV )
        {
            MESS_OUT_message_4( (S8*)"EDF: running task #",
                            running_task_id,
                           (S8*)" deadline = ",
                           tasks[running_task_id]->abs_dline,
                           (S8*)" - next task #",
                            first_id,
                           (S8*)" deadline = ",
                            tasks[first_id]->abs_dline, 
                            MESSAGE_OP_VERBOSE );
        }
        else
        {
            MESS_OUT_message( (S8*)"EDF: No running task", MESSAGE_OP_VERBOSE );
        }

        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }

        dispatch_next_task();                 /* dispatch new task     */
    }
} 

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using priority algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void schedule_spt_algorithm( void )
{
    U8 first_id;

    /* get index of first task in ready queue */
    first_id  = q_first_id( ZERO_CPV, ready_tasks );

    if(
          (tasks[first_id]->time_left < tasks[running_task_id]->time_left)
          or /* OR there are no running tasks */
          (running_task_id is ZERO_CPV)
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }
        dispatch_next_task();                 /* dispatch new task     */
    }
} 

/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using algorithm
*  Notes       : Adapted from Dertouzos and Mok's paper
*                (Dertouzos and Mok 1989)
******************************************************************************/
static void schedule_llf_algorithm( const U32 timeval )
{
    U8 first_id;

    calculate_all_laxities( timeval );         /* calculate the laxities */

    /* get index of first task in ready queue */
    first_id = q_first_id( ZERO_CPV, ready_tasks );

    if(
          (tasks[first_id]->laxity < tasks[running_task_id]->laxity)
          or
          (running_task_id is ZERO_CPV)
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }
        dispatch_next_task();                 /* dispatch new task     */
    }
} 


/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using algorithm
*  Notes       : Adapted from Sung-Heun Oh and Seung-Min Yang's paper
*                (Oh and Yang 1998)
******************************************************************************/
static void schedule_mllf_algorithm( const U32 timeval )
{
    calculate_all_laxities( timeval );         /* calculate the laxities */

    if(
          ( tasks[ q_first_id(ZERO_CPV, ready_tasks) ]->laxity is ZERO_CPV )
          or
          ( running_task_id is ZERO_CPV )
      )
    {
        /* pre-empt task, if there is a running task */
        if( running_task_id > ZERO_CPV )
        {
            preempt_task( running_task_id );  /* pre-empt running task */
        }
        dispatch_next_task();                 /* dispatch new task     */
    }
} 


/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using skip algorithm.
*                Tasks are RED only, meaning no pre-emptions unless existing
*                task can complete
*  Notes       : Based on the Red Task Only Skip-Over algorithm in paper
*                by Koren and Shasha (1995).
******************************************************************************/
static void schedule_skip_rto_algorithm( const U32 timeval )
{
    U8 first_id;
    U8 second_id;

    /* get index of first task in ready queue */
    first_id  = q_first_id(  ZERO_CPV, ready_tasks );
    second_id = q_second_id( ZERO_CPV, ready_tasks );


    if( running_task_id is ZERO_CPV)
    {

        if( task_skippable(first_id) )
        {
            if( can_meet_deadline(first_id, timeval) )
            {
                if( can_meet_deadline(second_id, timeval) )
                {
                    /* dispatch the next task in the ready queue */
                    dispatch_next_task();
                }
                else
                {
                    /* skip ready task */
                    skip_first_task( timeval );

                    /* dispatch the next task in the ready queue */
                    dispatch_next_task();
                }
            }
            else
            {
                /* skip ready task */
                skip_first_task( timeval );

                /* dispatch the next task in the ready queue */
                dispatch_next_task();
            }
        }
        else
        {
            /* do nothing - dispatch anyway */
            dispatch_next_task();
        }
    }
    else if(
            ( tasks[first_id]->abs_dline < tasks[running_task_id]->abs_dline )
            and /* AND only if this task will complete */
            ( tasks[first_id]->time_left < tasks[running_task_id]->laxity)
           )
    {
        preempt_task( running_task_id );  /* pre-empt running task */
    }
    else
    {
        /* do nothing */
    }
} 

#if(0)
/******************************************************************************
*  Description : private helper implements Deadline Monotonic algorithm
*                Performs task schedulability test.
*  Note        : Taken from pseudo code in Audsley et al (1992)
*
*                  for each t do
*                      t = sum C
*                      continue = TRUE
*                      while (continue) do
*                          if(I/t + C/t <= 1)
*                              continue = FALSE
*                              # NB t is schedulable
*                          else
*                              t = I + C
*                          endif
*                          if( t > D
*                              exit
*                              # NB t is not schedulable
*                          endif
*                      endwhile
*                  endfor
*
*  Parameters  : timeval - system time
*  Returns     : TRUE if the task is schedulable
******************************************************************************/
static boolean dm_schedulable( const U32 timeval )
{
    boolean schedulable = FALSE;

    return( schedulable );

} /* dm_schedulable() */
#endif


#if(0)
/******************************************************************************
*  Description : private helper compares running task with first ready task
*                and preempts if algorithm permits using D* algorithm
******************************************************************************/
static void schedule_d_star_algorithm( const U32 timeval )
{
}
#endif

/******************************************************************************
*  Description : private helper preempts task, transferring it to the
*                ready queue
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void preempt_task( const U8 task )
{
    /* report task replacement */
    MESS_OUT_message_1( (S8*)"Pre-empting task ", task, MESSAGE_OP_VERBOSE );

    tasks[task]->state = TSK_PREMPT;            /* set state to pre-empted   */
    tasks[task]->pre_count++;                   /* increment pre-empt counts */
    TST_incr_test_report_value( TST_PREEMPTS ); /* increment pre-empt counts */
    q0_insert( task, ready_tasks, algorithm );  /* put it into ready queue   */

    /* if in adaptive algorithm 05 then we are using laxity to */
    /* detect overload. */
    if( (algorithm is ALG_ADAP_05) or (algorithm is ALG_ADAP_06) )
    {
        q2_insert( task, lst_tasks, ALG_LLF );
    }

    running_task_id = 0;                        /* clear running task ID     */
}

/******************************************************************************
*  Description : private helper dispatched q0_next task in ready queue
*                updates the tasks[] array
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void dispatch_next_task( void )
{
    /* dispatch new task */
    running_task_id = q_extract_first( ready_tasks );

    if( (algorithm is ALG_ADAP_05) or (algorithm is ALG_ADAP_06) )
    {
        q2_extract( running_task_id, lst_tasks );
    }

    if( running_task_id > ZERO_CPV )
    {
        /* set state of new running task */
        tasks[running_task_id]->state = TSK_RUN;

        /* decrement non skipped count */
        decrement_not_skipped_count( running_task_id );
    }
} 

/******************************************************************************
*  Description : private helper skips first ready task by transferring it from
*                front of ready queue to skipped queue
*  Notes       : Based on the "Skip-Over" paper by Koren and Shasha (1995).
******************************************************************************/
static void skip_first_task( const U32 timeval )
{
    U8 id;

    /* extract task from ready queue, keeping its id */
    id = q_extract_first( ready_tasks );

    /* report task replacement */
    MESS_OUT_message_1( (S8*)"Skipping task ", id, MESSAGE_OP_VERBOSE );

    /* put task in skipped queue */
    q0_insert( id, skipped_tasks, algorithm );

    /* set state to skipped */
    tasks[id]->state = TSK_SKIPPED;

    /* reset not skipped count */
    NOT_SKIPPED(id) = (U8)SKIP_GAP(id);

    /* how long to skip - set abs_dline to hold task in skip queue */
    if( tasks[id]->abs_dline <= timeval )
    {
        tasks[id]->abs_dline = timeval + tasks[id]->rel_dline;
    }

    /* increment skip count */
    TOTAL_SKIPS(id)++;
} 

/* queue utility functions */

/******************************************************************************
*  Description : private helper links task into head of task list
*                according to scheduler algorithm.
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : pointer to task
*                pointer to queue
******************************************************************************/
static void q0_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm )
{
    U8  prev;      /* pointer to previous task */
    U8  next;      /* pointer to next task     */

	MESS_OUT_message( (S8*)"q0_insert() called", MESSAGE_OP_DEBUG );

    /* initialise auxiliary pointers */
    prev = ZERO_CPV;
    next = header->q0_next;

    switch ( scheduling_algorithm )
    {
        case ALG_RM:
        case ALG_DRM:
            /* Priority scheduling: insert in priority order */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->priority > tasks[next]->priority )
                 )
            {
                prev = next;
                next = tasks[next]->q0_next;
            }
            break;

        case ALG_MUF:
        case ALG_MMUF:
        case ALG_MMMUF:
            /* MUF, MMUF & MMMUF: insert in muf urgency first order */
            while(
                     ( next is_not ZERO_CPV )
                      and
                     ( MUF_URGENCY(task) < MUF_URGENCY(next) )
                 )
            {
                prev = next;
                next = tasks[next]->q0_next;
            }
            break;

        case ALG_SPT:
            /* SPT queue in shortest processing time order */
            while(
                     ( next is_not 0 )
                      and
                     ( tasks[task]->time_left >= tasks[next]->time_left )
                 )
            {
                prev = next;
                next = tasks[next]->q0_next;
            }
            break;

        case ALG_ADAP_01:
        case ALG_ADAP_02:
        case ALG_ADAP_03:
        case ALG_ADAP_04:
        case ALG_ADAP_05:
        case ALG_ADAP_06:
        case ALG_ADAP_07:
        case ALG_EDF_RTO:
        case ALG_EDF:
            /* EDF: insert in deadline first order */
            while(
                     ( next is_not ZERO_CPV )
                      and
                     ( tasks[task]->abs_dline >= tasks[next]->abs_dline )
                 )
            {
                prev = next;
                next = tasks[next]->q0_next;
            }
            break;

        case ALG_LLF:
        case ALG_MLLF:
            /* LLF: insert in least laxity order */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->laxity > tasks[next]->laxity )
                 )
            {
                prev = next;
                next = tasks[next]->q0_next;
            }
            break;

        case ALG_IRM:
        case ALG_D_STAR:
        case ALG_DD_STAR:
        case ALG_D_OVER:
        case ALG_CYC:
        case ALG_RR:
        case ALG_RM_RTO:
        case ALG_EDF_BWP:
        case ALG_MAX_ALG:
        default:
            /* do nothing */
			MESS_OUT_message_1(  (S8*)"q0_insert() default switch with ",
                                 (U32)scheduling_algorithm,
			                     MESSAGE_OP_ERROR );
            break;
    }

    /* common code */
    if( prev is_not ZERO_CPV )
    {
        /* not at start */
        tasks[prev]->q0_next = task;
    }
    else
    {
        /* first ready task */
        header->q0_next = task;
    }

    if( next is_not ZERO_CPV )
    {
        /* not at end */
        tasks[next]->q0_prev = task;
    }

    /* set new ready task pointers */
    tasks[task]->q0_next = next;
    tasks[task]->q0_prev = prev;
} 

/******************************************************************************
*  Description : private helper links task into head of task list
*                according to scheduler algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : pointer to task
*                pointer to queue
*                algorithm to select
******************************************************************************/
static void q1_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm )
{
    U8  prev;      /* pointer to previous task */
    U8  next;      /* pointer to next task     */


	MESS_OUT_message( (S8*)"q1_insert() task", MESSAGE_OP_DEBUG );

    /* initialise auxiliary pointers */
    prev = ZERO_CPV;
    next = header->q1_next;

    switch ( scheduling_algorithm )
    {
        case ALG_RM:
            /* Priority: insert in priority order */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->priority > tasks[next]->priority )
                 )
            {
                prev = next;
                next = tasks[next]->q1_next;
            }
            break;

        case ALG_LLF:
            /* LLF: insert in least laxity order */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->laxity > tasks[next]->laxity )
                 )
            {
                prev = next;
                next = tasks[next]->q1_next;
            }
            break;

        case ALG_ADAP_07:
            /* used for queuing doubled tasks */
            /* store in e_priority order - highest is bottom */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->e_priority > tasks[next]->e_priority )
                 )
            {
                prev = next;
                next = tasks[next]->q1_next;
            }
            break;

        case ALG_IRM:
        case ALG_DRM:
        case ALG_D_STAR:
        case ALG_DD_STAR:
        case ALG_D_OVER:
        case ALG_MLLF:
        case ALG_MUF:
        case ALG_MMUF:
        case ALG_SPT:
        case ALG_EDF_RTO:
        case ALG_EDF:
        case ALG_CYC:
        case ALG_RR:
        case ALG_RM_RTO:
        case ALG_EDF_BWP:
        case ALG_MMMUF:
        case ALG_ADAP_01:
        case ALG_ADAP_02:
        case ALG_ADAP_03:
        case ALG_ADAP_04:
        case ALG_ADAP_05:
        case ALG_ADAP_06:
        case ALG_MAX_ALG:
        default:
            /* do nothing */
			MESS_OUT_message_1(  (S8*)"q1_insert() default switch with ",
                                 (U32)scheduling_algorithm,
			                      MESSAGE_OP_ERROR );
            break;
    }

    if( prev is_not ZERO_CPV )
    {
        /* not at start */
        tasks[prev]->q1_next = task;
    }
    else
    {
        /* first ready task */
        header->q1_next = task;
    }

    if( next is_not ZERO_CPV )
    {
        /* not at end */
        tasks[next]->q1_prev = task;
    }

    /* set new ready task pointers */
    tasks[task]->q1_next = next;
    tasks[task]->q1_prev = prev;
} 

/******************************************************************************
*  Description : private helper links task into head of task list
*                according to scheduler algorithm
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : pointer to task
*                pointer to queue
*                algorithm to select
******************************************************************************/
static void q2_insert( const U8 task,
                       struct task_type *header,
                       const algorithm_type scheduling_algorithm )
{
    U8  prev;      /* pointer to previous task */
    U8  next;      /* pointer to next task     */

    /* initialise auxiliary pointers */
    prev = ZERO_CPV;
    next = header->q2_next;

    switch ( scheduling_algorithm )
    {
        case ALG_LLF:
            /* LLF: insert in least laxity order */
            while(
                     ( next is_not ZERO_CPV )
                     and
                     ( tasks[task]->laxity > tasks[next]->laxity )
                 )
            {
                prev = next;
                next = tasks[next]->q2_next;
            }
            break;

        case ALG_RM:
        case ALG_IRM:
        case ALG_DRM:
        case ALG_D_STAR:
        case ALG_DD_STAR:
        case ALG_D_OVER:
        case ALG_MLLF:
        case ALG_MUF:
        case ALG_MMUF:
        case ALG_SPT:
        case ALG_EDF_RTO:
        case ALG_EDF:
        case ALG_CYC:
        case ALG_RR:
        case ALG_RM_RTO:
        case ALG_EDF_BWP:
        case ALG_MMMUF:
        case ALG_ADAP_01:
        case ALG_ADAP_02:
        case ALG_ADAP_03:
        case ALG_ADAP_04:
        case ALG_ADAP_05:
        case ALG_ADAP_06:
        case ALG_ADAP_07:
        case ALG_MAX_ALG:
        default:
            /* do nothing */
			MESS_OUT_message_1( (S8*)"q2_insert() default switch with ",
                                (U32)scheduling_algorithm,
			                    MESSAGE_OP_ERROR );
            break;
    }


    if( (task is_not prev) and (task is_not next) )
    {
        if( prev is_not ZERO_CPV )
        {
            /* not at start */
            tasks[prev]->q2_next = task;
        }
        else
        {
            /* first ready task */
            header->q2_next = task;
        }

        if( next is_not ZERO_CPV )
        {
            /* not at end */
            tasks[next]->q2_prev = task;
        }

        /* set new ready task pointers */
        tasks[task]->q2_next = next;
        tasks[task]->q2_prev = prev;
    }
    else
    {
        /* task is already in que - abandon operation */
        MESS_OUT_message_1( (S8*)"Trying to insert task twice, abandoning ",
                       task, MESSAGE_OP_ERROR );
    }
} 

/******************************************************************************
*  Description : private helper returns first ready task in header queue
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : queue header pointer
******************************************************************************/
static U8 q_extract_first( struct task_type *header )
{
    U8 pointer; /* pointer to task */

    pointer = header->q0_next;

    if( pointer is_not ZERO_CPV )
    {
        /* queue is not empty, get following task id */
        header->q0_next = tasks[pointer]->q0_next;

        tasks[tasks[pointer]->q0_next]->q0_prev = ZERO_CPV;
    }

    return( pointer );
} 

/******************************************************************************
*  Description : private helper removes task from queue list 0.
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
******************************************************************************/
static void q0_extract( const U8 task )
{
    task_state_type state;
    struct task_type *header = ready_tasks; /* initialise to please Lint */

    U8 p;       /* auxiliary pointers */
    U8 q;       /* auxiliary pointers */

    state = tasks[task]->state;

    if( (state is TSK_READY)or(state is TSK_PREMPT) )
    {
        header = ready_tasks;
    }
    else if( state is TSK_REMOVED )
    {
        header = removed_tasks;
    }
    else if( state is TSK_IDLE )
    {
        header = idle_tasks;
    }
    else
    {
        /* not possible */
    }

    p = tasks[task]->q0_prev;
    q = tasks[task]->q0_next;

    if( p is ZERO_CPV )
    {
        /* task is first element of queue */
        header->q0_next = q;
    }
    else
    {
        tasks[p]->q0_next = q;
    }

    if( q is_not ZERO_CPV )
    {
        tasks[q]->q0_prev = p;
    }

    /* clear removed task pointers */
    tasks[task]->q0_prev = ZERO_CPV;
    tasks[task]->q0_next = ZERO_CPV;
} 

/******************************************************************************
*  Description : private helper removes task from queue list 1.
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : task number
*                task list header
******************************************************************************/
static void q1_extract( const U8 task, struct task_type *header )
{
    U8 p;       /* auxiliary pointers */
    U8 q;       /* auxiliary pointers */

    MESS_OUT_message_1( (S8*)"DEBUG: q1_extract() task ", 
                        task, MESSAGE_OP_DEBUG );

    p = tasks[task]->q1_prev;
    q = tasks[task]->q1_next;

    if( (p is ZERO_CPV) and (header->q1_next is task) )
    {
        /* task is first element of queue */
        header->q1_next = q;
    }
    else
    {
        /* task is in middle or end of queue */
        tasks[p]->q1_next = q;
    }

    if( q is_not ZERO_CPV )
    {
        /* task is not at the end of the queue */
        tasks[q]->q1_prev = p;
    }

    /* clear removed task pointers */
    tasks[task]->q1_prev = ZERO_CPV;
    tasks[task]->q1_next = ZERO_CPV;
} 

/******************************************************************************
*  Description : private helper removes task from queue list 2.
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : task number
*                task list header
******************************************************************************/
static void q2_extract( const U8 task, struct task_type *header )
{
    U8 p;       /* auxiliary pointers */
    U8 q;       /* auxiliary pointers */

    p = tasks[task]->q2_prev;
    q = tasks[task]->q2_next;

    if( p is ZERO_CPV )
    {
        /* task is first element of queue */
        header->q2_next = q;
    }
    else
    {
        tasks[p]->q2_next = q;
    }

    if( q is_not ZERO_CPV )
    {
        tasks[q]->q2_prev = p;
    }

    /* clear removed task pointers */
    tasks[task]->q2_prev = ZERO_CPV;
    tasks[task]->q2_next = ZERO_CPV;
} 

/******************************************************************************
*  Description : private helper returns TRUE if queue is empty
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : queue link number
*                pointer to queue header
******************************************************************************/
static boolean q_empty( const U8 queue, const struct task_type *header )
{
    boolean is_empty;

    switch( queue )
    {
        case ZERO_CPV:
            if(header->q0_next is ZERO_CPV)
            {
                is_empty = TRUE;
            }
            else
            {
                is_empty = FALSE;
            }

            /* is_empty = ( (boolean)(header->q0_next is ZERO_CPV) ); */
            break;
        case ONE_CPV:
            is_empty = ( (boolean)(header->q1_next is ZERO_CPV) );
            break;
        case TWO_CPV:
            is_empty = ( (boolean)(header->q2_next is ZERO_CPV) );
            break;
        default:
            is_empty = FALSE;
            MESS_OUT_message_1( (S8*)"RUN ERROR: default switch in q_empty() with ",
                           queue, MESSAGE_OP_ERROR );
            break;
    }
    return( is_empty );
}

/******************************************************************************
*  Description : private helper returns first task id in queue
*  Notes       : Adapted from code in Buttazzo's book (Buttazzo 2005)
*  Parameters  : queue link number
*                pointer to queue
******************************************************************************/
static U8 q_first_id( const U8 queue, const struct task_type *header )
{
    U8 id;

    switch( queue )
    {
        case ZERO_CPV:
            id = header->q0_next;
            break;
        case ONE_CPV:
            id = header->q1_next;
            break;
        case TWO_CPV:
            id = header->q2_next;
            break;
        default:
            id = ZERO_CPV;
            MESS_OUT_message_1( (S8*)"RUN ERROR: default switch in q_first_id() with ",
                           queue, MESSAGE_OP_ERROR );
            break;
    }
    return( id );
} 

/******************************************************************************
*  Description : private helper returns second task id in queue
*  Parameters  : pointer to queue
*  Returns     : first id in queue, or zero if empty
******************************************************************************/
static U8 q_second_id( const U8 queue, const struct task_type *header )
{
    U8 id;

    switch( queue )
    {
        case 0:
            id = tasks[header->q0_next]->q0_next;
            break;
        case 1:
            id = tasks[header->q1_next]->q1_next;
            break;
        case 2:
            id = tasks[header->q2_next]->q2_next;
            break;
        default:
            id = ZERO_CPV;
            MESS_OUT_message_1( (S8*)"RUN ERROR: default switch in q_second_id() with ",
                           queue, MESSAGE_OP_ERROR );
            break;
    }
    return( id );
} 

/******************************************************************************
*  Description : private helper calculates all task laxities. Steps through
*                task array calculating laxity.
******************************************************************************/
static void calculate_all_laxities( const U32 timeval )
{
    task_state_type state;
    U8 index;

    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        state = tasks[index]->state;

        if(
            (state is TSK_READY)
            or
            (state is TSK_RUN)
            or
            (state is TSK_SKIPPED)
            or
            (state is TSK_PREMPT)
          )
        {
            (void)calculate_laxity( index, timeval );
        }
    }
} 

/******************************************************************************
*  Description : private helper calculates laxity for task.
******************************************************************************/
static U32 calculate_laxity( const U8 task, const U32 timeval )
{
    U32 deadline;
    U32 time_left;

    deadline  = tasks[task]->abs_dline;
    time_left = tasks[task]->time_left;

    if( deadline > (timeval + time_left) )
    {
        tasks[task]->laxity = deadline - (timeval + time_left);
    }
    else
    {
        tasks[task]->laxity = ZERO_CPV;
    }

    return( tasks[task]->laxity );
} 

/******************************************************************************
*  Description : private helper calculates all task muf urgencies. Steps
*                through task array calculating.
*  Notes       : Adapted from by Stewart and Khoslas paper proposing the
*                Maximum Urgency First (MUF) algorithm
*                (Stewart and Khosla 1991).
******************************************************************************/
static void calculate_all_muf_urgencies( void )
{
    task_state_type state;
    U8 index;

    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        state = tasks[index]->state;

        if(
              (state is TSK_READY)
              or
              (state is TSK_RUN)
              or
              (state is TSK_SKIPPED)
              or
              (state is TSK_PREMPT)
          )
        {
            calculate_muf_urgency( index );
        }
    }
} 

/******************************************************************************
*  Function    : calculate_muf_urgency
*  Description : private helper calculates laxity for task. The following is
*                a bit map of the urgency word.
*
*                1 111-1111-1111-1111-1111-1111-1111 1111
*                | |                               | |  |
*                | |           27 bits             |  \/
*                | |                               |   |       4 bits
*                | |                               |   \- user defined priority
*                | |                               |
*                | \-------------   --------------/
*                |               \ /
*                |                \--inverse of laxity (dynamic priority)
*                |
*                |
*                \---------- msb=1 if in critical set, ELSE msb=0
*
*  Notes       : Adapted from by Stewart and Khoslas paper proposing the
*                Maximum Urgency First (MUF) algorithm
*                (Stewart and Khosla 1991).
******************************************************************************/
static void calculate_muf_urgency( const U8 task )
{
    const U32 l_shift = 4U;                /* laxity shift            */
    const U32 c_mask  = (U32)0x80000000u;  /* critical mask           */
    const U32 l_mask  = (U32)0x07ffffffu;  /* laxity mask (pre-shift) */
    const U32 p_mask  = (U32)0x0000000fu;  /* priority mask           */

    U32 critical;
    U32 i_laxity;

    /*  set critical bit */
    if( MUF_CRITICAL(task) is ONE_CPV )
    {
        critical = c_mask;
    }
    else
    {
        critical = ZERO_CPV;
    }
    
    MUF_URGENCY(task) = critical;

    /* set inverse laxity and load bits */
    i_laxity = (~tasks[task]->laxity) & l_mask;

    MUF_URGENCY(task) |= ( i_laxity << l_shift );

    /* load priority bits */
    MUF_URGENCY(task) |= ( tasks[task]->priority & p_mask );
} 

/******************************************************************************
*  Description : private helper returns active task with the lowest priority
*  Notes       : number #1 is highest priority
******************************************************************************/
static U8 get_lowest_priority_active_task( void )
{
    task_state_type state;
    U8 index;
    U8 lowest_priority = ZERO_CPV;
    U8 lowest_index    = ZERO_CPV;

    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        state = tasks[index]->state;

        if( (state is TSK_READY)
            or
            (state is TSK_RUN)
            or
            (state is TSK_SKIPPED)
            or
            (state is TSK_IDLE)
            or
            (state is TSK_PREMPT) )
        {
            if( (tasks[index]->priority > lowest_priority)
                and
                (tasks[index]->period_multiplyer is ZERO_CPV)
                and
                (PERIOD_FLEXIBLE(index) is ONE_CPV) )
            {
                /* indexed task has lower priority (higher number) */
                lowest_priority = tasks[index]->priority;
                lowest_index    = index;
            }
        }
    }

    MESS_OUT_message_1( (S8*)"get_lowest_priority_active_task() returns ",
                        lowest_index,
                        MESSAGE_OP_DEBUG ); 

    return( lowest_index );
}

/******************************************************************************
*  Description : private helper returns candidate task for task doubling
*                THE RULES:
*                (1) task must be classed as soft
*                (2) task must be in: TSK_READY, TSK_RUN, TSK_IDLE or TSK_PREMPT
*                (3) task must be lowest priority
*                (4) task doubling adds priority
*
*  Notes       : number #1 is highest priority
*  Returns     : candidate task index, or zero if unsuccessful
******************************************************************************/
static U8 get_active_task_priority( void )
{
    task_state_type state;
    U8 index;
    U8 lowest_priority = ~ZERO_CPV; /* need to invert - 1 is highest */
    U8 lowest_index    = ZERO_CPV;

    /* run through tasks calculating priority */
    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        calculate_active_task_priority( index );

        /* localise task state */
        state = tasks[index]->state;

        if( ((state is TSK_READY)
            or
            (state is TSK_RUN)
            or
            (state is TSK_IDLE)
            or
            (state is TSK_PREMPT))
            and
            (PERIOD_FLEXIBLE(index)) )
        {
            /* look for lowest value */

            if (tasks[index]->e_priority < lowest_priority)
            {
                /* indexed task has lower priority (higher number) */
                lowest_priority = tasks[index]->e_priority;
                lowest_index    = index;
            }
        }
    }

    return( lowest_index );
} 

/******************************************************************************
*  Description : private helper used by get_active_task_priority
*
*                The following is a bit map of the priority word.
*
*                1111-1111
*                |  | |  |
*                |  | \ /
*                |  |  |
*                |  |  \--- 4 bit user defined priority
*                \ /
*                 |
*                 \-------- 4 bit task doubling value priority
*
******************************************************************************/
static void calculate_active_task_priority( const U8 task )
{
    const U8 m_shift = (U8)FOUR_CPV; /* multiplier shift            */
    const U8 m_mask  = (U8)0x0fU;    /* multiplier mask (pre shift) */
    const U8 p_mask  = (U8)0x0fU;    /* priority mask               */

    U8 multiplyer;

    /* mask, shift and load the multiplier bits */
    multiplyer = tasks[task]->period_multiplyer & m_mask;
    tasks[task]->e_priority = (U8)(multiplyer << m_shift);

    /* load priority bits - need to invert because one is highest */
    tasks[task]->e_priority |= ( ~tasks[task]->priority & p_mask );
} 

/******************************************************************************
*  Description : private helper - tests if task is skippable
*                skippable means that not skipped count S is zero
*  Notes       : Based on the Red Task Only Skip-Over algorithm in paper
*                by Koren and Shasha (1995).
*  Parameters  : task - id of task
*                time
*  Returns     : TRUE - if skippable
******************************************************************************/
static U8 task_skippable( const U8 task )
{
    return( NOT_SKIPPED(task) is ZERO_CPV );
} 

/******************************************************************************
*  Description : private helper - decrements running tasks non skipped count
*  Notes       : Based on the Red Task Only Skip-Over algorithm in paper
*                by Koren and Shasha (1995).
******************************************************************************/
static void decrement_not_skipped_count( const U8 task )
{
    if( NOT_SKIPPED(task) > ZERO_CPV )
    {
        NOT_SKIPPED(task)--;
    }
} 

/******************************************************************************
*  Description : private helper - returns TRUE if task has time to meet
*                its deadline
*  Notes       : Based on the Red Task Only Skip-Over algorithm in paper
*                by Koren and Shasha (1995).
******************************************************************************/
static boolean can_meet_deadline( const U8 task, const U32 timeval )
{
    return(boolean)(tasks[task]->time_left + timeval);
} 

/******************************************************************************
*  Description : private helper calculates utility of task
******************************************************************************/
static U16 calculate_all_task_utilities( void )
{
    task_state_type state;
    U8 index;
    U16 task_utility;
    U16 task_set_utility = ZERO_CPV;

    /* step through all atsks */
    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        /* localise the state */
        state = tasks[index]->state;

        if(
              (state is TSK_READY)
              or
              (state is TSK_RUN)
              or
              (state is TSK_SKIPPED)
              or
              (state is TSK_IDLE)
              or
              (state is TSK_PREMPT)
          )
        {
            /* calculate the task utility */
            task_utility = calculate_task_utility( index );

            if( PERIOD_FLEXIBLE(index)is ONE_CPV )
            {
                task_set_utility += (task_utility
                                  >> tasks[index]->period_multiplyer );
            }
            else
            {
                task_set_utility += task_utility;
            }
        }
    }

    return( task_set_utility );
}


/******************************************************************************
*  Description : private helper calculates utility of task
*  Notes       : utility = period / duration
*  Returns     : utility of task (x 1000)
******************************************************************************/
static U16 calculate_task_utility( const U8 task )
{
    const U32 UTIL_MOD = 1000;

    struct UTL_div_type div;

    U16 utility;

    UTL_divide( (tasks[task]->c_duration * UTIL_MOD),
                 tasks[task]->period,
                 &div);

    utility = div.quotient;

    if( utility is_not tasks[task]->task_util )
    {
        /* task utility changed */
        MESS_OUT_message_3( (S8*)"Task ",
                            task,
                            (S8*)" utility updated from ",
                            tasks[task]->task_util,
                            (S8*)" to ",
                            utility, 
                            MESSAGE_OP_VERBOSE );

        /* update calculated utility */
        tasks[task]->task_util = utility;
    }
    return( utility );
} 

/******************************************************************************
*  Description : private helper - returns total value of estimated
*                task set utility (x1000) by summing the utility member of 
*                each task
******************************************************************************/
static U16 get_task_set_utility( void )
{
    task_state_type state;
    U8 index;
    U32 task_set_utility = ZERO_CPV;

    /* step through all tasks */
    for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
    {
        /* localise the task state */
        state = tasks[index]->state;

        /* select tasks with valid utility */
        if(
              (state is TSK_READY)
              or
              (state is TSK_RUN)
              or
              (state is TSK_SKIPPED)
              or
              (state is TSK_IDLE)
              or
              (state is TSK_PREMPT)
          )
        {
            if( PERIOD_FLEXIBLE(index) is ONE_CPV )
            {
                /* task period is flexible - check multiplyer */
                task_set_utility += ( tasks[index]->task_util
                                  >> tasks[index]->period_multiplyer );
            }
            else
            {
                /* just add the task utility to the total */
                task_set_utility += tasks[index]->task_util;
            }
        }
    }
    return( task_set_utility );
} 

/******************************************************************************
*  Description : private helper - searches removed queue for task with a lower
*                utility value than util
*  Parameters  : util - value to compare tasks in removed queue
*  Returns     : id of task with lower or equal util, or zero
******************************************************************************/
static U8 removed_queue_has_task_with_lower_utility( const U16 util )
{
    U8 q_ptr;
    U8 return_id;

    q_ptr = removed_tasks->q0_next; /* auxiliary pointer */

    while(
            (q_ptr is_not ZERO_CPV)
            and
            (tasks[q_ptr]->task_util > util)
         )
    {
        return_id = q_ptr;

        /* point to next element */
        q_ptr = tasks[q_ptr]->q0_next;
    }

    if( tasks[q_ptr]->task_util <= util )
    {
        /* if task has lower or equal utility to spare then return id */
        return_id = q_ptr;
    }
    else
    {
        return_id = ZERO_CPV;
    }

    return( return_id );
} 

/******************************************************************************
*  Description : private helper - doubles the period of a task by incrementing
*                period_multiplyer, if period_flexible flag is set
******************************************************************************/
static U8 double_task_period( const U8 task )
{
    U32 old_abs_dline;
    task_state_type state;
    U8 old_multiplyer;
    U8 new_multiplyer;

    /* double check that task period extension is enabled for this task */
    if( PERIOD_FLEXIBLE(task) is ONE_CPV )
    {
		/* report activity */
		TST_incr_test_report_value( TST_PER_DOUBLES );
		tasks[task]->per_doubles++;

        /* record value for verbose reporting */
        old_multiplyer = tasks[task]->period_multiplyer;

        /* increment and localise the multiplier */
        tasks[task]->period_multiplyer++;
        new_multiplyer = tasks[task]->period_multiplyer;

        /* report period update */
        MESS_OUT_message_1( (S8*)"Doubled period for task ", 
                            task, MESSAGE_OP_VERBOSE );

        MESS_OUT_message_3( (S8*)"Task ",
                            task,
                            (S8*)" period doubled from ",
                            tasks[task]->rel_dline << old_multiplyer,
                            (S8*)" to ",
                            tasks[task]->rel_dline << new_multiplyer,
                            MESSAGE_OP_VERBOSE );
        MESS_OUT_message_3( (S8*)"Task ",
                            task,
                            (S8*)" utility halved from ",
                            tasks[task]->task_util >> old_multiplyer,
                            (S8*)" to ",
                            tasks[task]->task_util >> new_multiplyer,
                            MESSAGE_OP_VERBOSE );

        /* if the task is ready or running update deadline now */
        state = tasks[task]->state;
        if(
              (state is TSK_READY)
              or
              (state is TSK_RUN)
              or
              (state is TSK_PREMPT)
          )
        {
            /* record for verbose reporting */
            old_abs_dline = tasks[task]->abs_dline;

            /* set the deadline */
            tasks[task]->abs_dline += (tasks[task]->rel_dline * new_multiplyer);

            MESS_OUT_message_3( (S8*)"Task ",
                                task,
                                (S8*)" deadline increased from ",
                                old_abs_dline,
                                (S8*)" to ",
                                tasks[task]->abs_dline, 
                                MESSAGE_OP_VERBOSE );
        }
    }
    return( tasks[task]->period_multiplyer );
} 

/******************************************************************************
*  Description : private helper - halves the period of a task
*  Notes       : It is not safe to shorten the deadline at this stage
******************************************************************************/
static void half_task_period( const U8 task )
{
    U8 old_multiplyer;
    U8 new_multiplyer;

    /* check that task period multiplier has value */
    if( tasks[task]->period_multiplyer > 0 )
    {
        /* remember the old multiplier value for reporting */
        old_multiplyer = tasks[task]->period_multiplyer;

        /* decrement the multiplier */
        tasks[task]->period_multiplyer--;
        new_multiplyer = tasks[task]->period_multiplyer;

       /* report period restoration */
        MESS_OUT_message_3( (S8*)"Task ",
                             task,
                             (S8*)" period halved from ",
                             tasks[task]->rel_dline << old_multiplyer,
                             (S8*)" to ",
                             tasks[task]->rel_dline << new_multiplyer, 
                             MESSAGE_OP_VERBOSE );
        MESS_OUT_message_3( (S8*)"Task ",
                             task,
                             (S8*)" utility doubled from ",
                             tasks[task]->task_util >> old_multiplyer,
                             (S8*)" to ",
                             tasks[task]->task_util >> new_multiplyer, 
                             MESSAGE_OP_VERBOSE );

    } /* end if tasks[task]->period_multiplyer > 0 */
}

/******************************************************************************
*  Description : private helper tests queue
******************************************************************************/
#if(0)
static void q_dump( const struct task_type *header )
{
    U32 q0_next;

    MESS_OUT_message( (S8*)"Q0-DUMP: ", MESSAGE_OP_DEBUG );

    q0_next = header->q0_next;

    while( q0_next is_not 0 )
    {
        MESS_OUT_message_2( (S8*)"Task", q0_next,
                            (S8*)" deadline = ", tasks[q0_next]->abs_dline,
                            MESSAGE_OP_DEBUG);

        q0_next = tasks[q0_next]->q0_next;
    }

} 
#endif


/******************************************************************************
*  Description : private helper tests queue 1
******************************************************************************/
#if(0)
static void q1_dump( const struct task_type *header )
{
    U32 q1_next;

    MESS_OUT_message( (S8*)"Q1-DUMP: ", MESSAGE_OP_DEBUG );

    q1_next = header->q1_next;

    while( q1_next is_not 0 )
    {
        if( algorithm is ALG_ADAP_06 )
        {
            MESS_OUT_message_4( (S8*)"Task", q1_next,
                           (S8*)" priority = ", tasks[q1_next]->priority,
                           (S8*)" q1_next = ", tasks[q1_next]->q1_next,
                           (S8*)" q1_prev = ", tasks[q1_next]->q1_prev,
                           MESSAGE_OP_DEBUG );
        }
        else if( algorithm is ALG_ADAP_07 )
        {
            MESS_OUT_message_4( (S8*)"Task", q1_next,
                           (S8*)" e-priority = ", tasks[q1_next]->e_priority,
                           (S8*)" q1_next = ", tasks[q1_next]->q1_next,
                           (S8*)" q1_prev = ", tasks[q1_next]->q1_prev,
                           MESSAGE_OP_DEBUG );
        }
        else
        {
            MESS_OUT_message_2( (S8*)"Task", q1_next,
                           (S8*)" priority = ", tasks[q1_next]->priority,
                           MESSAGE_OP_DEBUG);
        }

        q1_next = tasks[q1_next]->q1_next;
    }

} 
#endif


/******************************************************************************
*  Description : private debug helper - dumps task parameters
******************************************************************************/
#if(0)
static void task_params( void )
{
    U8 index;

    MESS_OUT_message( (S8*)"            TASK-1  TASK-2  TASK-3  TASK-4  TASK-5  TASK-6  TASK-7\n", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"            ======  ======  ======  ======  ======  ======  ======\n", MESSAGE_OP_INFO );

    OPR_task_report( "Abs Dline ",
                     (U32)tasks[ONE_CPV  ]->abs_dline,
                     (U32)tasks[TWO_CPV  ]->abs_dline,
                     (U32)tasks[THREE_CPV]->abs_dline,
                     (U32)tasks[FOUR_CPV ]->abs_dline,
                     (U32)tasks[FIVE_CPV ]->abs_dline,
                     (U32)tasks[SIX_CPV  ]->abs_dline,
                     (U32)tasks[SEVEN_CPV]->abs_dline );

    OPR_task_report( "q0_prev   ",
                     (U32)tasks[ONE_CPV  ]->q0_prev,
                     (U32)tasks[TWO_CPV  ]->q0_prev,
                     (U32)tasks[THREE_CPV]->q0_prev,
                     (U32)tasks[FOUR_CPV ]->q0_prev,
                     (U32)tasks[FIVE_CPV ]->q0_prev,
                     (U32)tasks[SIX_CPV  ]->q0_prev,
                     (U32)tasks[SEVEN_CPV]->q0_prev );

    OPR_task_report( "q0_next   ",
                     (U32)tasks[ONE_CPV  ]->q0_next,
                     (U32)tasks[TWO_CPV  ]->q0_next,
                     (U32)tasks[THREE_CPV]->q0_next,
                     (U32)tasks[FOUR_CPV ]->q0_next,
                     (U32)tasks[FIVE_CPV ]->q0_next,
                     (U32)tasks[SIX_CPV  ]->q0_next,
                     (U32)tasks[SEVEN_CPV]->q0_next );
} 
#endif
