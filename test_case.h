/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  test_case.h                                                  *
*                                                                             *
* Description :  interface file for test_case.c                               *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 26-04-05 |JRO| Created                                                      *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TEST_CASE_H_
#define _TEST_CASE_H_

/* structure to contain a test case */
struct test_case_type
{
    const U8             id;            /* id of test case     */
    const U32            length;        /* length of test      */
    const algorithm_type algorithm;     /* scheduler algorithm */
    const U8             task_1;        /* test task           */
    const U8             task_2;        /* test task           */
    const U8             task_3;        /* test task           */
    const U8             task_4;        /* test task           */
    const U8             task_5;        /* test task           */
    const U8             task_6;        /* test task           */
    const U8             task_7;        /* test task           */
};

struct test_case_type *TSC_get_test_case( const U8 case_no );
void TSC_report_test_case( const U8 c, const message_class class );
void TSC_report_test_case_notes( const U8 test_case );

#endif
