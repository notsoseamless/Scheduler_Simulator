/******************************************************************************
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  rt_clock.c                                                   *
*                                                                             *
* Description :  Physical layer functionality                                 *
*                manages hardware interface to real time clock                *
*                Keil Development System, or PC (see in globals.h)            *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 03-06-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "rt_clock.h"  

#ifdef DEV_SYSTEM
/***************************************/
/* compile for Keil development system */
/***************************************/
#include "LPC21XX.H"                       /* LPC21XX Peripheral Registers */
   
static volatile U32 real_time;

static U32 ref_time;

void RTC_isr(void)     __attribute__ ((interrupt));

/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/
  

/******************************************************************************
*  Description : Setup clock registers
******************************************************************************/
void CLK_init( void )
{
    CCR     = 0x02;        /* reset the clock                      */
    PREINT  = 0x00000392;  /* Set RTC prescaler for 12.000Mhz Xtal */
    PREFRAC = 0x00004380;
    CIIR    = 0x00;        /* interrupt increment register         */
    AMR     = 0x00;        /* alarm mask register                  */
    SEC     = 0x00;        /* seconds register                     */
    MIN     = 0x00;        /* minutes register                     */
    HOUR    = 0x00;        /* hours register                       */
    CCR     = 0x01;        /* enable the clock                     */

    VICVectAddr13 = (U32)RTC_isr; /* Set timer ISR vector address  */
    VICVectCntl13 = 0x0000002D;   /* Set channel                   */
    VICIntEnable  = 0x00002000;   /* Enable the interrupt          */
} 

/******************************************************************************
*  Description : Getter function returns real time
******************************************************************************/
U32 CLK_get_real_time( void )
{
    real_time = CTC;       /* read the clock tick counter */

    return( real_time );
}


/******************************************************************************
*  Description : Resets a reference timer
******************************************************************************/
void CLK_start_timer( const U32 pre_load )
{
    /* get the time */
    real_time = CTC;       /* read the clock tick counter */

    /* set the reference time */
    ref_time = real_time + pre_load;
}

/******************************************************************************
*  Description : Returns time since call to CLK_start_timer
******************************************************************************/
U32 CLK_end_timer( void )
{
    U32 delta_time;
    
    real_time  = CTC;           /* get the time */
         
    delta_time = real_time - ref_time;

    return( delta_time );
}

/******************************************************************************
*  Description : Returns the clocks per second value
******************************************************************************/
U32 CLK_clocks_per_second( void )
{
    return( 32768 );
}

/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/

   
   
/******************************************************************************
*  Description : Real time clock interrupt 
*  Notes       : Adapted from uVision/ARM development tools
******************************************************************************/
void RTC_isr(void)     
{
    if(ILR & 0x00000001)		/* Test for RTC counter interrupt */
    {
        ILR = 0x00000001;		/* Clear the interrupt register */
    }

    if(ILR & 0x00000002)
    {
        ILR = 0x00000002;
    }

    VICVectAddr = 0x00000000;	/* Dummy write to signal end of interrupt */
}




#else
/*******************************/
/* compile for PC based system */
/*******************************/

/*lint -e628 no argument information provided for function clock() */
#include <time.h>
/*lint -e628 */

static U32 ref_time;



/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/




/******************************************************************************
*  Description : Set up the real time clock
*  Notes       : uclock() returns UCLOCKS_PER_SEC defined as 1193180
******************************************************************************/
void CLK_init( void )
{
    ref_time = (U32)clock();
}

/******************************************************************************
*  Description : Getter function returns elapsed time since start-up
*  Notes       : uclock() returns UCLOCKS_PER_SEC defined as 1193180
******************************************************************************/
U32 CLK_get_real_time( void )
{
    return( (U32)clock() );
}

/******************************************************************************
*  Description : Resets a reference timer
*  Notes       : uclock() returns UCLOCKS_PER_SEC defined in time.c as 1193180
******************************************************************************/
void CLK_start_timer( const U32 pre_load )
{
    ref_time = (U32)clock() + pre_load;
}


/******************************************************************************
*  Description : Returns time since call to CLK_start_timer
*  Notes       : uclock() returns UCLOCKS_PER_SEC defined in time.c as 0x1234DC
******************************************************************************/
U32 CLK_end_timer( void )
{
    U32 u_time;
    U32 d_time;

    u_time = (U32)clock();

    d_time = u_time - ref_time;

    return( d_time );
}

/******************************************************************************
*  Description : Returns the value of CLOCKS_PER_SEC macro
*  Notes       : uclock() returns CLOCKS_PER_SEC defined in time.c as 0x1234DC
******************************************************************************/
U32 CLK_clocks_per_second( void )
{
    return( (U32)CLOCKS_PER_SEC );
}

#endif


/*******************************/
/* compile for common system   */
/*******************************/



