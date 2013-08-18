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

extern long frameplace, chainplace, chainnumpages;
//extern char palette[768];
EXTERN  palette[768];

static char waterlookup[256];

void InitWater()
{
	long i;

	for(i=0;i<256;i++)
		waterlookup[i] = (palette[i*3]+palette[i*3+1]+palette[i*3+2])/6;
}

void DoWater(long dahoriz)
{
	long x, y, topleftptr;
	short tilenume;
	char *ptr, *ptr2, ch, chinc;

	tilenume = 572;    //WATER TILE
	if ((gotpic[tilenume>>3]&(1<<(tilenume&7))) == 0) return;
	gotpic[tilenume>>3] &= ~(1<<(tilenume&7));

	for(x=0;x<xdim;x++)
	{
		if (chainnumpages >= 2)
		{
			outpw(0x3c4,2+(256<<(x&3)));
			outpw(0x3ce,4+((x&3)<<8));
			ptr = (char *)(chainplace+ylookup[dahoriz]+(x>>2));
			topleftptr = chainplace;
		}
		else
		{
			ptr = (char *)(frameplace+ylookup[dahoriz]+x);
			topleftptr = frameplace;
		}
		ptr2 = ptr; chinc = 0;

		for(y=dahoriz;y<ydim;y++)
		{
			if (*ptr == 255)
			{
				if ((chinc < 32) && (ptr2 >= topleftptr))
				{
					ch = min(max(waterlookup[*ptr2]-chinc,0),31)+96;
					ptr2 -= ylookup[1]; if (ptr2 < topleftptr) ch = 96;
					if (y&1) chinc++;
				}
				*ptr = ch;
			}
			else
				{ ptr2 = ptr; chinc = 0; }
			ptr += ylookup[1];
		}
	}
}
