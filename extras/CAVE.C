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

//

// Includes

#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys\stat.h>


// Defines

#define MAXXDIM 320
#define MAXYDIM 400
#define MAXMEMNEEDED (1<<24)
#define MINMEMNEEDED (1<<22)
#define MAXTILES 1024
#define MAXPALOOKUPS 64
#define NUMOPTIONS 8
#define NUMKEYS 9
#define MAXPLAYERS 16
#define MAXOBJECTS 1024
#define MAXVIEWOBJECTS MAXOBJECTS
#define MAXSTATUS 32
#define MAXCHANNELS 8
#define MAXSOUNDS 128

#define  sc_Escape       0x01   // Menu
#define  sc_P            0x19   // Pause

#define  sc_F1           0x3b   // Help
#define  sc_F2           0x3c   // Load
#define  sc_F3           0x3d   // Save
#define  sc_F4           0x3e   // initaboard()
#define  sc_Tab          0x0f   // Topdown view
#define  sc_CapsLock     0x3a   // Runlock

#define  sc_UpArrow      0xc8   // Move Fowards
#define  sc_DownArrow    0xd0   // Move Backwards
#define  sc_LeftArrow    0xcb   // Move Left
#define  sc_RightArrow   0xcd   // Move Right

#define  sc_A            0x1e   // Up/Jump
#define  sc_Z            0x2c   // Down/Duck
#define  sc_F            0x21   // Flash ight
#define  sc_kpad_1       0x4f   // Look Up
#define  sc_kpad_0       0x52   // Look Down

#define  sc_LeftAlt      0x38   // Strafe
#define  sc_Comma        0x33   // Strafe Left
#define  sc_Period       0x34   // Strafe Right
#define  sc_LeftControl  0x1d   // Use Weapon
#define  sc_LeftShift    0x2a   // Run
#define  sc_Space        0x39   // Use

#define IO_LOAD 0
#define IO_SAVE 1
#define IO_NOCLOSE 2

#define CLIP_HITCEILING 1
#define CLIP_HITFLOOR 2
#define CLIP_HALT 4

#define TOPHEIGHT(X,Y,H)  map.h1[((Y)<<8)+(X)]=(H)
#define TOPCOLOR(X,Y,C)   map.c1[((Y)<<8)+(X)]=(C)
#define TOPSHADE(X,Y,S)   map.s1[((Y)<<8)+(X)]=(S)
#define BOTHEIGHT(X,Y,H)  map.h2[((Y)<<8)+(X)]=(H)
#define BOTCOLOR(X,Y,C)   map.c2[((Y)<<8)+(X)]=(C)
#define BOTSHADE(X,Y,S)   map.s2[((Y)<<8)+(X)]=(S)


// Typedefs

typedef struct
{
    char **ptr;
    long amount;
} chunktype;

typedef struct
{
    unsigned char *h1, *h2, *c1, *c2, *s1, *s2;
    short headobject[MAXSTATUS+1];
    short prevobject[MAXOBJECTS];
    short nextobject[MAXOBJECTS];
    short objectstat[MAXOBJECTS];
} maptype;

typedef struct
{
    short viewobject;
    short viewobjectang;
    long viewobjectdist;
} viewobjecttype;

typedef struct
{
    long sndno,i;
    char *ptr;
} soundplayingtype;

typedef struct
{
    char *fn, pri;
    long len;
} soundtype;

typedef struct
{
    long posx, posy, posz, horiz;
    long vel, svel, zvel, angvel;
    short ang, i;

    // Flags
    char runmode, topdown, flashlight;
} selftype;

typedef struct
{
    long x, y, z, xvel, yvel, zvel, templong;
    short picnum, angvel, owner, clipdist, ang;
} objecttype;

typedef struct
{
    char manufacturer;
    char version;
    char encoding;
    char bits_per_pixel;
    short xmin,ymin;
    short xmax,ymax;
    short hres;
    short vres;
    char palette[48];
    char reserved;
    char colour_planes;
    short bytes_per_line;
    short palette_type;
    char filler[58];
} pcxtype;


// Globals

maptype map;
selftype self[MAXPLAYERS];
objecttype object[MAXOBJECTS];

//  Disposible (Non savable) globals
soundplayingtype soundplaying[MAXCHANNELS];
soundtype sounds[MAXSOUNDS];
viewobjecttype scrnobj[MAXVIEWOBJECTS];
short viewobjectcnt;

long  frameplace,xdim,ydim;
unsigned char palookup[MAXPALOOKUPS][256],palette[768];
long pixs, vidmode, detmode;
long moustat, mousx, mousy;
long randomseed, screenpeek;
int iofil;

short sintable[2048], tantable[2048];
short radarang[320], muly80[MAXYDIM];
long mul16000[4] = {0,16000,32000,48000};
char *scrbuf;

pcxtype pcxheader;
unsigned int pcxwidth, pcxdepth;
unsigned int pcxbytes, pcxbits;

char atile[] =
{
0,0,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,  0,254,254,  0,254,  0,  0,  0,  0,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,  0,  0,  0,254,  0,  0,  0,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,254,254,254,254,  0,254,254,254,  0,
254,  0,254,254,  0,254,  0,  0,  0,  0,254,  0,  0,  0,254,  0,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254
};

char *mainmem;
long memptr, totalmemory, chunkloc, maxchunks;
chunktype *chunk;

volatile char keystatus[256], readch, oldreadch, extended;

volatile long clockspeed, totalclock, numframes;
void (__interrupt __far *oldtimerhandler)();
void __interrupt __far timerhandler(void);
void (__interrupt __far *oldkeyhandler)();
void __interrupt __far keyhandler(void);

// Pragmas

#pragma aux toutp =\
	"out dx, al",\
	parm [edx][eax]\
	modify exact \

#pragma aux toutpw =\
	"out dx, ax",\
	parm [edx][eax]\
	modify exact \


#pragma aux setvmode =\
    "int 0x10",\
	parm [eax]\

#pragma aux drawpixel =\
    "mov byte ptr [edi], al",\
    parm [edi][eax]\

#pragma aux drawpixels =\
    "mov word ptr [edi], ax",\
    parm [edi][eax]\

#pragma aux drawpixeles =\
    "mov dword ptr [edi], eax",\
    parm [edi][eax]\

#pragma aux labs =\
	"test eax, eax",\
	"jns skipnegate",\
	"neg eax",\
	"skipnegate:",\
	parm nomemory [eax]\


#pragma aux scale =\
	"imul ebx",\
	"idiv ecx",\
	parm [eax][ebx][ecx]\
	modify [eax edx]\

#pragma aux sqr =\
  "imul eax, eax",\
   parm nomemory [eax]\
   modify exact [eax]\
   value [eax]

#pragma aux sgn =\
	"add ebx, ebx",\
	"sbb eax, eax",\
	"cmp eax, ebx",\
	"adc al, 0",\
	parm nomemory [ebx]\
	modify exact [eax ebx]\


