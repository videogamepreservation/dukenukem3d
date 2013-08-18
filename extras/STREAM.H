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

// stream.h

#ifndef _stream_public
#define _stream_public
#ifdef __cplusplus
extern "C" {
#endif

//====================================================================
//
// streamlib
//
//====================================================================
// streamlib is a primitive stream parser, it separates a text stream
// into separate tokens which are denoted by non-printable characters,
// spaces and the semi-colon.
//
// comments can be added using the standard c "//"
// Several of the functions take crossline as a parameter this tells
// the library whether or not to cross over the next EOL or semi-colon.
// this behavior changes slightly with these three functions:
// STREAM_Get - gets the next token in the stream, crossline denotes
//              whether or not to cross over semi-colons or EOLS.
//
// NOTE: multiple tokens can be grabbed as a string if the '"' is used
// around the sequence of tokens.  The semi-colon is not active when
// it is between two quotes.
//
//

void STREAM_Free( int32 streamhandle );
void STREAM_Reset ( int32 streamhandle );
int32 STREAM_Parse ( char *data, int32 length, char * name );
int32 STREAM_GetLineNumber( int32 streamhandle );
boolean STREAM_More (int32 streamhandle, boolean crossline);
char * STREAM_Get
   (
   int32 streamhandle,
   boolean crossline
   );

#ifdef __cplusplus
};
#endif
#endif
