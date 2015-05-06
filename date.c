/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :  SCHEDULER SIMULATOR                                          *
*                                                                             *
* File Name   :  date.c                                                       *
*                                                                             *
* Description :  This module is responsible for creating a date string used   *
*                by log files                                                 *                                                                             *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 23-09-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h" 
#ifndef DEV_SYSTEM
/*lint -e85 has dimension 0 */
/*lint --fmd */
#include <stdio.h>
#include <time.h>
/*lint ++fmd */
/*lint +e85 has dimension 0 */
#include "date.h"

/******************************************************************************
*                            PRIVATE DATA TYPES								  *
******************************************************************************/
static time_t start_time_stamp;
static boolean start_time_recorded = FALSE;


/******************************************************************************
*                       PRIVATE FUNCTION PROTOTYPES				     		  *
******************************************************************************/
static struct tm * get_time_struct( struct tm *loctime );



/******************************************************************************
*                              PUBLIC FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Description : puts date and time into date_str
******************************************************************************/
void DATE_get_date( S8* date_str )
{
 	struct tm loc;
 	struct tm *loctime;

	loctime = &loc;

	/* Get the current time and convert it to local time representation */
   	loctime = get_time_struct( loctime );

	/* Print it out in a nice format. */
  	(void)strftime( date_str, (size_t)TWO_HUNDRED_CPV, "%A %d %B %Y  %H:%M:%S", 
  	                loctime );

} /* DATE_get_date() */


/******************************************************************************
*  Description : puts time into time_str
******************************************************************************/
void DATE_get_time( S8* time_str )
{
	struct tm loc;
	struct tm *loctime;

	loctime = &loc;

	/* Get the current time and convert it to local time representation */
  	loctime = get_time_struct( loctime );

	/* Print it out in a nice format. */
 	(void)strftime( time_str, (size_t)ONE_HUNDRED_CPV, "%H:%M:%S", loctime );

} /* DATE_get_time() */


/******************************************************************************
*  Description : time stamps start of session
******************************************************************************/
void DATE_stamp_start( void )
{
	start_time_stamp    = time( NULL );
	start_time_recorded = TRUE;

} /* DATE_stamp_start() */


/******************************************************************************
*  Description : returns the time difference in seconds
******************************************************************************/
U32 DATE_get_duration( void )
{ 
	U32    seconds;
	time_t end_time_stamp;
	
	if( start_time_recorded is TRUE )
	{
		/* the time now */
		end_time_stamp = time( NULL );

		/* the time difference */
		seconds = (U32)difftime( end_time_stamp, start_time_stamp );	 
	}
	else
	{
		/* no reference so return zero */
		seconds = (U32)ZERO_CPV;
	}

	return seconds;

} /* DATE_get_duration() */


/******************************************************************************
*                            PRIVATE FUNCTIONS								  *
******************************************************************************/


/******************************************************************************
*  Description : Get the current time and convert it to local time 
*                representation, placing it into loctime
******************************************************************************/
static struct tm * get_time_struct( struct tm *loctime )
{
	time_t curtime;
	
	/* Get the current time */
	curtime = time( NULL );

	/* Convert it to local time representation */
   	loctime = localtime( &curtime );

	return loctime;

} /* get_time_struct() */

#endif /* DEV_SYSTEM */
