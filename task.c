/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR                                         *
*                                                                             *
* File Name   :  task.c                                                       *
*                                                                             *
* Description :  This module contains all task objects, including task        *
*                control blocks (TCBs) and task emulation functions.          *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 12-03-05 |JRO| Created                                                      *
* 04-04-05 |JRO| Added pre-emption count to tasks[]                           *
* 05-04-05 |JRO| Changed if/else construct in build_task_report_word() to     *
*          |   | a switch/case statement. No functional change.               *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
* 17-04-05 |JRO| Added screen control for development system                  *
* 18-06-05 |JRO| Added pre-caluclated utility * 1000 member                   *
* 25-06-05 |JRO| Added ALG_ADAP_01 & ALG_ADAP_02 scheduler                    *
* 26-06-05 |JRO| Added ALG_ADAP_03 & ALG_ADAP_04 scheduler                    *
* 15-07-05 |JRO| Added ALG_ADAP_05 & ALG_ADAP_06 scheduler                    *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#include "globals.h"
#include "utils.h"              /* general utilities            */
#include "messages_out.h"       /* access to output messaging   */
#include "scheduler.h"			/* main scheduler               */
#include "rt_clock.h"           /* real time clock timer        */
#include "test.h" 

#ifdef DEV_SYSTEM    /* CONDITIONAL COMPILE - Keil Development System */
#include "led_controller.h"     /* led output                   */
#endif  /* DEV_SYSTEM */

#include "task.h"				/* interface file               */
#include "task_data.h"    		/* task data base               */

U8 running_task_id;

static boolean task_error; 

