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

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include "debug4g.h"

// #include <memcheck.h>

/*
**  Under DOS/4GW, the first megabyte of physical memory - the
**  real memory - is mapped as a shared linear address space.
**  This allows your application to access video RAM using its
**  linear address.  The DOS segment:offset of B800:0000
**  corresponds to a linear (flat) address of B8000.
*/
#define MONO_AREA 0xb000
#define MONO_FLAT_ADDR ((MONO_AREA) << 4)
#define MONO_WIDTH  80
#define MONO_HEIGHT 25
#define MONO_SIZE (MONO_WIDTH*MONO_HEIGHT)
#define MONO_ATTR 0x700

#define BUFSIZE  256

static unsigned short *vidAddr = 0;
static int vidx=0, vidy=0;
static int debugOn = 0;

int setvmode(int);
#pragma aux setvmode =\
	"int 	0x10",\
	parm [eax]\

void dprintf_clrscr( void )
{
	int i;
	for ( i=0; i<MONO_SIZE; i++ ) {
		vidAddr[i] = MONO_ATTR | ' ';
	}
	vidx = 0;
	vidy = 0;
}

void dprintf_newline( void )
{
	int i;
	vidx = 0;
	vidy++;
	if (vidy == MONO_HEIGHT)
	{
		// move lines 2-25 up one line
		memmove(vidAddr, vidAddr + MONO_WIDTH, (MONO_SIZE - MONO_WIDTH) * sizeof(short));
		//for (i=MONO_WIDTH; i<MONO_SIZE; i++)
		//	vidAddr[i-MONO_WIDTH] = vidAddr[i];

		// clear last line
		for (i=MONO_SIZE-MONO_WIDTH; i<MONO_SIZE; i++)
			vidAddr[i] = MONO_ATTR | '\x20';
		vidy--;
	}
}

void dprintf_do( const char *__msg )
{
	int i;
	char *p;

	// first time initialization
	if (vidAddr == 0) {
		vidAddr = (unsigned short *)MONO_FLAT_ADDR;

		p = getenv("DEBUG");

		if ( p != NULL )
		{
			if (stricmp(p, "CONSOLE") == 0)
				debugOn = 2;
			else
			{
				debugOn = 1;
				dprintf_clrscr();
			}
		}
	}

	if ( !debugOn )
		return;

	if (debugOn == 2)
	{
		cputs(__msg);
		return;
	}

	// print the message
	i = 0;
	while ( __msg[i] != 0x00 ) {
		if (vidx == MONO_WIDTH) {    // handle wordwrap
			dprintf_newline();
		}
		if ( __msg[i] == 0x0A ) { // handle linefeed
			dprintf_newline();
		}
		if ( __msg[i] <= 0x1F) {  // skip control characters
			i++;
			continue;
		}
		vidAddr[(MONO_WIDTH * vidy) + vidx++] = (short)(MONO_ATTR | __msg[i++]);
	}
}

int dprintf( const char *__format, ... )
{
	int     len;
	va_list argptr;
	char    buf[BUFSIZE];

	// print variable argument string into buf for output
	va_start( argptr, __format );
	len = vsprintf( buf, __format, argptr );
	va_end( argptr );

	if (len >= BUFSIZE) {  // buf overwritten? return error.
		len = INT_MIN;
	}
	else if (len >= 0) {   // no vsprintf error? print buf.
		dprintf_do(buf);
	}
	return len;
}

int dprintfxy( int x, int y, const char *__format, ... )
{
	int     len;
	va_list argptr;
	char    buf[BUFSIZE];

	// print variable argument string into buf for output
	va_start( argptr, __format );
	len = vsprintf( buf, __format, argptr );
	va_end( argptr );

	if (len >= BUFSIZE) {  // buf overwritten? return error.
		len = INT_MIN;
	}
	else if (len >= 0) {   // no vsprintf error? print buf.
		vidx = x;
		vidy = y;
		dprintf_do(buf);
	}
	return len;
}

void __dassert( char *expr, char *file, int line)
{
	if (debugOn == 2)
		setvmode(3);
	dprintf("Assertion failed: %s in file %s at line %i\n", expr, file, line);
	exit(1);
}


#if 0
void main(void)
{
	int i;
	for (i=0;i<100;i++)
		dprintf("%d: This is a test.\n",i);
}

#endif