#pragma aux mulscale =\
	"imul ebx",\
	"shrd eax, edx, cl",\
	parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux divscale =\
	"cdq",\
	"shld edx, eax, cl",\
	"sal eax, cl",\
	"idiv ebx",\
	parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux groudiv =\
	"shl eax, 12",\
    "sub eax, ecx",\
	"shld edx, eax, 16",\
	"sar ebx, 8",\
	"idiv bx",\
    parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux drawtopslab =\
	"shr ecx, 1",\
	"jnc skipdraw1a",\
	"mov [edi], al",\
	"add edi, 80",\
	"skipdraw1a: shr ecx, 1",\
	"jnc skipdraw2a",\
	"mov [edi], al",\
	"mov [edi+80], al",\
	"add edi, 160",\
	"skipdraw2a: jecxz skipdraw4a",\
	"startdrawa: mov [edi], al",\
	"mov [edi+80], al",\
	"mov [edi+160], al",\
	"mov [edi+240], al",\
	"add edi, 320",\
	"loop startdrawa",\
	"skipdraw4a: mov eax, edi",\
	parm [edi][ecx][eax]\
	modify [edi ecx eax]\

#pragma aux drawbotslab =\
	"shr ecx, 1",\
	"jnc skipdraw1b",\
	"mov [edi], al",\
	"sub edi, 80",\
	"skipdraw1b: shr ecx, 1",\
	"jnc skipdraw2b",\
	"mov [edi], al",\
	"mov [edi-80], al",\
	"sub edi, 160",\
	"skipdraw2b: jecxz skipdraw4b",\
	"startdrawb: mov [edi], al",\
	"mov [edi-80], al",\
	"mov [edi-160], al",\
	"mov [edi-240], al",\
	"sub edi, 320",\
	"loop startdrawb",\
	"skipdraw4b: mov eax, edi",\
	parm [edi][ecx][eax]\
	modify [edi ecx eax]\

#pragma aux clearbuf =\
	"rep stosd",\
	parm [edi][ecx][eax]\
	modify exact [edi ecx]\

#pragma aux clearbufbyte =\
	"cmp ecx, 4",\
	"jae longcopy",\
	"test cl, 1",\
	"jz preskip",\
	"stosb",\
	"preskip: shr ecx, 1",\
	"rep stosw",\
	"jmp endit",\
	"longcopy: test edi, 1",\
	"jz skip1",\
	"stosb",\
	"dec ecx",\
	"skip1: test edi, 2",\
	"jz skip2",\
	"stosw",\
	"sub ecx, 2",\
	"skip2: mov ebx, ecx",\
	"shr ecx, 2",\
	"rep stosd",\
	"test bl, 2",\
	"jz skip3",\
	"stosw",\
	"skip3: test bl, 1",\
	"jz endit",\
	"stosb",\
	"endit:",\
	parm [edi][ecx][eax]\
	modify [ebx]\

#pragma aux copybuf =\
	"rep movsd",\
	parm [esi][edi][ecx]\
	modify exact [ecx esi edi]\

#pragma aux copybytes =\
    "rep movsb",\
	parm [esi][edi][ecx]\
    modify exact [ecx esi edi]\

#pragma aux copybufbyte =\
	"cmp ecx, 4",\
	"jae longcopy",\
	"test cl, 1",\
	"jz preskip",\
	"movsb",\
	"preskip: shr ecx, 1",\
	"rep movsw",\
	"jmp endit",\
	"longcopy: test edi, 1",\
	"jz skip1",\
	"movsb",\
	"dec ecx",\
	"skip1: test edi, 2",\
	"jz skip2",\
	"movsw",\
	"sub ecx, 2",\
	"skip2: mov ebx, ecx",\
	"shr ecx, 2",\
	"rep movsd",\
	"test bl, 2",\
	"jz skip3",\
	"movsw",\
	"skip3: test bl, 1",\
	"jz endit",\
	"movsb",\
	"endit:",\
	parm [esi][edi][ecx]\
	modify [ebx]\


#pragma aux showscreen4pix320200 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0f02",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux showscreen4pix320400 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0f02",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux showscreen2pix320200 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0302",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0c02",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux showscreen2pix320400 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0302",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0c02",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux showscreen1pix320200 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0102",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0202",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0402",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0802",\
	"out dx, ax",\
	"mov ecx, 4000",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux showscreen1pix320400 =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0102",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov esi, scrbuf",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0202",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0402",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov edi, frameplace",\
	"rep movsd",\
	"mov ax, 0x0802",\
	"out dx, ax",\
	"mov ecx, 8000",\
    "mov edi, frameplace",\
	"rep movsd",\
	modify [eax ecx edx esi edi]\

#pragma aux limitrate =\
	"mov dx, 0x3da",\
	"wait1: in al, dx",\
	"test al, 8",\
	"jnz wait1",\
	"wait2: in al, dx",\
	"test al, 8",\
	"jz wait2",\
	modify exact [eax edx]\


#pragma aux setupmouse =\
    "mov eax, 0",\
	"int 33h",\
    "mov moustat,eax",\

#pragma aux readmouse =\
    "mov eax, 11d",\
	"int 33h",\
    "sar ecx, 1",\
    "sar edx, 1",\
    "mov mousx, ecx",\
    "mov mousy, edx",\
    modify [eax ecx edx]\

#pragma aux interruptend =\
	"mov al, 20h",\
	"out 20h, al",\
	modify [eax]\

#pragma aux readkey =\
	"in al, 0x60",\
	"mov readch, al",\
	"in al, 0x61",\
	"or al, 0x80",\
	"out 0x61, al",\
	"and al, 0x7f",\
	"out 0x61, al",\
	modify [eax]\


// Interrupt Handlers

void __interrupt __far timerhandler()
{
	clockspeed++;
	interruptend();
}

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
	interruptend();
}


// Fixed math

tsqrt(long num)
{
    long root, temp;

    root = 128;
    do
    {
        temp = root;
        root = ((root+(num/root))>>1);
    }
    while (labs(temp-root) > 1);
    return(root);
}

long trand(void)
{
    randomseed = (randomseed*43+1)&(65535|(65535<<16));
    return(randomseed);
}

getangle(long xvect, long yvect)
{
    if ((xvect|yvect) == 0) return(0);
    if (xvect == yvect) return(256+((xvect<0)<<10));
    if (xvect == -yvect) return(768+((xvect>0)<<10));
    if (labs(xvect) > labs(yvect))
        return(radarang[scale(yvect,320>>1,xvect)+(320>>1)]+((xvect<0)<<10));
    return(radarang[(320>>1)-scale(xvect,320>>1,yvect)]+512+((yvect<0)<<10));
}

short angdif(short a,short na)
{
    a &= 2047;
    na &= 2047;

    if(labs(a-na) < 1024)
        return (na-a);
    else
    {
        if(na > 1024) na -= 2048;
        if(a > 1024) a -= 2048;

        na -= 2048;
        a -= 2048;
        return (na-a);
    }
}


rotatepoint(long xpivot, long ypivot, long x, long y, short daang, long *x2, long *y2)
{
    *x2 = xpivot+mulscale(x-xpivot,sintable[(daang+2560)&2047],14)-mulscale(y-ypivot,sintable[(daang+2048)&2047],14);
    *y2 = ypivot+mulscale(y-ypivot,sintable[(daang+2560)&2047],14)+mulscale(x-xpivot,sintable[(daang+2048)&2047],14);
}



