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

#ifndef __rox_private__
#define __rox_private__
#ifdef __cplusplus
extern "C" {
#endif

// Defines
#define ROX_FILE_DELIM '\\'
#define ROX_SECTION_NAME_SIZE 8
#define ROX_ENTRY_NAME_SIZE 12
#define ROX_MAX_PATH_NAME 128

#define ROX_SINGLE_FILE 0xffffffff

#define DWORD_AT_INDEX( ptr, index ) \
   ( *( ( ( dword * )( ptr ) ) + ( index ) ) )

// Data types
// Entry structure for ROX data and files
typedef struct roxentry
   {
   char   name[ROX_ENTRY_NAME_SIZE];
   int32  handle;
   uint32 offset;
   uint32 size;
   void   *data;
   struct roxentry * next;
   struct roxentry * prev;
   } roxentry_t;

// section structure for ROX data and files
typedef struct roxsection
   {
   char name[ROX_SECTION_NAME_SIZE];
   roxentry_t *entries;
   struct roxsection * subsections;
   struct roxsection * next;
   struct roxsection * prev;
   } roxsection_t;

// prototypes
roxsection_t * ROX_CreateSection
   (
   const char * name
   );
roxentry_t * ROX_CreateEntry
   (
   const char * name
   );
void ROX_FreeEntry
   (
   roxentry_t * entry
   );
void ROX_FreeSection
   (
   roxsection_t * section
   );
roxsection_t * ROX_AddSubSection
   (
   roxsection_t * parent,
   const char * name
   );
roxsection_t * ROX_FindSubSection
   (
   roxsection_t * section,
   const char * name
   );
roxentry_t * ROX_AddEntry
   (
   roxsection_t * parent,
   const char * name
   );
roxentry_t * ROX_FindEntry
   (
   roxsection_t * section,
   const char * name
   );
void ROX_FixFileName
   (
   char * filename
   );
void ROX_ReadEntry
   (
   roxentry_t * entry,
   char * filename,
   int32 zonenum,
   int32 tag
   );
void ROX_TouchEntry
   (
   roxentry_t * entry,
   int32 tag
   );

#ifdef __cplusplus
};
#endif
#endif
