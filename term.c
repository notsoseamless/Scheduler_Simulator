/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  *
*                                                                             *
* File Name   :  term.c                                                       *
*                                                                             *
* Description :  Terminal routines                                            *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 17-04-05 |JRO| Created                                                      *
* 10-07-05 |JRO| Added user options to menu                                   *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "messages_out.h"
#include "task.h"		   /* task object                   */

#if(0)
#include "task_gen.h"	   /* task list file generator      */
#endif

#ifdef DEV_SYSTEM
	#include "serial.h"    /* access to hardware serial i/o */
#endif

#include "utils.h"		   /* general utilities             */
#include "rt_clock.h"	   /* real time clock timer         */			   
#include "scheduler.h"     /* main scheduler                */
/* #include "task_file.h"  */

#ifndef DEV_SYSTEM
	/*lint -e85 has dimension 0 */
    #include <stdio.h>     /* gcc output libraries          */
	/*lint +e85 has dimension 0 */
#else
    #include "timers.h"    /* hardware timer                */
#endif

#include "test.h"          /* test manager                  */
#include "test_case.h"     /* test case container           */
#include "initialise.h"    /* initialisation functions      */
#include "term.h"          /* interface file                */

static boolean continuous_loop = FALSE;
static U8 start_test_case;
static U8 stop_test_case;

/******************************************************************************
*  PRIVATE FUNCTION PROTOTYPES
******************************************************************************/
static void process_input( const S8 *line );
static void load_test_case( const S8 *line );
static void load_and_run_test_case( const S8 *line );
static void run_test_case( void );
static void update_algorithm( const S8 *line );
static void update_duration( const S8 *line );
static void display_test( void );
static void set_up_loop_run( const S8 *line );
static void loop_through_all_test_cases( void );
static void print_welcome_screen( void );
static void print_menu( void );
static void why( void );
static void print_maintenance_menu( void );
static void print_key( void );
static void print_header( void );
static void print_algorithms( void );
static void print_test_cases( void );

static U32 get_numeric_input( const S8 *line,
                              const U8 ignore,
                              const U32 min,
                              const U32 max );

static U32 is_number( const S8 c );
static void rename_output_file( const U8 test_case );
static void calender_info( void );


/******************************************************************************
*                              PUBLIC FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : TRM_call
*  Description : Called by UART1 interrupt - reads serial input buffer
*                and following a CR, call helper to process any characters
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TRM_call( void )
{
#ifdef DEV_SYSTEM
    const U32 cr = 0x0d;

    static S8  line[MAX_LINE_LEN];
    static U32 line_pointer = ZERO_CPV;

    U32 input;

    input = SER_getchar();              /* call serial port for character  */

    /* build line with received characters, until a CR is found */
    if( input is_not cr )
    {
        /* add character to the end of the line */
        
        line[line_pointer] = (S8)input; /* add character to line buffer    */

        (void)SER_putchar( input );     /* echo character to serial port   */

        line_pointer++;                 /* increment line position pointer */
    }
    else
    {
        /* CR found, terminate and process the line                        */

        line[line_pointer] = '\0';      /* terminate line                  */

        line_pointer = ZERO_CPV;        /* reset line position pointer     */

        (void)SER_putchar( cr );        /* echo CR to serial port          */

        process_input( line );          /* process the input line          */
    }
#endif
} /* TRM_call() */


/******************************************************************************
*  Function    : TRM_test_call
*  Description : Called by test program to input from keyboard
*  Parameters  : welcome request
*  Returns     : none
******************************************************************************/
void TRM_test_call( const S8 *line )
{
#ifndef DEV_SYSTEM
    process_input( line );
#endif
} /* TRM_test_call() */


/******************************************************************************
*  Function    : TRM_end_of_test
*  Description : Informs object that a test case has completed
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TRM_end_of_test( void )
{
    loop_through_all_test_cases();

} /* TRM_end_of_test() */


/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/
	 