// I/O

char loadpcx(char *fname,char *p)
{
	 FILE *fp;
     int c, i, j, n;

	 /* attempt to open the file */
     if((fp=fopen(fname,"rb")) == NULL)
         return(1);

     /* read in the pcxheader */
     if( fread((char *)&pcxheader,1,sizeof(pcxtype),fp) != sizeof(pcxtype) )
         return(1);

     if(pcxheader.bits_per_pixel == 1)
         pcxbits=pcxheader.colour_planes;
     else
         pcxbits=pcxheader.bits_per_pixel;

	 /* check to make sure it's a picture */
     if(pcxbits !=8 || pcxheader.manufacturer != 0x0a || pcxheader.version != 5 )
     {
         fclose(fp);
         return(1);
     }

     /* Find the palette */
     if( fseek(fp,-769L,SEEK_END) == 0 )
         if( fgetc(fp) !=0x0c || fread( palette,1,768,fp) != 768 )
         {
             fclose(fp);
             return(1);
         }

     for(i=0;i<768;++i)
         palette[i]=palette[i] >> 2;

     fseek(fp,128L,SEEK_SET);

	 /* allocate a big buffer */
     pcxwidth = (pcxheader.xmax-pcxheader.xmin)+1;
     pcxdepth = (pcxheader.ymax-pcxheader.ymin)+1;
     pcxbytes=pcxheader.bytes_per_line;

	 /* unpack the file */
     for(j=0;j<pcxdepth;++j)
     {
        n = pcxbytes;
        do
        {
            c=fgetc(fp) & 0xff;       /* get a key byte */
           if((c & 0xc0) == 0xc0)     /* if it's a run of bytes field */
           {
               i = c&0x3f;            /* and off the high pcxbits */
               c=fgetc(fp);         /* get the run byte */
               while(i--)
                   *(p++)=c;         /* run the byte */
           }
           else *(p++)=c;            /* else just store it */
        }
        while(n--);
    }

    fclose(fp);

    return(0);
}


iodata(char *fn,char *ptr,long len,char iotype)
{
    if(iotype&IO_SAVE)
    {
        if (iofil != -1 || (iofil = open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) ) != -1 )
        {
            write(iofil,ptr,len);
            if( (iotype&IO_NOCLOSE) != IO_NOCLOSE)
            {
                close(iofil);
                iofil = -1;
            }
            return 1;
        }

        return 0;
    }

    if (iofil != -1 || (iofil = open(fn,O_BINARY|O_RDWR,S_IREAD)) != -1)
    {
        read(iofil,ptr,len);
        if( (iotype&IO_NOCLOSE) != IO_NOCLOSE)
        {
            close(iofil);
            iofil = -1;
        }
        return 1;
    }

    return 0;
}

long spawn(long picnum, long owner)
{
    long i;

    i = insertobject(0);

    object[i].owner = owner;
    object[i].picnum = picnum;
    object[i].x = self[screenpeek].posx;
    object[i].y = self[screenpeek].posy;
    object[i].z = self[screenpeek].posz;
    // map.h2[((self[screenpeek].posx>>10)<<8) + (self[screenpeek].posy>>10)]<<12;
    // self[screenpeek].posz;
    object[i].templong = 0;

    switch(picnum)
    {
        case 1:
            object[i].ang = self[screenpeek].ang;
            object[i].xvel =
                sintable[(2560+self[screenpeek].ang)&2047]>>1;
            object[i].yvel =
                sintable[(2048+self[screenpeek].ang)&2047]>>1;
            object[i].zvel = (self[screenpeek].horiz - 200)<<8;

            break;
    }

    return(i);
}

initaboard()
{
    long i, j;

    randomseed = 17L;
    screenpeek = 0;

    for(i=0;i<MAXPLAYERS;i++)
    {
        self[i].posx = 0;
        self[i].posy = 0;
        self[i].posz = ((128-32)<<12);
        self[i].ang = 0;
        self[i].horiz = (MAXYDIM>>1);
    }

    for(i=0;i<256;i++)
        for(j=0;j<256;j++)
        {
            if(j < 2 || j > 253 || i < 2 || i > 253)
            {
                map.h1[(i<<8)+j] = 0;
                map.h2[(i<<8)+j] = 0;
            }
            else
            {
                map.h1[(i<<8)+j] = (i>128)<<7;
                map.h2[(i<<8)+j] = 128;
            }

            map.c1[(i<<8)+j] = 0;
            map.c2[(i<<8)+j] = 0;

        }

//    loadpcx("cave.pcx",&map.c1[0]);

    self[screenpeek].posx = 128<<10;
    self[screenpeek].posy = 128<<10;
    self[screenpeek].posz = 128<<12;

    initobjectlists();
}




// Memory Management

char freemem(char **addr)
{
    long i;

    for(i=0;i<maxchunks;i++)
        if(*chunk[i].ptr == *addr)
    {
        chunk[i].amount = 0;
        *chunk[i].ptr = (char *) NULL;
        if(i == chunkloc)
            chunkloc = (chunkloc - 1)%maxchunks;
        return 1;
    }

    return 0;
}

char *getmem(char **addr,long memlen)
{
    long i, templong;

    // Check to see if the ptr already exists
    for(i=0;i<maxchunks;i++)
        if( i != chunkloc &&
            chunk[i].ptr == addr &&
            chunk[i].amount <= memlen )
    {
        chunk[i].amount = memlen;
        return(*addr);
    }

    // Set the last pointer to NULL
    *chunk[chunkloc].ptr = (char *) NULL;

    // Set the next chunk to addr
    chunk[chunkloc].ptr = addr;

    // At end of memory!?
    if( (memptr + memlen ) > totalmemory )
        memptr = 0;

    *addr = mainmem + memptr;
    chunk[chunkloc].amount = memlen;

    // NULLify all other chunks occupying same location
    for(i=0;i<maxchunks;i++)
        if( i != chunkloc &&
            chunk[i].amount > 0 &&
            *chunk[i].ptr >= (mainmem + memptr) &&
            *chunk[i].ptr < (mainmem + memptr + memlen) )
        {
            *chunk[i].ptr = (char *) NULL;
            chunk[i].amount = 0;
        }

    memptr += memlen;
    chunkloc = (chunkloc + 1)%maxchunks;

    return(mainmem + memptr - memlen);
}


// Inits/Uninits

setscreenmode()
{
    long i, j;

//    iodata("palette.dat",&palette[0],768L,IO_LOAD|IO_NOCLOSE);
//    iodata("palette.dat",&palookup[0][0],MAXPALOOKUPS<<8,IO_LOAD);

    setvmode(0x13);
    toutp(0x3c4,0x4); toutp(0x3c5,0x6);
    toutp(0x3d4,0x14); toutp(0x3d5,0x0);
    toutp(0x3d4,0x17); toutp(0x3d5,0xe3);
    if (ydim == 400)
    {
        toutp(0x3d4,0x9);
        toutp(0x3d5,inp(0x3d5)&254);
    }
  
    for(i=0;i<768;i+=3)
    {
        palette[i] = i/5;
        palette[i+1] = i/5;
        palette[i+2] = i/3;
    }

    for(i=0;i<MAXPALOOKUPS;i++)
        for(j=0;j<256;j++)
            palookup[i][j] = MAXPALOOKUPS - i - 1;

    toutp(0x3c8,0);
    for(i=0;i<768;i++)
        toutp(0x3c9,palette[i]);
}

