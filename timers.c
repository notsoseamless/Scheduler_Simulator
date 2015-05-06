/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*  NOTE: This file is adapted from part of the uVision/ARM development tools  *
*        Supplied with the uVision IDE time.c                                 *
*        (c) KEIL ELEKTRONIK GmbH 2002-2004                                   *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  *
*                                                                             *
* File Name   :  timers.c                                                     *
*                                                                             *
* Description :  Physical Layer Functionality                                 *
*                Low level time functions - conditionally compiles for either *
*                Keil Development System, or PC (see in globals.h).           *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 10-03-05 |JRO| Ported from Keil uVision/ARM development tools               *
* 11-03-05 |JRO| changed C++ comments to please pclint.                       *
* 17-04-05 |JRO| Added screen control for development system                  *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"

#include "timers.h"


#ifdef DEV_SYSTEM
/***************************************/
/* compile for Keil development system */
/***************************************/
#include "LPC21XX.H"                       /* LPC21XX Peripheral Registers */
#include "task.h"
#include "rt_clock.h"
#include "scheduler.h"
#include "term.h"

static volatile U32 timeval;

void tc0( void ) __attribute__ ((interrupt));  /* Generate Interrupt */



/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/



/******************************************************************************
*
*  Function    : TIM_init
*
*  Description : Setup Timer Counter 0 Interrupts
*
*  Notes       : Adapted from uVision/ARM development tools
*
*  Parameters  : none
*
*  Returns     : none
*
*  Reads       : none
*
*  Writes      : T0MR0         (Match Register 0)
*                T0MCR         (Match Control Register)
*                T0TCR         (Timer Control Register)
*                VICVectAddr0  (Vector Address Register 0)
*                VICVectCntl0  (Vector control 0 register)
*                timeval       (local)
*
*  Calls       : none
*
******************************************************************************/
void TIM_init( void )
{
    T0MR0        = 149999;              /* 10mSec = 150,000-1 counts    */
    T0MCR        = 3;                   /* Interrupt and Reset on MR0   */
    T0TCR        = 1;                   /* Timer0 Enable                */
    VICVectAddr0 = (U32)tc0;            /* set interrupt vector in 0    */
    VICVectCntl0 = 0x20 | 4;            /* use it for Timer 0 Interrupt */
    timeval      = 0;                   /* initialise the timer         */

} /* TIM_init() */


/******************************************************************************
*
*  Function    : TIM_start
*
*  Description : enables timer0 interrupt
*
*  Notes       : Adapted from uVision/ARM development tools
*
*  Parameters  : none
*
*  Returns     : none
*
*  Reads       : none
*
*  Writes      : VICIntEnable  (Interrupt Enable Register)
*
*  Calls       : none
*
******************************************************************************/
void TIM_start( void )
{
    VICIntEnable = 0x00000010;          /* Enable Timer0 Interrupt      */

} /* TIM_start() */


/******************************************************************************
*
*  Function    : TIM_start_simulation
*
*  Description : changes to simulation mode - starts simulation
*
*  Notes       : Adapted from uVision/ARM development tools
*
*  Parameters  : none
*
*  Returns     : none
*
*  Reads       : none
*
*  Writes      : timeval (local)
*
*  Calls       : SCH_start (scheduler.c)
*                TIM_start (local)
*
******************************************************************************/
void TIM_start_simulation( void )
{
    timeval = 0;     /* reset timer      */
    SCH_start( 0 );  /* run scheduler    */
    TIM_start();     /* start the timer  */

} /* TIM_start_simulation() */


/******************************************************************************
*
*  Function    : TIM_continue_simulation
*
*  Description : changes to simulation mode - starts simulation
*
*  Notes       : Adapted from uVision/ARM development tools
*
*  Parameters  : none
*
*  Returns     : none
*
*  Reads       : timeval               (local)
*                test_length_increment (test.c)
*
*  Writes      : test_length           (test.c)
*
*  Calls       : none
*
******************************************************************************/
void TIM_continue_simulation( void )
{
    SCH_start( timeval );                  /* run scheduler    */

} /* TIM_continue_simulation() */



/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/


/******************************************************************************
*
*  Function    : tc0
*
*  Description : Timer Counter 1 Interrupt executes each 100ms @60 MHz
*                CPU Clock
*
*  Notes       : Adapted from uVision/ARM development tools
*
*  Parameters  : none
*
*  Returns     : none
*
*  Reads       : mode                  (test.c)
*                timeval               (local)
*
*  Writes      : test_length           (test.c)
*                mode                  (test.c)
*                timeval               (local)
*                T0IR                  (Interrupt Register)
*                VICVectAddr           (Vector Address Register)
*
*  Calls       : TSK_run_task_cycle    (task.c)
*                SCH_start             (scheduler.c)
*                TSK_end_of_test       (task.c)
*
******************************************************************************/
void tc0( void )
{ 
    if( mode is MODE_SIMULATION )
    {
        timeval++;                      /* increment timer       */

        TSK_run_task_cycle( timeval );  /* run task              */

        SCH_start( timeval );           /* call scheduler        */

        if( timeval > test_duration )
        {
            mode = MODE_TERMINAL;       /* end test              */
            TSK_end_of_test();          /* inform task object    */
            TRM_end_of_test();          /* inform term object    */
        }
    }

    T0IR        = 1;                    /* Clear interrupt flag  */
    VICVectAddr = 0;                    /* Acknowledge Interrupt */

} /* tc0() */


#else
/*******************************/
/* compile for PC based system */
/*******************************/

/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/


/* dummy functions */
void TIM_init( void ){}
void TIM_start_simulation( void ){}

#endif
