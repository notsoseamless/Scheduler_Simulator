/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  initialise.c                                                 *
*                                                                             *
* Description :  This module is responsible for initialising all objects      *
*                and then for closing them down again.                        *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date    |Auth| Description                                                  *
* 18-03-05|JRO | Created                                                      *
*         |    |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "messages_out.h"
#include "timers.h"
#include "rt_clock.h"
#include "task.h"
/* #include "task_set.h" */
#include "test.h"
#include "scheduler.h"
#include "q_utils.h"

#ifdef DEV_SYSTEM  /* CONDITIONAL COMPILE - Keil Development System */
#include "serial.h"
#endif /* DEV_SYSTEM */

#include "initialise.h"


/******************************************************************************
*  Description : common initialisation routine
******************************************************************************/
void INIT_initialise( void )
{
    /* initialise serial object */
    #ifdef DEV_SYSTEM
        SER_init();                 /* serial object               */
    #endif

	/* default globals */
    mode              = MODE_TERMINAL; /* set simulator mode       */
    test_duration     = TEN_CPV;    /* default test length         */

	/* general initialisation */
    MESS_OUT_initialise();          /* message output              */
    TSK_init();                     /* initialise task object      */
    TST_init();                     /* initialise test object      */
/*	TASK_SET_init();	*/			/* task set object             */
    SCH_init();                     /* scheduler object            */
    TIM_init();                     /* timer object and interrupts */
	QUTIL_init();					/* queue utilities             */
    CLK_init();                     /* clock object                */

} /* INIT_initialise() */


/******************************************************************************
*  Description : common soft reset routine
******************************************************************************/
void INIT_soft_reset( void )
{
    MESS_OUT_message( (S8*)"INIT_soft_reset", MESSAGE_OP_DEBUG );
    TST_soft_reset();
    TSK_soft_reset();
    SCH_soft_reset();
    test_duration  = TEN_CPV;       /* default test length         */

} /* INIT_soft_reset() */


/******************************************************************************
*  Description : common closedown routine
******************************************************************************/
void INIT_closedown( void )
{
/*	TASK_SET_close();	 */    /* task set object             */
	MESS_OUT_close();        /* message out object          */
	QUTIL_close();			 /* queue utilities             */

} /* INT_closedown() */