void clearmemvars(void)
{
    long i;

    for(i=0;i<maxchunks;i++)
    {
        chunk[i].ptr = (char **) NULL;
        chunk[i].amount = 0;
    }

    memptr = 0;
    chunkloc = 0;
}

void initmem(void)
{
    totalmemory = MAXMEMNEEDED;
    do
    {
        mainmem = (char *) malloc(totalmemory + 1);
        if(mainmem == (char *) NULL)
            totalmemory -= (1<<16);

    } while(mainmem == (char *) NULL && totalmemory >= MINMEMNEEDED);

    if( totalmemory < MINMEMNEEDED )
    {
        puts("Not enough memory.");
        exit(-1);
    }

    chunk = (chunktype *)mainmem;
    totalmemory -= totalmemory>>10;
    mainmem += totalmemory>>10;
    maxchunks
        = ( (totalmemory >> 10) / sizeof(chunktype) ) - 1;

    printf("Alloc:%ld(%ld chunks) @ 0x%X.\n",totalmemory,maxchunks,mainmem);
    clearmemvars();
}

void initgame(void)
{
    int i, fil;

    pixs = 4;
    vidmode = 1;
    xdim = MAXXDIM;
    ydim = MAXYDIM;
    frameplace = 0xa0000;
    detmode = 1;
    iofil = -1;

    for(i=0;i<MAXYDIM;i++)
        muly80[i] = i*80;

    initmem();

    iodata("tables.dat",(char *)&sintable[0],4096L,IO_LOAD|IO_NOCLOSE);
    iodata("tables.dat",(char *)&tantable[0],4096L,IO_LOAD|IO_NOCLOSE);
    iodata("tables.dat",(char *)&radarang[0],640L,IO_LOAD);
            
    setupmouse();
    if(moustat == 1)
        puts("Mouse installed.");
    else
        puts("Mouse not found.");

    oldkeyhandler = _dos_getvect(0x9);
    _disable(); _dos_setvect(0x9, keyhandler); _enable();
    clockspeed = 0L;
    totalclock = 0L;
    numframes = 0L;
    toutp(0x43,54); toutp(0x40,4972&255); toutp(0x40,4972>>8);
    oldtimerhandler = _dos_getvect(0x8);
    _disable(); _dos_setvect(0x8, timerhandler); _enable();

    getmem(&map.h1,65536L*6);
    map.h2 = map.h1 + 65536L;
    map.c1 = map.h2 + 65536L;
    map.c2 = map.c1 + 65536L;
    map.s1 = map.c2 + 65536L;
    map.s2 = map.s1 + 65536L;

    getmem(&scrbuf,128000L);

    setscreenmode();
}

void uninitgame(void)
{
    int i;

     toutp(0x43,54); toutp(0x40,255); toutp(0x40,255);
    _dos_setvect(0x8, oldtimerhandler);
    _dos_setvect(0x9, oldkeyhandler);

    setvmode(0x3);

    free(mainmem);
}

void gameexit(char *message)
{
    uninitgame();
    puts(message);
    exit(0);
}


// Sounds

void playsound(long i,long sndnum)
{
    long j;
    char pri;

    pri = sounds[sndnum].pri;
    j = 0;
    while(soundplaying[j].ptr != NULL && j < MAXCHANNELS)
        j++;

    if(j == MAXCHANNELS)
        for(j=0;j<MAXCHANNELS;j++)
            if(sounds[soundplaying[j].sndno].pri <= pri)
                break;

    if(j == MAXCHANNELS)
        return;

    if(soundplaying[j].ptr == (char *) NULL)
    {
        getmem(&soundplaying[j].ptr,sounds[sndnum].len);
        iodata(sounds[sndnum].fn,soundplaying[j].ptr,sounds[sndnum].len,IO_LOAD);
    }

    soundplaying[j].sndno = sndnum;
    soundplaying[j].i = i;
}

void pan3dsounds(long i)
{
    long j;

    for(j=0;j<MAXCHANNELS;j++)
        if(soundplaying[j].ptr != (char *) NULL)
    {
        // Pan it here to current player;
    }
}


// Drawing Code

void shiftpalette(char r,char g,char b,long e)
{
    int i;
    char temparray[768];

    toutp(0x3c8,0);

    for(i=0;i<768;i+=3)
    {
        temparray[i+0] =
            palette[i+0]+((((long)r-(long)palette[i+0])*(long)(e&127))>>6);
        temparray[i+1] =
            palette[i+1]+((((long)g-(long)palette[i+1])*(long)(e&127))>>6);
        temparray[i+2] =
            palette[i+2]+((((long)b-(long)palette[i+2])*(long)(e&127))>>6);

        toutp(0x3c9,temparray[i+0]);
        toutp(0x3c9,temparray[i+1]);
        toutp(0x3c9,temparray[i+2]);
    }
}

void fadepalette(char dir,char fspeed)
{
    long i;
    if(dir)
        for(i = 0;i < 63;i += fspeed)
    {
        shiftpalette(0,0,0,i);
        limitrate();
    }
    else
        for(i = 0;i < 63;i += fspeed)
    {
        shiftpalette(0,0,0,63-i);
        limitrate();
    }
}

void light(long x, long y, long z, long myang, long deg, long rad)
{
    long dadist, daplc, daang, dasin, dacos, truex, truey, dashade, anginc;
    unsigned char *mapptr, *didits1, *didits2;

    getmem(&didits1,65536L);
    getmem(&didits2,65536L);

    clearbuf(didits1,16384);
    clearbuf(didits2,16384);

    z >>= 12;

    anginc = 1 + (rad>>4);

    for(daang=myang-deg;daang < myang+deg; daang += anginc )
    {
        dacos = sintable[(daang+512)&2047]>>4;
        dasin = sintable[(daang)&2047]>>4;

        truex = x;
        truey = y;

        for(dadist = 1;dadist < rad; dadist++)
        {
            daplc = ((truex>>10)<<8)+(truey>>10);
            mapptr = &map.h1[daplc];

            if(didits1[daplc] == 0)
            {
                didits1[daplc] = 1;
                dashade = scale(63, dadist + (labs(z - *mapptr)>>1), rad);
                mapptr += (65536L<<2);

                if(dashade < 0) *mapptr = 0;
                else if(dashade < *mapptr) *mapptr = dashade;
                if( z < *(mapptr-(65536L<<2)) ) break;
                mapptr -= (65536L*3);
            }
            else mapptr += 65536L;

            if(didits2[daplc] == 0)
            {
                didits2[daplc] = 1;
                dashade = scale(63, dadist + (labs(z - *mapptr)>>1), rad);
                mapptr += (65536L<<2);

                if(dashade < 0) *mapptr = 0;
                else if(dashade < *mapptr) *mapptr = dashade;
                if( z > *(mapptr-(65536L<<2)) ) break;
            }

            truex += dacos;
            truey += dasin;
        }
    }

    freemem(&didits2);
    freemem(&didits1);

}

