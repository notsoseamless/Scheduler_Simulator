/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  main.c                                                       *
*                                                                             *
* Description :  Main launch module                                           *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date    |Auth| Description                                                  *
* 10-03-05|JRO | Created                                                      *
*         |    |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "initialise.h"
#include "test.h"


S32 main( void )
{
    INIT_initialise();    /* initialise objects */
    TST_test();           /* run test           */
    INIT_closedown();     /* shut things down   */
    return( 0 ); 
}
