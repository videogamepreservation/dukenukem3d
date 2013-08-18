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

/////////////////////////////////////////////////////////////////////////////
//
//      LBMLIB.H
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _lbmlib_private_
#define _lbmlib_private_
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
   {
   ms_none,
   ms_mask,
   ms_transcolor,
   ms_lasso
   } mask_t;

typedef enum
   {
   cm_none,
   cm_rle1
   } compress_t;

typedef struct
   {
   uint16          w,h;
   int16           x,y;
   byte            nPlanes;
   byte            masking;
   byte            compression;
   byte            pad1;
   uint16          transparentColor;
   byte            xAspect,yAspect;
   int16           pageWidth,pageHeight;
   } bmhd_t;

//****************************************************************************
//
//                                      LBM STUFF
//
//****************************************************************************

#define TINYID  ('T'+('I'<<8)+((int32)'N'<<16)+((int32)'Y'<<24))
#define TINYSTRING "TINY"
#define FORMID  ('F'+('O'<<8)+((int32)'R'<<16)+((int32)'M'<<24))
#define FORMSTRING "FORM"
#define ILBMID  ('I'+('L'<<8)+((int32)'B'<<16)+((int32)'M'<<24))
#define ILBMSTRING "ILBM"
#define PBMID   ('P'+('B'<<8)+((int32)'M'<<16)+((int32)' '<<24))
#define PBMSTRING "PBM "
#define BMHDID  ('B'+('M'<<8)+((int32)'H'<<16)+((int32)'D'<<24))
#define BMHDSTRING "BMHD"
#define BODYID  ('B'+('O'<<8)+((int32)'D'<<16)+((int32)'Y'<<24))
#define BODYSTRING "BODY"
#define CMAPID  ('C'+('M'<<8)+((int32)'A'<<16)+((int32)'P'<<24))
#define CMAPSTRING "CMAP"

#define BPLANESIZE      128
#define TINYWIDE        80
#define TINYDEEP        50
#define TINYBYTES       10

#ifdef __cplusplus
};
#endif
#endif
