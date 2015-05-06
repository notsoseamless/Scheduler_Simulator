/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  *
*                                                                             *
* File Name   :  test_case.c                                                  *
*                                                                             *
* Description :  container for test cases                                     *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |who| description                                                  *
* 26-04-05 |JRO| Created                                                      *
* 25-06-05 |JRO| Added ALG_ADAP_01 & ALG_ADAP_02 scheduler                    *
* 26-06-05 |JRO| Added ALG_ADAP_03 & ALG_ADAP_04 scheduler                    *
* 15-07-05 |JRO| Added ALG_ADAP_05 & ALG_ADAP_06 scheduler                    *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "messages_out.h"
#include "scheduler.h"
#include "utils.h"
#include "test_case.h"
#include "test_case_data.h"


/******************************************************************************
*                             PRIVATE DATA
******************************************************************************/
#ifndef DEV_SYSTEM
static boolean notes_initialised = FALSE;

struct notes_type
{
	S8 note[MAX_LINE_LEN];
};

static struct notes_type notes[MAX_TEST_CASES];
#endif /* DEV_SYSTEM */


/******************************************************************************
*                      PRIVATE FUNCTION PROTOTYPES
******************************************************************************/
static void init_notes( void );


/******************************************************************************
*                            PUBLIC FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : TSC_get_test_case
*  Description : places test_case into test_case
*  Parameters  : case_no   - number of test case
*  Returns     : pointer to test case indexed by case_no
******************************************************************************/
struct test_case_type *TSC_get_test_case( const U8 case_no )
{
    return( &test_cases[case_no] );

} /* TSC_get_test_case() */