void blast(long gridx, long gridy, long rad, char blastingcol)
{
    short tempshort;
    long i, j, dax, day, daz, dasqr, templong;

    templong = rad+2;
    for(i=-templong;i<=templong;i++)
        for(j=-templong;j<=templong;j++)
        {
            dax = ((gridx+i+256)&255);
            day = ((gridy+j+256)&255);

            if(dax < 1 || dax > 255 || day < 1 || day > 255)
                continue;

            dasqr = sqr(rad)-(sqr(i)+sqr(j));

            if (dasqr >= 0)
                daz = tsqrt(dasqr)<<1;
            else
                daz = -tsqrt(-dasqr)<<1;

            if ((self[screenpeek].posz>>12)-daz < map.h1[(dax<<8)+day])
            {
                map.h1[(dax<<8)+day] = (self[screenpeek].posz>>12)-daz;
                if (((self[screenpeek].posz>>12)-daz) < 0)
                    map.h1[(dax<<8)+day] = 0;
            }

            if ((self[screenpeek].posz>>12)+daz > map.h2[(dax<<8)+day])
            {
                map.h2[(dax<<8)+day] = (self[screenpeek].posz>>12)+daz;
                if (((self[screenpeek].posz>>12)+daz) > 255)
                    map.h2[(dax<<8)+day] = 255;
            }

/*            tempshort = map.h1[(dax<<8)+day];
            if (tempshort >= map.h2[(dax<<8)+day]) tempshort = (self[screenpeek].posz>>12);
            tempshort = labs(64-(tempshort&127))+(rand()&3)-2;
            if (tempshort < 0) tempshort = 0;
            if (tempshort > 63) tempshort = 63;
            map.c1[(dax<<8)+day] = (char)(tempshort+blastingcol);

            tempshort = map.h2[(dax<<8)+day];
            if (tempshort <= map.h1[(dax<<8)+day]) tempshort = (self[screenpeek].posz>>12);
            tempshort = labs(64-(tempshort&127))+(rand()&3)-2;
            if (tempshort < 0) tempshort = 0;
            if (tempshort > 63) tempshort = 63;
            map.c2[(dax<<8)+day] = (char)(tempshort+blastingcol);
*/

        }
}

void grouvline (long x)
{
    long dist[2], dinc[2], incr[2];
    short grid[2], dir[2];
    unsigned char *mapptr;

    char oh1, oh2;
    short um, dm, h, i, horiz;
    long plc1, plc2, cosval, sinval;
    long snx, sny, dax, c, shade, cnt;
    long posx, posy, posz, posa;

    plc1 = (x>>2) + FP_OFF(scrbuf);
    plc2 = plc1 + muly80[ (ydim-1) ];

    switch(pixs)
    {
        case 1:
            if ((x&2) > 0)
            {
                plc1 += 32000<<vidmode;
                plc2 += 32000<<vidmode;
            }
            if ((x&1) > 0)
            {
                plc1 += 16000<<vidmode;
                plc2 += 16000<<vidmode;
            }
            break;
        case 2:
            if ((x&2) > 0)
            {
                plc1 += 16000<<vidmode;
                plc2 += 16000<<vidmode;
            }
            break;
    }

    posx = self[screenpeek].posx;
    posy = self[screenpeek].posy;
    posz = self[screenpeek].posz;
    posa = self[screenpeek].ang;
    horiz = self[screenpeek].horiz;

    cosval = sintable[(posa+2560)&2047];
    sinval = sintable[(posa+2048)&2047];

    dax = (x<<1) + (pixs-1) - xdim;

    incr[0] = cosval - scale(sinval,dax,xdim);
    incr[1] = sinval + scale(cosval,dax,xdim);

    if (incr[0] < 0) dir[0] = -1, incr[0] = -incr[0]; else dir[0] = 1;
    if (incr[1] < 0) dir[1] = -1, incr[1] = -incr[1]; else dir[1] = 1;
    snx = (posx&1023); if (dir[0] == 1) snx ^= 1023;
    sny = (posy&1023); if (dir[1] == 1) sny ^= 1023;
    cnt = ((snx*incr[1] - sny*incr[0])>>10);

    grid[0] = ((posx>>10)&255); grid[1] = ((posy>>10)&255);

    if (incr[0] != 0)
    {
        dinc[0] = divscale(65536>>vidmode,incr[0],12);
        dist[0] = mulscale(dinc[0],snx,10);
    }
    if (incr[1] != 0)
    {
        dinc[1] = divscale(65536>>vidmode,incr[1],12);
        dist[1] = mulscale(dinc[1],sny,10);
    }

    um = (0)-horiz;
    dm = (ydim-1)-horiz;

    i = incr[0]; incr[0] = incr[1]; incr[1] = -i;

    shade = 0;
    while (dist[cnt>=0] < 8192)
    {
        i = (cnt>=0);

        grid[i] = ((grid[i]+dir[i])&255);
        dist[i] += dinc[i];
        cnt += incr[i];
        shade++;
    }

    mapptr = &map.h1[(grid[0]<<8)+grid[1]];

    while (shade < 256)
    {
        i = (cnt>=0);
        oh1 = *mapptr;
        oh2 = *(mapptr+65536);

        h = groudiv((long)oh1,dist[i],posz);
        if (um <= h)
        {
            c = (shade>>2) + *(mapptr + (65536<<2)); // map.s1[bufplc];
            if(c > 63) c = 63;
            c = palookup[c][*(mapptr + (65536<<1))]; // map.c1[bufplc]
            if (h > dm) break;
            plc1 = drawtopslab(plc1,h-um+1,c);
            um = h+1;
        }

        h = groudiv((long)oh2,dist[i],posz);
        if (dm >= h)
        {
            c = (shade>>2) + *(mapptr + (65536*5)); // map.s2[bufplc];
            if(c > 63) c = 63;
            c = palookup[c][*(mapptr + (65536*3))]; // map.c2[bufplc]
            if (h < um) break;
            plc2 = drawbotslab(plc2,dm-h+1,c);
            dm = h-1;
        }

        grid[i] = ((grid[i]+dir[i])&255);
        mapptr = &map.h1[(grid[0]<<8)+grid[1]];

        if (*mapptr > oh1) // map.h1[bufplc]
        {
            h = groudiv((long)*mapptr,dist[i],posz); // map.h1[bufplc]

            if (um <= h)
            {
                c = (shade>>2) + *(mapptr + (65536<<2)); // map.s1[bufplc];
                if(c > 63) c = 63;
                c = palookup[c][*(mapptr + (65536<<1))]; // map.c1[bufplc]
                if (h > dm) break;
                plc1 = drawtopslab(plc1,h-um+1,c);
                um = h+1;
            }
        }


        if (*(mapptr + 65536) < oh2) // map.h2[bufplc]
        {
            h = groudiv((long)*(mapptr + 65536),dist[i],posz); // map.h2[bufplc]
            if (dm >= h)
            {
                c = (shade>>2) + *(mapptr + (65536*5)); // map.s2[bufplc];
                if(c > 63) c = 63;
                c = palookup[c][*(mapptr + (65536*3))]; // map.c2[bufplc]
                if (h < um) break;
                plc2 = drawbotslab(plc2,dm-h+1,c);
                dm = h-1;
            }
        }

        dist[i] += dinc[i];
        cnt += incr[i];
        shade++;
    }

    if (dm >= um)
    {
        if (shade >= 256) c = 0;
        drawtopslab(plc1,dm-um+1,c);
    }
}


