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

#ifndef __rox_public__
#define __rox_public__
#ifdef __cplusplus
extern "C" {
#endif

/*
====================
=
= ROX_Get
=
= Get a specific data file, the name is the pathname of the data
= directories are separated by either '\' or '/', it is your preference
= The delimiter will be properly resolved for the system you are working
= on.
= zonenum refers to the memory zone in which you would like this piece of
= data to exist.
= tag refers to the memory tag to give the piece of data (refer to zone.h)
=
====================
*/

void * ROX_Get
   (
   const char * name,
   int32 zonenum,
   int32 tag
   );

void ROX_Init ( void );
void ROX_Shutdown ( void );

#ifdef __cplusplus
};
#endif
#endif