/******************************************************************************
*  Function    : process_input
*  Description : private helper - processes keyboard input
*  Parameters  : line   -  buffer containing user input for processing
*  Returns     : none
******************************************************************************/
static void process_input( const S8 *line )
{
	MESS_OUT_message_S2( (S8*)"process_input() data: ", 
	                     line, 
	                     MESSAGE_OP_DEBUG );

    switch( line[ZERO_CPV] )
    {
        case 'l':
        case 'L':
            /* load test case */
            load_test_case( line );
            break;

        case 'r':
        case 'R':
            /* run or load and run the test case */
            load_and_run_test_case( line );
            break;

        case 'k':
        case 'K':
            /* print key to test results */
            print_key();
            break;

        case 'v':
        case 'V':
            MESS_OUT_toggle_verbose_mode();
            break;

        case 'c':
        case 'C':
            switch( line[ONE_CPV] )
            {
                case 'a':
                case 'A':
                    update_algorithm( line );
                    break;
                case 'd':
                case 'D':
                    update_duration( line );
                    break;
                default:
                    /* invalid input */
    				MESS_OUT_message_S2( (S8*)"process_input() invalid input: ", 
                    line,
                    MESSAGE_OP_ERROR );
                    print_menu();
                    break;
            } /* end of switch( line[ONE_CPV] ) */
            break;

        case 'd':
        case 'D':
            switch( line[ONE_CPV] )
            {
                case 'l':
                case 'L':
                    /* display loaded test case set-up */
                    display_test();
                    break;
                case 'a':
                case 'A':
                    /* display algorithm list */
                    print_algorithms();
                    break;
                case 't':
                case 'T':
                    /* display test case list */
                    print_test_cases();
                    break;
                case 'w':
                case 'W':
                    print_welcome_screen();
                    break;
                default:
                    /* invalid input */
    				MESS_OUT_message_S2( (S8*)"process_input() invalid input: ", 
                    line,
                    MESSAGE_OP_ERROR );
                    print_menu();
                    break;
            } /* end of switch( line[ONE_CPV] ) */
            break;

        /* this section only used on the PC build */
        #ifndef DEV_SYSTEM
        case 's':
        case 'S':
            MESS_OUT_toggle_screen_results_mode();
            break;

        case 't':
        case 'T':
            /* task list generator */
            break;
        case 'a':
        case 'A':
            /* Abort back to main() */
            mode = MODE_ABORT;
            break;
        #endif

        case 'z':
        case 'Z':
            /* explain WHY we are doing all this */
            why();
            break;

        case 'm':
        case 'M':
            /* maintenance menu */
            switch (line[ONE_CPV])
            {
                case 'm':
                case 'M':
                    print_maintenance_menu();
                    break;

                case 's':
                case 'S':
                    UTL_type_report();
                    break;

                case 'd':
                case 'D':
                    /* toggle debug mode */
                    MESS_OUT_toggle_debug_mode();
                    break;

                case 't':
                case 'T':
                    MESS_OUT_message_1( (S8*)"Time : ", 
                                        (U32)CLK_get_real_time(), 
                                        MESSAGE_OP_INFO );
                    
                    MESS_OUT_message_1( (S8*)"Clocks per second : ", 
                                        (U32)CLK_clocks_per_second(), 
                                        MESSAGE_OP_INFO );
                    break;

                case 'r':
                case 'R':
                    /* loop through all the test cases */
                    set_up_loop_run( line );
                    break;

                case 'c':
                case 'C':
                    /* calendar info */
                    calender_info();
                    break;

				case 'z':
				case 'Z':
					/* this is a test option */
   					MESS_OUT_message( (S8*)"Running test", MESSAGE_OP_LOG );
					/* TSC_report_test_case_notes(1);	*/
   					MESS_OUT_message( (S8*)"Test ended", MESSAGE_OP_LOG );
					break;

                default:
                    /* invalid input */
    				MESS_OUT_message_S2( (S8*)"process_input() invalid input: ", 
                    line,
                    MESSAGE_OP_ERROR );
                    print_menu();
                    break;
            } /* end of switch( line[ONE_CPV] ) */
            break;

        default:
            /* print help screen */
            print_menu();
            break;

    } /* end switch line[ZERO_CPV] */

} /* process_input() */


