/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  test.c                                                       *
*                                                                             *
* Description :  Manages all testing. In PC compilation produces system       *
*                time. Requests scheduler to load tasks.                      *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 10-03-05 |JRO| Created                                                      *
* 29-03-05 |JRO| Added date and test number announcement                      *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
* 17-04-05 |JRO| Added screen control for development system                  *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#ifdef DEV_SYSTEM
#include "timers.h"
#else
/*lint -e85 has dimension 0 */
#include <stdio.h>
/*lint +e85 has dimension 0 */
#endif
#include "messages_out.h"
#include "utils.h"
#include "scheduler.h"
#include "task.h"
#include "term.h"
#include "test_case.h"
#include "test.h"

#ifndef DEV_SYSTEM
static U32 timeval;
#endif

/* globally shared test variables */
mode_type mode;
U32 test_duration;
U8  test_num;

/* test case report variable array */
struct test_case_report_values_type
{
    S8   name[MAX_LINE_LEN];   /* name of parameter  */
    U32  value;                /* value of parameter */
};

static struct test_case_report_values_type test_case_report_values[TST_MAX_TST];
static U32 calculate_utility_value( void );
static void reset_report_values( void );


/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : TST_init
*  Description : Initialised test object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TST_init( void )
{
    UTL_string_copy( test_case_report_values[ZERO_CPV ].name, (S8*)"Test Duration    : " );
    UTL_string_copy( test_case_report_values[ONE_CPV  ].name, (S8*)"Pre-emptions     : " );
    UTL_string_copy( test_case_report_values[TWO_CPV  ].name, (S8*)"Skips            : " );
    UTL_string_copy( test_case_report_values[THREE_CPV].name, (S8*)"Removed tasks    : " );
    UTL_string_copy( test_case_report_values[FOUR_CPV ].name, (S8*)"Doubled periods  : " );
    UTL_string_copy( test_case_report_values[FIVE_CPV ].name, (S8*)"Missed deadlines : " );
    UTL_string_copy( test_case_report_values[SIX_CPV  ].name, (S8*)"Met deadlines    : " );
    UTL_string_copy( test_case_report_values[SEVEN_CPV].name, (S8*)"Value            : " );
    UTL_string_copy( test_case_report_values[EIGHT_CPV].name, (S8*)"Achieved util (%): " );

    reset_report_values();

} /* TST_init() */


/******************************************************************************
*  Function    : TST_soft_reset
*  Description : Soft reset of test object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TST_soft_reset( void )
{
    reset_report_values();

} /* TST_init() */


/******************************************************************************
*  Function    : TST_test
*  Description : manages testing and starts scheduler. Contains
*                main program loop. Conditionally compiles
*                When development system enters main loop, control is handed
*                over to serial and timer interrupt handlers.
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TST_test( void )
{
    #ifndef DEV_SYSTEM
	    /* request welcome screen from terminal */
        S8 input[MAX_LINE_LEN];
		sprintf( input, "DW\n" );
	    TRM_test_call( input );
	#else
	    /* request welcome screen from terminal */
	    TRM_test_call( (S8*)"DW\n" );
    #endif



    #ifdef DEV_SYSTEM
        /***************************************/
        /* compile for Keil development system */
        /***************************************/

        TIM_start();      /* start the timer              */
        while( TRUE );    /* development system MAIN LOOP */

    #else
        /********************************/
        /* compile for PC based  system */
        /********************************/

        /* main program loop (pc compile) */
        while( mode is_not MODE_ABORT )
        {
            if( mode is MODE_TERMINAL )
            {
                MESS_OUT_message( (S8*)"\nEnter an option... ", MESSAGE_OP_INFO_NOCR );

                /*lint -e421 Caution - 'gets(char *)' considered dangerous */
                (void)gets( input );
                /*lint +e421 */

				/* send a blank line to the screen */
                MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO );

				/* process the input data */
                TRM_test_call( input );

            }
            else
            {
                /* simulation mode - run scheduler */

                timeval++;                      /* increment timer       */

                TSK_run_task_cycle( timeval );  /* run task              */

                SCH_start( timeval );           /* call scheduler        */

                if( timeval > test_duration )
                {
                    mode = MODE_TERMINAL;       /* end test              */

                    TSK_end_of_test();          /* inform task object    */

                    TRM_end_of_test();          /* inform term object    */
                }
            } /* end if mode is MODE_TERMINAL */
        } /* end while */

    #endif  /* DEV_SYSTEM */

} /* test() */


/******************************************************************************
*  Function    : TST_start_simulation
*  Description : Starts the scheduler simulation
*  Notes       : Conditional compilation
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TST_start_simulation( void )
{
    MESS_OUT_message( (S8*)"Entering TST_start_simulation()", 
                      MESSAGE_OP_DEBUG );

    #ifndef DEV_SYSTEM
    timeval = 0;     /* reset timer    */
    SCH_start( 0 );  /* run scheduler  */
    #endif

} /* TST_start_simulation() */