/******************************************************************************
*  Function    : TSC_report_test_case
*  Description : make test case report - calls out_res.OPR_0_message() to report
*                selected test case.
*  Parameters  : c - test case number
*  Returns     : none
******************************************************************************/
void TSC_report_test_case( const U8 c, const message_class class )
{
    switch ( c )
    {
        case 1:
            MESS_OUT_message( (S8*)"(01) RM T={t1,t2,t3,t4,t5,t6,t7} U=0.726 Standard Set-up)\n", class );
            break;
        case 2:
            MESS_OUT_message( (S8*)"(02) RM {t7,t2,t3,t4,t5,t6,t1} U=0.726 (With Task Swap)\n", class );
            break;
        case 3:
            MESS_OUT_message( (S8*)"(03) RM T={t1,t2,t3,t4,t5,t6,t7} U=0.856 (Pushing utility)\n", class );
            break;
        case 4:
            MESS_OUT_message( (S8*)"(04) RM T={t1,t2,t3,t4,t5,t6,t7} U=0.921 (Harmonic Periods)\n", class );
            break;
        case 5:
            MESS_OUT_message( (S8*)"(05) RM T={t1,t2,t3,t4,t5,t6,t7} U=1.000 (EDF Comparison Demo)\n", class );
            break;
        case 6:
            MESS_OUT_message( (S8*)"(06) EDF T={t1,t2,t3,t4,t5,t6,t7} U=1.000 (EDF Comparison Demo)\n", class );
            break;
        case 7:
            MESS_OUT_message( (S8*)"(07) EDF T={t1,t2,t3,t4,t5,t6,t7} U=1.001 (Minor overload)\n", class );
            break;
        case 8:
            MESS_OUT_message( (S8*)"(08) RM T={t1,t2,t3} U=1.250 (Buttazzo's overload)\n", class );
            break;
        case 9:
            MESS_OUT_message( (S8*)"(09) LLF T={t1,t2} U=0.700 (LLF Demo)\n", class );
            break;
        case 10:
            MESS_OUT_message( (S8*)"(10) EDF T={t1,t2} U=0.700 (LLF Demo)\n", class );
            break;
        case 11:
            MESS_OUT_message( (S8*)"(11) MLLF T={t1,t2} U=0.700 (LLF Demo)\n", class );
            break;
        case 12:
            MESS_OUT_message( (S8*)"(12) MLLF T={t1,t2,t3,t4,t5,t6,t7} U=0.999\n", class );
            break;
        case 13:
            MESS_OUT_message( (S8*)"(13) RM T={t1,t2,t3,t4} U=1.250\n", class );
            break;
        case 14:
            MESS_OUT_message( (S8*)"(14) EDF T={t1,t2,t3,t4} U=1.250\n", class );
            break;
        case 15:
            MESS_OUT_message( (S8*)"(15) MUF T={t1,t2,t3,t4} U=1.250\n", class );
            break;
        case 16:
            MESS_OUT_message( (S8*)"(16) MMUF T={t1,t2,t3,t4} U=1.250\n", class );
            break;
        case 17:
            MESS_OUT_message( (S8*)"(17) MMMUF T={t1,t2,t3,t4} U=1.250\n", class );
            break;
        case 18:
            MESS_OUT_message( (S8*)"(18) EDF T={t1, t2} U=1.100 (demo for task skip)\n", class );
            break;
        case 19:
            MESS_OUT_message( (S8*)"(19) Red Tasks Only (RTO) Skip Algorithm T={t1,t2} U=1.100 S=2\n", class );
            break;
        case 20:
            MESS_OUT_message( (S8*)"(20) Red Tasks Only (RTO) Skip Algorithm T={t1,t2} U=1.100 S=1\n", class );
            break;
        case 21:
            MESS_OUT_message( (S8*)"(21) SPT T={t1, t2} U=1.100\n", class );
            break;
        case 22:
            MESS_OUT_message( (S8*)"(22) RM T={t1,t2,t3,t4,t5,t6} U=0.860\n", class );
            break;
        case 23:
            MESS_OUT_message( (S8*)"(23) RM T={t1,t2,t3,t4,t5,t6} U=0.860 with sporadic 50 task overload\n", class );
            break;
        case 24:
            MESS_OUT_message( (S8*)"(24) EDF T={t1,t2,t3,t4,t5,t6} U=0.860\n", class );
            break;
        case 25:
            MESS_OUT_message( (S8*)"(25) EDF T={t1,t2,t3,t4,t5,t6} U=0.860 with sporadic 50 task overload\n", class );
            break;
        case 26:
            MESS_OUT_message( (S8*)"(26) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 control set\n", class );
            break;
        case 27:
            MESS_OUT_message( (S8*)"(27) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=10 task overload\n", class );
            break;
        case 28:
            MESS_OUT_message( (S8*)"(28) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=20 task overload\n", class );
            break;
        case 29:
            MESS_OUT_message( (S8*)"(29) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=30 task overload\n", class );
            break;
        case 30:
            MESS_OUT_message( (S8*)"(30) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=40 task overload\n", class );
            break;
        case 31:
            MESS_OUT_message( (S8*)"(31) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=50 task overload\n", class );
            break;
        case 32:
            MESS_OUT_message( (S8*)"(32) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=60 task overload\n", class );
            break;
        case 33:
            MESS_OUT_message( (S8*)"(33) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=70 task overload\n", class );
            break;
        case 34:
            MESS_OUT_message( (S8*)"(34) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=80 task overload\n", class );
            break;
        case 35:
            MESS_OUT_message( (S8*)"(35) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=90 task overload\n", class );
            break;
        case 36:
            MESS_OUT_message( (S8*)"(36) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=100 task overload\n", class );
            break;
        case 37:
            MESS_OUT_message( (S8*)"(37) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=110 task overload\n", class );
            break;
        case 38:
            MESS_OUT_message( (S8*)"(38) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=120 task overload\n", class );
            break;
        case 39:
            MESS_OUT_message( (S8*)"(39) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=130 task overload\n", class );
            break;
        case 40:
            MESS_OUT_message( (S8*)"(40) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=140 task overload\n", class );
            break;
        case 41:
            MESS_OUT_message( (S8*)"(41) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=150 task overload\n", class );
            break;
        case 42:
            MESS_OUT_message( (S8*)"(42) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=160 task overload\n", class );
            break;
        case 43:
            MESS_OUT_message( (S8*)"(43) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=170 task overload\n", class );
            break;
        case 44:
            MESS_OUT_message( (S8*)"(44) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=180 task overload\n", class );
            break;
        case 45:
            MESS_OUT_message( (S8*)"(45) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=190 task overload\n", class );
            break;
        case 46:
            MESS_OUT_message( (S8*)"(46) EDF T={t1,t2,t3,t4,t5,t6} U=1.000 with sporadic T=200 task overload\n", class );
            break;
        case 47:
            MESS_OUT_message( (S8*)"(47) EDF T={t1,t2,t3,t4,t5,t6,t7} U=1.001 test 7 with 300,000 duration\n", class );
            break;
        case 48:
            MESS_OUT_message( (S8*)"(48) ADAPTABLE-01 T={t1,t2,t3,t4,t5,t6} task removal\n", class );
            break;
        case 49:
            MESS_OUT_message( (S8*)"(49) ADAPTABLE-02 T={t1,t2,t3,t4,t5,t6} task removal & recovery\n", class );
            break;
        case 50:
            MESS_OUT_message( (S8*)"(50) ADAPTABLE-03 T={t1,t2,t3,t4,t5,t6} period doubler\n", class );
            break;
        case 51:
            MESS_OUT_message( (S8*)"(51) ADAPTABLE-04 T={t1,t2,t3,t4,t5,t6} period doubler & halver\n", class );
            break;
        case 52:
            MESS_OUT_message( (S8*)"(52) ADAPTABLE-04 Test Case 51 with 10,000 duration\n", class );
            break;
        case 53:
            MESS_OUT_message( (S8*)"(53) ADAPTABLE-04 Test case 51 with 10,000 duration & U=1003\n", class );
            break;
        case 54:
            MESS_OUT_message( (S8*)"(54) ADAPTABLE-04 Test Case 51 with 10,000 duration & U=1003 (bad utility)\n", class );
            break;
        case 55:
            MESS_OUT_message( (S8*)"(55) ADAPTABLE-05 Dynamic overrun detection using laxity\n", class );
            break;
        case 56:
            MESS_OUT_message( (S8*)"(56) ADAPTABLE-06 Test Case 51 with 1,000 dur & calculated util + laxity\n", class );
            break;
        case 57:
            MESS_OUT_message( (S8*)"(57) ADAPTABLE-07 Test Case 51 with 1,000 dur & calculated util + laxity\n", class );
            break;
        case 58:
            MESS_OUT_message( (S8*)"(58) ADAPTABLE-06 2 periodic + 1 aperiodic using utility - doubling of 1 task\n", class );
            break;
        case 59:
            MESS_OUT_message( (S8*)"(59) ADAPTABLE-07 2 periodic + 1 aperiodic using utility - multi doubling of 1 task\n", class );
            break;
        case 60:
            MESS_OUT_message( (S8*)"(60) ADAPTABLE-06 2 periodic + 1 aperiodic using utility - multi doubling of 2 tasks\n", class );
            break;
        case 61:
            MESS_OUT_message( (S8*)"(61) ADAPTABLE-07 2 periodic + 1 aperiodic using utility - multi doubling of 2 tasks\n", class );
            break;
        default:
            MESS_OUT_message( (S8*)"NO TEST CASE SELECTED\n", class );
            break;
    }
} /* TSC_report_test_case() */


