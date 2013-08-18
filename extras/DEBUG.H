//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment

This file is NOT part of Duke Nukem 3D version 1.5 - Atomic Edition
However, it is either an older version of a file that is, or is
some test code written during the development of Duke Nukem 3D.
This file is provided purely for educational interest.

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#ifndef _debug_public
#define _debug_public
#ifdef __cplusplus
extern "C" {
#endif

// DEBUG
//
// DEBUG works in two different modes, level and masked.
//
// To set the current DEBUG mode call DBG_SetDebugMode and to get the current
// mode call DBG_GetDebugMode.
//
// In level mode all DEBUG messages with a debug level equal to or less than
// the current debug level (obtained by DBG_GetDebugLevel) are outputted,
// all others are ignored.  You can set the debug level with DBG_SetDebugLevel
//
// In masked mode DEBUG messages are filtered according to a debug mask.
// this debug mask is initialized when DEBUG is started to filter out all
// DEBUG messages.  To enable a certain DEBUG level call
// DBG_EnableDebugLevel( int32 level ) and to disable a certain level call
// DBG_DisableDebugLevel( int32 level ). The entire filter can be cleared with
// DBG_DisableAllDebugLevels( int32 level ).
//
// The output of DEBUG messages are controlled by the function
// DBG_SetDebugOutputMode, the current output mode can be obtained by
// DBG_GetDebugOutputMode.
//
// The output functions supported by DEBUG natively is through printf and
// to a file.  Two other output modes have been left unimplemented,
// dbgout_user1 and dbgout_user2. Output functions can be registered for
// these modes using the function DBG_RegisterOutputFunction which takes
// the mode you want to customize as well as three function pointers:
//
// startup - a void function which starts up the output method
// print - a function which takes a char * which actually does the printing
// shutdown - a void function which shutsdown the output method
//


#define DEBUG 1

typedef enum
   {
   debug_normal = 5,
   debug_max = 20
   } debuglevel_t;

typedef enum
   {
   dbgmode_level,
   dbgmode_masked
   } debugmode_t;

typedef enum
   {
   dbgout_screen,
   dbgout_file,
   dbgout_user1,
   dbgout_user2,
   dbgout_endtype
   } debugoutput_t;

void DBG_DebugString( int32 level, char *msg, ... );
void DBG_DbgString( char *msg, ... );

#if ( DEBUG == 1 )
   #define Debug  DBG_DebugString
   #define Dbg    DBG_DbgString
#else
   #define Debug  if (1) {} else DBG_DebugString
   #define Dbg  if (1) {} else DBG_DbgString
#endif

void DBG_SetDebugLevel( int32 level );
int32 DBG_GetDebugLevel( void );
void DBG_EnableDebugLevel( int32 level );
void DBG_DisableDebugLevel( int32 level );
void DBG_DisableAllDebugLevels( void );
void DBG_SetDebugMode( int32 mode );
int32 DBG_GetDebugMode( void );
void DBG_SetDebugOutputMode( int32 mode );
int32 DBG_GetDebugOutputMode( void );
void DBG_Shutdown( void );
void DBG_Startup( int32 mode, int32 level, int32 output );
void DBG_RegisterOutputFunction
   (
   int32 outmode,
   void (*startup) (void),
   void (*print) (char *),
   void (*shutdown) (void)
   );

#ifdef __cplusplus
};
#endif
#endif
