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
   module: PS.C

   author: James R. Dose
   date:   December 12, 1995

   Simple sound player.

   (c) Copyright 1995 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fx_man.h"


/*---------------------------------------------------------------------
   Function prototypes
---------------------------------------------------------------------*/

char *LoadFile( char *filename, int *length );
char *GetUserText( const char *parameter );
int   CheckUserParm( const char *parameter );
void  DefaultExtension( char *path, char *extension );

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

#define NUMCARDS 8

char *SoundCardNames[] =
   {
   "Sound Blaster", "Awe 32", "Pro AudioSpectrum",
   "Sound Man 16", "Ensoniq SoundScape", "Gravis UltraSound",
   "Disney SoundSource", "Tandy SoundSource"
   };

int SoundCardNums[] =
   {
   SoundBlaster, Awe32, ProAudioSpectrum, SoundMan16,
   SoundScape, UltraSound, SoundSource, TandySoundSource
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
   int voices;
   int rate;
   int bits;
   int channels;
   int reverb;
   int status;
   int length;
   int voice;
   fx_device device;
   char *SoundPtr = NULL;
   char *ptr;
   char  filename[ 128 ];
   char ch;

   printf( "\nPS Version 1.0 by Jim Dose\n" );

   if ( ( CheckUserParm( "?" ) ) || ( argc < 2 ) )
      {
      int index;

      printf(
         "Usage: PS [ soundfile ] CARD=[ card # ] VOICES=[ # of voices ]\n"
         "          BITS=[ 8 or 16 ] [MONO/STEREO] RATE=[ mixing rate ]\n"
         "          REVERB=[ reverb amount]\n\n"
         "   sound card # = \n" );
      for( index = 0; index < NUMCARDS; index++ )
         {
         printf( "      %d : %s\n", index, SoundCardNames[ index ] );
         }

      printf( "\nDefault: PS [ soundfile ] CARD=0 VOICES=4 "
                "BITS=8 MONO RATE=11000\n\n" );
      exit( 0 );
      }

   // Default is Sound Blaster
   card     = 0;
   voices   = 4;
   bits     = 8;
   channels = 1;
   reverb   = 0;
   rate     = 11000;

   ptr = GetUserText( "VOICES" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &voices );
      }

   ptr = GetUserText( "BITS" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &bits );
      }

   ptr = GetUserText( "RATE" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &rate );
      }

   ptr = GetUserText( "REVERB" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &reverb );
      }

   ptr = GetUserText( "MONO" );
   if ( ptr != NULL )
      {
      channels = 1;
      }

   ptr = GetUserText( "STEREO" );
   if ( ptr != NULL )
      {
      channels = 2;
      }

   ptr = GetUserText( "CARD" );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &card );
      }

   if ( ( card < 0 ) || ( card >= NUMCARDS ) )
      {
      printf( "Value out of range for sound card #: %d\n", card );
      exit( 1 );
      }

   strcpy( filename, argv[ 1 ] );
   DefaultExtension( filename, ".wav" );
   SoundPtr = LoadFile( filename, &length );
   if ( !SoundPtr )
      {
      strcpy( filename, argv[ 1 ] );
      DefaultExtension( filename, ".voc" );
      SoundPtr = LoadFile( filename, &length );
      if ( !SoundPtr )
         {
         strcpy( filename, argv[ 1 ] );
         SoundPtr = LoadFile( filename, &length );
         if ( !SoundPtr )
            {
            printf( "Cannot open '%s' for read.\n", argv[ 1 ] );
            exit( 1 );
            }
         }
      }

   status = FX_SetupCard( card, &device );
   if ( status != FX_Ok )
      {
      printf( "%s\n", FX_ErrorString( status ) );
      exit( 1 );
      }

   status = FX_Init( card, voices, channels, bits, rate );
   if ( status != FX_Ok )
      {
      printf( "%s\n", FX_ErrorString( status ) );
      exit( 1 );
      }

   FX_SetReverb( reverb );

   FX_SetVolume( 255 );

   printf( "Playing file '%s'.\n\n", filename );
   printf( "Press any key to play the sound.\n"
           "Press ESCape to end.\n\n" );

   ch = 0;
   while( ch != 27 )
      {
      if ( stricmp( &filename[ strlen( filename ) - 3 ], "WAV" ) == 0 )
         {
         voice = FX_PlayWAV( SoundPtr, 0, 255, 255, 255, 0, 0 );
         }
      else if ( stricmp( &filename[ strlen( filename ) - 3 ], "VOC" ) == 0 )
         {
         voice = FX_PlayVOC( SoundPtr, 0, 255, 255, 255, 0, 0 );
         }
      else
         {
         voice = FX_PlayRaw( SoundPtr, length, rate, 0, 255, 255, 255, 0, 0 );
         }

      if ( voice < FX_Ok )
         {
         printf( "Sound error - %s\n", FX_ErrorString( status ) );
         }

      ch = getch();
      }

   FX_StopAllSounds();
   free( SoundPtr );
   FX_Shutdown();

   printf( "\n" );
   }


/*---------------------------------------------------------------------
   Function: LoadFile

   Loads a file from disk.
---------------------------------------------------------------------*/

char *LoadFile
   (
   char *filename,
   int  *length
   )

   {
   FILE   *in;
   long   size;
   char   *ptr;

   if ( ( in = fopen( filename, "rb" ) ) == NULL )
      {
      return NULL;
      }

   fseek( in, 0, SEEK_END );
   size = ftell( in );
   fseek( in, 0, SEEK_SET );

   ptr = ( char * )malloc( size );
   if ( ptr == NULL )
      {
      printf( "Out of memory while reading '%s'.\n", filename );
      exit( 1 );
      }

   if ( fread( ptr, size, 1, in ) != 1 )
      {
      printf( "Unexpected end of file while reading '%s'.\n", filename );
      exit(1);
      }

   fclose( in );

   *length = size;

   return( ptr );
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
   char  *src;

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
