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

// vid.h -- video driver defs
#ifndef __vid_h
#define __vid_h
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {disp_bitmap, disp_framebuffer}	display_t;

typedef void pixel_t;
// a pixel can be one, two, or four bytes

typedef byte pixel;

typedef struct
   {
	int x;
	int y;
	int width;
	int height;
	} vrect_t;

typedef struct
   {
	pixel_t		*buffer;
	pixel_t		*colormap;	// 256*pixbytes*grades size
	unsigned	pixbytes;  		// 1, 2, or 4
	unsigned	rowbytes;		// may be > width*pixbytes if displayed in a window
	unsigned	width;
	unsigned	height;
	float		aspect;			// width / height -- < 0 is taller than wide
	unsigned	cbits;
	unsigned	grades;			// 1<<cbits for convenience
   } viddef_t;

extern	viddef_t	vid;		// global video state

void VID_NormalizePalette (byte *palette);
// Changes a 0-255 rgb palette to a 0-63 rgb palette compatible with VGA

void	VID_SetPalette (byte *palette);
// called at startup and after any gamma correction

void	VID_GetPalette (byte *palette);
// called at startup and after any gamma correction

void  VID_Init (int32 width, int32 height, byte *palette, display_t displaytype, int scale );
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void	VID_Shutdown (void);
// Called at shutdown

void VID_GetBuffer (void);
// Called each frame before any drawing is to take place
// changes vid for next frame

void	VID_Update (int rectcount, vrect_t *rects);
// flushes the given rectangles from the view buffer to the screen

void  VID_UpdateScreen( void );
// flushes the entire view buffer to the screen

void  VID_ClearViewBuffer( int color );
// Sets the view buffer to a single color.

void ServiceEvents( void );
// Services any pending events (mouse, keyboard, window, etc.)
// Required for any events to take place.

void KEYBOARD_Startup
   (
   void
   );

void KEYBOARD_Shutdown
   (
   void
   );

#ifdef __cplusplus
};
#endif
#endif /* __vid_h */
