/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  utils.c                                                      *
*                                                                             *
* Description :  This module contains any common utilities                    *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 18-05-05 |JRO| Created                                                      *
* 30-05-05 |JRO| Added UTL_type_report()                                      *
* 17-07-05 |JRO| Added UTL_divide()                                           *
* 12-01-06 |JRO| Fixed terrible bug in divide                                 *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "task.h"
#include "messages_out.h"
#include "scheduler.h"
#include "test_case.h"
#include "utils.h"



/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/




/******************************************************************************
*  Function    : UTL_string_copy
*  Description : Copies source s to destination d
*                used because the Keil package does not include stdio.h
*  Parameters  : s - pointer to source array
*                d - pointer to destination array
*  Returns     : none
******************************************************************************/
void UTL_string_copy( S8 *d, const S8 *s )
{
    while( (*d++ = *s++) is_not '\0' )
    {
        /* nothing */
    }

} /* UTL_string_copy() */


/******************************************************************************
*  Function    : UTL_divide
*  Description : crude interger divide
*  Notes       : Because Keil gcc does not include DIV or '/'
*  Parameters  : number  - number to be divided
*                denom   - interger to divide into number
*                UTL_div - struct for quotient and remainder
*  Returns     : none
*  Reads       : none
******************************************************************************/
void UTL_divide( const U32 number, const U32 denom, struct UTL_div_type *UTL_div )
{
	U32 sum;

	sum   = ZERO_CPV;
	UTL_div->quotient = ZERO_CPV;

	if( denom > ZERO_CPV )
	{
		/* divide denom into number, whilst sum is small enough */
		do
		{
			sum += denom;
			UTL_div->quotient ++;

		}while( (sum+denom) <= number );

		/* check for remainder */
		if( sum is number )
		{
			/* exact divide */
			UTL_div->remainder = ZERO_CPV;
		}
		else if( sum < number )	  
		{
			/* find remainder */
			UTL_div->remainder = number - sum;
		}
		else
		{										  
			/* sanity check failed */
			MESS_OUT_message_1( (S8*)"UTL_divide() ", sum, MESSAGE_OP_ERROR ); 
		}
	}
	else
	{
		/* Division by ZERO */
		MESS_OUT_message( (S8*)"UTL_divide(): DIVISION BY ZERO", 
		                  MESSAGE_OP_ERROR );
		UTL_div->quotient  = ZERO_CPV;
		UTL_div->remainder = ZERO_CPV;
	}

} /* UTL_divide() */


/******************************************************************************
*  Function    : UTL_type_report
*  Description : private helper - outputs report on type sizes
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void UTL_type_report( void )
{
    MESS_OUT_message(   (S8*)"\n\nTYPE REPORT\n", MESSAGE_OP_INFO );
    MESS_OUT_message(   (S8*)    "===========\n", MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"unsigned char      : ", (U32)sizeof( unsigned char ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"signed char        : ", (U32)sizeof( signed char ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"unsigned short int : ", (U32)sizeof( unsigned short int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"signed short int   : ", (U32)sizeof( signed short int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"unsigned long int  : ", (U32)sizeof( unsigned long int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"signed long int    : ", (U32)sizeof( signed long int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"unsigned int       : ", (U32)sizeof( unsigned int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"signed int         : ", (U32)sizeof( signed int ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"int*               : ", (U32)sizeof( int* ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"void*              : ", (U32)sizeof( void* ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"boolean            : ", (U32)sizeof( boolean ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"task_type          : ", (U32)sizeof( struct task_type ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"task_type[]        : ", (U32)sizeof( struct task_type[100] ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"test_case_type     : ", (U32)sizeof( struct test_case_type ), MESSAGE_OP_INFO );
    MESS_OUT_message_1( (S8*)"test_case_type[]   : ", (U32)sizeof( struct test_case_type[MAX_TEST_CASES] ), MESSAGE_OP_INFO );
    MESS_OUT_message(   (S8*)"\n\n", MESSAGE_OP_INFO );

} /* UTL_type_report() */