/******************************************************************************
*  Function    : load_test_case
*  Description : private helper - loads a test case, validating input
*  Parameters  : line  -  pointer to input line received from UART
*  Returns     : none
******************************************************************************/
static void load_test_case( const S8 *line )
{
    U8 input;

    MESS_OUT_message_S2( (S8*)"load_test_case() data: ", 
                         line,
                         MESSAGE_OP_DEBUG );

    /* get the user input */
    input = (U8)get_numeric_input( line, ONE_CPV, ZERO_CPV, MAX_TEST_CASES );

    if( input > ZERO_CPV )
    {
        /* request a soft reset */
        INIT_soft_reset();

        test_num = input;

        /* load test case */
        TST_load_test( test_num );

        /* display test setup */
        display_test();	

        /* report to screen */
        MESS_OUT_message_1( (S8*)"Loaded test case ", test_num, MESSAGE_OP_LOG );
    }
    else
    {
        /* no valid test case - print out error message */
        MESS_OUT_message((S8*)"load_test_case(): Invalid test case", MESSAGE_OP_ERROR );
        
        MESS_OUT_message_1( (S8*)"Valid input between 1 and ",
                            MAX_TEST_CASES, 
                            MESSAGE_OP_ERROR );
    }

} /* load_test_case() */


/******************************************************************************
*  Function    : load_and_run_test_case
*  Description : private helper - loads and runs a loaded test case
*  Parameters  : none
*  Returns     : none
*****************************************************************************/
static void load_and_run_test_case( const S8 *line )
{
    U8 input;

    MESS_OUT_message_S2( (S8*)"load_and_run_test_case() called with: ", 
                         line,
                         MESSAGE_OP_DEBUG );

    /* get the user input */
    input = (U8)get_numeric_input( line, ONE_CPV, ZERO_CPV, MAX_TEST_CASES );

    if( input > ZERO_CPV )
    {
        /* request a soft reset */
        INIT_soft_reset();

        test_num = input;

        /* load test case */
        TST_load_test( test_num );

        /* now run the test case */
        run_test_case();
    }
    else
    {
        /* attempt to run a previously loaded test case */
        run_test_case();
    }

} /* load_and_run_test_case() */


/******************************************************************************
*  Function    : run_test_case
*  Description : private helper - runs a loaded test case
*  Parameters  : none
*  Returns     : none
*****************************************************************************/
static void run_test_case( void )
{
    MESS_OUT_message( (S8*)"run_test_case() called", MESSAGE_OP_DEBUG );

    MESS_OUT_message( (S8*)"\n\n\n", MESSAGE_OP_INFO );

    if( test_num > ZERO_CPV )
    {
        /* report to screen */
        MESS_OUT_message_1( (S8*)"Running test case ", test_num, MESSAGE_OP_LOG );

        rename_output_file( test_num ); /* rename output file           */

        display_test();                 /* report test conditions       */

        TSK_start_of_test();            /* warn task object             */

        /* start the timer */
        #ifdef DEV_SYSTEM
            TIM_start_simulation();     /* start simulation             */
        #else
            TST_start_simulation();     /* start simulation             */
        #endif

        /* change mode to simulation */
        mode = MODE_SIMULATION;         /* change mode                  */
    }
    else
    {
        /* no test case loaded - print out error message */
        MESS_OUT_message( (S8*)"run_test_case() No test case loaded", MESSAGE_OP_ERROR );
    } 

} /* run_test_case() */


/******************************************************************************
*  Function    : update_algorithm
*  Description : private helper - allows user to change scheduling algorithm
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void update_algorithm( const S8  *line )
{
    U8 filtered_input;

    MESS_OUT_message_S2( (S8*)"update_algorithm() called with",
                          line,
                          MESSAGE_OP_DEBUG );

    /* update algorithm */
    filtered_input = (U8)get_numeric_input( line, TWO_CPV, ONE_CPV, (U32)ALG_MAX_ALG );

    if( filtered_input > ZERO_CPV )
    {
        /* update algorithm */
        SCH_set_algorithm( (algorithm_type)(filtered_input-1) );

        /* display updated algorithm */
        MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO );
        SCH_what_algorithm( MESSAGE_OP_INFO );
        MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO );
    }
    else
    {
        /* print out error message */
        MESS_OUT_message( (S8*)"update_algorithm() invalid algorithm", MESSAGE_OP_ERROR );

        MESS_OUT_message_1( (S8*)"Valid algorithm is between 1 and ",
                            (U32)ALG_MAX_ALG, 
                            MESSAGE_OP_ERROR );
    }

} /* update_algorithm() */


