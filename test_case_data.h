/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  test_case_data.h                                             *
*                                                                             *
* Description :  test case data file                                          *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 25-11-05 |JRO| Created - to reduce size of data with embedded build         *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _TEST_CASE_DATA_H_
#define _TEST_CASE_DATA_H_

/* array of test cases */
static struct test_case_type test_cases[MAX_TEST_CASES] =
{
/*
      ----------------- test case id
      |       ----------------- test length
      |      |              ----------------- algorithm
      |      |              |   ----------------- task 1 id
      |      |              |   |   ----------------- task 2 id
      |      |              |   |   |   ----------------- task 3 id
      |      |              |   |   |   |   ----------------- task 4 id
      |      |              |   |   |   |   |   ----------------- task 5 id
      |      |              |   |   |   |   |   |   ----------------- task 6 id
      |      |              |   |   |   |   |   |   |   ----------------- task 7 id
      |      |              |   |   |   |   |   |   |   |
      |      |              |   |   |   |   |   |   |   | */
    { 0,     0,        ALG_RM,  0,  0,  0,  0,  0,  0,  0 }, /* dummy */
    { 1,   400,        ALG_RM,  1,  2,  3,  4,  5,  6,  7 }, /* RM demo */
    { 2,   400,        ALG_RM,  8,  2,  3,  4,  5,  6,  9 }, /* RM with 1&7 priorities swapped */
    { 3,   400,        ALG_RM,  1, 10, 11, 12, 13, 14, 15 }, /* U=0.856 */
    { 4,   550,        ALG_RM, 16, 17, 18, 19, 20, 21, 22 }, /* U=0.921 Harmonic periods */
    { 5,   900,        ALG_RM, 30, 24, 25, 26, 27, 28, 29 }, /* RM  U=1.000 */
    { 6, 30000,       ALG_EDF, 30, 24, 25, 26, 27, 28, 29 }, /* EDF U=1.000 */
    { 7, 30000,       ALG_EDF, 30, 24, 25, 26, 27, 28, 31 }, /* EDF U=1.001 - showing overrun */
    { 8,   300,        ALG_RM, 32, 33, 34,  0,  0,  0,  0 }, /* buttazzo's overload */
    { 9,   600,       ALG_LLF, 35, 36,  0,  0,  0,  0,  0 }, /* LLF        */
    {10,   600,       ALG_EDF, 35, 36,  0,  0,  0,  0,  0 }, /* EDF        */
    {11,   600,      ALG_MLLF, 35, 36,  0,  0,  0,  0,  0 }, /* MLLF       */
    {12,   600,      ALG_MLLF, 23, 24, 25, 26, 27, 28, 29 }, /* MLLF       */
    {13,   600,        ALG_RM, 37, 38, 39, 40,  0,  0,  0 }, /* MUF        */
    {14,   600,       ALG_EDF, 37, 38, 39, 40,  0,  0,  0 }, /* MUF        */
    {15,   600,       ALG_MUF, 37, 38, 39, 40,  0,  0,  0 }, /* MUF        */
    {16,   600,      ALG_MMUF, 37, 38, 39, 40,  0,  0,  0 }, /* MMUF       */
    {17,   600,     ALG_MMMUF, 37, 38, 39, 40,  0,  0,  0 }, /* MMMUF      */
    {18,   600,       ALG_EDF, 41, 42,  0,  0,  0,  0,  0 }, /* task skip  */
    {19,   600,   ALG_EDF_RTO, 41, 42,  0,  0,  0,  0,  0 }, /* task skip  */
    {20,   600,   ALG_EDF_RTO, 43, 44,  0,  0,  0,  0,  0 }, /* task skip  */
    {21,   600,       ALG_SPT, 43, 44,  0,  0,  0,  0,  0 }, /* SPT */
    {22,  1000,        ALG_RM, 45, 46, 47,  0, 49, 50, 51 }, /* RM on the edge */
    {23,  1000,        ALG_RM, 45, 46, 47, 48, 49, 50, 51 }, /* RM Sporadic overload */
    {24, 10000,       ALG_EDF, 45, 46, 47,  0, 49, 50, 51 }, /* EDF on the edge */
    {25, 10000,       ALG_EDF, 45, 46, 47, 48, 49, 50, 51 }, /* EDF Sporadic overload */
    {26, 30000,       ALG_EDF, 45, 46, 47,  0, 49, 50, 52 }, /* EDF right on the edge */
    {27, 30000,       ALG_EDF, 45, 46, 47, 53, 49, 50, 52 }, /* EDF Sporadic overload */
    {28, 30000,       ALG_EDF, 45, 46, 47, 54, 49, 50, 52 }, /* EDF Sporadic overload */
    {29, 30000,       ALG_EDF, 45, 46, 47, 55, 49, 50, 52 }, /* EDF Sporadic overload */
    {30, 30000,       ALG_EDF, 45, 46, 47, 56, 49, 50, 52 }, /* EDF Sporadic overload */
    {31, 30000,       ALG_EDF, 45, 46, 47, 57, 49, 50, 52 }, /* EDF Sporadic overload */
    {32, 30000,       ALG_EDF, 45, 46, 47, 58, 49, 50, 52 }, /* EDF Sporadic overload */
    {33, 30000,       ALG_EDF, 45, 46, 47, 59, 49, 50, 52 }, /* EDF Sporadic overload */
    {34, 30000,       ALG_EDF, 45, 46, 47, 60, 49, 50, 52 }, /* EDF Sporadic overload */
    {35, 30000,       ALG_EDF, 45, 46, 47, 61, 49, 50, 52 }, /* EDF Sporadic overload */
    {36, 30000,       ALG_EDF, 45, 46, 47, 62, 49, 50, 52 }, /* EDF Sporadic overload */
    {37, 30000,       ALG_EDF, 45, 46, 47, 63, 49, 50, 52 }, /* EDF Sporadic overload */
    {38, 30000,       ALG_EDF, 45, 46, 47, 64, 49, 50, 52 }, /* EDF Sporadic overload */
    {39, 30000,       ALG_EDF, 45, 46, 47, 65, 49, 50, 52 }, /* EDF Sporadic overload */
    {40, 30000,       ALG_EDF, 45, 46, 47, 66, 49, 50, 52 }, /* EDF Sporadic overload */
    {41, 30000,       ALG_EDF, 45, 46, 47, 67, 49, 50, 52 }, /* EDF Sporadic overload */
    {42, 30000,       ALG_EDF, 45, 46, 47, 68, 49, 50, 52 }, /* EDF Sporadic overload */
    {43, 30000,       ALG_EDF, 45, 46, 47, 69, 49, 50, 52 }, /* EDF Sporadic overload */
    {44, 30000,       ALG_EDF, 45, 46, 47, 70, 49, 50, 52 }, /* EDF Sporadic overload */
    {45, 30000,       ALG_EDF, 45, 46, 47, 71, 49, 50, 52 }, /* EDF Sporadic overload */
    {46, 30000,       ALG_EDF, 45, 46, 47, 72, 49, 50, 52 }, /* EDF Sporadic overload */
    {47,(U32)300000,  ALG_EDF, 30, 24, 25, 26, 27, 28, 31 }, /* EDF U=1.001 - showing overrun  */
    {48,  1000,   ALG_ADAP_01, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-01 */
    {49,  1000,   ALG_ADAP_02, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-02 */
    {50,  1000,   ALG_ADAP_03, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-03 */
    {51,  1000,   ALG_ADAP_04, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-04 */
    {52, 10000,   ALG_ADAP_04, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-04 but with 10,000 duration */
    {53, 10000,   ALG_ADAP_04, 80, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-04 but with 10,000 duration & U1003 */
    {54, 10000,   ALG_ADAP_04, 81, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-04 but with 10,000 duration & overrun */
    {55,  1000,   ALG_ADAP_05, 73, 74, 75, 76, 77, 78, 79 }, /* ADAPTABLE-05 */
    {56,  1000,   ALG_ADAP_06, 82, 83, 84, 85, 86, 87, 88 }, /* ADAPTABLE-06 */
    {57,  1000,   ALG_ADAP_07, 82, 83, 84, 85, 86, 87, 88 }, /* ADAPTABLE-07 control */
    {58,   200,   ALG_ADAP_06, 89, 90, 91,  0,  0,  0,  0 }, /* ADAPTABLE-06 3 tasks */
    {59,   200,   ALG_ADAP_07, 89, 90, 91,  0,  0,  0,  0 }, /* ADAPTABLE-07 3 tasks */
    {60,   200,   ALG_ADAP_06, 89, 92, 91,  0,  0,  0,  0 }, /* ADAPTABLE-06 3 tasks */
    {61,   200,   ALG_ADAP_07, 89, 92, 91,  0,  0,  0,  0 }  /* ADAPTABLE-07 3 tasks */
};

#endif /* _TEST_CASE_DATA_H_ */
