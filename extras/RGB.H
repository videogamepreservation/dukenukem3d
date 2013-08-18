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
//    RGB.C - Creates a best fit color table for RGB values.
//
//***************************************************************************

#ifndef _rgb_public
#define _rgb_public
#ifdef __cplusplus
extern "C" {
#endif

#define NUM_RED   64
#define NUM_GREEN 64
#define NUM_BLUE  64

#define REAL_RED( r )   ( ( r ) << 2 )
#define REAL_GREEN( g ) ( ( g ) << 2 )
#define REAL_BLUE( b )  ( ( b ) << 2 )

#if    !defined(__MSDOS__) || defined(__FLAT__)
extern byte RGB_Lookup[ NUM_RED ][ NUM_GREEN ][ NUM_BLUE ];
#else
extern byte huge RGB_Lookup[ NUM_RED ][ NUM_GREEN ][ NUM_BLUE ];
#endif
extern byte RGB_EGATable[ 16 ];

#define RGB_GetColor( r, g, b ) \
   ( RGB_Lookup[ ( r ) >> 2 ][ ( g ) >> 2 ][ ( b ) >> 2 ] )

#define RGB_EGAColor( c ) \
   ( RGB_EGATable[ ( c ) ] )

void RGB_FindEGAColors( void );
byte RGB_FindBestColor( int16 r, int16 g, int16 b, byte *palette );
void RGB_GetPalette( byte *pal );
byte RGB_GetBestColor( int16 r, int16 g, int16 b );
void RGB_MakeColorTable( void );
#ifdef __cplusplus
};
#endif
#endif
