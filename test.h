/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  
*                                                                             *
* File Name   :  test.h                                                       *
*                                                                             *
* Description :  header file for test module                                  *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 10-03-05 |JRO| Created                                                      *
* 17-04-05 |JRO| Added screen control for development system                  *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TEST_H_
#define _TEST_H_


/* enumeration of test case report parameters */
typedef enum
{
    TST_DURATION,       /* Duration of test */
    TST_PREEMPTS,       /* Pre-emptions     */
    TST_SKIPS,          /* Skips            */
	TST_REMOVALS,		/* Removed tasks    */
	TST_PER_DOUBLES,	/* period doubles   */
    TST_DEADLN_MIS,     /* Missed deadlines */
    TST_DEADLN_MET,     /* Met deadlines    */
    TST_VALUE,          /* Value            */
	TST_UTILITY,		/* Achieved utility */
    TST_MAX_TST
}test_case_parm_index_type;


void TST_init( void );
void TST_soft_reset( void );
void TST_test( void );
void TST_start_simulation( void );
void TST_continue_simulation( void );
void TST_abort( void );
void TST_load_test( const U8 num );
void TST_get_test_loop( const U8 num );
void TST_incr_test_report_value( const test_case_parm_index_type parm );
void TST_set_test_report_value( const test_case_parm_index_type parm,
                                const U32 val );
void TST_add_test_report_value( const test_case_parm_index_type parm,
                                const U32 value );
void TST_test_report( void );

#endif /* _TEST_H_ */
