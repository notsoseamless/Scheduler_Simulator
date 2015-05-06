/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR										  *
*                                                                             *
* File Name   :  globals.h                                                    *
*                                                                             *
* Description :  central file for global data types                           *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 10-03-05 |JRO| Created                                                      *
* 13-03-05 |JRO| Added boolean and NULL types                                 *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/* conditional compilation between Keil Development system and PC */

/************************/
/**** BUILD SELECTOR ****/
/************************/
#if(1)
    #undef  DEV_SYSTEM
#else
    #define DEV_SYSTEM
#endif
/************************/


/* other global defines */
#define MAX_LINE_LEN    500       /* standard test line           */
#define MAX_NAME_LEN    100       /* standard name string length  */
#define NUM_OF_TASKS      8       /* number of scheduler tasks    */
#define MAX_TEST_CASES   62       /* number of test cases         */

#define version_num "Version: 10.12"


/* global data types */
/* ================= */

typedef unsigned char          U8;
typedef signed   char          S8;
typedef unsigned short int     U16;
typedef unsigned int           U32;
typedef signed   int           S32;

/* easier to read types */
#define and             &&
#define or              ||
#define is              ==
#define is_not          !=

/* magic numbers */
#define ZERO_CPV         0
#define ONE_CPV          1
#define TWO_CPV          2
#define THREE_CPV        3
#define FOUR_CPV         4
#define FIVE_CPV         5
#define SIX_CPV          6
#define SEVEN_CPV        7
#define EIGHT_CPV        8
#define NINE_CPV         9
#define TEN_CPV          10
#define ELEVEN_CPV       11
#define TWELVE_CPV       12
#define FIFTEEN_CPV      15
#define TWENTY_CPV       20
#define ONE_HUNDRED_CPV  100
#define TWO_HUNDRED_CPV  200
#define ONE_THOUSAND_CPV 1000
#define TEN_THOUSAND_CPV 10000

/* define boolean type */
#ifndef TRUE
typedef enum
{
    FALSE = 0,
    TRUE  = 0xFF
} boolean;
#endif

#ifndef NULL
    #define NULL 0
#endif

/* simulator modes */
typedef enum
{
    MODE_TERMINAL,     /* terminal mode   */
    MODE_SIMULATION,   /* simulation mode */
    MODE_ABORT         /* abort to main   */
}mode_type;

extern mode_type mode; /* defined in test.c */

/* timer tick length of test */
extern U32 test_duration;
extern U8  test_num;

#endif  /* _GLOBALS_H_ */
