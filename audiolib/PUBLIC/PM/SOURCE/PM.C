/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
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

*/
/**********************************************************************
   module: PM.C

   author: James R. Dose
   date:   January 16, 1994

   Simple music player.

   (c) Copyright 1994 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "music.h"


/*---------------------------------------------------------------------
   Function prototypes
---------------------------------------------------------------------*/

void  LoadTimbres( char *timbrefile );
char *LoadMidi( char *filename );
char *GetUserText( const char *parameter );
int   CheckUserParm( const char *parameter );
void  DefaultExtension( char *path, char *extension );
void  TurnOffTextCursor( void );
void  TurnOnTextCursor( void );

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

#define NUMCARDS 10

char *SoundCardNames[] =
   {
   "General Midi", "Sound Canvas", "Awe 32", "WaveBlaster",
   "SoundBlaster", "Pro AudioSpectrum", "Sound Man 16", "Adlib",
   "Ensoniq SoundScape", "Gravis UltraSound"
   };

int SoundCardNums[] =
   {
   GenMidi, SoundCanvas, Awe32, WaveBlaster,
   SoundBlaster, ProAudioSpectrum, SoundMan16,
   Adlib, SoundScape, UltraSound
   };

/*---------------------------------------------------------------------
   Function: main

   Sets up sound cards, calls the demo, and then cleans up.
---------------------------------------------------------------------*/

