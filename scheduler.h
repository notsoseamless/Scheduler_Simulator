/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :  SCHEDULER SIMULATOR			                              *                                                                             *
* File Name   :  scheduler.h                                                  *
* Description :  scheduler interface file                                     *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* Date     |Aut| Description                                                  *
* 12-03-05 |JRO| Created                                                      *
* 09-04-05 |JRO| Added LLF scheduler                                          *
* 10-04-05 |JRO| Added MLLF scheduler                                         *
* 15-04-05 |JRO| Added MUF scheduler                                          *
* 16-04-05 |JRO| Added RTO skip scheduler                                     *
* 17-04-05 |JRO| Added screen control for development system                  *
* 20-04-05 |JRO| Added RR & CYC scheduler                                     *
* 22-05-05 |JRO| Added MMUF scheduler                                         *
* 25-06-05 |JRO| Added ALG_ADAP_01 & ALG_ADAP_02 scheduler                    *
* 26-06-05 |JRO| Added ALG_ADAP_03 & ALG_ADAP_04 scheduler                    *
* 15-07-05 |JRO| Added ALG_ADAP_05 & ALG_ADAP_06 scheduler                    *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "messages_out.h"

/* scheduler algorithms */
typedef enum
{
	ALG_CYC,     /* cyclic executive                        */
	ALG_RR,      /* round robin                             */
	ALG_RM,      /* rate monotonic                          */
	ALG_IRM,     /* intelligent rate monotonic              */
	ALG_DRM,     /* delayed rate monotonic                  */
	ALG_EDF,     /* earliest deadline first                 */
	ALG_SPT,     /* shortest processing time                */
	ALG_LLF,     /* least laxity first                      */
	ALG_MLLF,    /* modified leat laxity first              */
	ALG_MUF,     /* maximum urgency first                   */
	ALG_MMUF,    /* modified maximum urgency first          */
	ALG_MMMUF,   /* modified modified maximum urgency first */
	ALG_D_STAR,  /* D* algorithm                            */
	ALG_DD_STAR, /* DD* algorithm                           */
	ALG_D_OVER,  /* Dover algorithm                         */
	ALG_RM_RTO,  /* RM red task only                        */
	ALG_EDF_RTO, /* EDF red task only                       */
	ALG_EDF_BWP, /* EDF blue when possible                  */
	ALG_ADAP_01, /* Adaptive algorithm Number One           */
	ALG_ADAP_02, /* Adaptive algorithm Number Two           */
	ALG_ADAP_03, /* Adaptive algorithm Number Three         */
	ALG_ADAP_04, /* Adaptive algorithm Number Four          */
	ALG_ADAP_05, /* Adaptive algorithm Number Five          */
	ALG_ADAP_06, /* Adaptive algorithm Number Six           */
	ALG_ADAP_07, /* Adaptive algorithm Number Seven         */
	ALG_MAX_ALG
} algorithm_type;

void SCH_init( void );
void SCH_soft_reset( void );
void SCH_start( const U32 timeval );
void SCH_add_task( const U8 task_id, const U8 test_task_id );
void SCH_set_algorithm( const algorithm_type alg );
void SCH_report_algorithm( const algorithm_type a, const message_class class );
void SCH_what_algorithm( const message_class class );
algorithm_type SCH_get_algorithm( void );
void SCH_reset_queues( void );

#endif /* _SCHEDULER_H_ */