/******************************************************************************
*  Function    : update_duration
*  Description : private helper - allows user to change test duration
*  Parameters  : pointer to line input
*  Returns     : none
******************************************************************************/
static void update_duration( const S8* const line )
{
    const U16 max_duration = 0xffff;
    U16 input;

    MESS_OUT_message_S2( (S8*)"update_duration() called with",
                          line,
                          MESSAGE_OP_DEBUG );

    /* update test tick duration of test */
    /* get the test number */
    input = (U16)get_numeric_input( line, TWO_CPV, ONE_CPV, max_duration );
    if( input > ZERO_CPV )
    {
        test_duration = input;  /* update test duration */
        MESS_OUT_message_1( (S8*)"New duration = ", 
                            test_duration, 
                            MESSAGE_OP_LOG );
    }
    else
    {
        /* print out error message */
        MESS_OUT_message(   (S8*)"update_duration() duration out of range", MESSAGE_OP_ERROR );
        MESS_OUT_message_1( (S8*)"Valid range is between 1 and ",
                            max_duration, 
                            MESSAGE_OP_ERROR );
    }

} /* update_duration() */


/******************************************************************************
*  Function    : display_test
*  Description : private helper - displays test set-up
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void display_test( void )
{
    MESS_OUT_message( (S8*)"display_test() called", MESSAGE_OP_DEBUG );

    if( test_num > ZERO_CPV )
    {
        /* test case */

		/* report to results */
        MESS_OUT_message( (S8*)"Test case:", MESSAGE_OP_RESULTS );
        TSC_report_test_case( test_num, MESSAGE_OP_RESULTS );
		TSC_report_test_case_notes( test_num );

        /* report algorithm */
        MESS_OUT_message( (S8*)"Algorithm:", MESSAGE_OP_RESULTS );
        SCH_what_algorithm( MESSAGE_OP_RESULTS );

        /* test duration */
        MESS_OUT_message_1( (S8*)"Test duration:", 
                            test_duration, 
                            MESSAGE_OP_RESULTS );

        /* report task parameters */
        TSK_task_report();

        /* report system status */
        MESS_OUT_report_status( MESSAGE_OP_RESULTS );
    }
    else
    {
        MESS_OUT_message( (S8*)"display_test() No test case loaded", MESSAGE_OP_INFO );
    }

} /* display_test() */


/******************************************************************************
*  Function    : set_up_loop_run
*  Description : private helper - sets up a loop test run
*                Runs all test cases, unless user enters parameters for
*                first and last test case number
*  Parameters  : pointer to user line input 
*  Returns     : none
******************************************************************************/
static void set_up_loop_run( const S8  *line )
{
    U8 input;

    MESS_OUT_message_S2( (S8*)"set_up_loop_run() data: ", 
                         line,
                         MESSAGE_OP_DEBUG );

    /* get the first user input */
    input = (U8)get_numeric_input( line, THREE_CPV, ZERO_CPV, MAX_TEST_CASES );

    if( input > ZERO_CPV )
    {
        start_test_case = input;

        /* get second user input */
        input = (U8)get_numeric_input( line, FIVE_CPV, ZERO_CPV, MAX_TEST_CASES );
        if( (input > start_test_case) )
        {
            stop_test_case  = input;
        }
        else
        {
            /* no valid stop number - set default */
            stop_test_case  = MAX_TEST_CASES;
        }
    }
    else
    {
        /* no valid numbers - set defaults */
        start_test_case = ONE_CPV;
        stop_test_case  = MAX_TEST_CASES;
    }

    MESS_OUT_message_2( (S8*)"start: ", start_test_case,
                        (S8*)"stop: ",  stop_test_case,
                        MESSAGE_OP_DEBUG );

    continuous_loop = TRUE;
    loop_through_all_test_cases();

} /* set_up_loop_run() */


/******************************************************************************
*  Function    : loop_through_all_test_cases
*  Description : re-entrant private helper - runs all test cases
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void loop_through_all_test_cases( void )
{
    static U8 test_case_number = ZERO_CPV;

    test_case_number++;

    if( test_case_number is ONE_CPV )
    {
        /* this is the first loop - set default */
        test_case_number = start_test_case;
    }

    if( (test_case_number < stop_test_case) and (continuous_loop) )
    {
        /* do a soft reset */
        INIT_soft_reset();

        /* set the test case number global */
        test_num = test_case_number;

        /* load next test case */
        TST_load_test( test_num );

        /* run the test case */
        run_test_case();
    }
    else
    {
        test_case_number = 0;
        continuous_loop  = FALSE;
        test_num         = 0;
    }

} /* simulation_loop_report() */


