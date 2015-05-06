/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
* Project     :   SCHEDULER SIMULATOR
*                                                                             *
* File Name   :  messages_out.h                                               *
*                                                                             *
* Description :  messages_out.c interface file                                *
*                                                                             *
*******************************************************************************
* Modification record:                                                        *
*                                                                             *
* date     |Aut| Description                                                  *
* 18-11-05 |JRO| Created - porting from scheduler project                     *
*          |   |                                                              *
*                                                                             *
******************************************************************************/
#ifndef _MESSAGES_OUT_H_
#define _MESSAGES_OUT_H_


/* message classes */
typedef enum
{
	MESSAGE_OP_INFO,    	/* general info message */
	MESSAGE_OP_INFO_NOCR,	/* general info message (with no CR) */
	MESSAGE_OP_LOG,	    	/* general log message  */
	MESSAGE_OP_ERROR,   	/* error message        */
	MESSAGE_OP_DEBUG,		/* debug message        */
	MESSAGE_OP_RESULTS,		/* results message      */
	MESSAGE_OP_VERBOSE,		/* verbose message      */
	MESSAGE_OP_MAX
}message_class;	


/* public function prototypes */
void MESS_OUT_initialise( void );
void MESS_OUT_close( void );
void MESS_OUT_message(   const S8* const message, const message_class class );
void MESS_OUT_message_S2( const S8* const message1, const S8* const message2, 
                         const message_class class );
void MESS_OUT_message_1( const S8* const message, const U32 num, 
                         const message_class class );
void MESS_OUT_message_2( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
                         const message_class class );
void MESS_OUT_message_3( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
                         const message_class class );
void MESS_OUT_message_4( const S8* const message_1, const U32 num_1, 
						 const S8* const message_2, const U32 num_2,
						 const S8* const message_3, const U32 num_3,
						 const S8* const message_4, const U32 num_4,
                         const message_class class );
void MESS_OUT_task_report(  const S8* const param,
							const U32 task_1,
							const U32 task_2,
							const U32 task_3,
							const U32 task_4,
							const U32 task_5,
							const U32 task_6,
							const U32 task_7 );
void MESS_OUT_scheduler_report( const U32 time,
								const S8* const task_1,
								const S8* const task_2,
								const S8* const task_3,
								const S8* const task_4,
								const S8* const task_5,
								const S8* const task_6,
								const S8* const task_7,
								const U32 timestamp,
								const S8* const overrun );
void MESS_OUT_toggle_debug_mode( void );		
void MESS_OUT_toggle_verbose_mode( void );		
void MESS_OUT_toggle_screen_results_mode( void );
void MESS_OUT_report_status( const message_class class );
void MESS_OUT_report_duration( void );
void MESS_OUT_rename_results_file( const S8* const file_name );
#endif /* _MESSAGES_OUT_H_ */