void drawobject(long thex, long they, long thez, char *tileptr,char shade)
{
    long x, y, x1, y1, x2, y2, dasizx, dasizy, xscale, yscale;
    char *p;

    dasizx = *(tileptr++);
    dasizy = *(tileptr++);

	x1 = thex;
	y1 = they;
    x2 = x1 + dasizx;
    y2 = y1 + dasizy;

    if ((x2 >= xdim) || (x2 < 0) || (x1 >= xdim) || (y2 < 0) || (y1 >= ydim))
        return;

    if(thez > (65536L<<1)) return;
//    else if(thez > 256)
//    {
  //      xscale = 1 + mulscale(dasizx,thez,18);
    //    yscale = 1 + mulscale(dasizy,thez,18);
//    }
//    else

    xscale = yscale = 1;

    p = scrbuf + muly80[y1];

    switch(pixs)
    {
        case 1:
            for(y=y1;y<y2;y++)
            {
                for(x=x1;x<x2;x+=xscale)
                {
                    if(*tileptr < 255)
                        *(p + (mul16000[(x&3)]<<vidmode) + (x>>2)) =
                            *tileptr;
                    tileptr += xscale;
                }
                p += 80;
            }
            break;
        case 2:
            for(y=y1;y<y2;y++)
            {
                for(x=x1;x<x2;x+=(xscale<<1))
                {
                    if(*tileptr < 255)
                        *(p + (mul16000[(x>>1)&1]<<vidmode) + (x>>2)) =
                            *tileptr;
                    tileptr += (xscale<<1);
                }
                tileptr += (x-x2)*(xscale<<1);
                p += 80;
            }
            break;

        case 4:
            for(y=y1;y<y2;y++)
            {
                for(x=x1;x<x2;x+=(xscale<<2))
                {
                    if(*tileptr < 255)
                        *(p + (x>>2) ) = palookup[shade][*tileptr];
                    tileptr += (xscale<<2);
                }
                tileptr += (x-x2)*(xscale<<2);
                p += 80;
            }
            break;
    }
}

void sortobjectstoview(void)
{
    long tempx, tempy, dadist, posx, posy;
    short i, j, daang, daangdif;
    viewobjecttype tempviewobject;

    daang = self[screenpeek].ang;
    posx = self[screenpeek].posx;
    posy = self[screenpeek].posy;

    viewobjectcnt = 0;

    for(j=0;j<MAXSTATUS;j++)
        for(i = map.headobject[j];i>=0;i=map.nextobject[i])
    {
        daangdif = angdif(daang,getangle(object[i].x-posx,object[i].y-posy));
        if( labs(daangdif) < (256) )
        {
            dadist = mulscale(object[i].x-posx,\
                sintable[(-daang+2560)&2047],14)-mulscale(object[i].y-posy,\
                sintable[(-daang+2048)&2047],14);

            if( dadist > 0xff ) // && dadist < 0x18000)
            {
                scrnobj[viewobjectcnt].viewobject = i;
                scrnobj[viewobjectcnt].viewobjectdist = dadist;
                scrnobj[viewobjectcnt].viewobjectang = daangdif;
                viewobjectcnt++;
            }
        }
    }

    // Bubble sort by z (furthest to closest)

    for(i=0;i<(viewobjectcnt-1);i++)
        for(j = (i+1);j<viewobjectcnt; j++)
            if(scrnobj[j].viewobjectdist > scrnobj[i].viewobjectdist)
        {
            copybytes(&scrnobj[i],     &tempviewobject, sizeof(viewobjecttype));
            copybytes(&scrnobj[j],     &scrnobj[i],     sizeof(viewobjecttype));
            copybytes(&tempviewobject, &scrnobj[j],     sizeof(viewobjecttype));
        }
}

void animateobjects(void)
{
    long i, j;

    if(self[screenpeek].flashlight)
        light(
            self[screenpeek].posx,
            self[screenpeek].posy,
            self[screenpeek].posz,
            self[screenpeek].ang,
            128,
            96);

    for(i=0;i<viewobjectcnt;i++)
    {
        j = scrnobj[i].viewobject;
        light(
           object[j].x,
           object[j].y,
           object[j].z,
           0,
           1024,
           96);
    }
}

void drawallobjects(void)
{
    long i, j, posx, posy, posz, horiz;
    long scrnx, scrny;

    posx = self[screenpeek].posx;
    posy = self[screenpeek].posy;
    posz = self[screenpeek].posz;
    horiz = self[screenpeek].horiz;

    for(j=0;j<viewobjectcnt;j++)
    {
        i = scrnobj[j].viewobject;

        scrnx = mulscale(MAXXDIM,256+scrnobj[j].viewobjectang,9);
        scrny = scale( object[i].z-posz, 32, scrnobj[j].viewobjectdist);

        drawobject(scrnx,horiz + scrny,scrnobj[j].viewobjectdist,atile,scrnobj[j].viewobjectdist>>10);
    }
}


void drawmap(void)
{
    long i, j;

    if(self[screenpeek].topdown == 0)
    {
        for(i=0;i<MAXXDIM;i+=pixs)
            grouvline(i);

        drawallobjects();
    }
    else
        clearbuf(scrbuf,16000,0xffffffff);

}

void nextpage(void)
{
    if (vidmode == 0)                            //Copy to screen
    {
        if (pixs == 4) showscreen4pix320200();
        if (pixs == 2) showscreen2pix320200();
        if (pixs == 1) showscreen1pix320200();
    }
    else
    {
        if (pixs == 4) showscreen4pix320400();
        if (pixs == 2) showscreen2pix320400();
        if (pixs == 1) showscreen1pix320400();
    }

    toutp(0x3d4,0xc); toutp(0x3d5,(frameplace&65535)>>8);  //Nextpage
    if (vidmode == 0)
    {
        frameplace += 16384;
        if (frameplace >= 0xb0000) frameplace = 0xa0000;
    }
    else
    {
        frameplace += 32768;
        if (frameplace > 0xa8000) frameplace = 0xa0000;
    }
}

void drawscreen(void)
{
    sortobjectstoview();
    animateobjects();
    drawmap();                                   //Draw to non-video memory
    nextpage();
}


// Object Code

void initobjectlists(void)
{
	long i;

    for(i=0;i<MAXSTATUS;i++)
        map.headobject[i] = -1;
    map.headobject[MAXSTATUS] = 0;
    for(i=0;i<MAXOBJECTS;i++)
	{
        map.prevobject[i] = i-1;
        map.nextobject[i] = i+1;
        map.objectstat[i] = MAXSTATUS;
	}
    map.prevobject[0] = -1;
    map.nextobject[MAXOBJECTS-1] = -1;
}