/******************************************************************************
*  Function    : print_welcome_screen
*  Description : private helper - prints welcome screen for user
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_welcome_screen( void )
{
	print_header();
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=          enter ? for help          =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );

} /* print_welcome_screen() */


/******************************************************************************
*  Function    : print_key
*  Description : private helper - prints key message for user
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_key( void )
{
	print_header();
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     ++++      Running              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     LATE      Running & Late       =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     OVER      Running & Over       =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     sk..      Skipped              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     rm..      Removed              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     i...      Idle                 =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     r...      Ready                =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     RL..      Ready & Late         =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     RO..      Ready & Over         =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     p...      Pre-empted           =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     PL..      Pre-empted & Late    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     PO..      Pre-empted & Over    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     w...      Waiting              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     s...      Sleep                =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=     <-DDDD->  Deadline             =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================\n", MESSAGE_OP_INFO );

} /* print_key() */

/******************************************************************************
*  Function    : print_header
*  Description : private helper - print common screen header
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_header( void )
{
    MESS_OUT_message( (S8*)"\n"                                  , MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"\n"                                  , MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"\n"                                  , MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "======================================", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "=         SCHEDULER SIMULATOR        =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "=           "                          , MESSAGE_OP_INFO_NOCR );
    MESS_OUT_message( (S8*) version_num                             , MESSAGE_OP_INFO_NOCR );
    MESS_OUT_message( (S8*) "           ="                          , MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*) "=                                    =", MESSAGE_OP_INFO );


} /* print_header() */

/******************************************************************************
*  Function    : print_algorithms
*  Description : private helper - prints algorithm list
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_algorithms( void )
{
    U32 index;

    MESS_OUT_message( (S8*)"\n\nScheduling Algorithms\n", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)    "=====================\n", MESSAGE_OP_INFO );

    for( index=ZERO_CPV; index<(U32)ALG_MAX_ALG; index++ )
    {
        SCH_report_algorithm( (algorithm_type)index, MESSAGE_OP_INFO );
    }
    MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO );

} /* print_algorithms() */


/******************************************************************************
*  Function    : print_test_cases
*  Description : private helper - prints test case list
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_test_cases( void )
{
    U8 index;

    MESS_OUT_message( (S8*)"\n\nTest Cases\n", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)    "==========\n", MESSAGE_OP_INFO );

    for( index=1; index<MAX_TEST_CASES-1; index++ )
    {
        TSC_report_test_case( index, MESSAGE_OP_INFO );
    }
    MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO );

} /* print_test_cases() */


/******************************************************************************
*  Function    : print_menu
*  Description : private helper - prints menu for user
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_menu( void )
{
	print_header();
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   Ln  - Load nth test case         =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   R   - Run test case              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   K   - Key to test results        =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   V   - Toggle verbose mode        =", MESSAGE_OP_INFO );
#ifndef DEV_SYSTEM
    MESS_OUT_message( (S8*)"=   S   - Toggle screen results      =", MESSAGE_OP_INFO );
#endif
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   CAn - Change to nth algorithm    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   CDn - Change duration to n       =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   DL  - Display loaded test        =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   DA  - Display all algorithms     =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   DT  - Display all test cases     =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
#ifndef DEV_SYSTEM
    MESS_OUT_message( (S8*)"=   T   - Generate task list file    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
#endif
#ifndef DEV_SYSTEM
    MESS_OUT_message( (S8*)"=   A   - Abort program              =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
#endif
    MESS_OUT_message( (S8*)"=   Z   - Why?                       =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   H or ? This menu                 =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================\n", MESSAGE_OP_INFO );

} /* print_menu() */


/******************************************************************************
*  Function    : why
*  Description : private helper - prints the reasons why I'm doing this
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void why( void )
{
	print_header();
    MESS_OUT_message( (S8*)"=                WHY?                =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=      An Investigation into an      =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=       Adaptive Scheduler for       =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=    Overloaded Real-Time Systems    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                 by                 =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=            John Oldman             =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );

} /* why() */


