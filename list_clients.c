/***************************************************************************
*
*                        Project : ServerClients
*
*
****************************************************************************
*
* File Name   :  list_headers.c
*
* Description :  Container that builds and manages a linked list of header
*                files
*
* Modification record:
*     |     |
* 000a| JRO | 16-01-03: Initial build
*
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "co_typs.h"
#include "co_defs.h"
#include "diagnostics.h"
#include "sources.h"
#include "clients.h"
#include "functions.h"
#include "list_clients.h"



/* function prototypes */
static void search_for_clients( const sint8* source_dir );
static void search_through_clients( const sint8* source_dir,
                                    const sint8* function_file_name,
                                    const sint8* function_name );
static void parse_client_file( const sint8* source_dir,
                               const sint8* client_file_name,
                               const sint8* function_file_name,
                               const sint8* function_name );
static boolean parse_line( const sint8 *line, const sint8 *function_name );
static boolean line_valid( const sint8 *line );
static boolean is_token_a_comment( const sint8 *token );
static boolean compare_file_names( const sint8 *name_1, const sint8 *name_2 );
static boolean file_type_is_valid( const sint8 *name, const int name_length );
static void remove_file_extent( sint8 *name, const int name_length );




/*****************************************************************************
*   PUBLIC FUNCTIONS                                                         *
******************************************************************************/




/*****************************************************************************/
boolean LCLNS_build_client_list( const sint8* source_dir )
{
    /* using data stored in functions container */
    boolean success = TRUE; 

    search_for_clients( source_dir );

    return( success );

} /* LCLNS_build_client_list */





/*************************
*   PRIVATE FUNCTIONS
**************************/


/*****************************************************************************/
static void search_for_clients( const sint8* source_dir )
{
    /* search for each server function in all source files */

    sint8 function_file_name[BUFFSIZE];
    sint8 function_name[BUFFSIZE];

    /* get a server function name */
    if( FNS_get_function( function_file_name, function_name ) == TRUE )
    {
        /* search through all of the client files for occurences */
        search_through_clients( source_dir, function_file_name, function_name );

        while( FNS_get_next_function( function_file_name, function_name ) == TRUE )
        {
            /* search through all of the client files for occurences */
            search_through_clients( source_dir, function_file_name, function_name );
        }
    }

} /* search_for_clients() */


/*****************************************************************************/
static void search_through_clients( const sint8* source_dir,
                                    const sint8* function_file_name,
                                    const sint8* function_name )
{
    /* count occurences of function_file_name in all source files */

    sint8  message[BUFFSIZE];
    sint8 client_file_name[BUFFSIZE];

    if( SRS_get_source( client_file_name ) == TRUE )
    {
        strcpy( message, "Opened ");
        strcat( message, function_file_name );
        strcat( message, " file for parsing");
        debug( message );

        parse_client_file( source_dir,
                           client_file_name,
                           function_file_name,
                           function_name );

        while( SRS_get_next_source( client_file_name ) == TRUE )
        {
            parse_client_file( source_dir,
                               client_file_name,
                               function_file_name,
                               function_name );
        }
    }

} /* search_through_clients() */



/*****************************************************************************/
static void parse_client_file( const sint8* source_dir,
                               const sint8* client_file_name,
                               const sint8* function_file_name,
                               const sint8* function_name )
{
    /* opens a client file for searching */

    FILE *file_ptr;
    sint8  full_client_name[BUFFSIZE];
    sint8  message[BUFFSIZE];
    sint8  client_function[BUFFSIZE];
    sint8  number_of_uses[BUFFSIZE];
    sint8  buffer[BUFFSIZE];
    sint8  count;


    if( compare_file_names(client_file_name, function_file_name) == FALSE )
    {


        strcpy (client_function, "" ); /* this is not yet used */

        strcpy( full_client_name, source_dir );
        strcat( full_client_name, "\\" );
        strcat( full_client_name, client_file_name );

        /* open the client source file for reading */
        if( (file_ptr = fopen( full_client_name, "r" )) != NULL )
        {
            (void)fflush( file_ptr );  /* flush out the stream */

            /* step through each line of the source file */
            count = 0;
            while( fgets( buffer, (BUFFSIZE-1), file_ptr )  )
            {
                if( parse_line( buffer, function_name ) == TRUE )
                {
                    /* function found, increment the counter */
                    count++;
                }

            }

            if( count > 0 )
            {
                /* store positive result */

                sprintf( number_of_uses, "%d", count );

                (void)CLNS_add_client( source_dir,
                                       function_file_name,
                                       function_name,
                                       client_file_name,
                                       client_function,
                                       number_of_uses );

                count = 0; /* rest the uses counter */
            }

            fclose( file_ptr ); /* close the source file */
        }
        else
        {
            strcpy( message, "ERROR: Failed to open ");
            strcat( message, full_client_name );
            strcat( message, " file");
            debug( message );
        }
    }

} /* parse_client_file() */



