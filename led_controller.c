/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  led_controller.c                                             *
*                                                                             *
* Description :  Physical layer functionality                                 *
*                manages hardware interface to LEDS                           *
*                Keil Development System, or PC (see in globals.h)            *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date    |Auth| Description                                                  *
* 12-03-05|JRO | Created                                                      *
*         |    |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"

#ifdef DEV_SYSTEM         /* CONDITIONAL COMPILE - Keil Development System */
    #include "LPC21xx.H"
#endif  /* DEV_SYSTEM */

#include "led_controller.h"


#ifdef DEV_SYSTEM
/***************************************/
/* compile for Keil development system */
/***************************************/
#define NUM_OF_LEDS 7  /* 0-7 */
#define LED_MASK    0xff0000;

static U32 led_state;

/* private function prototypes */
static void all_off( void );
static void wait( void );
static void alternate( void );
static void step_down( void );
static void step_up( void );


/******************************************************************************
*                             PUBLIC FUNCTIONS
******************************************************************************/



/******************************************************************************
*  Function    : LED_fan_fair
*  Description : Flashes all leds - indicates sucessful boot-up
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
void LED_fan_fair( void )
{
    alternate();
    step_down();
    step_up();
    step_down();
    alternate();
    step_up();
    step_down();
    step_up();
    alternate(); 

} /* LED_fan_fair() */


/******************************************************************************
*  Function    : LED_set
*  Description : switches an led
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
void LED_set( const U32 led_num )
{
    /* switch all leds off */
    all_off();

    /* set led mask according to number */
    if( led_num <= NUM_OF_LEDS )
    {
        switch (led_num )
        {
            case 0:
                led_state = (U32)0x00800000;
                break;
            case 1:
                led_state = (U32)0x00400000;
                break;
            case 2:
                led_state = (U32)0x00200000;
                break;
            case 3:
                led_state = (U32)0x00100000;
                break;
            case 4:
                led_state = (U32)0x00080000;
                break;
            case 5:
                led_state = (U32)0x00040000;
                break;
            case 6:
                led_state = (U32)0x00020000;
                break;
            case 7:
                led_state = (U32)0x00010000;
                break;
            default:
                /* do nothing */
                break;
        }

        /* set led */
        IOSET1 = led_state;
    }

} /* LED_set() */


/******************************************************************************
*  Description : sets leds to specific code set in code
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
void LED_set_code( const U32 code )
{
    all_off();

    IOSET1 = code;

} /* LED_set_code() */

/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/


/*****************************************************************************/
static void all_off( void )
{
    IOCLR1 = LED_MASK;

} /* all_off */


/******************************************************************************
*  Description : sets a delay
******************************************************************************/
static void wait( void )
{
    U32 count;

    count = 0x00000000U;

    while( count < 0x0000FFFFU )
    {
        count++;
    }

} /* wait() */


/******************************************************************************
*  Description : switches alternate leds
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
static void alternate( void )
 {
    U32 index;
    IODIR1 = 0x00FF0000L;               /* P1.16..23 defined as Outputs */
    
    index = 0x00aa0000;                           /* alternative leds   */
    IOSET1 = index;                               /* Turn on LED        */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    IOCLR1 = index;                               /* Turn off LED       */
    index = 0x00550000;                           /* alternative leds   */
    IOSET1 = index;                               /* Turn on LED        */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    wait();                                       /* call wait function */
    IOCLR1 = index;                               /* Turn off LED       */

} /* alternate() */


/******************************************************************************
*  Description : steps down leds
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
static void step_down( void )
{
    U32 index;
    IODIR1 = 0x00FF0000L;              /* P1.16..23 defined as Outputs */

    for (index=0x00010000; index<0x00800000; index<<=1)
    { /* Blink LED 0,1,2,3,4,5,6 */
        IOSET1 = index;                           /* Turn on LED        */
        wait();                                   /* call wait function */
        IOCLR1 = index;                           /* Turn off LED       */
    }

} /* step_down() */


/******************************************************************************
*  Description : steps up leds
*  Notes       : Adapted from part of the uVision/ARM development tools
*                supplied with the uVision IDE blinky.c
******************************************************************************/
static void step_up( void )
{
    U32 index;
    IODIR1 = 0x00FF0000L;              /* P1.16..23 defined as Outputs */

    for (index=0x00010000; index<0x00800000; index<<=1)
    { /* Blink LED 0,1,2,3,4,5,6 */
        IOSET1 = index;                           /* Turn on LED        */
        wait();                                   /* call wait function */
        IOCLR1 = index;                           /* Turn off LED       */
    }

} /* step_up() */



#else
/*******************************/
/* compile for PC based system */
/*******************************/

/******************************************************************************
*  Description : dummy function
******************************************************************************/
void LED_fan_fair( void ) {}

#endif /* DEV_SYSTEM */
