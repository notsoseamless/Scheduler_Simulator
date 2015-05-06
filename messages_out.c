/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :  SCHEDULER SIMULATOR                                          *
*                                                                             *
* File Name   :  messages_out.c                                               *
*                                                                             *
* Description :  This module is responsible for managing all message output   *
*                from this application                                        *                                                                             *
*                                                                             *
*******************************************************************************
*  NOTE: This file is adapted from part of the uVision/ARM development tools  *
*        Supplied with the uVision IDE                                        *
*        (c) KEIL ELEKTRONIK GmbH 2002-2004                                   *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 13-09-05 |JRO| Created, to replace out_res.c, with improved encapsulation.  *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#ifdef DEV_SYSTEM


/***************************************/
/* compile for Keil development system */
/***************************************/ 
#include "serial.h"
#include "rt_clock.h"
#include "messages_out.h"


/******************************************************************************
*                            PRIVATE DATA TYPES								  *
******************************************************************************/
static boolean debug_mode;
static boolean verbose_mode;


/******************************************************************************
*                       PRIVATE FUNCTION PROTOTYPES				     		  *
******************************************************************************/
static void putstr( const S8 *p );
static void build_dec( const U32 num, S8 *num_string );
static void build_dec8( const U8 num, S8 *num_string );
static void build_dec16( const U16 num, S8 *num_string );
static void build_dec32( const U32 num, S8 *num_string );
static S8 dec2str( const U32 num );


/******************************************************************************
*  Description : dummy function
******************************************************************************/
void MESS_OUT_initialise( void )
{
	/* initialise defaults reporting modes */
	debug_mode          = FALSE; 
	verbose_mode        = TRUE;
	
} /* MESS_OUT_initialise() */


/******************************************************************************
*  Description : dummy function
******************************************************************************/
void MESS_OUT_close( void ){}


