/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  *
*                                                                             *
* File Name   :  util.h                                                       *
*                                                                             *
* Description :  Interface for utils.c                                        *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 18-05-05 |JRO| Created                                                      *
* 30-05-05 |JRO| Added UTL_type_report()                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

struct UTL_div_type
{
    U32 quotient;
    U32 remainder;
};

void UTL_string_copy( S8 *d, const S8 *s );
void UTL_divide( const U32 number, const U32 denom, struct UTL_div_type *UTL_div);
void UTL_type_report( void );

#endif
