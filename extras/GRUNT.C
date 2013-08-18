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

// Includes

#include <dos.h>

// Macros

#define MAXX 320
#define MAXY 200
#define TIMERINTSPERSECOND 120

#define  sc_None         0
#define  sc_Bad          0xff
#define  sc_Comma        0x33
#define  sc_Period       0x34
#define  sc_Return       0x1c
#define  sc_Enter        sc_Return
#define  sc_Escape       0x01
#define  sc_Space        0x39
#define  sc_BackSpace    0x0e
#define  sc_Tab          0x0f
#define  sc_LeftAlt      0x38
#define  sc_LeftControl  0x1d
#define  sc_CapsLock     0x3a
#define  sc_LeftShift    0x2a
#define  sc_RightShift   0x36
#define  sc_F1           0x3b
#define  sc_F2           0x3c
#define  sc_F3           0x3d
#define  sc_F4           0x3e
#define  sc_F5           0x3f
#define  sc_F6           0x40
#define  sc_F7           0x41
#define  sc_F8           0x42
#define  sc_F9           0x43
#define  sc_F10          0x44
#define  sc_F11          0x57
#define  sc_F12          0x58
#define  sc_Kpad_Star    0x37
#define  sc_Paused       0x59
#define  sc_ScrollLock   0x46

#define  sc_OpenBracket  0x1a
#define  sc_CloseBracket 0x1b

#define  sc_1            0x02
#define  sc_2            0x03
#define  sc_3            0x04
#define  sc_4            0x05
#define  sc_5            0x06
#define  sc_6            0x07
#define  sc_7            0x08
#define  sc_8            0x09
#define  sc_9            0x0a
#define  sc_0            0x0b
#define  sc_Minus        0x0c
#define  sc_Equals       0x0d
#define  sc_Plus         0x0d

#define  sc_kpad_1       0x4f
#define  sc_kpad_2       0x50
#define  sc_kpad_3       0x51
#define  sc_kpad_4       0x4b
#define  sc_kpad_5       0x4c
#define  sc_kpad_6       0x4d
#define  sc_kpad_7       0x47
#define  sc_kpad_8       0x48
#define  sc_kpad_9       0x49
#define  sc_kpad_0       0x52
#define  sc_kpad_Minus   0x4a
#define  sc_kpad_Plus    0x4e
#define  sc_kpad_Period  0x53

#define  sc_A            0x1e
#define  sc_B            0x30
#define  sc_C            0x2e
#define  sc_D            0x20
#define  sc_E            0x12
#define  sc_F            0x21
#define  sc_G            0x22
#define  sc_H            0x23
#define  sc_I            0x17
#define  sc_J            0x24
#define  sc_K            0x25
#define  sc_L            0x26
#define  sc_M            0x32
#define  sc_N            0x31
#define  sc_O            0x18
#define  sc_P            0x19
#define  sc_Q            0x10
#define  sc_R            0x13
#define  sc_S            0x1f
#define  sc_T            0x14
#define  sc_U            0x16
#define  sc_V            0x2f
#define  sc_W            0x11
#define  sc_X            0x2d
#define  sc_Y            0x15
#define  sc_Z            0x2c

// Extended scan codes

#define  sc_UpArrow      0x5a
#define  sc_DownArrow    0x5b
#define  sc_LeftArrow    0x5c
#define  sc_RightArrow   0x5d
#define  sc_Insert       0x5e
#define  sc_Delete       0x5f
#define  sc_Home         0x61
#define  sc_End          0x62
#define  sc_PgUp         0x63
#define  sc_PgDn         0x64
#define  sc_RightAlt     0x65
#define  sc_RightControl 0x66
#define  sc_kpad_Slash   0x67
#define  sc_kpad_Enter   0x68
#define  sc_PrintScreen 0x69
#define  sc_LastScanCode 0x6a

#define NOERROR 0
#define ERROR 1

#define END 0
#define COLOR 1
#define LINE 2
#define LINETO 3
#define IMAGE 4
#define XADD 5
#define YADD 6
#define ZADD 7
#define DRAG 8