insertobject(short statnum)
{
	short blanktouse;

    if ((statnum >= MAXSTATUS) || (map.headobject[MAXSTATUS] == -1))
		return(-1);  //list full

    blanktouse = map.headobject[MAXSTATUS];

    map.headobject[MAXSTATUS] = map.nextobject[blanktouse];
    if (map.headobject[MAXSTATUS] >= 0)
        map.prevobject[map.headobject[MAXSTATUS]] = -1;

    map.prevobject[blanktouse] = -1;
    map.nextobject[blanktouse] = map.headobject[statnum];
    if (map.headobject[statnum] >= 0)
        map.prevobject[map.headobject[statnum]] = blanktouse;
    map.headobject[statnum] = blanktouse;

    map.objectstat[blanktouse] = statnum;

	return(blanktouse);
}

deleteobject (short deleteme)
{
    if (map.objectstat[deleteme] == MAXSTATUS)
		return(-1);

    if (map.headobject[map.objectstat[deleteme]] == deleteme)
        map.headobject[map.objectstat[deleteme]] = map.nextobject[deleteme];

    if (map.prevobject[deleteme] >= 0) map.nextobject[map.prevobject[deleteme]] = map.nextobject[deleteme];
    if (map.nextobject[deleteme] >= 0) map.prevobject[map.nextobject[deleteme]] = map.prevobject[deleteme];

    if (map.headobject[MAXSTATUS] >= 0) map.prevobject[map.headobject[MAXSTATUS]] = deleteme;
    map.prevobject[deleteme] = -1;
    map.nextobject[deleteme] = map.headobject[MAXSTATUS];
    map.headobject[MAXSTATUS] = deleteme;

    map.objectstat[deleteme] = MAXSTATUS;
	return(0);
}

long clipmove(long *x, long *y, long *z, long vx, long vy, long vz, unsigned long rad)
{
    long hiz, loz, dax, day, daz, retcode;
    char charx, chary;

    retcode = 0;

    if( vx|vy|vz )
    {
        dax = *x;
        day = *y;
        daz = *z;

        dax += vx;
        day += vy;
        daz += vz;

        charx = (dax>>10)&255;
        chary = (day>>10)&255;

        hiz = map.h1[ (charx << 8) + chary ]<<12;
        loz = map.h2[ (charx << 8) + chary ]<<12;

        if( ( hiz + rad ) > daz )
        {
            daz = ( hiz + rad );
            retcode = CLIP_HITCEILING|CLIP_HALT;
        }

        if( ( loz - rad ) < daz )
        {
            daz = ( loz - rad );
            retcode |= CLIP_HITFLOOR;
        }

        dax &= 0x3ffffff;
        day &= 0x3ffffff;

        if( (retcode&CLIP_HALT) != CLIP_HALT )
        {
            *x = dax;
            *y = day;
        }

        *z += (daz-(*z))>>2;

    }

    return(retcode);

}

void loadboard(char *fn)
{
    iodata(fn,(char *)&map,sizeof(map),IO_LOAD|IO_NOCLOSE);
    iodata(fn,(char *)&self[0],sizeof(self),IO_LOAD|IO_NOCLOSE);
    iodata(fn,(char *)&object[0],sizeof(object),IO_LOAD|IO_NOCLOSE);

    iodata(fn,&map.h1[0],65536L,IO_LOAD|IO_NOCLOSE);
    iodata(fn,&map.c1[0],65536L,IO_LOAD|IO_NOCLOSE);
    iodata(fn,&map.h2[0],65536L,IO_LOAD|IO_NOCLOSE);
    iodata(fn,&map.c2[0],65536L,IO_LOAD);
}

void saveboard(char *fn)
{
    iodata(fn,(char *)&map,sizeof(map),IO_SAVE|IO_NOCLOSE);
    iodata(fn,(char *)&self[0],sizeof(self),IO_SAVE|IO_NOCLOSE);
    iodata(fn,(char *)&object[0],sizeof(object),IO_SAVE|IO_NOCLOSE);

    iodata(fn,&map.h1[0],65536L,IO_SAVE|IO_NOCLOSE);
    iodata(fn,&map.c1[0],65536L,IO_SAVE|IO_NOCLOSE);
    iodata(fn,&map.h2[0],65536L,IO_SAVE|IO_NOCLOSE);
    iodata(fn,&map.c2[0],65536L,IO_SAVE);
}


// Player Code

void processinput(short snum)
{
    long i, xvel, yvel, clipval;

        if(keystatus[sc_F2] > 0)
            saveboard("level_01.map");

        if(keystatus[sc_F3] > 0)
            loadboard("level_01.map");

        if( keystatus[sc_F4] > 0)
            initaboard();

        if( keystatus[sc_P] > 0)
        {
            keystatus[sc_P] = 0;
            while( keystatus[sc_P] == 0);
            keystatus[sc_P] = 0;
        }

        if (keystatus[sc_LeftControl] > 0)
            blast(((self[snum].posx>>10)&255),((self[snum].posy>>10)&255),16,trand()&255);
    
        self[snum].vel = 0L;
        self[snum].svel = 0L;
        self[snum].angvel = 0;
        if (moustat == 1)
        {
            readmouse();
            self[snum].ang += mousx;
            self[snum].vel = (((long)-mousy)<<1);
        }

        if(keystatus[sc_Space] > 0)
        {
            spawn(1, -1);
            keystatus[sc_Space] = 0;
        }

        if (keystatus[sc_kpad_1] > 0)
            self[snum].horiz += 16L;
        if (keystatus[sc_kpad_0] > 0)
            self[snum].horiz -= 16L;

        if ( keystatus[sc_A] > 0 )
            self[snum].posz -= (16384<<1);

//            self[snum].zvel = -(4096*clockspeed>>1);
        if ( keystatus[sc_Z] > 0 )
            self[snum].zvel += (1024*clockspeed>>1);

        if ( keystatus[sc_LeftAlt] == 0 )
        {
            if (keystatus[sc_RightArrow] > 0)
                self[snum].angvel += 32;
            if (keystatus[sc_LeftArrow] > 0)
                self[snum].angvel -= 32;
        }
        else
        {
            if (keystatus[sc_RightArrow] > 0) self[snum].svel = -12L;
            if (keystatus[sc_LeftArrow] > 0) self[snum].svel = 12L;
        }

        if (keystatus[sc_Comma] > 0) self[snum].svel = 12L;
        if (keystatus[sc_Period] > 0) self[snum].svel = -12L;

        if (keystatus[sc_DownArrow] > 0) self[snum].vel = -12L;
        if (keystatus[sc_UpArrow] > 0) self[snum].vel = 12L;

        if(keystatus[sc_F] > 0)
        {
            self[snum].flashlight = 1-self[snum].flashlight;
            keystatus[sc_F] = 0;
        }


        if(keystatus[sc_CapsLock] > 0)
        {
            self[snum].runmode = 1-self[snum].runmode;
            keystatus[sc_CapsLock] = 0;
        }

        if(keystatus[sc_Tab] > 0)
        {
            self[snum].topdown = 1-self[snum].topdown;
            keystatus[sc_Tab] = 0;
        }

        if (keystatus[sc_LeftShift] > 0 || self[snum].runmode )
        {
            self[snum].vel <<= 1;
            self[snum].svel <<= 1;
            self[snum].angvel <<= 1;
        }

        if (self[snum].angvel != 0)
        {
            self[snum].ang += (self[snum].angvel*clockspeed)>>3;
            self[snum].ang = (self[snum].ang+2048)&2047;
        }

        xvel =
            ((self[snum].vel*clockspeed*sintable[(2560+self[snum].ang)&2047])>>12)+
            ((self[snum].svel*clockspeed*sintable[(2048+self[snum].ang)&2047])>>12);
        yvel =
            ((self[snum].vel*clockspeed*sintable[(2048+self[snum].ang)&2047])>>12)-
            ((self[snum].svel*clockspeed*sintable[(2560+self[snum].ang)&2047])>>12);

        clipval = clipmove(
            &self[snum].posx,
            &self[snum].posy,
            &self[snum].posz,
            xvel,
            yvel,
            self[snum].zvel,
            2048<<5);

          if( (clipval&CLIP_HITFLOOR) != CLIP_HITFLOOR )
          {
              if(self[snum].zvel < 0x10000)
                  self[snum].zvel += clockspeed<<9;
          }
          else
              self[snum].zvel = 0;


        if (detmode == 0)
        {
            if ((self[snum].vel|self[snum].svel|self[snum].angvel) == 0)
                pixs = 1;
            else
                pixs = 2;
        }
        if (keystatus[0x10] > 0) keystatus[0x10] = 0, pixs = 1;
        if (keystatus[0x11] > 0) keystatus[0x11] = 0, pixs = 2;
        if (keystatus[0x12] > 0) keystatus[0x12] = 0, pixs = 4;
        if (keystatus[0x13] > 0) keystatus[0x13] = 0, detmode = 1-detmode;
        if ((keystatus[0x1f]|keystatus[0x20]) > 0)
        {
            if (keystatus[0x1f] > 0)
            {
                if (vidmode == 0)
                {
                    frameplace = 0xa0000;
                    vidmode = 1;
                    toutp(0x3d4,0x9); toutp(0x3d5,inp(0x3d5)&254);
                    keystatus[0x1f] = 0;
                    ydim = 400L;
                    self[snum].horiz <<= 1;
                }
            }
            if (keystatus[0x20] > 0)
            {
                if (vidmode == 1)
                {
                    vidmode = 0;
                    toutp(0x3d4,0x9); toutp(0x3d5,inp(0x3d5)|1);
                    keystatus[0x20] = 0;
                    ydim = 200L;
                    self[snum].horiz >>= 1;
                }
            }
        }

        numframes++;
        totalclock += clockspeed;
        clockspeed = 0L;
}