void main
   (
   int argc,
   char *argv[]
   )

   {
   int card;
   int address;
   int status;
   char *SongPtr = NULL;
   char *ptr;
   char  filename[ 128 ];
   char  timbrefile[ 128 ];
   int   gotopos = 0;
   int   measure = 0;
   int   beat = 0;
   int   tick = 0;
   int   time = 0;

   printf( "\nPM   EMIDI Music Player   Version 1.21  Copyright (c) 1996 by Jim Dose\n" );

   if ( ( CheckUserParm( "?" ) ) || ( argc < 2 ) )
      {
      int index;

      printf(
         "Usage: PM [ midifile ] CARD=[ card number ] MPU=[ port address in hex ]\n"
         "          TIMBRE=[ timbre file ] TIME=[minutes:seconds:milliseconds]\n"
         "          POSITION=[measure:beat:tick]\n\n"
         "   card number = \n" );
      for( index = 0; index < NUMCARDS; index++ )
         {
         printf( "      %d : %s\n", index, SoundCardNames[ index ] );
         }

      printf( "\n" );
      exit( 0 );
      }

   // Default is GenMidi
   card = 0;
   address = 0x330;

   ptr = getenv( "PM" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d,%x", &card, &address );
      }

   ptr = GetUserText( "MPU" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%x", &address );
      }

   ptr = GetUserText( "TIMBRE" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%s", timbrefile );
      LoadTimbres( timbrefile );
      }

   ptr = GetUserText( "POSITION" );
   if ( ptr != NULL )
      {
      gotopos = 1;
      sscanf( ptr, "%d:%d:%d", &measure, &beat, &tick );
      }

   ptr = GetUserText( "TIME" );
   if ( ptr != NULL )
      {
      int minutes = 0;
      int seconds = 0;
      int milli   = 0;

      gotopos = 2;
      sscanf( ptr, "%d:%d:%d", &minutes, &seconds, &milli );
      time = minutes * ( 60 * 1000 ) + seconds * 1000 + milli;
      }

   ptr = GetUserText( "CARD" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &card );
      }

   if ( ( card < 0 ) || ( card >= NUMCARDS ) )
      {
      printf( "Value out of range for CARD: %d\n", card );
      }

   status = MUSIC_Init( SoundCardNums[ card ], address );
   if ( status != MUSIC_Ok )
      {
      printf( "Error - %s\n", MUSIC_ErrorString( status ) );
      exit( 1 );
      }

   strcpy( filename, argv[ 1 ] );
   DefaultExtension( filename, ".mid" );

   SongPtr = LoadMidi( filename );

   MUSIC_SetVolume( 255 );

   status = MUSIC_PlaySong( SongPtr, MUSIC_LoopSong );
   if ( status != MUSIC_Ok )
      {
      printf( "Error - %s\n", MUSIC_ErrorString( status ) );
      }
   else
      {
      char ch;
      songposition pos;

      if ( gotopos == 1 )
         {
         MUSIC_SetSongPosition( measure, beat, tick );
         }
      else if ( gotopos == 2 )
         {
         MUSIC_SetSongTime( time );
         }

      printf( "Playing file '%s'.\n\n", filename );
      printf( "Press F to advance one measure.\n"
              "Press R to rewind one measure.\n"
              "Press G to go to the selected position.\n"
              "Press ESCape to end.\n\n" );

      MUSIC_GetSongLength( &pos );
      printf( "Song length: time (m:s:ms) = %d:%d:%d, "
         "(measure:beat:tick) = %d:%d:%d\n\n",
         pos.milliseconds / (60*1000),
         ( pos.milliseconds / 1000 ) % 60,
         pos.milliseconds % 1000,
         pos.measure, pos.beat, pos.tick );
      ch = 0;

      TurnOffTextCursor();
      while( ch != 27 )
         {
         MUSIC_GetSongPosition( &pos );
         printf( "time (m:s:ms) = %d:%d:%d     \t(measure:beat:tick) = %d:%d:%d          \r",
            pos.milliseconds / (60*1000),
            ( pos.milliseconds / 1000 ) % 60,
            pos.milliseconds % 1000,
            pos.measure, pos.beat, pos.tick );
         fflush( stdout );

         if ( kbhit() )
            {
            ch = getch();

            if ( ( ch == 'G' ) || ( ch == 'g' ) )
               {
               if ( gotopos == 1 )
                  {
                  MUSIC_SetSongPosition( measure, beat, tick );
                  }
               else
                  {
                  MUSIC_SetSongTime( time );
                  }

               ch = 0;
               while( kbhit() )
                  {
                  getch();
                  }
               }
            if ( ( ch == 'F' ) || ( ch == 'f' ) )
               {
               MUSIC_SetSongPosition( pos.measure + 1, 1, 0 );
               ch = 0;
               while( kbhit() )
                  {
                  getch();
                  }
               }
            if ( ( ch == 'R' ) || ( ch == 'r' ) )
               {
               MUSIC_SetSongPosition( pos.measure - 1, 1, 0 );
               ch = 0;
               while( kbhit() )
                  {
                  getch();
                  }
               }
            }
         }

      TurnOnTextCursor();
      MUSIC_StopSong();
      }

   free( SongPtr );
   MUSIC_Shutdown();
   printf( "\n" );
   }


/*---------------------------------------------------------------------
   Function: LoadTimbres

   Loads the instruments from disk
---------------------------------------------------------------------*/

void LoadTimbres
   (
   char *timbrefile
   )

   {
   FILE   *in;
   long   size;
   char   *TimbrePtr;

   if ( ( in = fopen( timbrefile, "rb" ) ) == NULL )
      {
      printf( "Cannot open '%s' for read.\n", timbrefile );
      exit( 1 );
      }

   fseek( in, 0, SEEK_END );
   size = ftell( in );
   fseek( in, 0, SEEK_SET );

   TimbrePtr = ( char * )malloc( size );
   if ( TimbrePtr == NULL )
      {
      printf( "Out of memory while reading '%s'.\n", timbrefile );
      exit( 1 );
      }

   if ( fread( TimbrePtr, size, 1, in ) != 1 )
      {
      printf( "Unexpected end of file while reading '%s'.\n", timbrefile );
      exit(1);
      }

   fclose( in );

   MUSIC_RegisterTimbreBank( TimbrePtr );
   }