// Globals

char far *screen;
int mul320[MAXY];

volatile char keystatus[256], readch, oldreadch, extended;
volatile unsigned int clockspeed, totalclock, numframes, randomseed;

int ip_A[] =
{
LINE    ,-9,0,-9,-9,
LINETO  ,0,-18,
LINETO  ,9,-9,
LINETO  ,9,0,
LINE    ,-9,-9,9,-9,
END
};

int ip_B[] =
{
LINE    ,-9,0,-9,-18,
LINETO  ,9,-18,
LINETO  ,9,-13,
LINETO  ,5,-9,
LINETO  ,9,-5,
LINETO  ,9,0,
LINETO  ,-9,0,
LINE    ,-9,-9,5,-9,
END
};

int ip_C[] =
{
LINE    ,9,0,-9,0,
LINETO  ,-9,-18,
LINETO  ,9,-18,
END
};

int ip_D[] =
{
LINE    ,5,0,-9,0,
LINETO  ,-9,-18,
LINETO  ,5,-18,
LINETO  ,9,-16,
LINETO  ,9,-4,
LINETO  ,5,0,
END
};

int ip_E[] =
{
IMAGE   ,ip_C,
LINE    ,-9,-9,5,-9,
END
};

int ip_F[] =
{
IMAGE   ,ip_E,
LINE    ,-9,0,9,0,
END
};

int ip_G[] =
{
IMAGE   ,ip_C,
LINE    ,9,0,9,-9,
LINETO  ,0,-9,
END
};

int ip_H[] =
{
IMAGE   ,ip_F,
LINE    ,9,-18,9,0,
LINE    ,7,-9,9,-9,
COLOR   ,0,
LINE    ,-9,-18,9,-18,
END
};

int ip_I[] =
{
LINE    ,-9,0,9,0,
LINE    ,-9,-18,9,-18,
LINE    ,0,-18,0,0,
END
};

int ip_TITLE[] =
{
IMAGE   ,ip_A,
XADD    ,20,
IMAGE   ,ip_D,
XADD    ,20,
IMAGE   ,ip_D,
XADD    ,20,
IMAGE   ,ip_A,
XADD    ,20,
IMAGE   ,ip_C,
XADD    ,20,
IMAGE   ,ip_A,
END
};

int ip_Landscape[] =
{
COLOR   ,9,
LINE    ,-1000,0,1000,-20,
DRAG    ,3,1,
END
};


// Pragmas

#pragma aux setvmode =\
    "int 0x10",\
    parm [ax]\

#pragma aux readkey =\
    "in al, 0x60",\
    "mov readch, al",\
    "in al, 0x61",\
    "or al, 0x80",\
    "out 0x61, al",\
    "and al, 0x7f",\
    "out 0x61, al",\
    modify [ax]\


// Function Prototypes

void (__interrupt __far *oldtimerhandler)();
void __interrupt __far timerhandler(void);
void (__interrupt __far *oldkeyhandler)();
void __interrupt __far keyhandler(void);


// Code

//  Init Code

void __interrupt __far keyhandler()
{
	oldreadch = readch;
	readkey();
	if ((readch|1) == 0xe1)
		extended = 128;
	else
	{
		if (oldreadch != readch)
			keystatus[(readch&127)+extended] = ((readch>>7)^1);
		extended = 0;
	}

    outp(0x20,0x20);
}

void initkeyboard(void)
{
    oldkeyhandler = _dos_getvect(0x9);
    _disable(); _dos_setvect(0x9, keyhandler); _enable();
}

void uninitkeyboard(void)
{
    _dos_setvect(0x9, oldkeyhandler);
    outp(0x43,54); outp(0x40,255); outp(0x40,255);
}

void __interrupt __far timerhandler()
{
    totalclock++;
    _chain_intr(oldtimerhandler);
    outp(0x20,0x20);
}

