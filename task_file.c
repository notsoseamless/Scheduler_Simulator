/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  task_file.c                                                  *
*                                                                             *
* Description :  This module is responsible for managing a task data file     *                                                                             *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 13-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#if(0)
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "globals.h" 
#include "messages_out.h"
#include "task_file.h"


/******************************************************************************
*                            PRIVATE DATA TYPES								  *
******************************************************************************/
static FILE *task_file_ptr;
static const S8 *file_name = (S8*)"task_data_01.txt";


/******************************************************************************
*                        PRIVATE FUNCTION PROTOTYPES     					  *
******************************************************************************/
static boolean open_file( void );
static void close_file( void );


/******************************************************************************
*                              PUBLIC FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Function    : TASK_FILE_init
*  Description : initialises task file object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TASK_FILE_init( void )
{

} /* TASK_FILE_init() */


/******************************************************************************
*  Function    : TASK_FILE_dump_tasks
*  Description : test function - dumps file contents 
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TASK_FILE_dump_tasks( void )
{
    #define BUFFSIZE 200
    
    S8 buffer[BUFFSIZE];
	
	if( open_file() is TRUE )
	{
		/* step through each line of the task file */
		MESS_OUT_message( "Starting task file dump\n", MESSAGE_OP_DEBUG );
 		while( fgets( buffer, (BUFFSIZE-1), task_file_ptr ) )
		{
			printf("step\n");
			
			MESS_OUT_message( buffer, MESSAGE_OP_DEBUG );

		  /*	parse_line( buffer ); */
		}  	
		MESS_OUT_message( "Ended task file dump\n", MESSAGE_OP_DEBUG );
		
		close_file();
	}

} /* TASK_FILE_dump_tasks() */


/******************************************************************************
*  Function    : TASK_FILE_get_task
*  Description : reads a task from the task file 
*  Parameters  : task id
*                pointer to task struct
*  Returns     : none
******************************************************************************/
void TASK_FILE_get_task( const U8 task_id, const U8* const task_data )
{

		



} /* TASK_FILE_get_task() */


/******************************************************************************
*                            PRIVATE FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Function    : open_file
*  Description : opens task file for reading
*  Parameters  : none
*  Returns     : TRUE if successful
******************************************************************************/
static boolean open_file( void )
{
    boolean result;
    
    /* open file for reading */
    task_file_ptr = fopen( file_name, "r" );

	if( task_file_ptr is_not NULL )
	{
		MESS_OUT_message( "task_data_file opened\n", MESSAGE_OP_DEBUG );
		result = TRUE;
	}
	else
	{
		MESS_OUT_message( "problem opening task_data_file", MESSAGE_OP_ERROR );
		result = FALSE;
	}

	return result;

} /* open_file() */


/******************************************************************************
*  Function    : close_file
*  Description : close task file 
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void close_file( void )
{
	if( task_file_ptr is_not NULL )
	{
		fclose( task_file_ptr );
		task_file_ptr = NULL;
		MESS_OUT_message( "task_data_file closed\n", MESSAGE_OP_DEBUG );
	} 

} /* close_file() */

#endif
