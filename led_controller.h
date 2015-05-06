/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  led_controller.h                                             *
*                                                                             *
* Description :  interface file for led_controller.c                          *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date    |Auth| Description                                                  *
* 12-03-05|JRO | Created                                                      *
*         |    |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

void LED_fan_fair( void );

#ifdef DEV_SYSTEM         /* CONDITIONAL COMPILE - Keil Development System */
    void LED_set_code( const U32 code );
#endif  /* DEV_SYSTEM */


#endif