#if(0)
/* initialisation of test task data array */
static struct task_type test_tasks[MAX_TEST_TASKS] =
{
  /*  ------------ task id
      |    ------------ release time
      |    |    ------------ execution time
      |    |    |    ------------ reletive deadline
      |    |    |    |    -------------- period
      |    |    |    |    |     ------------- task utility ((C/T) * 1000)
      |    |    |    |    |     |  -------------- flexible periodic if TRUE
      |    |    |    |    |     |  |  ------------- priority
      |    |    |    |    |     |  |  |   ------------ pre-emptable if TRUE
      |    |    |    |    |     |  |  |   |   ------------ MUF critical set member if TRUE
      |    |    |    |    |     |  |  |   |   |   ------------ red task if TRUE (skip over algorithm)
      |    |    |    |    |     |  |  |   |   |   | --------- number of skips or zero
      |    |    |    |    |     |  |  |   |   |   | | ---------- Buttazzo's elastic coefficient (E)
      |    |    |    |    |     |  |  |   |   |   | | | -----------MUF urgency value
      |    |    |    |    |     |  |  |   |   |   | | | | ---------- time left
      |    |    |    |    |     |  |  |   |   |   | | | | | ----------- time taken
      |    |    |    |    |     |  |  |   |   |   | | | | | | ------------ absolute deadline
      |    |    |    |    |     |  |  |   |   |   | | | | | | | ------------ laxity
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | -------------- calculated duration
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | -------------- calculated utility
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | -------------- pre-emption counter
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | -------------- times not skipped
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | -------------- e_priority
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | -------------- times period multiplied
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | --------------- task state
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         ------------queue pointers
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | |
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | ---- times task was skipped
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | ----- task utility
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | ---- deadlines missed
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | ---- deadlines achieved
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | ---- net value of task
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | | ----- total value of task
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | | |
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | | |
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | | |
      |    |    |    |    |     |  |  |   |   |   | | | | | | | | | | | | | | |         | | | | | | | | | | | | */
    { 0,   0,   0,   0,   0,    0, 0, 0,  0,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 1,   0,  17, 100, 100,  170, 1, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 2,   0,  22, 150, 150,  147, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 3,   0,  31, 180, 180,  172, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 4,   0,  15, 250, 250,   60, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 5,   0,  26, 260, 260,  100, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 6,   0,  18, 310, 310,   58, 0, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 7,   0,   6, 320, 320,   19, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 8,   0,  17, 100, 100,  170, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 9,   0,   6, 320, 320,   19, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {10,   0,  22, 110, 110,  200, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {11,   0,  31, 180, 180,  172, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {12,   0,  15, 200, 200,   75, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {13,   0,  26, 210, 210,  124, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {14,   0,  18, 250, 250,   72, 0, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {15,   0,  10, 300, 300,   33, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {16,   0,  45, 100, 100,  450, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {17,   0,  30, 200, 200,  150, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {18,   0,  30, 300, 300,  100, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {19,   0,  30, 400, 400,   75, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {20,   0,  30, 500, 500,   60, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {21,   0,  30, 600, 600,   50, 0, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {22,   0,  25, 700, 700,   36, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {23,   0,  29, 100, 100,  290, 0, 0,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {24,   0,  35, 150, 150,  233, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {25,   0,  10, 180, 180,   56, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {26,   0,  25, 250, 250,  100, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {27,   0,  38, 260, 260,  146, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {28,   0,  28, 310, 310,   90, 0, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {29,   0,  27, 320, 320,   85, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {30,   0,  29, 100, 100,  290, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {31,   0,  27, 315, 315,   85, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {32,   0,   4,   8,   8,  500, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {33,   0,   6,  12,  12,  500, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {34,   0,   5,  20,  20,  250, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {35,   0, 100, 200, 200,  500, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {36,   0,  20, 100, 100,  200, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {37,   0,   2,   6,   6,  333, 0, 1,  1,  1,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {38,   0,   4,  10,  10,  400, 0, 2,  1,  1,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {39,   0,   3,  12,  12,  250, 0, 3,  1,  1,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {40,   0,   4,  15,  15,  267, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {41,   0,   7,  10,  10,  700, 0, 1,  1,  0,  1,2,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {42,   0,   3,   5,   5,  600, 0, 2,  1,  0,  1,2,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {43,   0,   7,  10,  10,  700, 0, 1,  1,  0,  1,1,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {44,   0,   3,   5,   5,  600, 0, 2,  1,  0,  1,1,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {45,   0,  25, 100, 100,  250, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {46,   0,  35, 150, 150,  233, 0, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {47,   0,  10, 180, 180,   56, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {48, 400,  50, 250, 250,  200, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {49,   0,  38, 260, 260,  146, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {50,   0,  30, 310, 310,   97, 0, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {51,   0,  25, 320, 320,   78, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {52,   0,  67, 307, 320,  219, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {53, 400,  10, 250, 250,   40, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {54, 400,  20, 250, 250,   80, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {55, 400,  30, 250, 250,  120, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {56, 400,  40, 250, 250,  160, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {57, 400,  50, 250, 250,  200, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {58, 400,  60, 250, 250,  240, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {59, 400,  70, 250, 250,  280, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {60, 400,  80, 250, 250,  320, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {61, 400,  90, 250, 250,  360, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {62, 400, 100, 250, 250,  400, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {63, 400, 110, 250, 250,  440, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {64, 400, 120, 250, 250,  480, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {65, 400, 130, 250, 250,  520, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {66, 400, 140, 250, 250,  560, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {67, 400, 150, 250, 250,  600, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {68, 400, 160, 250, 250,  640, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {69, 400, 170, 250, 250,  680, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {70, 400, 180, 250, 250,  720, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {71, 400, 190, 250, 250,  760, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {72, 400, 200, 250, 250,  800, 0, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {73,   0,  10,  50,  50,  200, 1, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {74,   0,  20, 100, 100,  200, 1, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {75,   0,  30, 110, 110,  273, 1, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {76,   0,  30, 120, 120,  250, 1, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {77, 100,  15, 150, 150,  100, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {78,  13,  15, 150, 150,  100, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {79,  20,  20, 150, 150,  133, 1, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {80,   0,  14,  50,  50,  280, 1, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {81,   0,  14,  50,  50,  200, 1, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {82,   0,  10,  50,  50,  166, 1, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {83,   0,  20, 100, 100,  100, 1, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {84,   0,  30, 110, 110,   33, 1, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {85,   0,  30, 120, 120,   40, 1, 3,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {86, 100,  15, 150, 150,   90, 0, 2,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {87,  13,  15, 150, 150,   19, 0, 7,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {88,  20,  20, 150, 150,   33, 1, 1,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {89,   0,  15,  30,  30,  500, 1, 6,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {90,   0,  20,  50,  50,  400, 0, 5,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 },
    {91,  13,  65, 110, 110,  590, 0, 4,  1,  0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,TSK_SLEEP,0,0,0,0,0,0,0,0,0,0,0,0 }
};
#endif

/* initialisation of running tasks data array */
struct task_type *tasks[NUM_OF_TASKS];
static struct task_type task_array[NUM_OF_TASKS];     

/* private function prototypes */
static void send_report( const U32 timeval );
static void build_task_report_word( const U32 timeval,
                                    const U8 task_num,
                                    S8 *r_word ); 
static void reset_task_parameters( struct task_type* task );
static void reset_task_computed_parameters( void );

 
/******************************************************************************
*                              PUBLIC FUNCTIONS
******************************************************************************/


/******************************************************************************
*  Function    : TSK_init
*  Description : initialises task object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_init( void )
{
    U16 index;
    
    /* clear running task id */
    running_task_id = ZERO_CPV;

	/* dereference the task structure pointers */
	for( index=0; index<NUM_OF_TASKS; index++ )
	{
		tasks[index] = &task_array[index];
	}

    /* clear task error flag */
    task_error = FALSE;

} /* TSK_init() */


/******************************************************************************
*  Function    : TSK_close
*  Description : closes task object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_close( void )
{

} /* TSK_close() */


/******************************************************************************
*  Function    : TSK_soft_reset
*  Description : Soft reset of task object
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_soft_reset( void )
{
    reset_task_computed_parameters();

    /* clear running task id */
    running_task_id = ZERO_CPV;

} /* TSK_soft_reset() */


/******************************************************************************
*  Function    : TSK_add_task
*  Description : Imports task data into local structure
*  Parameters  : test_task_id  - test task index
*				 pointer to task or NULL
*  Returns     : None 
******************************************************************************/
void TSK_add_task( const U8 task_id, struct task_type* task )
{
    /* sanity check on args */
    if( task_id < MAX_TEST_TASKS )
    {
		 MESS_OUT_message_1( (S8*)"TSK_add_task(): loading test task ", 
							 task_id,
							 MESSAGE_OP_DEBUG );

		 /* purge the structure */
		 reset_task_parameters( task );	  

		 /* load data */ 
		 task->id        = stored_task_data[task_id].id;       
		 task->release   = stored_task_data[task_id].release;     
		 task->duration  = stored_task_data[task_id].duration; 
		 task->rel_dline = stored_task_data[task_id].rel_dline;
		 task->period    = stored_task_data[task_id].period;   
		 task->priority  = stored_task_data[task_id].priority; 
		 task->preempt   = stored_task_data[task_id].preempt;  
		 task->spare_1   = stored_task_data[task_id].spare_1;  
		 task->spare_2   = stored_task_data[task_id].spare_2;  
		 task->spare_3   = stored_task_data[task_id].spare_3;  
		 task->spare_4   = stored_task_data[task_id].spare_4;  
    }
    else
    {
        MESS_OUT_message_1( (S8*)"TSK_load_task(): bad task number ",
                             task_id, 
                             MESSAGE_OP_ERROR );
    }

} /* TSK_add_task() */


/******************************************************************************
*  Function    : TSK_add_task
*  Description : Imports task data into local structure
*  Parameters  : task_id       - working task index
*                test_task_id  - test task index
*  Returns     : pointer to task or NULL
******************************************************************************/
struct task_type *TSK_load_task( const U8 task_id, const U8 test_task_id )
{
	if( (task_id < NUM_OF_TASKS) and (test_task_id < MAX_TEST_TASKS) )
	{
#if(0)   
		tasks[task_id] = &test_tasks[test_task_id];  /* copy pointer */
#endif
		return( tasks[task_id] );             /* return pointer */
	}
	else
	{
		MESS_OUT_message_1( (S8*)"TSK_load_task(): bad task number ",
			test_task_id, 
			MESSAGE_OP_ERROR );

		return( tasks[0] );             /* return pointer */
	}

} /* TSK_add_task() */


/******************************************************************************
*  Function    : TSK_start_of_test
*  Description : At start of test, prints headers, also for re-starting
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_start_of_test( void )
{
    /* print results header */
    MESS_OUT_message( (S8*)"\n\n", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)"TIME   TASK-1  TASK-2  TASK-3  TASK-4  TASK-5  TASK-6  TASK-7  TICKS   Err", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)"=====  ======  ======  ======  ======  ======  ======  ======  =====   ===", MESSAGE_OP_RESULTS );

} /* TSK_start_of_test() */


/******************************************************************************
*  Function    : TSK_run_task_cycle
*  Description : executes a task, simulating work done by decrementing task
*                time_left member
*  Parameters  : timeval - system time, passes on to helper functions
*  Returns     : none
******************************************************************************/
void TSK_run_task_cycle( const U32 timeval )
{

    if( (running_task_id > ZERO_CPV) and (running_task_id < NUM_OF_TASKS) )
    {
        if( tasks[running_task_id]->time_left > ZERO_CPV )
        {
            tasks[running_task_id]->time_left --;  /* decrement running time */
            tasks[running_task_id]->net_value ++;  /* increment net value    */
            tasks[running_task_id]->time_taken ++; /* increment time taken   */
        }
    }
    else if( running_task_id >= NUM_OF_TASKS )
    {
        MESS_OUT_message_1( (S8*)"TSK_run_task_cycle() bad task number: ",
                            running_task_id, 
                            MESSAGE_OP_ERROR );
    }
    else
    {
        /* do nothing */
    }

    /* increment the time history */
    TST_incr_test_report_value( TST_DURATION );

    /* report task states to output */
    send_report( timeval );

} /* TSK_run_task_cycle() */


/******************************************************************************
*  Function    : TSK_end_of_test
*  Description : At middle of test loop, calls next test,
*                at end of test, calls reports and does any clearing up
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_end_of_test( void )
{
    /* report algorithm */
    MESS_OUT_message( (S8*)"\nAlgorithm:", MESSAGE_OP_RESULTS );
    SCH_what_algorithm( MESSAGE_OP_RESULTS );

    TSK_task_report();                   /* task report              */ 
    TSK_task_results();                  /* task results             */
    TST_test_report();                   /* request for test results */

    #ifndef DEV_SYSTEM
        /* report clock ticks per second*/
        MESS_OUT_message_1( (S8*)"Ticks per second : ", 
                            (U32)CLK_clocks_per_second(), 
                            MESSAGE_OP_RESULTS );
    #endif

} /* TSK_end_of_test() */


/******************************************************************************
*  Function    : TSK_task_report
*  Description : prints loaded task status
*                calculates and stores task set utility
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_task_report( void )
{
	U32 total_util;
	U8 index;
	algorithm_type algorithm;

	/* get the algorithm in use */
	algorithm = SCH_get_algorithm();
	MESS_OUT_message_1( (S8*)"Algorithm is ", (U32)algorithm, MESSAGE_OP_RESULTS );

	/* print results header */
	MESS_OUT_message( (S8*)"\nTask Parameters", MESSAGE_OP_RESULTS );
	MESS_OUT_message( (S8*)  "===============", MESSAGE_OP_RESULTS );
	MESS_OUT_message( (S8*)"            TASK-1  TASK-2  TASK-3  TASK-4  TASK-5  TASK-6  TASK-7", MESSAGE_OP_RESULTS );
	MESS_OUT_message( (S8*)"            ======  ======  ======  ======  ======  ======  ======", MESSAGE_OP_RESULTS );

	MESS_OUT_task_report( (S8*)"Priority    ",
		(U32)tasks[ONE_CPV  ]->priority,
		(U32)tasks[TWO_CPV  ]->priority,
		(U32)tasks[THREE_CPV]->priority,
		(U32)tasks[FOUR_CPV ]->priority,
		(U32)tasks[FIVE_CPV ]->priority,
		(U32)tasks[SIX_CPV  ]->priority,
		(U32)tasks[SEVEN_CPV]->priority );

	MESS_OUT_task_report( (S8*)"Release     ",
		(U32)tasks[ONE_CPV  ]->release,
		(U32)tasks[TWO_CPV  ]->release,
		(U32)tasks[THREE_CPV]->release,
		(U32)tasks[FOUR_CPV ]->release,
		(U32)tasks[FIVE_CPV ]->release,
		(U32)tasks[SIX_CPV  ]->release,
		(U32)tasks[SEVEN_CPV]->release );

	MESS_OUT_task_report( (S8*)"Duration    ",
		(U32)tasks[ONE_CPV  ]->duration,
		(U32)tasks[TWO_CPV  ]->duration,
		(U32)tasks[THREE_CPV]->duration,
		(U32)tasks[FOUR_CPV ]->duration,
		(U32)tasks[FIVE_CPV ]->duration,
		(U32)tasks[SIX_CPV  ]->duration,
		(U32)tasks[SEVEN_CPV]->duration );

	MESS_OUT_task_report( (S8*)"Deadline    ",
		(U32)tasks[ONE_CPV  ]->rel_dline,
		(U32)tasks[TWO_CPV  ]->rel_dline,
		(U32)tasks[THREE_CPV]->rel_dline,
		(U32)tasks[FOUR_CPV ]->rel_dline,
		(U32)tasks[FIVE_CPV ]->rel_dline,
		(U32)tasks[SIX_CPV  ]->rel_dline,
		(U32)tasks[SEVEN_CPV]->rel_dline );

	MESS_OUT_task_report( (S8*)"Period      ",
		(U32)tasks[ONE_CPV  ]->period,
		(U32)tasks[TWO_CPV  ]->period,
		(U32)tasks[THREE_CPV]->period,
		(U32)tasks[FOUR_CPV ]->period,
		(U32)tasks[FIVE_CPV ]->period,
		(U32)tasks[SIX_CPV  ]->period,
		(U32)tasks[SEVEN_CPV]->period );

	MESS_OUT_task_report( (S8*)"Pre-empt    ",
		(U32)tasks[ONE_CPV  ]->preempt,
		(U32)tasks[TWO_CPV  ]->preempt,
		(U32)tasks[THREE_CPV]->preempt,
		(U32)tasks[FOUR_CPV ]->preempt,
		(U32)tasks[FIVE_CPV ]->preempt,
		(U32)tasks[SIX_CPV  ]->preempt,
		(U32)tasks[SEVEN_CPV]->preempt );

	if(  (algorithm is ALG_RM_RTO) or (algorithm is ALG_EDF_RTO) 
		or (algorithm is ALG_EDF_BWP)   )
	{
		MESS_OUT_task_report( (S8*)"Skip Gap    ",
			(U32)SKIP_GAP(ONE_CPV  ),
			(U32)SKIP_GAP(TWO_CPV  ),
			(U32)SKIP_GAP(THREE_CPV),
			(U32)SKIP_GAP(FOUR_CPV ),
			(U32)SKIP_GAP(FIVE_CPV ),
			(U32)SKIP_GAP(SIX_CPV  ),
			(U32)SKIP_GAP(SEVEN_CPV) );
	}

	#if(0)
	if( 0 )
	{
		MESS_OUT_task_report( (S8*)"Elasticity  ",
			(U32)tasks[ONE_CPV  ]->elastic,
			(U32)tasks[TWO_CPV  ]->elastic,
			(U32)tasks[THREE_CPV]->elastic,
			(U32)tasks[FOUR_CPV ]->elastic,
			(U32)tasks[FIVE_CPV ]->elastic,
			(U32)tasks[SIX_CPV  ]->elastic,
			(U32)tasks[SEVEN_CPV]->elastic );
	}
	#endif

	if(  (algorithm is ALG_MUF) or (algorithm is ALG_MMUF) 
		or (algorithm is ALG_MMMUF)   )
	{
		MESS_OUT_task_report( (S8*)"MUF Crit    ",
			MUF_CRITICAL(ONE_CPV  ),
			MUF_CRITICAL(TWO_CPV  ),
			MUF_CRITICAL(THREE_CPV),
			MUF_CRITICAL(FOUR_CPV ),
			MUF_CRITICAL(FIVE_CPV ),
			MUF_CRITICAL(SIX_CPV  ),
			MUF_CRITICAL(SEVEN_CPV) );
	}

	if(    (algorithm is ALG_ADAP_01) or (algorithm is ALG_ADAP_02) 
		or (algorithm is ALG_ADAP_03) or (algorithm is ALG_ADAP_04)   
		or (algorithm is ALG_ADAP_05) or (algorithm is ALG_ADAP_06)   
		or (algorithm is ALG_ADAP_07)    )
	{
		MESS_OUT_task_report( (S8*)"Flex period ",
			(U32)PERIOD_FLEXIBLE(ONE_CPV  ),
			(U32)PERIOD_FLEXIBLE(TWO_CPV  ),
			(U32)PERIOD_FLEXIBLE(THREE_CPV),
			(U32)PERIOD_FLEXIBLE(FOUR_CPV ),
			(U32)PERIOD_FLEXIBLE(FIVE_CPV ),
			(U32)PERIOD_FLEXIBLE(SIX_CPV  ),
			(U32)PERIOD_FLEXIBLE(SEVEN_CPV) );
	}

	MESS_OUT_task_report( (S8*)"Util x 1000 ",
		(U32)tasks[ONE_CPV  ]->task_util,
		(U32)tasks[TWO_CPV  ]->task_util,
		(U32)tasks[THREE_CPV]->task_util,
		(U32)tasks[FOUR_CPV ]->task_util,
		(U32)tasks[FIVE_CPV ]->task_util,
		(U32)tasks[SIX_CPV  ]->task_util,
		(U32)tasks[SEVEN_CPV]->task_util );

	/* sum task utility values */
	total_util = ZERO_CPV;
	for( index=ONE_CPV; index<NUM_OF_TASKS; index++ )
	{
		total_util += tasks[index]->task_util;
	}

	/* report task set utility */
	MESS_OUT_message_1( (S8*)"\nTask set utility (x1000) = ", 
		total_util, 
		MESSAGE_OP_RESULTS );

} /* TSK_task_report() */


/******************************************************************************
*  Function    : TSK_task_results
*  Description : prints loaded task results
*  Parameters  : none
*  Returns     : none
******************************************************************************/
void TSK_task_results( void )
{
    /* print results header */
    MESS_OUT_message( (S8*)"\nTask Test Report", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)   "================", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)"                 TASK-1  TASK-2  TASK-3  TASK-4  TASK-5  TASK-6  TASK-7", MESSAGE_OP_RESULTS );
    MESS_OUT_message( (S8*)"                 ======  ======  ======  ======  ======  ======  ======", MESSAGE_OP_RESULTS );

    MESS_OUT_task_report(   (S8*)"Met Deadlines    ",
							(U32)tasks[ONE_CPV  ]->dlines_met,
							(U32)tasks[TWO_CPV  ]->dlines_met,
							(U32)tasks[THREE_CPV]->dlines_met,
							(U32)tasks[FOUR_CPV ]->dlines_met,
							(U32)tasks[FIVE_CPV ]->dlines_met,
							(U32)tasks[SIX_CPV  ]->dlines_met,
							(U32)tasks[SEVEN_CPV]->dlines_met );

    MESS_OUT_task_report(   (S8*)"Missed Deadlines ",
							(U32)tasks[ONE_CPV  ]->dlines_missed,
							(U32)tasks[TWO_CPV  ]->dlines_missed,
							(U32)tasks[THREE_CPV]->dlines_missed,
							(U32)tasks[FOUR_CPV ]->dlines_missed,
							(U32)tasks[FIVE_CPV ]->dlines_missed,
							(U32)tasks[SIX_CPV  ]->dlines_missed,
							(U32)tasks[SEVEN_CPV]->dlines_missed );

    MESS_OUT_task_report(   (S8*)"Periods Doubled  ",
							(U32)tasks[ONE_CPV  ]->per_doubles,
							(U32)tasks[TWO_CPV  ]->per_doubles,
							(U32)tasks[THREE_CPV]->per_doubles,
							(U32)tasks[FOUR_CPV ]->per_doubles,
							(U32)tasks[FIVE_CPV ]->per_doubles,
							(U32)tasks[SIX_CPV  ]->per_doubles,
							(U32)tasks[SEVEN_CPV]->per_doubles );

    MESS_OUT_task_report(   (S8*)"Pre-emptions     ",
							(U32)tasks[ONE_CPV  ]->pre_count,
							(U32)tasks[TWO_CPV  ]->pre_count,
							(U32)tasks[THREE_CPV]->pre_count,
							(U32)tasks[FOUR_CPV ]->pre_count,
							(U32)tasks[FIVE_CPV ]->pre_count,
							(U32)tasks[SIX_CPV  ]->pre_count,
							(U32)tasks[SEVEN_CPV]->pre_count );

    MESS_OUT_task_report(   (S8*)"Skips/Removals   ",
							(U32)TOTAL_SKIPS(ONE_CPV  ),
							(U32)TOTAL_SKIPS(TWO_CPV  ),
							(U32)TOTAL_SKIPS(THREE_CPV),
							(U32)TOTAL_SKIPS(FOUR_CPV ),
							(U32)TOTAL_SKIPS(FIVE_CPV ),
							(U32)TOTAL_SKIPS(SIX_CPV  ),
							(U32)TOTAL_SKIPS(SEVEN_CPV) );

    MESS_OUT_task_report(   (S8*)"Value            ",
							(U32)tasks[ONE_CPV  ]->value,
							(U32)tasks[TWO_CPV  ]->value,
							(U32)tasks[THREE_CPV]->value,
							(U32)tasks[FOUR_CPV ]->value,
							(U32)tasks[FIVE_CPV ]->value,
							(U32)tasks[SIX_CPV  ]->value,
							(U32)tasks[SEVEN_CPV]->value );

} /* TSK_task_results() */


/******************************************************************************
*  Function    : TSK_task_completed
*  Description : informs object that task completed. Updates
*                results counters.
*                increments deadlines as met or missed.
*                if a deadline is met, its value is its duration, else, if
*                it misses then its value is zero.
*  Parameters  : task number (1-7)
*                time
*  Returns     : none
******************************************************************************/
void TSK_task_completed( const U8 task, const U32 timeval )
{
    if( tasks[task]->abs_dline >= timeval )
    {
        /* completed by deadline: increment deadlines met counters */
        tasks[task]->dlines_met++;
        TST_incr_test_report_value( TST_DEADLN_MET );

        /* add value to report arrays */
        tasks[task]->value += tasks[task]->net_value;
        TST_add_test_report_value( TST_VALUE, tasks[task]->net_value );
    }
    else
    {
        /* task over-ran its deadline increment deadlines missed counters */
        tasks[task]->dlines_missed++;
        TST_incr_test_report_value( TST_DEADLN_MIS );

        MESS_OUT_message_1( (S8*)"Overrun for task ", 
                            task, MESSAGE_OP_VERBOSE );
    }

    /* check if duration overran */
    if( tasks[task]->time_taken > tasks[task]->c_duration )
    {
        /* duration overran - update calculated duration */
        /* report task duration update */
        MESS_OUT_message_3( (S8*)"Task ", task,
                            (S8*)" duration updated from ", tasks[task]->c_duration,
                            (S8*)" to ", tasks[task]->time_taken,
                            MESSAGE_OP_VERBOSE );
        tasks[task]->c_duration = tasks[task]->time_taken;

    }

    tasks[task]->time_taken = ZERO_CPV;  /* reset time_taken        */
    tasks[task]->net_value  = ZERO_CPV;  /* reset net value counter */

} /* TSK_task_completed() */





/******************************************************************************
*                            PRIVATE FUNCTIONS
******************************************************************************/





/******************************************************************************
*  Function    : send_report
*  Description : private helper that send a report to output object
*  Parameters  : timeval - system time
*  Returns     : none
******************************************************************************/
static void send_report( const U32 timeval )
{
    #define word_size  12

    S8 task_1[word_size];
    S8 task_2[word_size];
    S8 task_3[word_size];
    S8 task_4[word_size];
    S8 task_5[word_size];
    S8 task_6[word_size];
    S8 task_7[word_size];
    U32 sched_time;
    S8 overrun[word_size];

    /* build the task state words */
    build_task_report_word(timeval, ONE_CPV  , task_1 );
    build_task_report_word(timeval, TWO_CPV  , task_2 );
    build_task_report_word(timeval, THREE_CPV, task_3 );
    build_task_report_word(timeval, FOUR_CPV , task_4 );
    build_task_report_word(timeval, FIVE_CPV , task_5 );
    build_task_report_word(timeval, SIX_CPV  , task_6 );
    build_task_report_word(timeval, SEVEN_CPV, task_7 );

    /* get the timer value */
    sched_time = CLK_end_timer();

    /* report error */
    if( task_error )
    {
        UTL_string_copy( overrun, (S8*)"OVERRUN" );
        task_error = FALSE;
    }
    else
    {
        UTL_string_copy( overrun, (S8*)"" );
    }

    MESS_OUT_scheduler_report(  timeval,
								task_1,
								task_2,
								task_3,
								task_4,
								task_5,
								task_6,
								task_7,
								(U32)sched_time,
								overrun );

} /* send_report() */


/******************************************************************************
*  Function    : build_task_report_word
*  Description : private helper that builds a word for send_report(). Stores
*                result in r_word
*  Parameters  : timeval  - system time
*                task_num - number of task
*                r_word   - report word
*  Returns     : none
******************************************************************************/
static void build_task_report_word( const U32 timeval,
                                    const U8 task_num,
                                    S8 *r_word )
{
    switch (tasks[task_num]->state)
    {
        case TSK_RUN:
            /* check for deadline */
            if( tasks[task_num]->abs_dline is timeval )
            {
               UTL_string_copy( r_word, (S8*)"<-DDDD->" );
            }
            else if( tasks[task_num]->time_left > (tasks[task_num]->abs_dline - timeval) )
            {
               /* running late */
               UTL_string_copy( r_word, (S8*)"  LATE  " );
            }
            else if( tasks[task_num]->abs_dline < timeval )
            {
               /* deadline overrun */
               UTL_string_copy( r_word, (S8*)"  OVER  " );

               /* set task error flag */
               task_error = TRUE;
            }
            else
            {
               /* task running */
               UTL_string_copy( r_word, (S8*)"  ++++  " );
            }
            break;

        case TSK_READY:
            /* check for deadline */
            if( tasks[task_num]->abs_dline is timeval )
            {
               UTL_string_copy( r_word, (S8*)"<-DDDD->" );
            }
            else if( tasks[task_num]->time_left > (tasks[task_num]->abs_dline - timeval) )
            {
               /* late */
               UTL_string_copy( r_word, (S8*)"  RL..  " );
            }
            else if( tasks[task_num]->abs_dline < timeval )
            {
               /* deadline exceeded */
               UTL_string_copy( r_word, (S8*)"  RO..  " );

               /* set task error flag */
               task_error = TRUE;
            }
            else
            {
               UTL_string_copy( r_word, (S8*)"  r...  " );
            }
            break;


        case TSK_PREMPT:
            /* check for deadline */
            if( tasks[task_num]->abs_dline is timeval )
            {
               UTL_string_copy( r_word, (S8*)"<-DDDD->" );
            }
            else if( tasks[task_num]->time_left > (tasks[task_num]->abs_dline - timeval) )
            {
               /* late */
               UTL_string_copy( r_word, (S8*)"  PL..  " );
            }
            else if( tasks[task_num]->abs_dline < timeval )
            {
               /* deadline exceeded */
               UTL_string_copy( r_word, (S8*)"  PO..  " );

               /* set task error flag */
               task_error = TRUE;
            }
            else
            {
               /* pre-empted */
               UTL_string_copy( r_word, (S8*)"  p...  " );
            }
            break;


        case TSK_WAITING:
               UTL_string_copy( r_word, (S8*)"  wait  " );
            break;

        case TSK_IDLE:
            /* check for deadline */
            if( tasks[task_num]->abs_dline is timeval )
            {
               UTL_string_copy( r_word, (S8*)"<-DDDD->" );
            }
            else
            {
               UTL_string_copy( r_word, (S8*)"  i...  " );
            }
            break;

        case TSK_SKIPPED:
            /* check for deadline */
            if( tasks[task_num]->abs_dline is timeval )
            {
               UTL_string_copy( r_word, (S8*)"<-DDDD->" );
            }
            else
            {
               UTL_string_copy( r_word, (S8*)"  sk..  " );
            }
            break;

        case TSK_REMOVED:
               UTL_string_copy( r_word, (S8*)"  rm..  " );
            break;

        case TSK_SLEEP:
        default:
            UTL_string_copy( r_word, (S8*)"  s...  " );
            break;
    }

} /* build_task_report_word() */


/******************************************************************************
*  Function    : reset_task_parameters
*  Description : private helper - initialises task struct data
*  Parameters  : pointer to task data
*  Returns     : none
******************************************************************************/
static void reset_task_parameters( struct task_type* task )
{
	task->id                = ZERO_CPV;             
	task->release           = ZERO_CPV;        
	task->duration          = ZERO_CPV;       
	task->rel_dline         = ZERO_CPV;      
	task->period            = ZERO_CPV;         
	task->task_util         = ZERO_CPV;      
	task->priority          = ZERO_CPV;       
	task->preempt           = ZERO_CPV;        
	task->spare_1           = ZERO_CPV;        
	task->spare_2           = ZERO_CPV;        
	task->spare_3           = ZERO_CPV;        
	task->spare_4           = ZERO_CPV;
	task->time_left         = ZERO_CPV;
	task->time_taken        = ZERO_CPV;
	task->abs_dline         = ZERO_CPV;
	task->laxity            = ZERO_CPV;
	task->c_duration        = ZERO_CPV;
	task->task_util         = ZERO_CPV;
	task->pre_count         = ZERO_CPV;
	task->period_multiplyer = ZERO_CPV;
	task->state             = TSK_SLEEP;
	task->q0_prev           = ZERO_CPV;					
	task->q2_prev           = ZERO_CPV;

	task->q0_next           = ZERO_CPV;
	task->q1_prev           = ZERO_CPV;
	task->q1_next           = ZERO_CPV;			 	
	task->q2_next           = ZERO_CPV;
	task->per_doubles       = ZERO_CPV;
	task->dlines_missed     = ZERO_CPV;
	task->dlines_met        = ZERO_CPV;
	task->net_value         = ZERO_CPV;
	task->value             = ZERO_CPV;

} /* reset_task_parameters() */

/******************************************************************************
*  Function    : reset_task_computed_parameters
*  Description : private helper - initialises computed generated parameters
*                from all tasks in array
*  Parameters  : none
*  Returns     : none
******************************************************************************/
static void reset_task_computed_parameters( void )
{
#if(0)
    U8 index;

    for( index=ONE_CPV; index<MAX_TEST_TASKS; index++ )
    {
        /* clear the calculated area of the task */
        test_tasks[index].time_left         = ZERO_CPV;
        test_tasks[index].time_taken        = ZERO_CPV;
        test_tasks[index].abs_dline         = ZERO_CPV;
        test_tasks[index].laxity            = ZERO_CPV;
        test_tasks[index].c_duration        = ZERO_CPV;
        test_tasks[index].task_util         = ZERO_CPV;
        test_tasks[index].pre_count         = ZERO_CPV;
        test_tasks[index].period_multiplyer = ZERO_CPV;
        test_tasks[index].state             = TSK_SLEEP;
        test_tasks[index].q0_prev           = ZERO_CPV;
        test_tasks[index].q0_next           = ZERO_CPV;
        test_tasks[index].q1_prev           = ZERO_CPV;
        test_tasks[index].q1_next           = ZERO_CPV;
        test_tasks[index].q2_prev           = ZERO_CPV;
        test_tasks[index].q2_next           = ZERO_CPV;
        test_tasks[index].total_skips       = ZERO_CPV;
        test_tasks[index].per_doubles       = ZERO_CPV;
        test_tasks[index].dlines_missed     = ZERO_CPV;
        test_tasks[index].dlines_met        = ZERO_CPV;
        test_tasks[index].net_value         = ZERO_CPV;
        test_tasks[index].value             = ZERO_CPV;
    }
#endif
} /* reset_task_computed_parameters() */