/*****************************************************************************/
static boolean parse_line( const sint8 *line, const sint8 *function_name )
{
    /* parses a single line, returning true if function name is found */

    boolean result;
    uint16 position = 0;

    if( line_valid(line) == TRUE ) /* sanity check that we have a line */
    {
        /* strip leading white spaces */
        while( line[position] == ' ' )
        {
            position++;
        }

        /* find function name in line */
        if( strstr( line, function_name ) != NULL )
        {
            result = TRUE;
        }
        else
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }

    return( result );

} /* parse_line() */



/*****************************************************************************/
static boolean line_valid( const sint8 *line )
{
    /* verify that line is worth parsing for a function name */

    sint8 temp_line[BUFFSIZE];
    sint8 delimit[] = " ";
    sint8 *token;
    boolean valid;

    strcpy( temp_line, line );  /* local copy of line */

    if( strlen(line) > 1 )
    {
         /* extract the first string token from the line */
         if( (token = strtok(temp_line, delimit) ) != NULL )
         {
             if( is_token_a_comment(token) == TRUE )
             {
                valid = FALSE;
             }
         }

        valid = TRUE;
    }
    else
    {
        valid = FALSE;
    }

    return( valid );

} /* line_valid() */


/*****************************************************************************/
static boolean is_token_a_comment( const sint8 *token )
{
    /* return true if a comment */

    boolean result;

    if( strlen(token) > 3 )
    {
        /* check for c++ & c comments */
        if( (token[0] == '/') && (token[1] == '/') )
        {
            result = TRUE;
        }
        else if( (token[0] == '/') && (token[1] == '*') )
        {
            result = TRUE;
        }
        else
        {
            result = TRUE; /* to keep lint happy */
            /* ignore */
        }
    }
    else
    {
        result = FALSE;
    }

    return( result );

} /* is_token_a_comment() */


/*****************************************************************************/
static boolean compare_file_names( const sint8 *name_1, const sint8 *name_2 )
{
    /* returns true is file names are same, ignoring the file type */
    boolean result;
    uint8 name_1_length;
    uint8 name_2_length;
    sint8 loc_name_1[BUFFSIZE];
    sint8 loc_name_2[BUFFSIZE];

    strcpy( loc_name_1, name_1 );
    strcpy( loc_name_2, name_2 );

    /* check they are the same length */
    name_1_length = (uint8)strlen( loc_name_1 );
    name_2_length = (uint8)strlen( loc_name_2 );

    if( name_1_length == name_2_length )
    {
        /* check they are header or source files */
        if(   (file_type_is_valid(loc_name_1, name_1_length) == TRUE)
            &&(file_type_is_valid(loc_name_2, name_2_length) == TRUE)  )
        {
            /* remove the extents */
            remove_file_extent( loc_name_1, name_1_length );
            remove_file_extent( loc_name_2, name_2_length );

            if( strcmp(loc_name_1,loc_name_2) == 0 )
            {
                result = TRUE;
            }
            else
            {
                result = FALSE;
            }
        }
        else
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }

    return( result );

} /* compare_file_names() */


/*****************************************************************************/
static boolean file_type_is_valid( const sint8 *name, const int name_length )
{
    /* returns true if file name is source or header */
    boolean result;
    sint8   dot;
    sint8   extent;

    dot    = name[name_length - 2];
    extent = name[name_length - 1];


    if( (dot == '.')
        && (   (extent == 'c')||(extent == 'C')
             ||(extent == 'h')||(extent == 'H') ))
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return( result );

} /* file_type_is_valid() */


/*****************************************************************************/
static void remove_file_extent( sint8 *name, int name_length )
{
    /* removes the file extent */
    name[name_length-2] = '\0';

} /* remove_file_extent() */




/* end */