/******************************************************************************
*  Function    : TST_continue_simulation
*  Description : continues the scheduler simulation
*  Parameters  : None
*  Returns     : none
******************************************************************************/
void TST_continue_simulation( void )
{
    #ifndef DEV_SYSTEM
        SCH_start( timeval );   /* run scheduler  */
    #endif

} /* TST_continue_simulation() */


/******************************************************************************
*  Function    : TSK_load_test
*  Description : loads a test sequence
*  Parameters  : test_num - number of test run
*  Returns     : none
******************************************************************************/
void TST_load_test( const U8 num )
{
    struct test_case_type test_case;
    struct test_case_type *test_case_ptr;

	MESS_OUT_message_1( (S8*)"TST_load_test(): called with ", 
						 num, MESSAGE_OP_DEBUG );

    test_case_ptr = &test_case;                       /* dereference pointer */
    test_case_ptr = TSC_get_test_case( num );         /* get test case       */
    test_num      = test_case_ptr->id;                /* test case number    */
    test_duration = test_case_ptr->length;            /* test length         */
    SCH_set_algorithm( test_case_ptr->algorithm );    /* algorithm           */
    SCH_add_task( ONE_CPV  , test_case_ptr->task_1 ); /* load task           */
    SCH_add_task( TWO_CPV  , test_case_ptr->task_2 ); /* load task           */
    SCH_add_task( THREE_CPV, test_case_ptr->task_3 ); /* load task           */
    SCH_add_task( FOUR_CPV , test_case_ptr->task_4 ); /* load task           */
    SCH_add_task( FIVE_CPV , test_case_ptr->task_5 ); /* load task           */
    SCH_add_task( SIX_CPV  , test_case_ptr->task_6 ); /* load task           */
    SCH_add_task( SEVEN_CPV, test_case_ptr->task_7 ); /* load task           */

} /* TST_load_test() */


/******************************************************************************
*  Function    : TST_incr_test_report_value
*  Description : increments parameter in test case report values array
*  Parameters  : parm  - index in report value array
*  Returns     : none
******************************************************************************/
void TST_incr_test_report_value( const test_case_parm_index_type parm )
{
    test_case_report_values[parm].value++;

} /* TST_incr_test_report_value() */


/******************************************************************************
*  Function    : TST_add_test_report_value
*  Description : adds parameter in test case report values array
*  Parameters  : parm  - index in report value array
*                val   - value to be added to value array
*  Returns     : none
******************************************************************************/
void TST_add_test_report_value( const test_case_parm_index_type parm,
                                const U32 val )
{
    test_case_report_values[parm].value += val;

} /* TST_add_test_report_value() */


/******************************************************************************
*  Function    : TST_set_test_report_value
*  Description : sets parameter in test case report values array
*  Parameters  : parm  - index in report value array
*                val   - value to be set to value array
*  Returns     : none
******************************************************************************/
void TST_set_test_report_value( const test_case_parm_index_type parm,
                                const U32 val )
{
    test_case_report_values[parm].value = val;

} /* TST_add_test_report_value() */


/******************************************************************************
*  Function    : TST_test_report
*  Description : reports test case results at end of test
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TST_test_report( void )
{
    U32 index;
	U32 utility;

    MESS_OUT_message( (S8*)"\nTest Case Report", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)  "================", MESSAGE_OP_RESULTS );

	/* calculate the utility value */
	utility = calculate_utility_value(); 
	TST_set_test_report_value( TST_UTILITY, utility );

    for( index=ZERO_CPV; index<(U32)TST_MAX_TST; index++ )
    {
        MESS_OUT_message_1( (S8*)test_case_report_values[index].name,
                            test_case_report_values[index].value, 
                            MESSAGE_OP_RESULTS );
    }

} /* TST_test_report() */


/******************************************************************************
*  Function    : calculate_utility_value
*  Description : private helper used by TST_test_report to calculate the
*                achieved task set utility value
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static U32 calculate_utility_value( void )
{
	struct UTL_div_type UTL_div; 
	
	UTL_divide( (test_case_report_values[TST_VALUE].value * ONE_HUNDRED_CPV), 
			    test_case_report_values[TST_DURATION].value, 
			    &UTL_div );

	return( UTL_div.quotient );    

} /* calculate_utility_value() */


/******************************************************************************
*  Function    : reset_report_values
*  Description : Resets values in the report array
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void reset_report_values( void )
{
    U32 index;

    /* initialise test report */
    for( index=ZERO_CPV; index<(U32)TST_MAX_TST; index++ )
    {
        test_case_report_values[index].value = ZERO_CPV;
    }

} /* reset_report_values() */