// Object Code

char thinkobject(long i)
{
    long j;
    objecttype *o;
    char killit_flag;

    o = &object[i];
    
    killit_flag = 0;
    switch(o->picnum)
    {
        case 1:
            j = ((o->x>>10)<<8)+(o->y>>10);
            if(map.h2[j] == map.h1[j] )
            {
                killit_flag = 1;
                break;
            }
            object[i].templong++;
            if(object[i].templong > 10)
                killit_flag = 1;
            break;
    }

    if(killit_flag) deleteobject(i);

    return(killit_flag);
}

void processobjects(void)
{
    long i, j, nexti;

    for(j=0;j<MAXSTATUS;j++)
    {
        i = map.headobject[j];
        while(i >= 0)
        {
            nexti = map.nextobject[i];
            if( thinkobject(i) == 0 )
                clipmove(
                    &object[i].x,
                    &object[i].y,
                    &object[i].z,
                    object[i].xvel,
                    object[i].yvel,
                    object[i].zvel,
                    0);
            i = nexti;
        }
    }
}



// Main Game Loop

void gameloop(void)
{

    initaboard();
    drawscreen();
    fadepalette(0,4);

    while (keystatus[sc_Escape] == 0)
    {
        clearbuf(&map.s1[0],65536L>>1,0x1f1f1f1f);

        processobjects();
        processinput(screenpeek);
        drawscreen();
        pan3dsounds(self[screenpeek].i);
    }

    fadepalette(1,4);
}

void main(int argc, char *argv)
{
    initgame();
    gameloop();
    gameexit("Thank you for playing!");
}

// "Duke 2000"
// "Virchua Duke"
// "Son of Death
// "Cromium"
// "Potent"
// "Flotsom"

// Volume One
// "Duke is brain dead",
// "BOOT TO THE HEAD"
// Damage too duke
// Weapons are computer cont.  Only logical thinking
// is disappearing.
// " Flips! "
// Flash on screen, inst.
// "BUMS"
// "JAIL"/"MENTAL WARD (Cop code for looney?  T. asks Cop.)"
// "GUTS OR GLORY"

// ( Duke's Mission

// Duke:    "Looks like some kind of transporter...?"
// Byte:    "...YES"

// Duke:    "Waa, here goes nuthin'. "
// (Duke puts r. arm in device)

// Duke:    AAAAHHHHHHHHHHHHHHHHHHHHHHHHH!!!
// (Duke's arm is seved.)
// Byte:    NO.NO.NO.NO.NO.NO.NO...
// ( Byte directs duke to the nearest heat source)
// (Shut Up Mode)
// ( Duke Staggers, end of arm bleeding, usual oozing arm guts. )
// Byte: Left, Left, Left, Left, Right.
// ( Duke, loozing consc, trips on broken pipe, )
// ( hits temple on edge of step. )
// ( Rats everywhere, byte pushing them away with weapon,
// ( eventually covered, show usual groosums, Duke appears dead
// ( Duke wakes up, in hospital, vision less blurry
// ( Hospital doing brain scan, 1/3 cran. mass MISSING!
// Doc: Hummm?  ( Grabbing upper lip to "appear" smart. )

// Stand back boys

// Schrapnel has busted my scull!
// Now I'nsane, Mental ward, got to escape.
// Search light everywhere.

// (M)Mendor, The Tree Dweller.
// (M)BashMan, The Destructor.
// (M)Lash, The Scavenger.
// (F)Mag, The Slut.
// (F)
// NRA OR SOMETHIN'

// Duke Nukem
// 5th Dimention
// Pentagon Man!


// I Hope your not stupid!
// The 70's meet the future.
// Dirty Harry style.  70's music with futuristic edge
// The Instant De-Welder(tm)
// I think I'm going to puke...
// Badge attitude.
// He's got a Badge, a Bulldog, a Bronco (beat up/bondoed).
// Gfx:
// Lite rail systems
// A church.  Large cross
// Sniper Scope,
// Really use the phone
// The Boiler Room
// The IRS, nuking other government buildings?
// You wouldn't have a belt of booz, would ya?
// Slow turning signes
// More persise shooting/descructions
// Faces, use phoneoms and its lookup.  Talking, getting in fights.
// Drug dealers, pimps, and all galore
// Weapons, Anything lying around.
// Trees to clime, burning trees.
// Sledge Hammer, Sledge hammer with Spike
// sancurary, get away from it all.
// Goodlife = ( War + Greed ) / Peace
// MonsterisM           (ACTION)
//    An exper
// Global Hunter        (RPG)
// Slick a Wick         (PUZZLE)
// Roach Condo          (FUNNY)
// AntiProfit           (RPG)
// Pen Patrol           (TD SIM)
// 97.5 KPIG! - Wanker County
// "Fauna" - Native Indiginouns Animal Life
// Red Mercury