/******************************************************************************
*  Function    : print_maintenance_menu
*  Description : private helper - prints maintenance menu for user
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void print_maintenance_menu( void )
{
	print_header();
    MESS_OUT_message( (S8*)"=          MAINTENANCE MENU          =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   MS  - Type sizes                 =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   MT  - Clock value                =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   MD  - Debug mode                 =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   MC  - Compile date & time        =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=   MR  - Run all test cases         =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"=                                    =", MESSAGE_OP_INFO );
    MESS_OUT_message( (S8*)"======================================", MESSAGE_OP_INFO );

} /* print_maintenance_menu() */


/******************************************************************************
*  Function    : get_numeric_input
*  Description : private helper - helper to get a number from the line input
*                at the keyboard.
*  Parameters  : message string - invitation message for input
*                ignore         - part of line to ignore
*                min            - minimum valid value
*                max            - maximum valid value
*  Returns     : extracted number, or zero
******************************************************************************/
static U32 get_numeric_input( const S8  *line,
                              const U8  ignore,
                              const U32 min,
                              const U32 max )
{
    const U32 max_count = 6; /* should be a long enough line length */
    const U32 mult[] =
    {
        (U32)1, (U32)10, (U32)100, (U32)1000, (U32)10000, (U32)100000
    };

    U32 total;
    U32 index;
	U32 max_index;
    U32 mult_index;
    U32 count;
    boolean silly = FALSE;

	/* check size of line */
	max_index = sizeof( line );

    /* strip white space */

    /* measure length of line */
    index = ignore;
    count = ZERO_CPV;
    while( (line[index] is_not '\0')and(count < max_count)and(index<max_index) )
    {
        count++;    /* increment count */
        index++;    /* increment index */
    }

    if( count > (max_count-1) )
    {
        /* too long */
        total = ZERO_CPV;
    }
    else
    {
        index--;
        mult_index = ZERO_CPV;
        total = ZERO_CPV;
        while( (count > ZERO_CPV)and(!silly) )
        {
            if( !is_number( line[index]) )
            {
                /* invalid character found */
                silly = TRUE;
            }
            else
            {
                total += ( ( line[index] & 0x0f )*(mult[mult_index]) );
            }
            count--;
            index--;
            mult_index ++;
        }

        /* test input */
        if( (total < min) or (total > max) or (silly is TRUE) )
        {
            total = ZERO_CPV;
        }
    }

    return( total );

} /* get_numeric_input() */


/******************************************************************************
*  Function    : is_number
*  Description : private helper - returns true if c is a number
*  Parameters  : c - character to test
*  Returns     : boolean result
******************************************************************************/
static U32 is_number( const S8 c )
{
    const S8 MIN_ASCII = 0x30;
    const S8 MAX_ASCII = 0x39;

    return( (c >= MIN_ASCII) and (c <= MAX_ASCII) );

} /* is_number() */


/******************************************************************************
*  Function    : rename_output_file
*  Description : private helper - makes request to update the test output
*                file name
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void rename_output_file( const U8 test_case )
{
    #ifndef DEV_SYSTEM
        S8 new_file_name[MAX_LINE_LEN];

        sprintf( new_file_name, "Test case %03d.txt", test_case );

        MESS_OUT_rename_results_file( new_file_name );
    #endif

} /* rename_output_file()*/


/******************************************************************************
*  Function    : calender_info
*  Description : private helper - outputs compile date and time
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void calender_info( void )
{
    const S8 compile_date[] = "Compile Date: " __DATE__; /* set up the  */
    const S8 compile_time[] = "Compile Time: " __TIME__; /* date macros */

    MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO  );
    MESS_OUT_message( compile_date, MESSAGE_OP_INFO  );
    MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO  );
    MESS_OUT_message( compile_time, MESSAGE_OP_INFO  );
    MESS_OUT_message( (S8*)"\n", MESSAGE_OP_INFO  );

    #ifdef DEV_SYSTEM
        MESS_OUT_message( (S8*)"Compiled using Development System", MESSAGE_OP_INFO );
    #endif
    #ifdef DJGPP
        MESS_OUT_message( (S8*)"Compiled using DJGPP", MESSAGE_OP_INFO );
    #endif
    #ifdef __GNUC__
        MESS_OUT_message( (S8*)"Compiled using GCC", MESSAGE_OP_INFO );
    #endif
    #ifdef _lint
        MESS_OUT_message( (S8*)"Compiled for LPLint", MESSAGE_OP_INFO );
    #endif

} /* calender_info() */
