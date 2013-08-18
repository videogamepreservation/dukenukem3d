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
//    WAD.C - Wad managment utilities
//
//***************************************************************************

#ifndef _wad_public
#define _wad_public
#ifdef __cplusplus
extern "C" {
#endif

#define WAD_LUMP_NOT_FOUND ((dword)0xffffffff )

word    WAD_GetWADCRC( void );                     // Calculates the CRC of the wad info

void    WAD_InitMultipleFiles( const char **filenames ); // Initialize multiple wads
void    WAD_InitFile( const char *filename );            // Init a single wad file

dword   WAD_CheckNumForName( const char *name );         // Check to see if the named lump exists
dword   WAD_GetNumForName( const char *name );           // Get the number for the named lump
char   *WAD_GetNameForNum( dword lump );           // Get the name for a number
int32   WAD_GetTypeForNum( dword lump );           // Get the type of the named lump
int32   WAD_GetTypeForName( const char *name );
         // Get the name of a numbered lump
int32   WAD_CountType( int32 lumptype );           // Get the number of lumps of the specified type

int32   WAD_GetLumpNumsOfType( int32 lumptype, dword **arrayptr );
// Allocates an array containing the lump numbers of all lumps with the
// specified lumptype.  Last element of the array is -1.  Array should
// be freed after it's done being used.

dword   WAD_NumLumps( void );                      // Get the current number of lumps managed
dword   WAD_LumpLength( dword lump );              // Get the length of the numbered lump
void    WAD_ReadLump( dword lump, void *dest );    // Read the numbered lump into a buffer
void    WAD_WriteLump( dword lump, void *src );

void   *WAD_CacheLumpNum( dword lump, int32 zone, int32 tag );             // Cache in the numbered lump with the appropriate memory tag
void   *WAD_CacheLumpName( const char *name, int32 zone, int32 tag );            // Cache in the named lump with the appropriate memory tag
void   *WAD_CacheLumpDec( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with decimal the appropriate memory tag
void   *WAD_CacheLumpHex( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with hexidecimal the appropriate memory tag
void   WAD_OpenWadFile ( const char * filename, boolean quiet);
void WAD_WriteWadLump
   (
   byte * data,
   int32 size,
   const char * name,
   int32 lumptype,
   boolean quiet
   );
void   WAD_CloseWadFile ( boolean quiet );
void   WAD_ShootWAD( const char *filename, boolean quiet );
char * WAD_LumptypeToString ( int32 lumptype );

#ifdef __cplusplus
};
#endif
#endif