/******************************************************************************
*  Function    : TSC_report_test_case_notes
*  Description : Outputs test case notes
*  Parameters  : test_case - test case number
*  Returns     : none
******************************************************************************/
void TSC_report_test_case_notes( const U8 test_case )
{
#ifndef DEV_SYSTEM /* exclude for development system build to reduce size */
	if( notes_initialised is FALSE )
	{
		/* initialise the notes structure */
		init_notes();
	}

    MESS_OUT_message_1( (S8*)"TSC_report_note() called with ",
				        test_case,
                        MESSAGE_OP_DEBUG );
	
	if( test_case < MAX_TEST_CASES )
	{  
		MESS_OUT_message( (S8*)"Notes:", MESSAGE_OP_RESULTS );
		MESS_OUT_message( notes[test_case].note, MESSAGE_OP_RESULTS );
		MESS_OUT_message( (S8*)"", MESSAGE_OP_RESULTS );
	}
	else
	{
		MESS_OUT_message_1( (S8*)"TSC_report_note: invalid test case: ",
		                     test_case, MESSAGE_OP_ERROR );
	}
#endif /* DEV_SYSTEM */

} /* TSC_report_test_case_notes() */
 


/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : init_notes
*  Description : private helper initialises the notes array
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void init_notes( void )
{
#ifndef DEV_SYSTEM /* too big for development system */

UTL_string_copy( notes[0].note,(S8*)
"dummy record" );

UTL_string_copy( notes[1].note,(S8*)
"Basic Rate Monotonic (RM) demonstration, with a schedulable task set"
"T={t1,t2,t3,t4,t5,t6,t7} U=0.726" );

UTL_string_copy( notes[2].note,(S8*)
"As test case one but with priorities of tasks 1 and 7 swapped to show the"	
"significance of the RM priority ordering" );

UTL_string_copy( notes[3].note,(S8*)
"RM with a utility of U=0.856, demonstrating that a higher utility than Leu &"
"Layland is achievable" );

UTL_string_copy( notes[4].note,(S8*)
"RM with a utility of U=0.921, demonstrating that with harmonic periods a"
"higher utility is possible" );

UTL_string_copy( notes[5].note,(S8*)
"Demonstration comparing RM with EDF, demonstrating that RM will overrun with"
"a utility of U=1.000" );

UTL_string_copy( notes[6].note,(S8*)
"Demonstration of EDF, over 30000 cycles, with a utility U=1.000 without"
"overrun" );

UTL_string_copy( notes[7].note,(S8*)
"EDF, over 30000 cycles, with utility U=1.001 - showing how when utility is"
"slightly over one, an overrun will occur" );

UTL_string_copy( notes[8].note,(S8*)
"Effects of overload on the EDF scheduling algorithm, observed by Buttazzo" );

UTL_string_copy( notes[9].note,(S8*)
"Least Laxity First (LLF) demonstration, shows jumping between tasks when"
"laxities become equal" );

UTL_string_copy( notes[10].note,(S8*)
"The same task set under EDF" );

UTL_string_copy( notes[11].note,(S8*)
"Same task set with Modified Least Laxity First (MLLF) that prevents jumping"
"between tasks" );

UTL_string_copy( notes[12].note,(S8*)
"Modified Least Laxity First (MLLF) with task set used in test case 5, showing"
"no overruns" );

UTL_string_copy( notes[13].note,(S8*)
"Maximum Urgency First (MUF): First demonstration with RM algorithm" );

UTL_string_copy( notes[14].note,(S8*)
"Maximum Urgency First (MUF): Second demonstration with EDF algorithm" );

UTL_string_copy( notes[15].note,(S8*)
"Maximum Urgency First (MUF): Third demonstration with MUF algorithm. Note task"
"skipping as found with LLF" );

UTL_string_copy( notes[16].note,(S8*)
"Modified Maximum Urgency First (MMUF)" );

UTL_string_copy( notes[17].note,(S8*)
"MMMUF      " );

UTL_string_copy( notes[18].note,(S8*)
"Task Skip  " );

UTL_string_copy( notes[19].note,(S8*)
"Task Skip  " );

UTL_string_copy( notes[20].note,(S8*)
"Task Skip  ");

UTL_string_copy( notes[21].note,(S8*)
"SPT ");

UTL_string_copy( notes[22].note,(S8*)
"RM on the edge");

UTL_string_copy( notes[23].note,(S8*)
"RM Sporadic overload");

UTL_string_copy( notes[24].note,(S8*)
"EDF on the edge");

UTL_string_copy( notes[25].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[26].note,(S8*)
"EDF right on the edge");

UTL_string_copy( notes[27].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[28].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[29].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[30].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[31].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[32].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[33].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[34].note,(S8*)
"EDF Sporadic overload ");

UTL_string_copy( notes[35].note,(S8*)
"EDF Sporadic overload ");

UTL_string_copy( notes[36].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[37].note,(S8*)
"EDF Sporadic overload");

UTL_string_copy( notes[38].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[39].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[40].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[41].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[42].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[43].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[44].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[45].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[46].note,(S8*)
"EDF Sporadic overload" );

UTL_string_copy( notes[47].note,(S8*)
"EDF U=1.001 - showing overrun"	  );

UTL_string_copy( notes[48].note,(S8*)
"ADAPTABLE-01: Removes low priority tasks to reduce utility" );

UTL_string_copy( notes[49].note,(S8*)
"ADAPTABLE-02 Removes low priority tasks to reduce utility. Reinstates tasks"
"whan an overload ceases" );

UTL_string_copy( notes[50].note,(S8*)
"ADAPTABLE-03"	);

UTL_string_copy( notes[51].note,(S8*)
"ADAPTABLE-04"	);

UTL_string_copy( notes[52].note,(S8*)
"ADAPTABLE-04 but with 10,000 duration" );

UTL_string_copy( notes[53].note,(S8*)
"ADAPTABLE-04 but with 10,000 duration & U1003" );

UTL_string_copy( notes[54].note,(S8*)
"ADAPTABLE-04 but with 10,000 duration & overrun" );

UTL_string_copy( notes[55].note,(S8*)
"ADAPTABLE-05" );

UTL_string_copy( notes[56].note,(S8*)
"ADAPTABLE-06"	);

UTL_string_copy( notes[57].note,(S8*)
"ADAPTABLE-07 control" );

UTL_string_copy( notes[58].note,(S8*)
"ADAPTABLE-06 3 tasks" );

UTL_string_copy( notes[59].note,(S8*)
"ADAPTABLE-07 3 tasks" );

UTL_string_copy( notes[60].note,(S8*)
"ADAPTABLE-06 3 tasks" );

UTL_string_copy( notes[61].note,(S8*)
"ADAPTABLE-07 3 tasks" );

notes_initialised = TRUE;

#endif /* DEV_SYSTEM */

} /* init_notes() */

