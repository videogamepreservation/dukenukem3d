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

//***************************************************************************
//
//    WAD2.C - Wad managment utilities
//
//***************************************************************************

#ifndef _wad2_public
#define _wad2_public
#ifdef __cplusplus
extern "C" {
#endif

#define WAD2_LUMP_NOT_FOUND ((dword)0xffffffff )

word    WAD2_GetWADCRC( void );                     // Calculates the CRC of the wad info

void    WAD2_InitMultipleFiles( const char **filenames ); // Initialize multiple wads
void    WAD2_InitFile( const char *filename );            // Init a single wad file

dword   WAD2_CheckNumForName( const char *name );         // Check to see if the named lump exists
dword   WAD2_GetNumForName( const char *name );           // Get the number for the named lump
char   *WAD2_GetNameForNum( dword lump );           // Get the name for a number
int32   WAD2_GetTypeForNum( dword lump );           // Get the type of the named lump
int32   WAD2_GetTypeForName( const char *name );
         // Get the name of a numbered lump
int32   WAD2_CountType( int32 lumptype );           // Get the number of lumps of the specified type

int32   WAD2_GetLumpNumsOfType( int32 lumptype, dword **arrayptr );
// Allocates an array containing the lump numbers of all lumps with the
// specified lumptype.  Last element of the array is -1.  Array should
// be freed after it's done being used.

dword   WAD2_NumLumps( void );                      // Get the current number of lumps managed
dword   WAD2_LumpLength( dword lump );              // Get the length of the numbered lump
void    WAD2_ReadLump( dword lump, void *dest );    // Read the numbered lump into a buffer
void    WAD2_WriteLump( dword lump, void *src );

void   *WAD2_CacheLumpNum( dword lump, int32 zone, int32 tag );             // Cache in the numbered lump with the appropriate memory tag
void   *WAD2_CacheLumpName( const char *name, int32 zone, int32 tag );            // Cache in the named lump with the appropriate memory tag
void   *WAD2_CacheLumpDec( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with decimal the appropriate memory tag
void   *WAD2_CacheLumpHex( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with hexidecimal the appropriate memory tag
void   WAD2_OpenWadFile (const char * filename, boolean quiet);
void WAD2_WriteWadLump
   (
   byte * data,
   int32 size,
   const char * name,
   int32 lumptype,
   boolean quiet
   );
void   WAD2_CloseWadFile ( boolean quiet );
void   WAD2_ShootWAD( const char *filename, boolean quiet );
int32  WAD2_LumpsInWad ( void );
char * WAD2_LumptypeToString ( int32 lumptype );

#ifdef __cplusplus
};
#endif
#endif