/******************************************************************************
*  Description : passes messages to the handlers
******************************************************************************/
void MESS_OUT_message( const S8* const message, const message_class class )
{
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();

		putstr( (S8*)message );

	   	if( class is_not MESSAGE_OP_INFO_NOCR )
        {
    	   	/* new line */
    		putstr( (S8*)"\n" );
        }

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message() */


/******************************************************************************
*  Description : two message strings
******************************************************************************/
void MESS_OUT_message_S2( const S8* const message1, const S8* const message2, 
                          const message_class class )
{
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();

		/* output first string */
		if( message1 is_not NULL )
		{
			putstr( message1 );
		}

		/* output space between strings */
		if( message1 is_not NULL )
		{
			putstr( (S8*)" " );
		}

		/* output second string */
		if( message2 is_not NULL )
		{
			putstr( message2 );
		}

		/* new line */
		putstr( (S8*)"\n" );

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message_S2() */


/******************************************************************************
*  Description : message with one number
*  Parameters  : message - the message
*                num     - number parameter
*                class   - the message type
******************************************************************************/
void MESS_OUT_message_1( const S8* const message, const U32 num, 
                         const message_class class )
{
	S8 num_string[13];
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();

		/* output string part, if any */
		if( message is_not NULL )
		{
			putstr( (S8*)message );
		}

		/* output num part */
		build_dec( num, num_string );
		putstr( (S8*) num_string );

		/* new line */
		putstr( (S8*)"\n" );

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message_1() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_2( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
                         const message_class class )
{
	S8 num_string[13];
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();


		/* output string part, if any */
		if( message_1 is_not NULL )
		{
			putstr( (S8*)message_1 );
		}

		build_dec( num_1, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_2 is_not NULL )
		{
			putstr( (S8*)message_2 );
		}

		build_dec( num_2, num_string );
		putstr( (S8*) num_string );

		/* new line */
		putstr( (S8*)"\n" );

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message_2() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                message_3 - the message
*                num_3     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_3( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
                         const message_class class )
{

	S8 num_string[13];
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();

		/* output string part, if any */
		if( message_1 is_not NULL )
		{
			putstr( (S8*)message_1 );
		}

		build_dec( num_1, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_2 is_not NULL )
		{
			putstr( (S8*)message_2 );
		}

		build_dec( num_2, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_3 is_not NULL )
		{
			putstr( (S8*)message_3 );
		}

		build_dec( num_3, num_string );
		putstr( (S8*) num_string );

		/* new line */
		putstr( (S8*)"\n" );

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message_3() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                message_3 - the message
*                num_3     - number parameter
*                message_4 - the message
*                num_4     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_4( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
						 const S8* const message_4, const U32 num_4,
                         const message_class class )
{
	S8 num_string[13];
	U32 temp_time;

	if( ((class is MESSAGE_OP_VERBOSE)and(verbose_mode is FALSE))
	     or 
		((class is MESSAGE_OP_DEBUG)and(debug_mode is FALSE)) )
	{
		/* do nothing */
	}
	else
	{
		/* process message */

		/* stop timer */
		temp_time = CLK_end_timer();

		/* output string part, if any */
		if( message_1 is_not NULL )
		{
			putstr( (S8*)message_1 );
		}

		build_dec( num_1, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_2 is_not NULL )
		{
			putstr( (S8*)message_2 );
		}

		build_dec( num_2, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_3 is_not NULL )
		{
			putstr( (S8*)message_3 );
		}

		build_dec( num_3, num_string );
		putstr( (S8*) num_string );

		/* output string part, if any */
		if( message_4 is_not NULL )
		{
			putstr( (S8*)message_4 );
		}

		build_dec( num_4, num_string );
		putstr( (S8*) num_string );

		/* new line */
		putstr( (S8*)"\n" );

		/* restart timer */
		CLK_start_timer( temp_time );
	}

} /* MESS_OUT_message_4() */


/******************************************************************************
*  Description : outputs a report string of task parameters to output
*  Parameters  : param           : title string
*                task_1 - task_7 : task states.
******************************************************************************/
void MESS_OUT_task_report(  const S8* const param,
							const U32 task_1,
							const U32 task_2,
							const U32 task_3,
							const U32 task_4,
							const U32 task_5,
							const U32 task_6,
							const U32 task_7 )
{
	S8 num_string[13];
	U32 temp_time;

	/* stop timer */
	temp_time = CLK_end_timer();

	putstr( param );
	build_dec16( task_1, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_2, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_3, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_4, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_5, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_6, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"   " );
	build_dec16( task_7, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"\n" );

	/* restart timer */
	CLK_start_timer( temp_time );

} /* MESS_OUT_task_report() */


/******************************************************************************
*  Description : outputs a scheduler report to the output
*  Parameters  : param           : title string
*                task_1 - task_7 : task states.
*                timestamp       : scheduler execution time
*                overrun         : Overrun error flag
******************************************************************************/
void MESS_OUT_scheduler_report( const U32 time,
								const S8* const task_1,
								const S8* const task_2,
								const S8* const task_3,
								const S8* const task_4,
								const S8* const task_5,
								const S8* const task_6,
								const S8* const task_7,
								const U32 timestamp,
								const S8* const overrun )
{
	S8 num_string[13];
	U32 temp_time;

	/* stop timer */
	temp_time = CLK_end_timer();

	build_dec16( time, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)" " );
	putstr( task_1 );
	putstr( task_2 );
	putstr( task_3 );
	putstr( task_4 );
	putstr( task_5 );
	putstr( task_6 );
	putstr( task_7 );
	build_dec16( timestamp, num_string );
	putstr( (S8*)num_string );
	putstr( (S8*)"  " );
	putstr( overrun );
	putstr( (S8*)"\n" );

	/* restart timer */
	CLK_start_timer( temp_time );

} /* MESS_OUT_scheduler_report() */


/*****************************************************************************/
void MESS_OUT_toggle_debug_mode( void )
{
	if( debug_mode is FALSE )
	{
 		MESS_OUT_message( "Debug mode ON",  MESSAGE_OP_INFO );
		debug_mode = TRUE;
	}
	else
	{
 		MESS_OUT_message( "Debug mode OFF", MESSAGE_OP_INFO );
		debug_mode = FALSE;
	}

} /* MESS_OUT_toggle_debug_mode() */
		
/*****************************************************************************/
void MESS_OUT_toggle_verbose_mode( void )
{
	if( verbose_mode is FALSE )
	{
 		MESS_OUT_message( "Verbose mode ON",  MESSAGE_OP_INFO );
		verbose_mode = TRUE;
	}
	else
	{
 		MESS_OUT_message( "Verbose mode OFF", MESSAGE_OP_INFO );
		verbose_mode = FALSE;
	}

} /* MESS_OUT_toggle_verbose_mode() */


/******************************************************************************
*  Description : dummy functions
******************************************************************************/
void MESS_OUT_toggle_screen_results_mode( void ){}
void MESS_OUT_report_status( const message_class class ){}
void MESS_OUT_report_duration( void ){}
void MESS_OUT_rename_results_file( const S8* const file_name ){}


/******************************************************************************
*                            PRIVATE FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Description : builds string representation of decimal number
*  Notes       : Maximum length of 8 bit number is 255
*  Parameters  : num - number to be converted
******************************************************************************/
static void build_dec( const U32 num, const S8* num_string )
{
	/* who we gonna call */

	if( num > 0x0000ffff )
	{
		build_dec32( num, num_string );
	}
	else if( num > 0x000000ff )
	{
		build_dec16( (U16)num, num_string );
	}
	else
	{
		build_dec8( (U8)num, num_string );
	}

} /* build_dec8() */


/******************************************************************************
*  Description : builds string representation of decimal number
*  Notes       : Maximum length of 8 bit number is 255
*  Parameters  : num - number to be converted
******************************************************************************/
static void build_dec8( const U8 num, S8 *num_string )
{
	U8 num_dec;
	num_dec = num;

	num_string[0] = dec2str( num_dec / 100 );
	num_dec %= ONE_HUNDRED_CPV;
	num_string[ONE_CPV] = dec2str( num_dec / 10 );
	num_dec %= TEN_CPV;
	num_string[TWO_CPV] = dec2str( num_dec );
	num_string[THREE_CPV] = '\0';

} /* build_dec8() */


/******************************************************************************
*  Notes       : Maximum length of 16 bit number is 65535
******************************************************************************/
static void build_dec16( const U16 num, S8 *num_string )
{
	U16 num_dec;
	num_dec = num;

	num_string[0] = dec2str( num_dec /10000 );
	num_dec %= 10000;
	num_string[ONE_CPV] = dec2str( num_dec /1000 );
	num_dec %= 1000;
	num_string[TWO_CPV] = dec2str( num_dec /100 );
	num_dec %= ONE_HUNDRED_CPV;
	num_string[THREE_CPV] = dec2str( num_dec /10 );
	num_dec %= TEN_CPV;
	num_string[FOUR_CPV] = dec2str( num_dec );
	num_string[FIVE_CPV] = '\0';

} /* build_dec16() */


/******************************************************************************
*  Description : builds string representation of decimal number
*  Notes       : Maximum length of 32 bit number is 4294967295
*  Parameters  : num - number to be converted
******************************************************************************/
static void build_dec32( const U32 num, S8 *num_string )
{
	U32 num_dec;
	num_dec = num;

	num_string[ZERO_CPV] = dec2str( num_dec / 1000000000 );
	num_dec %= 1000000000;
	num_string[ONE_CPV] = dec2str( num_dec /100000000 );
	num_dec %= 100000000;
	num_string[TWO_CPV] = dec2str( num_dec /10000000 );
	num_dec %= 10000000;
	num_string[THREE_CPV] = dec2str( num_dec /1000000 );
	num_dec %= 1000000;
	num_string[FOUR_CPV] = dec2str( num_dec /100000 );
	num_dec %= 100000;
	num_string[FIVE_CPV] = dec2str( num_dec /10000 );
	num_dec %= 10000;
	num_string[SIX_CPV] = dec2str( num_dec /1000 );
	num_dec %= 1000;
	num_string[SEVEN_CPV] = dec2str( num_dec /100 );
	num_dec %= ONE_HUNDRED_CPV;
	num_string[EIGHT_CPV] = dec2str( num_dec /10 );
	num_dec %= TEN_CPV;
	num_string[NINE_CPV] = dec2str( num_dec );
	num_string[TEN_CPV] = '\0';

} /* build_dec32() */


/******************************************************************************
*  Description : used by build_dec. Returns char of number between 0 and 9
*  Parameters  : num - number to be converted
*  Returns     : char to input num
******************************************************************************/
static S8 dec2str( const U32 num )
{
	S8 return_char = NULL;

	switch( num )
	{
	case 0:
		return_char = '0';
		break;

	case 1:
		return_char = '1';
		break;

	case 2:
		return_char = '2';
		break;

	case 3:
		return_char = '3';
		break;

	case 4:
		return_char = '4';
		break;

	case 5:
		return_char = '5';
		break;

	case 6:
		return_char = '6';
		break;

	case 7:
		return_char = '7';
		break;

	case 8:
		return_char = '8';
		break;

	case 9:
		return_char = '9';
		break;
	}

	return( return_char );
}


/******************************************************************************
*  Description : Write string to Serial Port
******************************************************************************/
static void putstr( const S8 *p )
{
	while( *p )
	{
		(void)SER_putchar( *p++ );
	}

} /* putstr() */



#else
/********************************/
/* compile for PC based  system */
/********************************/
/*lint -e85 has dimension 0 */
#include <stdio.h>
/*lint +e85 has dimension 0 */
#include <io.h>
#include <string.h>
#include "date.h"
#include "rt_clock.h"
#include "messages_out.h"


/******************************************************************************
*                            PRIVATE DATA TYPES								  *
******************************************************************************/
static const S8* log_name = (S8*)"log.txt";
static S8 results_name[MAX_NAME_LEN]; 
static FILE* log_ptr;
static FILE* results_ptr;
static boolean debug_mode;
static boolean verbose_mode;
static boolean screen_results_mode;

#define TIME_STR_SIZE 250


/******************************************************************************
*                       PRIVATE FUNCTION PROTOTYPES				     		  *
******************************************************************************/
static void close_file( FILE* file_ptr, const S8* const time_str );
static void error_message( const S8* const message, const S8* const time_str );
static void debug_message( const S8* const message, const S8* const time_str );
static void results_message( const S8* const message );
static void info_message( const S8* const message, const boolean cr );
static void log_message( const S8* const message, const S8* const time_str );
static void verbose_message( const S8* const message );
static void decode_seconds( const U32 seconds, S8* time_str );


/******************************************************************************
*                              PUBLIC FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Description : any initialisation goes in here
******************************************************************************/
void MESS_OUT_initialise( void )
{
	S8 time_str[TIME_STR_SIZE];

	/* initialise reporting mode defaults */
	debug_mode          = FALSE; 
	verbose_mode        = TRUE;
	screen_results_mode = FALSE;
	
	/* start stamp timer */
	DATE_stamp_start();
	
	/* get the time and date */
	DATE_get_date( time_str );
	
	/* opening message */
 	log_message( (S8*)"Session starting", time_str ); 	

} /* MESS_OUT_initialise() */


/******************************************************************************
*  Description : passes messages to the handlers
******************************************************************************/
void MESS_OUT_message( const S8* const message, const message_class class )
{
	U32 temp_time;
	S8 time_str[TIME_STR_SIZE];
	
	/* stop timer */
	temp_time = CLK_end_timer();

	DATE_get_time( time_str );	
	
	/* handle message class */
	switch (class)
	{
		case MESSAGE_OP_RESULTS :
			results_message( message );
			if( screen_results_mode is TRUE )
			{
				info_message( message, TRUE );
			}
			break;

		case MESSAGE_OP_INFO :
			info_message( message, TRUE );
			if( debug_mode is TRUE )
			{
				debug_message( message, time_str );			
			} 
			break;

		case MESSAGE_OP_INFO_NOCR :
			info_message( message, FALSE );
			if( debug_mode is TRUE )
			{
				debug_message( message, time_str );			
			} 
			break;

		case MESSAGE_OP_LOG :
			info_message( message, TRUE );
			log_message( message, time_str );
			break;

		case MESSAGE_OP_VERBOSE :
			if( verbose_mode is TRUE )
			{
				verbose_message( message );

				if( screen_results_mode is TRUE )
				{
					info_message( message, TRUE );
				}
			}
			break;

		case MESSAGE_OP_DEBUG :
			if( debug_mode is TRUE )
			{
				info_message( message, TRUE );
				debug_message( message, time_str );
				results_message( message );
			}
			break;

		case MESSAGE_OP_ERROR :		
			info_message( message, TRUE );
			error_message( message, time_str );
			break;

		case MESSAGE_OP_MAX :
		default :
			error_message( (S8*)"unknown message type", time_str );
			break;
	}

	/* restart timer */
	CLK_start_timer( temp_time );

} /* MESS_OUT_message() */


/******************************************************************************
*  Description : Names or renames a results file
******************************************************************************/
void MESS_OUT_rename_results_file( const S8* const file_name )
{
	S8 time_str[TIME_STR_SIZE];
	S8 message[MAX_NAME_LEN];

	DATE_get_time( time_str );	
	
	(void)sprintf( results_name, (S8*)"%s", file_name ); 
	
	/* log renaming */
	(void)sprintf( message, (S8*)"Results file named %s", file_name );
	MESS_OUT_message(message, MESSAGE_OP_LOG );

} /* MESS_OUT_launch_results_file() */


/******************************************************************************
*  Description : two message strings
*  Parameters  : message1 - first message string
*                message2 - second message string
*                class   - the message type
******************************************************************************/
void MESS_OUT_message_S2( const S8* const message1, const S8* const message2, 
                          const message_class class )
{
	S8 full_message[TWO_HUNDRED_CPV];

	sprintf( full_message, (S8*)"%s %s", message1, message2 );

	MESS_OUT_message( full_message, class );

} /* MESS_OUT_message_S2() */


/******************************************************************************
*  Description : message with one number
*  Parameters  : message - the message
*                num     - number parameter
*                class   - the message type
******************************************************************************/
void MESS_OUT_message_1( const S8* const message, const U32 num, 
                         const message_class class )
{
	S8 full_message[TWO_HUNDRED_CPV];

	sprintf( full_message, (S8*)"%s %d", message, num );

	MESS_OUT_message( full_message, class );

} /* MESS_OUT_message_1() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_2( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
                         const message_class class )
{
	S8 full_message[TWO_HUNDRED_CPV];

	sprintf( full_message, (S8*)"%s %d %s %d", message_1, num_1, message_2, num_2 );

	MESS_OUT_message( full_message, class );

} /* MESS_OUT_message_2() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                message_3 - the message
*                num_3     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_3( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
                         const message_class class )
{
	S8 full_message[TWO_HUNDRED_CPV];

	sprintf( full_message, (S8*)"%s %d %s %d %s %d", 
	         message_1, num_1, message_2, num_2, message_3, num_3 );

	MESS_OUT_message( full_message, class );

} /* MESS_OUT_message_3() */


/******************************************************************************
*  Description : message with two numbers
*  Parameters  : message_1 - the message
*                num_1     - number parameter
*                message_2 - the message
*                num_2     - number parameter
*                message_3 - the message
*                num_3     - number parameter
*                message_4 - the message
*                num_4     - number parameter
*                class     - the message type
******************************************************************************/
void MESS_OUT_message_4( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
						 const S8* const message_4, const U32 num_4,
                         const message_class class )
{
	S8 full_message[TWO_HUNDRED_CPV];

	sprintf( full_message, (S8*)"%s %d %s %d %s %d %s %d", 
	         message_1, num_1, message_2, num_2, 
	         message_3, num_3, message_4, num_4 );

	MESS_OUT_message( full_message, class );
}

/******************************************************************************
*  Description : outputs a scheduler report to the output
*  Parameters  : param          : title string
*                task_1 - task_7: task states.
******************************************************************************/
void MESS_OUT_task_report(  const S8* const param,
							const U32 task_1,
							const U32 task_2,
							const U32 task_3,
							const U32 task_4,
							const U32 task_5,
							const U32 task_6,
							const U32 task_7 )
{
	S8 line[MAX_LINE_LEN];
	U32 temp_time;

	/* stop timer */
	temp_time = CLK_end_timer();

	/* build a line */
	sprintf(    line, (S8*)"%s%6d  %6d  %6d  %6d  %6d  %6d  %6d",
				param,
				task_1,
				task_2,
				task_3,
				task_4,
				task_5,
				task_6,
				task_7 );

	/* send line to be copied to results file */
	results_message( line );

	if( screen_results_mode is TRUE )
	{
		info_message( line, TRUE );
	}
	
	/* restart timer */
	CLK_start_timer( temp_time );
} 

/******************************************************************************
*  Description : outputs a report string of task states to output
*  Parameters  : time : system time
*                task_1 - task_7: task states.
*                timestamp      : scheduler execution time
*                overrun        : Overrun error flag
******************************************************************************/
void MESS_OUT_scheduler_report( const U32 time,
								const S8* const task_1,
								const S8* const task_2,
								const S8* const task_3,
								const S8* const task_4,
								const S8* const task_5,
								const S8* const task_6,
								const S8* const task_7,
								const U32 timestamp,
								const S8* const overrun )
{
	S8 line[MAX_LINE_LEN];
	U32 temp_time;

	/* stop timer */
	temp_time = CLK_end_timer();

	sprintf( line, (S8*)"%05d %s%s%s%s%s%s%s %05d   %s",
		time,
		task_1,
		task_2,
		task_3,
		task_4,
		task_5,
		task_6,
		task_7,
		timestamp,
		overrun );

	/* send line to be copied to results file */
	results_message( line );

	if( screen_results_mode is TRUE )
	{
		info_message( line, TRUE );
	}

	/* restart timer */
	CLK_start_timer( temp_time );

} /* MESS_OUT_scheduler_report() */


/******************************************************************************
*  Description : Close down stuff goes in here
******************************************************************************/
void MESS_OUT_close( void )
{
	S8 end_message_str[TIME_STR_SIZE];
	S8 duration_str[TIME_STR_SIZE];	
	U32 duration;	

	/* get the date and time, for logging */
	S8 date_str[TIME_STR_SIZE];	
	DATE_get_date( date_str );

	duration = DATE_get_duration();
	decode_seconds( duration, duration_str );
	sprintf( end_message_str, (S8*)"Session ending (duration %s)\n", duration_str );
	info_message( end_message_str, FALSE );

	/* opening message */
 	log_message( end_message_str, date_str );

	/* close any opened file, though should be already closed */
	close_file( log_ptr, date_str );
	close_file( results_ptr, date_str );

} /* MESS_OUT_close() */


/******************************************************************************
*  Description : Access function to toggle debug messaging
******************************************************************************/
void MESS_OUT_toggle_debug_mode( void )
{
	/* get the time, for logging */
	S8 time_str[TIME_STR_SIZE];	
	DATE_get_time( time_str );

	if( debug_mode is FALSE )
	{
 		info_message( (S8*)"Debug mode ON", TRUE );
 		log_message(  (S8*)"Debug mode ON", time_str );		
		debug_mode = TRUE;
	}
	else
	{
 		info_message( (S8*)"Debug mode OFF", TRUE );
 		log_message(  (S8*)"Debug mode OFF", time_str );		
		debug_mode = FALSE;
	}

} /* MESS_OUT_toggle_debug_mode() */


/******************************************************************************
*  Description : Access function to toggle verbose_mode messaging
******************************************************************************/
void MESS_OUT_toggle_verbose_mode( void )
{
	/* get the time, for logging */
	S8 time_str[TIME_STR_SIZE];	
	DATE_get_time( time_str );

	if( verbose_mode is FALSE )
	{
 		info_message( (S8*)"Verbose mode ON", TRUE );
 		log_message(  (S8*)"Verbose mode ON", time_str );		
		verbose_mode = TRUE;
	}
	else
	{
 		info_message( (S8*)"Verbose mode OFF", TRUE );
 		log_message(  (S8*)"Verbose mode OFF", time_str );		
		verbose_mode = FALSE;
	}

} /* MESS_OUT_toggle_verbose_mode() */

/******************************************************************************
*  Description : Access function to toggle screen results messaging
*                normally results are sent to results file
******************************************************************************/
void MESS_OUT_toggle_screen_results_mode( void )
{
	/* get the time, for logging */
	S8 time_str[TIME_STR_SIZE];	
	DATE_get_time( time_str );

	if( screen_results_mode is FALSE )
	{
 		info_message( (S8*)"Screen results mode ON", TRUE );
 		log_message(  (S8*)"Screen results mode ON", time_str );		
		screen_results_mode = TRUE;
	}
	else
	{
 		info_message( (S8*)"Screen results mode OFF", TRUE );
 		log_message(  (S8*)"Screen results mode OFF", time_str );		
		screen_results_mode = FALSE;
	}

} /* MESS_OUT_toggle_screen_results_mode() */


/******************************************************************************
*  Description : Report status of system
******************************************************************************/
void MESS_OUT_report_status( const message_class class )
{
	MESS_OUT_message( (S8*)"Status:", class );

	if( verbose_mode is TRUE )
	{
	 	MESS_OUT_message( (S8*)"Verbose mode ON",  class );
	}
	else
	{
	 	MESS_OUT_message( (S8*)"Verbose mode OFF", class );
	}

	if( debug_mode is TRUE )
	{
	 	MESS_OUT_message( (S8*)"Debug mode ON",  class );
	}
	else
	{
	 	MESS_OUT_message( (S8*)"Debug mode OFF", class );
	}

	if( screen_results_mode is TRUE )
	{
	 	MESS_OUT_message( (S8*)"Screen reporting mode ON",  class );
	}
	else
	{
	 	MESS_OUT_message( (S8*)"Screen reporting mode OFF", class );
	}

} /* MESS_OUT_report_status() */


/******************************************************************************
*  Description : Report time session has run sofar
******************************************************************************/
void MESS_OUT_report_duration( void )
{
	S8 message_str[TIME_STR_SIZE];
	S8 duration_str[TIME_STR_SIZE] = "";	
	U32 duration;	

	duration = DATE_get_duration();
	decode_seconds( duration, duration_str );
	sprintf( message_str, (S8*)"Session duration %s\n\n", duration_str );
	info_message( message_str, FALSE );


} /* MESS_OUT_report_duration() */


/******************************************************************************
*                            PRIVATE FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Description : closes a file - should not be needed
******************************************************************************/
static void close_file( FILE* file_ptr, const S8* const time_str )
{
	/* close file if opened */
	if( file_ptr is_not NULL )
	{	
		MESS_OUT_message( (S8*)"File closed", MESSAGE_OP_DEBUG );
	   	(void)fclose( file_ptr );
		file_ptr = NULL;
	}

} /* close_file() */

 
/******************************************************************************
*  Description : Error message handler 
******************************************************************************/
static void error_message( const S8* const message, const S8* const time_str )
{
	/* open the error log file */
	log_ptr = fopen( log_name, (S8*)"a" );

	if( log_ptr is_not NULL )
	{
		/* send messaeg to the log file */
		(void)fprintf( log_ptr, (S8*)"ERROR %s : %s\n", time_str, message );

		/* now close the log file */
		(void)fclose( log_ptr );

		/* clear the pointer */
		log_ptr = NULL;
	}

} /* error_message() */


/******************************************************************************
*  Description : Debug message handler 
******************************************************************************/
static void debug_message( const S8* const message, const S8* const time_str )
{
	/* open the log file */
	log_ptr = fopen( log_name, (S8*)"a" );

	if( log_ptr is_not NULL )
	{
		/* send messaeg to the log file */
		(void)fprintf( log_ptr, (S8*)"DEBUG %s : %s\n", time_str, message );

		/* now close the log file */
		(void)fclose( log_ptr );

		/* clear the pointer */
		log_ptr = NULL;
	}

} /* debug_message() */


/******************************************************************************
*  Description : Results message handler - writes to results file 
******************************************************************************/
static void results_message( const S8* const message )
{
	/* open the results file */
	results_ptr = fopen( results_name, (S8*)"a" );

	if( results_ptr is_not NULL )
	{
		if( (screen_results_mode is_not TRUE) and (mode is MODE_SIMULATION) )
		{
			/* send progress indicator to terminal */
			info_message( (S8*)">", FALSE );
		} 
		
		/* send message to the log file */
		(void)fprintf( results_ptr, (S8*)"%s\n", message );

		/* now close the log file */
		(void)fclose( results_ptr );

		/* clear the pointer */
		results_ptr = NULL;
	}

} /* results_message() */  


/******************************************************************************
*  Description : Infomation message handler 
******************************************************************************/
static void info_message( const S8* const message, const boolean cr )
{
	if( cr is TRUE )
	{
		printf( (S8*)"%s\n", message );
	}
	else
	{
		printf( (S8*)"%s",   message );
	}

} /* info_message() */


/******************************************************************************
*  Description : Log message handler 
******************************************************************************/
static void log_message( const S8* const message, const S8* const time_str )
{
	/* open the log file */
	log_ptr = fopen( log_name, (S8*)"a" );

	if( log_ptr is_not NULL )
	{
		/* send messaeg to the log file */
		(void)fprintf( log_ptr, (S8*)"LOG %s: %s\n", time_str, message );

		/* now close the log file */
		(void)fclose(  log_ptr );

		/* clear the pointer */
		log_ptr = NULL;
	}

} /* info_message() */


/******************************************************************************
*  Description : Verbose message handler 
******************************************************************************/
static void verbose_message( const S8* const message )
{
	/* open the results file */
	results_ptr = fopen( results_name, "a" );

	if( results_ptr is_not NULL )
	{
		/* send message to the log file */
		(void)fprintf( results_ptr, (S8*)"%s\n", message );

		/* now close the log file */
		(void)fclose( results_ptr );

		/* clear the pointer */
		results_ptr = NULL;
	}

} /* verbose_message() */


/******************************************************************************
*  Description : Get the current time and convert it to local time 
*                representation, placing it into loctime
******************************************************************************/
static void decode_seconds( const U32 seconds, S8* time_str )
{
	const U32 secs_in_hour = (U32)3600;
	const U32 secs_in_min  = (U32)60;
	
	S8 hour_str[TIME_STR_SIZE];
	S8 mins_str[TIME_STR_SIZE];
	S8 secs_str[TIME_STR_SIZE];
	
	U32 hours;
	U32 mins;
	U32 secs;

	/* localise seconds */
	secs = seconds;
	
	/* process any hours */
	hours = secs / secs_in_hour;
	sprintf( hour_str, (S8*)"%02d:", hours );

	/* process any minutes */
	secs  %= secs_in_hour;
	mins = secs / secs_in_min;  
    sprintf( mins_str, (S8*)"%02d:", mins );

	/* process remaining seconds */	
	secs %= secs_in_min;
    sprintf( secs_str, (S8*)"%02d", secs );

	/* combine time strings */
	sprintf( time_str, (S8*)"%s%s%s", hour_str, mins_str, secs_str );

} /* decode_seconds() */

#endif /* DEV_SYSTEM */