/*---------------------------------------------------------------------
   Function: LoadMidi

   Loads the midi file from disk.
---------------------------------------------------------------------*/

char *LoadMidi
   (
   char *filename
   )

   {
   FILE   *in;
   long   size;
   char   *MidiPtr;

   if ( ( in = fopen( filename, "rb" ) ) == NULL )
      {
      printf( "Cannot open '%s' for read.\n", filename );
      exit( 1 );
      }

   fseek( in, 0, SEEK_END );
   size = ftell( in );
   fseek( in, 0, SEEK_SET );

   MidiPtr = ( char * )malloc( size );
   if ( MidiPtr == NULL )
      {
      printf( "Out of memory while reading '%s'.\n", filename );
      exit( 1 );
      }

   if ( fread( MidiPtr, size, 1, in ) != 1 )
      {
      printf( "Unexpected end of file while reading '%s'.\n", filename );
      exit(1);
      }

   fclose( in );

   return( MidiPtr );
   }


/*---------------------------------------------------------------------
   Function: GetUserText

   Checks if the specified string is present in the command line
   and returns a pointer to the text following it.
---------------------------------------------------------------------*/

char *GetUserText
   (
   const char *parameter
   )

   {
   int i;
   int length;
   char *text;
   char *ptr;

   extern int   _argc;
   extern char **_argv;

   text = NULL;
   length = strlen( parameter );
   i = 1;
   while( i < _argc )
      {
      ptr = _argv[ i ];

      if ( ( strnicmp( parameter, ptr, length ) == 0 ) &&
         ( *( ptr + length ) == '=' ) )
         {
         text = ptr + length + 1;
         break;
         }

      i++;
      }

   return( text );
   }


/*---------------------------------------------------------------------
   Function: CheckUserParm

   Checks if the specified string is present in the command line.
---------------------------------------------------------------------*/

int CheckUserParm
   (
   const char *parameter
   )

   {
   int i;
   int found;
   char *ptr;

   extern int   _argc;
   extern char **_argv;

   found = FALSE;
   i = 1;
   while( i < _argc )
      {
      ptr = _argv[ i ];

      // Only check parameters preceded by - or /
      if ( ( *ptr == '-' ) || ( *ptr == '/' ) )
         {
         ptr++;
         if ( stricmp( parameter, ptr ) == 0 )
            {
            found = TRUE;
            break;
            }
         }

      i++;
      }

   return( found );
   }


/*---------------------------------------------------------------------
   Function: DefaultExtension

   Checks if the specified filename contains an extension and adds
   one if it doesn't.
---------------------------------------------------------------------*/

void DefaultExtension
   (
   char *path,
   char *extension
   )

   {
	char	*src;

   //
   // if path doesn't have a .EXT, append extension
   // (extension should include the .)
   //
   src = path + strlen( path ) - 1;

   while( ( *src != '\\' ) && ( src != path ) )
      {
      if ( *src == '.' )
         {
         // it has an extension
         return;
         }
      src--;
      }

   strcat( path, extension );
   }


/*---------------------------------------------------------------------
   Function: TurnOffTextCursor

   Turns the cursor off.
---------------------------------------------------------------------*/

void TurnOffTextCursor
   (
   void
   )

   {
   union REGS regs;

   regs.w.ax = 0x0100;
   regs.w.cx = 0x2000;
#ifdef __FLAT__
   int386(0x10,&regs,&regs);
#else
   int86(0x10,&regs,&regs);
#endif
   }


/*---------------------------------------------------------------------
   Function: TurnOnTextCursor

   Turns the cursor on.
---------------------------------------------------------------------*/

void TurnOnTextCursor
   (
   void
   )

   {
   union REGS regs;

   regs.w.ax = 0x0100;
   regs.w.cx = 0x0708;
#ifdef __FLAT__
   int386(0x10,&regs,&regs);
#else
   int86(0x10,&regs,&regs);
#endif
   }