void inittimer(void)
{
	outp(0x43,0x34);
	outp(0x40,(1193181/TIMERINTSPERSECOND)&255);
	outp(0x40,(1193181/TIMERINTSPERSECOND)>>8);
	oldtimerhandler = _dos_getvect(0x8);
	_disable(); _dos_setvect(0x8, timerhandler); _enable();
    totalclock = 0;
}

void uninittimer(void)
{
	outp(0x43,0x34); outp(0x40,0); outp(0x40,0);
	_disable(); _dos_setvect(0x8, oldtimerhandler); _enable();
}

//  Drawing Code

int lastx,lasty,lastz,lastcolor;
void line(int x1, int y1, int x2, int y2, unsigned char color)
{
    int i, dx, dy, np;
    int d, di1, di2, x;
    int y, xi1, xi2, yi1, yi2;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    xi2 = 1;
    yi2 = 1;

    if (dx >= dy)
	{
		np  = dx + 1;
		d   = (dy << 1) - dx;
		di1 = dy << 1;
		di2 = (dy - dx) << 1;
		xi1 = 1;
		yi1 = 0;
	}
    else
	{
        np  = dy + 1;
		d   = (dx << 1) - dy;
		di1 = dx << 1;
		di2 = (dx - dy) << 1;
		xi1 = 0;
		yi1 = 1;
	}

    if (x1 > x2)
	{
		xi1 = -xi1;
		xi2 = -xi2;
	}

    if (y1 > y2)
	{
		yi1 = -yi1;
		yi2 = -yi2;
	}

    x = x1;
    y = y1;

    for (i = np; i > 0; i--)
    {
        if(x >= 0 && x < MAXX && y >= 0 && y < MAXY)
            *(screen + mul320[y] + x) = color;

        if (d < 0)
        {
            d += di1;
            x += xi1;
            y += yi1;
        }
        else
        {
            d += di2;
            x += xi2;
            y += yi2;
        }
    }
}

char image(int *insp,int x1,int y1,int z1,int col)
{
    int *insptr;

    insptr = insp;
    lastcolor = col;
    lastz = z1;

    while(1)
    {
        switch(*insptr)
        {
            case END:
                return NOERROR;
            default:
                return ERROR;
            case COLOR:
                lastcolor = *(insptr + 1);
                insptr += 2;
                break;
            case LINE:
                lastx = *(insptr + 3);
                lasty = *(insptr + 4);
                line(x1+*(insptr + 1),y1+*(insptr + 2),x1+lastx,y1+lasty,lastcolor);
                insptr += 5;
                break;
            case LINETO:
                line(x1+lastx,y1+lasty,x1+*(insptr + 1),y1+*(insptr + 2),lastcolor);
                lastx = *(insptr + 1);
                lasty = *(insptr + 2);
                insptr += 3;
                break;
            case IMAGE:
                image((int *)*(insptr+1),x1,y1,lastz,col);
                insptr += 2;
                break;
            case XADD:
                x1 += *(insptr+1);
                insptr+=2;
                break;
            case YADD:
                y1 += *(insptr+1);
                insptr+=2;
                break;
            case ZADD:
                z1 += *(insptr+1);
                insptr+=2;
                break;
            case DRAG:
                *(insp + (*(insptr+1)) ) += *(insptr + 2);
                insptr += 3;
                break;
        }
    }
}


//  Rest of Code

int rnd(void)
{
    randomseed += randomseed + ( totalclock*999 );
    return randomseed;
}

void initgame(void)
{
    int y;

    setvmode(0x13);
    screen = (char far *)0xa0000000;
    randomseed = 17L;

    for( y = 199; y >= 0; y-- )
        mul320[y] = y * 320;

    initkeyboard();
    inittimer();
}

uninitgame(void)
{
    uninittimer();
    uninitkeyboard();
    setvmode(3);
}

void gameloop(void)
{
    image(ip_TITLE,160,100,0,31);
    while(keystatus[sc_Escape] == 0);
}

void main(void)
{
    initgame();
    gameloop();
    uninitgame();
}


// Notes
/*
    - Make a cool game
*/

