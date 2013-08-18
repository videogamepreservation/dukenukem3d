// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include <direct.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <conio.h>

#define MAXTILES 9216
#define MAXMENUFILES 256
#define MAXTILEFILES 256
#define NUMPALOOKUPS 32
#define GIFBUFLEN 4096
#define MAXMAPS 64

static struct sectortype
{
	short wallptr, wallnum;
	long ceilingz, floorz;
	short ceilingstat, floorstat;
	short ceilingpicnum, ceilingheinum;
	signed char ceilingshade;
	char ceilingpal, ceilingxpanning, ceilingypanning;
	short floorpicnum, floorheinum;
	signed char floorshade;
	char floorpal, floorxpanning, floorypanning;
	char visibility, filler;
	short lotag, hitag, extra;
} mapsector;

static struct walltype
{
	long x, y;
	short point2, nextwall, nextsector, cstat;
	short picnum, overpicnum;
	signed char shade;
	char pal, xrepeat, yrepeat, xpanning, ypanning;
	short lotag, hitag, extra;
} mapwall;

static struct spritetype
{
	long x, y, z;
	short cstat, picnum;
	signed char shade;
	char pal, clipdist, filler;
	unsigned char xrepeat, yrepeat;
	signed char xoffset, yoffset;
	short sectnum, statnum;
	short ang, owner, xvel, yvel, zvel;
	short lotag, hitag, extra;
} mapsprite;

short nummaps, numsprites[MAXMAPS];

static char *pic;
static short int mousx, mousy, bstatus, moustat;
static unsigned char col, trailstatus, tempbuf[4096];
static short xfillbuf[4096], yfillbuf[4096];
static unsigned char printbuf[128];
static short tilesizx[MAXTILES], tilesizy[MAXTILES];
static long picanm[MAXTILES], artversion, numtiles;
static long panx, pany, panxforu, panyforu, panyoff = 0, panxdim;
static long lastshowallxdim = 0x7fffffff, lastshowallydim = 0x7fffffff;
static unsigned char buf[1024*512], *buf2;
static long waloff[MAXTILES], totpicmem, xdim, ydim, asksave, c, d, picnum;
static long totpicsiz, animspeed, allasksave, mustsavelocals = 0;
static long needtosavenames = 0, gettilezoom = 1;
static short sintable[2048];
static char blackmasklookup[256];

static short tilookup[MAXTILES], tilookup2[MAXTILES], tilesreported = 0;

static char artfilename[20];
static long localtilestart[MAXTILEFILES], localtileend[MAXTILEFILES];
static long curtilefile, numtilefiles;

static long xres, yres;
static char pcxheader[128] =
{
	0xa,0x5,0x1,0x8,0x0,0x0,0x0,0x0,0x3f,0x1,0xc7,0x0,
	0x40,0x1,0xc8,0x0,0x0,0x0,0x0,0x8,0x8,0x8,0x10,0x10,
	0x10,0x18,0x18,0x18,0x20,0x20,0x20,0x28,0x28,0x28,0x30,0x30,
	0x30,0x38,0x38,0x38,0x40,0x40,0x40,0x48,0x48,0x48,0x50,0x50,
	0x50,0x58,0x58,0x58,0x60,0x60,0x60,0x68,0x68,0x68,0x70,0x70,
	0x70,0x78,0x78,0x78,0x0,0x1,0x40,0x1,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};

static long capfil = -1L;

static char names[MAXTILES][17];
static char menuname[MAXMENUFILES][17], curpath[160], menupath[160];
static long menunamecnt, menuhighlight;

static char textfont[128][8];
static char reversemask[16] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};

static char whitecol, browncol, yellowcol, greencol, blackcol;

static char palookup[NUMPALOOKUPS][256], palette[768];
static char palette2[768], palookupe[256], *transluc, translucloaded = 0;

static unsigned char gifdata[GIFBUFLEN];
static unsigned char gifsuffix[4100], giffilbuffer[768], giftempstack[4096];
static short int gifprefix[4100];

static long totalclock;
static void (__interrupt __far *oldtimerhandler)();
static void __interrupt __far timerhandler(void);

#pragma aux scale =\
	"imul ebx",\
	"idiv ecx",\
	parm [eax][ebx][ecx]\
	modify [eax edx]\

#pragma aux mulscale =\
	"imul ebx",\
	"shrd eax, edx, cl",\
	parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux boundmulscale =\
	"imul ebx",\
	"mov ebx, edx",\
	"shrd eax, edx, cl",\
	"sar edx, cl",\
	"xor edx, eax",\
	"js checkit",\
	"xor edx, eax",\
	"jz skipboundit",\
	"cmp edx, 0xffffffff",\
	"je skipboundit",\
	"checkit:",\
	"mov eax, ebx",\
	"sar eax, 31",\
	"add eax, 0x80000000",\
	"not eax",\
	"skipboundit:",\
	parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux divscale =\
	"cdq",\
	"shld edx, eax, cl",\
	"sal eax, cl",\
	"idiv ebx",\
	parm [eax][ebx][ecx]\
	modify [edx]\

#pragma aux setvmode =\
	"int 0x10",\
	parm [eax]\

#pragma aux clearbuf =\
	"rep stosd",\
	parm [edi][ecx][eax]\

#pragma aux cleartopbox =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0f02",\
	"out dx, ax",\
	"mov edi, 0xa1000",\
	"mov ecx, 15360",\
	"mov dl, blackcol",\
	"mov dh, dl",\
	"mov eax, edx",\
	"shl eax, 16",\
	"mov ax, dx",\
	"rep stosd",\
	modify [eax ecx edx edi]\

#pragma aux getpixel =\
	"xor eax, eax",\
	"mov al, [edi]",\
	parm [edi]\

#pragma aux drawchainpixel =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0102",\
	"mov cx, di",\
	"and cl, 3",\
	"shl ah, cl",\
	"out dx, ax",\
	"shr edi, 2",\
	"mov byte ptr [edi+0xa0000], bl",\
	parm [edi][ebx]\
	modify [eax ecx edx]\

#pragma aux drawpixel =\
	"mov [edi], al",\
	parm [edi][eax]\

#pragma aux drawpixels =\
	"mov [edi], ax",\
	parm [edi][eax]\

#pragma aux drawpixelses =\
	"mov [edi], eax",\
	parm [edi][eax]\

#pragma aux setupmouse =\
	"mov ax, 0",\
	"int 33h",\
	"mov moustat,1",\

#pragma aux readmouse =\
	"mov ax, 11d",\
	"int 33h",\
	"sar cx, 1",\
	"sar dx, 1",\
	"mov mousx, cx",\
	"mov mousy, dx",\
	"mov ax, 5d",\
	"int 33h",\
	"mov bstatus, ax",\
	modify [eax ebx ecx edx]\

#pragma aux limitrate =\
	"mov dx, 0x3da",\
	"wait1: in al, dx",\
	"test al, 8",\
	"jnz wait1",\
	"wait2: in al, dx",\
	"test al, 8",\
	"jz wait2",\
	modify [edx]\

#pragma aux cleartext =\
	"mov dx, 0x3c4",\
	"mov ax, 0x0f02",\
	"out dx, ax",\
	"mov edi, 0xa0800",\
	"mov ebx, 8",\
	"mov ah, browncol",\
	"mov al, ah",\
	"shl eax, 8",\
	"mov al, ah",\
	"shl eax, 8",\
	"mov al, ah",\
	"begclearit: mov ecx, 14",\
	"rep stosd",\
	"add edi, 200",\
	"sub ebx, 1",\
	"jnz begclearit",\
	modify [eax ebx ecx edx edi]\

	//the 200 above = 256-28chars*2

  //eax = (eax>>bitcnt)&((1<<numbits)-1);
#pragma aux gifgetdat =\
	"mov eax, dword ptr giffilbuffer[eax]",\
	"shr eax, cl",\
	"xchg ebx, ecx",\
	"mov ebx, 1",\
	"shl ebx, cl",\
	"dec ebx",\
	"and eax, ebx",\
	parm [eax][ebx][ecx]\
	modify [eax ebx ecx]\

void __interrupt __far timerhandler()
{
	totalclock++;
	_chain_intr(oldtimerhandler);
}

main(short argc, char **argv)
{
	char ch, *keystateptr, keystate, tempchar, filename[160];
	char buffer[160], buffer2[160], dacol, col1, col2;
	long i, j, k, l, m, n, x, y, x1, y1, x2, y2, xoff, yoff;
	long fil, templong, dat, good, vidpos;
	long markx, marky, markxlen, markylen;
	long xstep, ystep, lin, num, oxdim, oydim;

	//printf("------------------------------------------------------------------------------\n");
	//printf("EDITART.EXE Copyright (c) 1993 - 1996 Ken Silverman, 3D Realms Entertainment.\n");
	//printf("This version of EDITART was created for Duke Nukem 3D.\n");
	//printf("\n");
	//printf("IMPORTANT:  EDITART.EXE and associated tools and utilities are NOT\n");
	//printf("shareware and may NOT be freely distributed to any BBS, CD, floppy, or\n");
	//printf("any other media.  These tools may NOT be sold or repackaged for sale in\n");
	//printf("a commercial product.\n");
	//printf("\n");
	//printf("Please help us protect against software piracy (which drives up software\n");
	//printf("prices) by following these simple rules.\n");
	//printf("\n");
	//printf("Thank You!\n");
	//printf("------------------------------------------------------------------------------\n");
	//getch();

	if (argc >= 2)
	{
		strcpy(&artfilename,argv[1]);
		i = 0;
		while ((artfilename[i] != 0) && (i < 5))
			i++;
		while (i < 5)
		{
			artfilename[i] = '_';
			i++;
		}
		artfilename[5] = '0';
		artfilename[6] = '0';
		artfilename[7] = '0';
		artfilename[8] = '.';
		artfilename[9] = 'a';
		artfilename[10] = 'r';
		artfilename[11] = 't';
		artfilename[12] = 0;
	}
	else
		strcpy(&artfilename,"tiles000.art");

	panxdim = 1024L;

	setvmode(0x13);
	outp(0x3c4,0x4); outp(0x3c5,0x6);
	outp(0x3d4,0x14); outp(0x3d5,0x0);
	outp(0x3d4,0x17); outp(0x3d5,0xe3);
	outp(0x3d4,0x13); outp(0x3d5,panxdim>>3);

	outp(0x3c4,2); outp(0x3c5,15);
	clearbuf(0xa0000,16384,0L);

	loadtables();

	printext256(80L,84L,4,0,"Editart 7/24/96");
	printext256(80L,92L,4,0,"by Kenneth Silverman");
	sprintf(buffer,"Loading %s...",artfilename);
	printext256(80L,108L,4,0,buffer);

	initmenupaths(argv[0]);
	setupmouse();

	for(i=0;i<MAXTILES;i++)
		tilookup[i] = i;

	mousx = 0;
	mousy = 0;
	bstatus = 0;
	if ((fil = open("palette.dat",O_BINARY|O_RDWR,S_IREAD)) != -1)
	{
		read(fil,&palette[0],768);
		read(fil,&palookup[0][0],NUMPALOOKUPS*256);
		gettextcolors();

		for(i=0;i<255;i++) blackmasklookup[i] = i;
		blackmasklookup[255] = blackcol;

		close(fil);
	}

	totpicmem = 16777216L;            //allocate pic = rest of memory
	while ((pic = (char *)malloc(totpicmem)) == 0)
		totpicmem -= 4096L;

	buf2 = NULL;
	if (totpicmem > 1048576)
	{
		buf2 = (char *)(FP_OFF(pic)+totpicmem-(1024*512));
		totpicmem -= (1024*512);
	}

	for(i=0;i<MAXTILES;i++)
	{
		tilesizx[i] = 0;
		tilesizy[i] = 0;
		picanm[i] = 0L;
	}
	loadnames();

	for(i=0;i<MAXTILEFILES;i++)
	{
		localtilestart[i] = -1;
		localtileend[i] = -1;
	}
	numtilefiles = 0;
	do
	{
		i = numtilefiles;
		strcpy(filename,artfilename);
		filename[7] = (numtilefiles%10)+48;
		filename[6] = ((numtilefiles/10)%10)+48;
		filename[5] = ((numtilefiles/100)%10)+48;
		if ((fil = open(filename,O_BINARY|O_RDONLY,S_IREAD)) != -1)
		{
			read(fil,&artversion,4);
			if (artversion != 1)
			{
				printf("Wrong art version");
				exit(0);
			}
			read(fil,&numtiles,4);
			read(fil,&localtilestart[numtilefiles],4);
			read(fil,&localtileend[numtilefiles],4);
			close(fil);
			numtilefiles++;
		}
	}
	while (numtilefiles != i);

	picnum = 0;
	curtilefile = -1;
	loadpics(picnum);
	menunamecnt = 0;
	menuhighlight = 0;
	keystateptr = (char *)0x417;
	col = 0;
	trailstatus = 0;
	ch = 0;
	loadwall(buf,picnum);
	c = (xdim>>1);
	d = (ydim>>1);
	markx = 0;
	marky = 0;
	markxlen = 0;
	markylen = 0;
	drawmainscreen();
	while (ch != 27)
	{
		if (kbhit() != 0)
		{
			ch = getch();

			if (ch == 0)
			{
				ch = getch();
				keystate = *keystateptr;
				if ((keystate&3) == 0)
				{
					drawdot(buf[(d<<10)+c]);
					if ((ch == 75) && (c > 0)) c--;
					if ((ch == 77) && (c < xdim-1)) c++;
					if ((ch == 72) && (d > 0)) d--;
					if ((ch == 80) && (d < ydim-1)) d++;
					drawdot((buf[(d<<10)+c]+16)&255);
				}
				else
				{
					drawcolordot(col);
					if ((ch == 75) && (col > 0)) col--;
					if ((ch == 77) && (col < 255)) col++;
					if ((ch == 72) && (col > 31)) col -= 32;
					if ((ch == 80) && (col < 224)) col += 32;
					drawcolordot((col+16)&255);
				}
				if ((ch == 73) && (picnum > 0))
				{
					if (savewall(buf,picnum) == 0)
					{
						picnum--;
						loadpics(picnum);
						loadwall(buf,picnum);
						showall(buf);
						drawdot((buf[(d<<10)+c]+16)&255);
					}
				}
				if ((ch == 81) && (picnum < MAXTILES-1))
				{
					if (savewall(buf,picnum) == 0)
					{
						picnum++;
						loadpics(picnum);
						loadwall(buf,picnum);
						showall(buf);
						drawdot((buf[(d<<10)+c]+16)&255);
					}
				}
				if (ch == 83)
				{
					xdim = 0;
					ydim = 0;
					c = (xdim>>1);
					d = (ydim>>1);
					asksave = 1;
					drawmainscreen();
				}
				if (ch == 121)    //ALT+2
				{
					xdim = 32;
					ydim = 32;
					c = (xdim>>1);
					d = (ydim>>1);
					asksave = 1;
					drawmainscreen();
				}
				if (ch == 22)  //Alt-U (update script!)
				{
					savewall(buf,picnum);
					savepics();

					updatescript(0L,MAXTILES-1L);

					loadpics(picnum);
					loadwall(buf,picnum);
					drawmainscreen();
					ch = 255;
				}
				if (ch == 134)   //F12
				{
					drawdot(buf[(d<<10)+c]);
					screencapture();
					drawdot((buf[(d<<10)+c]+16)&255);
				}
				ch = 0;
			}
			if ((ch == 'b') || (ch == 'B'))
			{
				drawdot(buf[(d<<10)+c]);
				screencapture();
				drawdot((buf[(d<<10)+c]+16)&255);
			}

			if ((ch == 't') || (ch == 'T'))
			{
				buf[(d<<10)+c] = col, asksave = 1;
				trailstatus = 1-trailstatus;
				if (trailstatus == 0)
					printmessage("Trail OFF");
				else
					printmessage("Trail ON");
			}
			if ((ch == 32) || (trailstatus == 1))
				buf[(d<<10)+c] = col, asksave = 1;
			if (ch == 92)                             // It'a a \!
			{
				drawdot(buf[(d<<10)+c]);
				c = (xdim>>1);
				d = (ydim>>1);
				drawdot((buf[(d<<10)+c]+16)&255);
			}
			if (ch == '|')
			{
				sprintf(&printbuf,"Coordinates: (%d,%d)",c,d);
				printmessage(&printbuf);
			}
			if (ch == 8)
			{
				drawcolordot(col);
				col = 255;
				drawcolordot((col+16)&255);
			}
			if ((ch == ',') || (ch == '.') || (ch == '<') || (ch == '>'))
			{
				xstep = 0;
				ystep = 0;
				if (markx > c) xstep = 1;
				if (markx < c) xstep = -1;
				if (marky > d) ystep = 1;
				if (marky < d) ystep = -1;
				i = c-xstep;
				do
				{
					i += xstep;
					j = d-ystep;
					do
					{
						j += ystep;
						if ((buf[(j<<10)+i]&31) > 0)
							if (ch == ',') buf[(j<<10)+i]--;
						if ((buf[(j<<10)+i]&31) < 31)
							if (ch == '.') buf[(j<<10)+i]++;
						if (ch == '<')
						{
							buf[(j<<10)+i] += 224;
							buf[(j<<10)+i] &= 255;
						}
						if (ch == '>')
						{
							buf[(j<<10)+i] += 32;
							buf[(j<<10)+i] &= 255;
						}
					}
					while (j != marky);
				}
				while (i != markx);
				showall(buf);
				asksave = 1;
			}
			if ((ch == 'M') || (ch == 'm'))
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							buf2[(j<<10)+i] = buf[(j<<10)+i];
					printmessage("Tile in memory (P - restore)");
				}
			}
			if ((ch == 'P') || (ch == 'p'))
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							buf[(j<<10)+i] = buf2[(j<<10)+i];
					showall(buf);
					asksave = 1;
					printmessage("Tile restored from memory");
				}
			}
			if ((ch == 'J') || (ch == 'j'))
			{
				for(i=0;i<xdim;i++)
					for(j=0;j<ydim;j++)
						if (buf[(j<<10)+i] == buf[(d<<10)+c])
							if ((rand()&15) == 0)
								buf[(j<<10)+i] = col;
				showall(buf);
				asksave = 1;
			}
			if ((ch == ']') && (xdim > 0) && (ydim > 0))
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					k = (buf[(d<<10)+c]>>5);
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							buf2[(j<<10)+i] = buf[(j<<10)+i];
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							if ((buf[(j<<10)+i]>>5) == k)
							{
								m = buf2[(((j+0)%ydim)<<10)+((i+0)%xdim)]*2, n = 2;
								tempchar = buf2[(((j+0)%ydim)<<10)+((i+1)%xdim)];
								if ((tempchar>>5) == k)
									m += tempchar, n += 1;
								tempchar = buf2[(((j+0)%ydim)<<10)+((i-1)%xdim)];
								if ((tempchar>>5) == k)
									m += tempchar, n += 1;
								tempchar = buf2[(((j+1)%ydim)<<10)+((i+0)%xdim)];
								if ((tempchar>>5) == k)
									m += tempchar, n += 1;
								tempchar = buf2[(((j-1)%ydim)<<10)+((i+0)%xdim)];
								if ((tempchar>>5) == k)
									m += tempchar, n += 1;

								buf[(j<<10)+i] = (buf[(j<<10)+i]&0xe0)+(((m+(n>>1))/n)&31);
							}
					showall(buf);
					asksave = 1;
				}
			}
			if ((ch == '[') && (xdim > 0) && (ydim > 0))
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					k = (buf[(d<<10)+c]>>5);
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							buf2[(j<<10)+i] = buf[(j<<10)+i];
					for(i=0;i<xdim;i++)
						for(j=0;j<ydim;j++)
							if ((buf[(j<<10)+i]>>5) == k)
							{
								m = buf2[(((j+0)%ydim)<<10)+((i+0)%xdim)]*2, n = 2;

								l = rand();
								tempchar = buf2[(((j+0)%ydim)<<10)+((i+1)%xdim)];
								if (((tempchar>>5) == k) && ((l&0x03) == 0))
									m += tempchar, n++;
								tempchar = buf2[(((j+0)%ydim)<<10)+((i-1)%xdim)];
								if (((tempchar>>5) == k) && ((l&0x0c) == 0))
									m += tempchar, n++;
								tempchar = buf2[(((j+1)%ydim)<<10)+((i+0)%xdim)];
								if (((tempchar>>5) == k) && ((l&0x30) == 0))
									m += tempchar, n++;
								tempchar = buf2[(((j-1)%ydim)<<10)+((i+0)%xdim)];
								if (((tempchar>>5) == k) && ((l&0xc0) == 0))
									m += tempchar, n++;

								buf[(j<<10)+i] = (buf[(j<<10)+i]&0xe0)+(((m+(n>>1))/n)&31);
							}
					showall(buf);
					asksave = 1;
				}
			}
			if ((ch == 39) && (xdim > 0) && (ydim > 0))    // It's a '
			{
				k = (buf[(d<<10)+c]>>5);
				for(i=0;i<xdim;i++)
					for(j=0;j<ydim;j++)
						if ((buf[(j<<10)+i]>>5) == k)
						{
							m = (buf[(j<<10)+i]&31);
							tempchar = buf[(((j+0)%ydim)<<10)+((i+1)%xdim)];
							if ((tempchar>>5) != k)
								m--;
							tempchar = buf[(((j+0)%ydim)<<10)+((i-1)%xdim)];
							if ((tempchar>>5) != k)
								m++;
							tempchar = buf[(((j+1)%ydim)<<10)+((i+0)%xdim)];
							if ((tempchar>>5) != k)
								m--;
							tempchar = buf[(((j-1)%ydim)<<10)+((i+0)%xdim)];
							if ((tempchar>>5) != k)
								m++;

							if (m < 0) m = 0;
							if (m > 31) m = 31;
							buf[(j<<10)+i] = (buf[(j<<10)+i]&0xe0)+m;
						}
				showall(buf);
				asksave = 1;
			}
			if ((ch == ';') && (xdim > 0) && (ydim > 0))
			{
				k = (buf[(d<<10)+c]>>5);
				for(i=0;i<xdim;i++)
					for(j=0;j<ydim;j++)
						if ((buf[(j<<10)+i]>>5) == k)
						{
							m = (buf[(j<<10)+i]&31);
							tempchar = buf[(((j+0)%ydim)<<10)+((i+1)%xdim)];
							if ((tempchar>>5) != k)
								m++;
							tempchar = buf[(((j+0)%ydim)<<10)+((i-1)%xdim)];
							if ((tempchar>>5) != k)
								m--;
							tempchar = buf[(((j+1)%ydim)<<10)+((i+0)%xdim)];
							if ((tempchar>>5) != k)
								m++;
							tempchar = buf[(((j-1)%ydim)<<10)+((i+0)%xdim)];
							if ((tempchar>>5) != k)
								m--;

							if (m < 0) m = 0;
							if (m > 31) m = 31;
							buf[(j<<10)+i] = (buf[(j<<10)+i]&0xe0)+m;
						}
				showall(buf);
				asksave = 1;
			}
			if (((ch == 'R') || (ch == 'r')) && (xdim > 0) && (ydim > 0))
			{
				printmessage("Rotate tile (Use arrows)");
				ch = getch();
				if (ch == 0)
				{
					ch = getch();
					if (ch == 75)
						for(i=xdim-1;i>=1;i--)
							for(j=0;j<ydim;j++)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(j<<10)+((i+1)%xdim)];
								buf[(j<<10)+((i+1)%xdim)] = tempchar;
							}
					if (ch == 77)
						for(i=1;i<xdim;i++)
							for(j=0;j<ydim;j++)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(j<<10)+((i+1)%xdim)];
								buf[(j<<10)+((i+1)%xdim)] = tempchar;
							}
					if (ch == 72)
						for(i=0;i<xdim;i++)
							for(j=ydim-1;j>=1;j--)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(((j+1)%ydim)<<10)+i];
								buf[(((j+1)%ydim)<<10)+i] = tempchar;
							}
					if (ch == 80)
						for(i=0;i<xdim;i++)
							for(j=1;j<ydim;j++)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(((j+1)%ydim)<<10)+i];
								buf[(((j+1)%ydim)<<10)+i] = tempchar;
							}
					showall(buf);
				}
				asksave = 1;
				cleartext();
			}
			if ((ch == 'C') || (ch == 'c'))
			{
				tempchar = buf[(d<<10)+c];
				for(i=0;i<xdim;i++)
					for(j=0;j<ydim;j++)
						if (buf[(j<<10)+i] == tempchar)
							buf[(j<<10)+i] = col;
				showall(buf);
				asksave = 1;
			}
			if (ch == '1')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					markx = c;
					marky = d;
					printmessage("1st point set (2 - copy)");
				}
			}
			if (ch == '2')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					x = markx, y = marky, x2 = c, y2 = d;
					if (x > x2)
						templong = x, x = x2, x2 = templong;
					if (y > y2)
						templong = y, y = y2, y2 = templong;
					markxlen = x2-x+1;
					markylen = y2-y+1;
					for(i=0;i<markxlen;i++)
						for(j=0;j<markylen;j++)
							buf2[(j<<10)+i] = buf[((j+y)<<10)+(i+x)];
					printmessage("Region copied (3 - paste)");
				}
			}
			if (ch == '3')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					for(i=0;i<markxlen;i++)
						for(j=0;j<markylen;j++)
							if ((i+c < xdim) && (j+d < ydim) && (buf2[(j<<10)+i] != 255))
								buf[((j+d)<<10)+(i+c)] = buf2[(j<<10)+i];
					showall(buf);
					asksave = 1;
				}
			}
			if (ch == '4')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					for(i=0;i<(markxlen>>1);i++)
						for(j=0;j<markylen;j++)
						{
							templong = buf2[(j<<10)+i];
							buf2[(j<<10)+i] = buf2[(j<<10)+(markxlen-1-i)];
							buf2[(j<<10)+(markxlen-1-i)] = templong;
						}
					printmessage("Region flipped x-wise");
				}
			}
			if (ch == '5')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					for(i=0;i<markxlen;i++)
						for(j=0;j<(markylen>>1);j++)
						{
							templong = buf2[(j<<10)+i];
							buf2[(j<<10)+i] = buf2[((markylen-1-j)<<10)+i];
							buf2[((markylen-1-j)<<10)+i] = templong;
						}
					printmessage("Region flipped y-wise");
				}
			}
			if (ch == '6')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					x2 = xdim;
					y2 = ydim;
					if (x2 > 480) x2 = 480;
					if (y2 > 480) y2 = 480;
					for(i=0;i<x2;i++)
						for(j=0;j<y2;j++)
							if (i > j)
							{
								templong = buf2[(i<<10)+j];
								buf2[(i<<10)+j] = buf2[(j<<10)+i];
								buf2[(j<<10)+i] = templong;
							}
					templong = markxlen;
					markxlen = markylen;
					markylen = templong;
					printmessage("X and Y swapped");
				}
			}
			if (ch == '8')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					if (translucloaded == 0)
					{
						if ((transluc = (char *)convalloc32(65536L)) != 0)
						{
							for(i=0;i<65536;i++)
								transluc[i] = 255;
							translucloaded = 1;
							printmessage("Transparent table allocated");
						}
						else
							printmessage("Not enough memory for table");
					}
					for(i=0;i<markxlen;i++)
						for(j=0;j<markylen;j++)
							if ((i+c < xdim) && (j+d < ydim) && (buf2[(j<<10)+i] != 255))
							{
								col1 = buf[((j+d)<<10)+(i+c)];
								col2 = buf2[(j<<10)+i];
								if (translucloaded == 1)
								{
									dacol = transluc[(col1<<8)+col2];
									if (dacol == 255)
									{
										dacol = gettrans(buf[((j+d)<<10)+(i+c)],buf2[(j<<10)+i]);
										transluc[(col1<<8)+col2] = dacol;
										transluc[(col2<<8)+col1] = dacol;
									}
								}
								else
									dacol = gettrans(buf[((j+d)<<10)+(i+c)],buf2[(j<<10)+i]);
								buf[((j+d)<<10)+(i+c)] = dacol;
							}
					showall(buf);
					asksave = 1;
				}
			}
			if (ch == '/')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					markxlen = (markxlen>>1)+1, markylen = (markylen>>1)+1;
					for(i=0;i<markxlen;i++)
						for(j=0;j<markylen;j++)
							buf2[(j<<10)+i] = buf2[((j<<1)<<10)+(i<<1)];
					printmessage("Region now half-size");
				}
			}
			if (ch == '*')
			{
				if (buf2 == NULL)
				{
					printmessage("Not enough memory for that!");
				}
				else
				{
					markxlen <<= 1, markylen <<= 1;
					if (markxlen > 1024) markxlen = 1024;
					if (markylen > 512) markylen = 512;
					for(i=markxlen;i>=0;i--)
						for(j=markylen;j>=0;j--)
							if ((i < xdim) && (j < ydim))
								buf2[(j<<10)+i] = buf2[((j>>1)<<10)+(i>>1)];
					printmessage("Region now double-size");
				}
			}
			if ((ch == 'F') || (ch == 'f'))
			{
				fillregion(c,d,col,col);
				showall(buf);
				printmessage("Flood Fill");
				asksave = 1;
			}
			if (ch == 9)
			{
				drawcolordot(col);
				col = buf[(d<<10)+c];
				drawcolordot((col+16)&255);
				printmessage("Current color updated");
			}
			if ((ch == '+') || (ch == '-'))
			{
				if ((ch == '+') && ((picanm[picnum]&63) < 63))
					picanm[picnum]++;
				if (ch == '-')
				{
					if ((picanm[picnum]&63) > 0)
						picanm[picnum]--;
					else
						picanm[picnum] = ((picanm[picnum]+64)&0x000000c0)+(picanm[picnum]&0xffffff3f);
				}
				asksave = 1;
				showall(buf);
			}
			if ((ch == 's') || (ch == 'S'))
			{
				x = xdim;
				y = ydim;

				ch = 0;
				while ((ch != 13) && (ch != 27))
				{
					sprintf(&buffer,"Xdim: %d_ ",xdim);
					printmessage(buffer);

					ch = getch();
					if ((ch >= 48) && (ch <= 57))
					{
						i = (xdim*10)+(ch-48);
						if (i <= 1024)
							xdim = i;
					}
					if (ch == 8)
					{
						xdim /= 10;
					}
				}
				if (ch == 13)
				{
					ch = 0;
					while ((ch != 13) && (ch != 27))
					{
						sprintf(&buffer,"Ydim: %d_ ",ydim);
						printmessage(buffer);

						ch = getch();
						if ((ch >= 48) && (ch <= 57))
						{
							i = (ydim*10)+(ch-48);
							if (i <= 512)
								ydim = i;
						}
						if (ch == 8)
						{
							ydim /= 10;
						}
					}
				}
				if (ch == 13)
				{
					c = (xdim>>1);
					d = (ydim>>1);
					asksave = 1;
					drawmainscreen();
				}
				else
				{
					xdim = x;
					ydim = y;
					showall(buf);
					cleartext();
				}
				ch = 255;
			}
			if ((ch == 'g') || (ch == 'G'))
			{
				ch = 0;
				j = picnum;
				while ((ch != 13) && (ch != 27))
				{
					sprintf(&buffer,"Goto tile: %d_ ",j);
					printmessage(buffer);

					ch = getch();
					if ((ch >= 48) && (ch <= 57))
					{
						i = (j*10)+(ch-48);
						if (i < MAXTILES)
							j = i;
					}
					if (ch == 8)
					{
						j /= 10;
					}
				}
				if (ch == 13)
				{
					savewall(buf,picnum);
					picnum = j;
					loadpics(picnum);
					loadwall(buf,picnum);
					showall(buf);
					drawdot((buf[(d<<10)+c]+16)&255);
				}
				else
				{
					cleartext();
				}
				ch = 255;
			}
			if (((ch == 'o') || (ch == 'O')) && (xdim > 0) && (ydim > 0))
			{
				x = 0;
				y = 0;

				l = 0;
				m = xdim;
				while ((m > 0) && (l == 0))
				{
					l = 0;
					for(k=0;k<ydim;k++)
						if (buf[(k<<10)+0] != 255) { l = 1; break; }
					if (l == 0)
					{
						x++;
						for(i=1;i<xdim;i++)
						{
							k = ((i+1)%xdim);
							for(j=0;j<ydim;j++)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(j<<10)+k];
								buf[(j<<10)+k] = tempchar;
							}
						}
						xdim--;
					}
					m--;
				}

				l = 0;
				m = ydim;
				while ((m > 0) && (l == 0))
				{
					l = 0;
					for(k=0;k<xdim;k++)
						if (buf[(0<<10)+k] != 255) { l = 1; break; }
					if (l == 0)
					{
						y++;
						for(j=1;j<ydim;j++)
						{
							k = ((j+1)%ydim);
							for(i=0;i<xdim;i++)
							{
								tempchar = buf[(j<<10)+i];
								buf[(j<<10)+i] = buf[(k<<10)+i];
								buf[(k<<10)+i] = tempchar;
							}
						}
						ydim--;
					}
					m--;
				}

				l = 0;
				while ((xdim > 0) && (l == 0))
				{
					l = 0;
					for(k=0;k<ydim;k++)
						if (buf[(k<<10)+(xdim-1)] != 255) { l = 1; break; }
					if (l == 0) { x--; xdim--; }
				}

				l = 0;
				while ((ydim > 0) && (l == 0))
				{
					l = 0;
					for(k=0;k<xdim;k++)
						if (buf[((ydim-1)<<10)+k] != 255) { l = 1; break; }
					if (l == 0) { y--; ydim--; }
				}

				i = (long)((signed char)((picanm[picnum]>>8)&255));
				i -= (x>>1);
				if (i < -128) i = -128;
				if (i > 127) i = 127;
				picanm[picnum] = (picanm[picnum]&0xffff00ff)+((i&255)<<8);

				i = (long)((signed char)((picanm[picnum]>>16)&255));
				i -= (y>>1);
				if (i < -128) i = -128;
				if (i > 127) i = 127;
				picanm[picnum] = (picanm[picnum]&0xff00ffff)+((i&255)<<16);

				c = (xdim>>1);
				d = (ydim>>1);
				asksave = 1;
				drawmainscreen();
				ch = 255;
			}

			if ((ch == 'u') || (ch == 'U'))
			{
				k = 0;

				if (loadtileofscript(picnum,filename,&x1,&y1,&x2,&y2) == 1)
				{
					strcpy(menupath,filename);
					i = strlen(filename)-1;
					while ((menupath[i] != '\\') && (i > 0)) i--;
					menupath[i] = 0;

					j = 0; i++;
					while (filename[i] != 0) buffer[j++] = filename[i++];
					buffer[j] = 0;
					loadpicture(buffer);

					if ((k=selectbox(&x1,&y1,&x2,&y2)) == 1)
						captureit(x1,y1,x2,y2,picnum,filename,1);
					drawmainscreen();
					chdir(curpath);

					menuhighlight = 0;
				}

				while (k == 0)
				{
					i = menuselect();
					if (i < 0)
					{
						chdir(curpath);        //Error or Escape (cancel)
						showall(buf);
						if (i == -2)
							printmessage("No files found.");
						break;
					}
					else
					{
						menuhighlight = i;

						loadpicture(menuname[menuhighlight]);

						strcpy(filename,menupath);
						strcat(filename,"\\");
						strcat(filename,menuname[menuhighlight]);
						x1 = 0x80000000;
						if ((k=selectbox(&x1,&y1,&x2,&y2)) == 1)
							captureit(x1,y1,x2,y2,picnum,filename,1);
						drawmainscreen();
						chdir(curpath);
					}
				}

				drawdot((buf[(d<<10)+c]+16)&255);
				ch = 255;
			}
			if ((ch == 'v') || (ch == 'V'))
			{
				savewall(buf,picnum);
				savepics();
				picnum = gettile(picnum);
				loadpics(picnum);
				loadwall(buf,picnum);
				drawmainscreen();
				ch = 255;
			}
			if (((ch == 'a') || (ch == 'A')) && ((picanm[picnum]&192) > 0) && ((picanm[picnum]&63) > 0))
			{
				savewall(buf,picnum);
				sprintf(&printbuf,"Use +/- to change speed");
				printmessage(&printbuf);
				i = 1;
				ch = 0;

				oldtimerhandler = _dos_getvect(0x8);
				_dos_setvect(0x8, timerhandler);
				outp(0x43,54); outp(0x40,9942&255); outp(0x40,9942>>8);
				totalclock = 0L;

				animspeed = ((((unsigned long)picanm[picnum])>>24)&15);

				j = picnum;

				while ((ch != 13) && (ch != 27))
				{
					templong = picnum;

					i = (totalclock>>animspeed);
					switch(picanm[templong]&192)
					{
						case 64:
							k = (i%((picanm[templong]&63)<<1));
							if (k < (picanm[templong]&63))
								templong += k;
							else
								templong += (((picanm[templong]&63)<<1)-k);
							break;
						case 128:
							templong += (i%((picanm[templong]&63)+1));
							break;
						case 192:
							templong -= (i%((picanm[templong]&63)+1));
					}

					if ((templong >= 0) && (templong < MAXTILES))
						loadwall(buf,templong);

					if (templong != j)
					{
						j = templong;
						//if ((xdim < lastshowallxdim) || (ydim < lastshowallydim))
							cleartopbox();
					}
					lastshowallxdim = xdim;
					lastshowallydim = ydim;

					copywalltoscreen(buf,templong,1);

					if (kbhit() != 0)
					{
						ch = getch();
						if ((ch == '+') && (animspeed > 0)) animspeed--;
						if ((ch == '-') && (animspeed < 15)) animspeed++;
					}
				}

				_dos_setvect(0x8, oldtimerhandler);
				outp(0x43,54); outp(0x40,255); outp(0x40,255);

				cleartopbox();

				cleartext();
				loadwall(buf,picnum);
				showall(buf);

				if (ch == 13)
				{
					picanm[picnum] = (picanm[picnum]&0xf0ffffff) + (((unsigned long)(animspeed&15))<<24);
					asksave = 1;
				}

				ch = 255;
			}
			if ((ch == '`') || (ch == '~'))
			{
				if ((xdim <= 320) && (ydim <= 200))
				{
					sprintf(&printbuf,"Use arrows to change center",animspeed);
					printmessage(&printbuf);
					templong = picanm[picnum];
					ch = 0;

					copywalltoscreen(buf,picnum,1);

					k = whitecol;
					while ((ch != 13) && (ch != 27) && (ch != '`') && (ch != '~'))
					{
						if (kbhit() != 0)
						{
							ch = getch();
							if ((ch == '/') || (ch == '?'))
							{
								picanm[picnum] &= 0xff0000ff;
								cleartopbox();
								copywalltoscreen(buf,picnum,1);
							}
							if (ch == 0)
							{
								ch = getch();
								if (ch == 75) picanm[picnum] = (((picanm[picnum]+(1<<8))&0x0000ff00)+(picanm[picnum]&0xffff00ff));
								if (ch == 77) picanm[picnum] = (((picanm[picnum]+(255<<8))&0x0000ff00)+(picanm[picnum]&0xffff00ff));
								if (ch == 72) picanm[picnum] = (((picanm[picnum]+(1<<16))&0x00ff0000)+(picanm[picnum]&0xff00ffff));
								if (ch == 80) picanm[picnum] = (((picanm[picnum]+(255<<16))&0x00ff0000)+(picanm[picnum]&0xff00ffff));
								cleartopbox();
								copywalltoscreen(buf,picnum,1);
							}
						}
						readmouse();
						if ((mousx != 0) || (mousy != 0))
						{
							i = (long)((signed char)((picanm[picnum]>>8)&255));
							i -= mousx;
							if (i < -128) i = -128;
							if (i > 127) i = 127;
							picanm[picnum] = (picanm[picnum]&0xffff00ff)+((i&255)<<8);

							i = (long)((signed char)((picanm[picnum]>>16)&255));
							i -= mousy;
							if (i < -128) i = -128;
							if (i > 127) i = 127;
							picanm[picnum] = (picanm[picnum]&0xff00ffff)+((i&255)<<16);

							cleartopbox();
							copywalltoscreen(buf,picnum,1);
						}

						outp(0x3c4,2); outp(0x3c5,15);
						for(i=0;i<320;i+=4)
							drawpixel(((((100+16-panyoff)*panxdim)+i)>>2)+0xa0000,k);
						outp(0x3c4,2); outp(0x3c5,1);
						for(j=0;j<200;j++)
							drawpixel(((((j+16-panyoff)*panxdim)+160)>>2)+0xa0000,k);

						k ^= whitecol^blackcol;
					}
					cleartopbox();
					cleartext();
					if (ch == 27)
						picanm[picnum] = templong;
					else
						asksave = 1;
					copywalltoscreen(buf,picnum,0);
					ch = 255;
				}
			}
			if ((ch == 'n') || (ch == 'N'))
			{
				strcpy(&buffer,&names[picnum][0]);
				i = 0;
				while (buffer[i] != 0)
					i++;
				buffer[i] = '_';
				buffer[i+1] = 0;
				strcpy(&buffer2,"Name: ");
				strcat(&buffer2,buffer);
				printmessage(&buffer2[0]);
				ch = 0;
				while ((ch != 13) && (ch != 27))
				{
					ch = getch();
					if ((ch > 32) && (i < 16))
					{
						buffer[i] = ch;
						buffer[i+1] = '_';
						buffer[i+2] = 32;
						buffer[i+3] = 0;
						i++;
						strcpy(&buffer2,"Name: ");
						strcat(&buffer2,buffer);
						printmessage(&buffer2[0]);
					}
					if ((ch == 8) && (i > 0))
					{
						i--;
						buffer[i] = '_';
						buffer[i+1] = 32;
						buffer[i+2] = 0;
						strcpy(&buffer2,"Name: ");
						strcat(&buffer2,buffer);
						printmessage(&buffer2[0]);
					}
				}
				if (ch == 13)
				{
					buffer[i] = 0;
					strcpy(&names[picnum][0],&buffer[0]);
					needtosavenames = 1;
				}
				cleartext();
				if (names[picnum][0] != 0)
				{
					strcpy(&buffer2,"Name: ");
					strcat(&buffer2,&names[picnum][0]);
					printmessage(&buffer2[0]);
				}
			}
		}
		if (moustat == 1)
		{
			readmouse();
			if ((mousx != 0) || (mousy != 0))
			{
				drawdot(buf[(d<<10)+c]);

				xstep = c;
				ystep = d;
				c += mousx;
				d += mousy;
				if (c < 0) c = 0;
				if (c >= xdim) c = xdim-1;
				if (d < 0) d = 0;
				if (d >= ydim) d = ydim-1;
				drawdot((buf[(d<<10)+c]+16)&255);
			}
			if (bstatus > 0)
			{
				x = c;
				y = d;
				j = labs(xstep-x)+labs(ystep-y);
				for(i=0;i<j;i++)
				{
					c = xstep + ((x-xstep)*i)/j;
					d = ystep + ((y-ystep)*i)/j;
					buf[(d<<10)+c] = col;
					drawdot(col);
				}
				asksave = 1;
				c = x;
				d = y;
			}
		}
		if (ch == 27)
		{
			printmessage("Quit? (Y/N)");
			do
			{
				ch = getch();
			}
			while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N') && (ch != 27));
			if ((ch == 'y') || (ch == 'Y'))
				ch = 27;
			else
			{
				cleartext();
				ch = 0;
			}
		}
	}
	savewall(buf,picnum);
	savepics();
	savenames();
	setvmode(0x3);
	updatemaps();
	if (capfil != -1) close(capfil);
	return(0);
}

loadtables()
{
	short int fil;

	if ((fil = open("tables.dat",O_BINARY|O_RDWR,S_IREAD)) != -1)
	{
		read(fil,&sintable[0],4096);
		lseek(fil,4096L+1280L,SEEK_SET);
		read(fil,&textfont[0],1024);
		close(fil);
	}
	return(0);
}

loadpics(long dapicnum)
{
	long i, offscount, siz, danum;
	short int fil;

	i = curtilefile;
	if (i < 0) i = 0;
	while ((dapicnum < localtilestart[i]) && (i > 0)) i--;
	while ((dapicnum > localtileend[i]) && (i < numtilefiles-1)) i++;

	if ((i == curtilefile) && (dapicnum <= localtileend[i]))
		return(-1);

	if (curtilefile == -1)
		allasksave = 0;
	else
		savepics();

	curtilefile = i;

	if (dapicnum > localtileend[i])
	{
		localtilestart[numtilefiles] = localtileend[numtilefiles-1]+1;
		localtileend[numtilefiles] = localtilestart[numtilefiles];
		localtileend[numtilefiles] += (localtileend[0]-localtilestart[0]);
		for(i=localtilestart[numtilefiles];i<=localtileend[numtilefiles];i++)
		{
			tilesizx[i] = 0;
			tilesizy[i] = 0;
			picanm[i] = 0L;
			waloff[i] = FP_OFF(pic);
		}
		numtilefiles++;
		curtilefile = numtilefiles-1;
		totpicsiz = 0L;
		return(0);
	}

	danum = curtilefile;

	artfilename[7] = (danum%10)+48;
	artfilename[6] = ((danum/10)%10)+48;
	artfilename[5] = ((danum/100)%10)+48;
	if ((fil = open(artfilename,O_BINARY|O_RDONLY,S_IREAD)) == -1)
	{
		if (danum == 0)
		{
			artversion = 1;
			numtilefiles = 0;
			numtiles = 0;

			localtilestart[numtilefiles] = 0;
			localtileend[numtilefiles] = 255;
			for(i=localtilestart[0];i<=localtileend[0];i++)
			{
				tilesizx[i] = 0;
				tilesizy[i] = 0;
				picanm[i] = 0L;
				waloff[i] = FP_OFF(pic);
			}
			numtilefiles++;
			curtilefile = numtilefiles-1;
			totpicsiz = 0L;
		}
		return(0);
	}
	read(fil,&artversion,4);
	if (artversion != 1) return(-1);
	read(fil,&numtiles,4);
	read(fil,&localtilestart[danum],4);
	read(fil,&localtileend[danum],4);
	read(fil,&tilesizx[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<1);
	read(fil,&tilesizy[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<1);
	read(fil,&picanm[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<2);

	totpicsiz = 0L;
	offscount = 0L;
	for(i=localtilestart[danum];i<=localtileend[danum];i++)
	{
		waloff[i] = offscount+FP_OFF(pic);
		siz = tilesizx[i]*tilesizy[i];
		if (siz > 0)
		{
			offscount += siz;
			totpicsiz += siz;
		}
	}

	if (totpicsiz >= totpicmem)
	{
		if (buf2 != NULL)
		{
			buf2 = NULL;
			totpicmem += (1024*512);
		}

		if (totpicsiz >= totpicmem)
		{
			printf("Not enough memory to fit artwork data.\n");
			exit(0);
		}
	}

	read(fil,&pic[0],totpicsiz);

	close(fil);
	return(0);
}

savepics()
{
	char ch, dabuffer[160];
	long i, templong, offscount, siz, danum;
	short int fil;

	if (allasksave == 0)
		return(0);
	allasksave = 0;

	i = 0;
	while (curpath[i] != 0) i++;
	curpath[i] = 92;
	curpath[i+1] = 0;
	chdir(curpath);
	curpath[i] = 0;

	numtiles = MAXTILES-1;
	while ((tilesizx[numtiles] < 2) && (tilesizy[numtiles] < 2) && (numtiles > 0))
		numtiles--;
	numtiles++;

	if (mustsavelocals != 0)
	{
		mustsavelocals = 0;

		for(danum=0;danum<numtilefiles;danum++)
			if (danum != curtilefile)
			{
				artfilename[7] = (danum%10)+48;
				artfilename[6] = ((danum/10)%10)+48;
				artfilename[5] = ((danum/100)%10)+48;

				if ((fil = open(artfilename,O_BINARY|O_WRONLY,S_IWRITE))!=-1)
				{
					write(fil,&artversion,4);
					write(fil,&numtiles,4);
					write(fil,&localtilestart[danum],4);
					write(fil,&localtileend[danum],4);
					close(fil);
				}
			}
	}

	danum = curtilefile;

	artfilename[7] = (danum%10)+48;
	artfilename[6] = ((danum/10)%10)+48;
	artfilename[5] = ((danum/100)%10)+48;

	sprintf(dabuffer,"Updating %s...",artfilename);
	printmessage(dabuffer);

	if (localtilestart[danum] > localtileend[danum])
		return(0);

	if (localtilestart[danum] >= MAXTILES)
		return(0);
	if (localtileend[danum] >= MAXTILES)
		localtileend[danum] = MAXTILES-1;

	if ((fil = open(artfilename,O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IWRITE))==-1)
		return(-1);
	write(fil,&artversion,4);
	write(fil,&numtiles,4);
	write(fil,&localtilestart[danum],4);
	write(fil,&localtileend[danum],4);
	write(fil,&tilesizx[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<1);
	write(fil,&tilesizy[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<1);
	write(fil,&picanm[localtilestart[danum]],(localtileend[danum]-localtilestart[danum]+1)<<2);

	if (write(fil,&pic[0],totpicsiz) < totpicsiz)
	{
		printf("SAVE UNSUCCESSFUL!\n");
		return(0);
	}

	close(fil);

	sprintf(dabuffer,"%s updated.",artfilename);
	printmessage(dabuffer);

	return(0);
}

loadwall(char *bufptr, long wallnum)
{
	long i, j, vidpos, dat;
	char *picptr;

	picptr = (char *)(waloff[wallnum]);
	xdim = tilesizx[wallnum];
	ydim = tilesizy[wallnum];

	clearbuf(buf,(1024*512)>>2,0xffffffff);

	vidpos = 0;
	for(i=0;i<xdim;i++)
		for(j=0;j<ydim;j++)
		{
			dat = *picptr++;
			*(bufptr+(j<<10)+i) = dat;
		}
	asksave = 0;

	if (c < 0) c = 0;
	if (d < 0) d = 0;
	if (c >= xdim) c = xdim-1;
	if (d >= ydim) d = ydim-1;
	updatepanning();

	return(0);
}

savewall(char *bufptr, long wallnum)
{
	long i, j, vidpos, dat;
	char *picptr, ch;

	if (asksave != 0)
	{
		if (asksave == 1)
		{
			asksave = 0;
			printmessage("Save current tile?");
			do
			{
				ch = getch();
			}
			while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));
			if ((ch == 'n') || (ch == 'N'))
				return(0);
		}

		allasksave = 1;

		if ((xdim != tilesizx[wallnum]) || (ydim != tilesizy[wallnum]))
		{
			if (resizetile(wallnum,(long)tilesizx[wallnum],(long)tilesizy[wallnum],xdim,ydim) == -1)
			{
				printmessage("OUT OF MEMORY! (cancelled)");
				return(-1);
			}
			else
			{
				tilesizx[wallnum] = xdim;
				tilesizy[wallnum] = ydim;
			}
		}

		picptr = (char *)(waloff[wallnum]);
		xdim = tilesizx[wallnum];
		ydim = tilesizy[wallnum];

		vidpos = 0;
		for(i=0;i<xdim;i++)
			for(j=0;j<ydim;j++)
			{
				dat = *(bufptr+(j<<10)+i);
				*picptr++ = dat;
			}
	}
	return(0);
}

showall(char *bufptr)
{
	//if ((xdim < lastshowallxdim) || (ydim < lastshowallydim))
		cleartopbox();
	lastshowallxdim = xdim;
	lastshowallydim = ydim;

	cleartext();
	switch(picanm[picnum]&192)
	{
		case 0: printext256(152L,0L,whitecol,blackcol,"NoAnm:"); break;
		case 64: printext256(152L,0L,whitecol,blackcol,"Oscil:"); break;
		case 128: printext256(152L,0L,whitecol,blackcol,"AnmFD:"); break;
		case 192: printext256(152L,0L,whitecol,blackcol,"AnmBK:"); break;
	}
	printbuf[0] = ((picanm[picnum]&63)/10)+48;
	printbuf[1] = ((picanm[picnum]&63)%10)+48;
	if ((picanm[picnum]&63) < 10)
	{
		printbuf[0] = ((picanm[picnum]&63)%10)+48;
		printbuf[1] = 32;
	}
	printbuf[2] = 0;
	printext256(200L,0L,whitecol,blackcol,&printbuf);

	if ((xdim == 0) && (ydim == 0))
	{
		printext256(80L,0L,whitecol,blackcol," -blank-");
	}
	else
	{
		if (xdim >= 1000)
			printbuf[0] = ((xdim/1000)%10)+48;
		else
			printbuf[0] = 32;
		if (xdim >= 100)
			printbuf[1] = ((xdim/100)%10)+48;
		else
			printbuf[1] = 32;
		if (xdim >= 10)
			printbuf[2] = ((xdim/10)%10)+48;
		else
			printbuf[2] = 32;
		printbuf[3] = (xdim%10)+48;
		printbuf[4] = '*';
		printbuf[5] = 32;
		printbuf[6] = 32;
		printbuf[7] = 32;
		printbuf[8] = 0;
		if (ydim >= 100)
		{
			printbuf[5] = ((ydim/100)%10)+48;
			printbuf[6] = ((ydim/10)%10)+48;
			printbuf[7] = (ydim%10)+48;
		}
		else if (ydim >= 10)
		{
			printbuf[5] = ((ydim/10)%10)+48;
			printbuf[6] = (ydim%10)+48;
		}
		else if (ydim >= 1)
		{
			printbuf[5] = (ydim%10)+48;
		}
		else
		{
			printbuf[5] = '0';
		}
		printext256(80L,0L,whitecol,blackcol,&printbuf);
	}

	sprintf(&printbuf,"Tile:");
	printbuf[5] = 32;
	printbuf[6] = 32;
	printbuf[7] = 32;
	printbuf[8] = 32;
	printbuf[9] = 0;
	if (picnum >= 1000)
	{
		printbuf[5] = ((picnum/1000)%10)+48;
		printbuf[6] = ((picnum/100)%10)+48;
		printbuf[7] = ((picnum/10)%10)+48;
		printbuf[8] = (picnum%10)+48;
	}
	else if (picnum >= 100)
	{
		printbuf[5] = ((picnum/100)%10)+48;
		printbuf[6] = ((picnum/10)%10)+48;
		printbuf[7] = (picnum%10)+48;
	}
	else if (picnum >= 10)
	{
		printbuf[5] = ((picnum/10)%10)+48;
		printbuf[6] = (picnum%10)+48;
	}
	else if (picnum >= 1)
	{
		printbuf[5] = (picnum%10)+48;
	}
	else
	{
		printbuf[5] = '0';
	}
	printext256(0L,0L,whitecol,blackcol,&printbuf);

	if (names[picnum][0] != 0)
		printmessage(&names[picnum][0]);

	copywalltoscreen(bufptr,picnum,0);

	return(0);
}

copywalltoscreen(char *bufptr, long dapicnum, char maskmode)
{
	char *ptr;
	long plane, i, j, jstart, jend, vidpos, dat, daydim, cnt, xoff, yoff;

	xoff = 0; yoff = 0;
	if ((xdim <= 320) && (ydim <= 200))
	{
		xoff = 160L-(xdim>>1)-(long)((signed char)((picanm[dapicnum]>>8)&255));
		yoff = 100L-(ydim>>1)-(long)((signed char)((picanm[dapicnum]>>16)&255));
	}

	daydim = ydim;
	if (daydim > 240) daydim = 240;

	if (maskmode == 1)
		blackmasklookup[255] = blackcol;
	else
		blackmasklookup[255] = 255;

	for(plane=0;plane<4;plane++)
	{
		outp(0x3c4,2); outp(0x3c5,1<<((plane+xoff)&3));

		jstart = panyoff;
		jend = daydim+panyoff;
		if (jstart-panyoff+yoff < 0) jstart = 0+panyoff-yoff;
		if (jend-panyoff+yoff > 240) jend = 240+panyoff-yoff;
		for(j=jstart;j<jend;j++)
		{
			vidpos = (((16+j-panyoff+yoff)*panxdim+plane+xoff)>>2)+0xa0000;

			cnt = xdim-plane;
			ptr = (char *)(bufptr+plane+(j<<10));

			while (cnt > 12)
			{
				dat = blackmasklookup[*ptr];
				dat += (blackmasklookup[*(ptr+4)]<<8);
				dat += (blackmasklookup[*(ptr+8)]<<16);
				dat += (blackmasklookup[*(ptr+12)]<<24);
				drawpixelses(vidpos,dat);
				vidpos += 4;
				ptr += 16;
				cnt -= 16;
			}
			while (cnt > 0)
			{
				drawpixel(vidpos,blackmasklookup[*ptr]);
				vidpos++;
				ptr += 4;
				cnt -= 4;
			}
		}
	}
}

drawdot(char col)
{
	long vidpos, dat, xoff, yoff;

	updatepanning();

	xoff = 0; yoff = 0;
	if ((xdim <= 320) && (ydim <= 200))
	{
		xoff = 160L-(xdim>>1)-(long)((signed char)((picanm[picnum]>>8)&255));
		yoff = 100L-(ydim>>1)-(long)((signed char)((picanm[picnum]>>16)&255));
	}

	dat = col;
	if ((xdim|ydim) == 0) return(-1);
	outp(0x3c4,2); outp(0x3c5,1<<((c+xoff)&3));
	vidpos = (((((d+yoff)+16-panyoff)*panxdim)+(c+xoff))>>2)+0xa0000;
	drawpixel(vidpos,dat);

	return(0);
}

updatepanning()
{
	long num, opanyoff;

	panx = 0;
	pany = 0;
	if (xdim > 320)
	{
		panx = c-160;
		if (panx < 0) panx = 0;
		if (panx > xdim-320) panx = xdim-320;
	}
	if (ydim > 184)
	{
		pany = d-92;
		if (pany < 0) pany = 0;
		if (pany > ydim-184) pany = ydim-184;
	}

	opanyoff = panyoff;
	while ((pany > panyoff+56) && (panyoff < 512)) panyoff += 56;
	while ((pany < panyoff) && (panyoff > 0)) panyoff -= 56;

	num = (pany-panyoff+16)*panxdim+panx;
	outp(0x3d4,0xc); outp(0x3d5,num>>10);
	outp(0x3d4,0xd); outp(0x3d5,(num>>2)&255);
	outp(0x3c0,inp(0x3c0)|0x13); outp(0x3c0,(num&3)<<1);

	if (panyoff != opanyoff)
		copywalltoscreen(buf,picnum,0);
}

updatepanningforumode(long dax, long day)
{
	long num;

	panxforu = 0;
	panyforu = 0;
	if (xres > 320)
	{
		panxforu = dax-160;
		if (panxforu < 0) panxforu = 0;
		if (panxforu > xres-320) panxforu = xres-320;
	}
	if (yres > 200)
	{
		panyforu = day-100;
		if (panyforu < 0) panyforu = 0;
		if (panyforu > yres-200) panyforu = yres-200;
	}

	num = panyforu*panxdim+panxforu;
	outp(0x3d4,0xc); outp(0x3d5,num>>10);
	outp(0x3d4,0xd); outp(0x3d5,(num>>2)&255);
	outp(0x3c0,inp(0x3c0)|0x13); outp(0x3c0,(num&3)<<1);
}

drawcolordot(char thecol)
{
	long vidpos, dat;

	outp(0x3c4,2); outp(0x3c5,1<<(((col&31)*3+1)&3));
	vidpos = ((((col>>5)*2)*panxdim+((col&31)*3+225))>>2)+0xa0000;
	drawpixel(vidpos,(long)thecol);
	drawpixel(vidpos+(panxdim>>2),(long)thecol);
}

loadpicture(char *filename)
{
	long i;

	outp(0x3c4,2); outp(0x3c5,0xf);
	clearbuf(0xa0000,16384L,0L); //clear frame (for small pictures)

	i = 0;
	while (filename[i] != '.') i++;
	switch(filename[i+1])
	{
		case 'B': loadbmp(filename); break;
		case 'P': loadpcx(filename); break;
		case 'G': loadgif(filename); break;
	}

	outp(0x3c7,0);
	for(i=0;i<768;i++)
		palette2[i] = inp(0x3c9);
	getpaletteconversion();
}

loadbmp(char *filename)
{
	long fil, i, j;

	if ((fil = open(filename,O_BINARY|O_RDWR,S_IREAD)) == -1)
		return(-1);
	read(fil,&tempbuf[0],4);
	if ((tempbuf[0] == 'B') && (tempbuf[1] == 'M') && (tempbuf[2] == 'Z') && (tempbuf[3] == 34))
	{
		read(fil,&tempbuf[0],22);
		read(fil,&tempbuf[0],768);
		outp(0x3c8,0);
		for(i=0;i<256;i++)
		{
			outp(0x3c9,tempbuf[i+i+i+2]>>2);
			outp(0x3c9,tempbuf[i+i+i+1]>>2);
			outp(0x3c9,tempbuf[i+i+i+0]>>2);
		}
	}
	else
	{
		read(fil,&tempbuf[0],50);
		read(fil,&tempbuf[0],1024);
		outp(0x3c8,0);
		for(i=0;i<1024;i++)
			if ((i&3) != 0)
				outp(0x3c9,tempbuf[i^3]>>2);
	}

	xres = 320;
	yres = 200;

	for(j=0;j<200;j++)
	{
		read(fil,&tempbuf[0],320);
		for(i=0;i<320;i++)
		{
			outp(0x3c4,2); outp(0x3c5,1<<(i&3));
			drawpixel((((199-j)*panxdim+i)>>2)+0xa0000,tempbuf[i]);
		}
	}
	close(fil);
	return(0);
}

loadpcx(char *filename)
{
	unsigned char dat;
	long fil, i, x, y, datcnt, leng;

	if ((fil = open(filename,O_BINARY|O_RDWR,S_IREAD)) == -1)
		return(-1);
	read(fil,&tempbuf[0],128);

	x = 0;
	y = 0;
	xres = ((long)tempbuf[12])+(((long)tempbuf[13])<<8);
	yres = ((long)tempbuf[14])+(((long)tempbuf[15])<<8);

	read(fil,&tempbuf[0],4096), datcnt = 0;
	while (y < yres)
	{
		dat = tempbuf[datcnt++];
		if (datcnt == 4096)
			read(fil,&tempbuf[0],4096), datcnt = 0;
		if ((dat&0xc0) == 0xc0)
		{
			leng = (dat&0x3f);
			dat = tempbuf[datcnt++];
			if (datcnt == 4096)
				read(fil,&tempbuf[0],4096), datcnt = 0;
			for(i=0;i<leng;i++)
			{
				if (y < 256)
				{
					outp(0x3c4,0x2); outp(0x3c5,1<<(x&3));
					drawpixel((((y*panxdim)+x)>>2)+0xa0000,dat);
				}
				x++;
				if (x >= xres)
				{
					x = 0;
					y++;
				}
			}
		}
		else
		{
			if (y < 256)
			{
				outp(0x3c4,0x2); outp(0x3c5,1<<(x&3));
				drawpixel((((y*panxdim)+x)>>2)+0xa0000,dat);
			}
			x++;
			if (x >= xres)
			{
				x = 0;
				y++;
			}
		}
	}
	dat = tempbuf[datcnt++];
	if (datcnt == 4096)
		read(fil,&tempbuf[0],4096), datcnt = 0;
	if (dat == 0xc)
	{
		outp(0x3c8,0);
		for(i=0;i<768;i++)
		{
			dat = tempbuf[datcnt++];
			if (datcnt == 4096)
				read(fil,&tempbuf[0],4096), datcnt = 0;
			outp(0x3c9,dat>>2);
		}
	}
	close(fil);

	return(0);
}

loadgif(char *filename)
{
	unsigned char header[13], imagestat[10], bitcnt, numbits;
	unsigned char globalflag, chunkind;
	short i, j, k, m;
	short currstr, bytecnt, numbitgoal;
	short numcols, numpals, dat, fil, blocklen, firstring;
	short unsigned int numlines, gifdatacnt;
	long x, y;

	if ((strstr(filename,".gif") == 0) && (strstr(filename,".GIF") == 0))
		strcat(filename,".gif");
	if ((fil = open(filename,O_BINARY|O_RDONLY,S_IREAD)) == -1)
		return(-1);
	gifdatacnt = 0;
	read(fil,&gifdata[0],(unsigned)GIFBUFLEN);
	for(j=0;j<13;j++)
		header[j] = gifdata[j+gifdatacnt];
	gifdatacnt += 13;
	if ((header[0] != 'G') || (header[1] != 'I') || (header[2] != 'F'))
		return(-1);
	globalflag = header[10];
	numcols = (1<<((globalflag&7)+1));
	firstring = numcols+2;
	if (header[12] != 0)
		return(-1);
	if ((globalflag&128) > 0)
	{
		numpals = numcols+numcols+numcols;
		for(j=0;j<numpals;j++)
			giffilbuffer[j] = gifdata[j+gifdatacnt];
		gifdatacnt += numpals;
	}
	chunkind = gifdata[gifdatacnt], gifdatacnt++;
	while (chunkind == '!')
	{
		gifdatacnt++;
		do
		{
			chunkind = gifdata[gifdatacnt], gifdatacnt++;
			if (chunkind > 0)
				gifdatacnt += chunkind;
		}
		while (chunkind > 0);
		chunkind = gifdata[gifdatacnt], gifdatacnt++;
	}
	if (chunkind == ',')
	{
		for(j=0;j<9;j++)
			imagestat[j] = gifdata[j+gifdatacnt];
		gifdatacnt += 9;
		xres = imagestat[4]+(imagestat[5]<<8);
		yres = imagestat[6]+(imagestat[7]<<8);
		if ((imagestat[8]&128) > 0)        //localflag
		{
			numpals = numcols+numcols+numcols;
			for(j=0;j<numpals;j++)
				giffilbuffer[j] = gifdata[j+gifdatacnt];
			gifdatacnt += numpals;
		}
		gifdatacnt++;

		outp(0x3c8,0);
		for(i=0;i<numpals;i++)
			outp(0x3c9,giffilbuffer[i]>>2);

		x = 0;
		y = 0;

		bitcnt = 0;
		for(i=0;i<numcols;i++)
		{
			gifsuffix[i] = i;
			gifprefix[i] = i;
		}
		currstr = firstring;
		numbits = (globalflag&7)+2;
		numbitgoal = (numcols<<1);
		blocklen = 0;
		blocklen = gifdata[gifdatacnt], gifdatacnt++;
		for(j=0;j<blocklen;j++)
			giffilbuffer[j] = gifdata[j+gifdatacnt];
		gifdatacnt += blocklen;
		bytecnt = 0;
		while (y < yres)
		{
			dat = gifgetdat(bytecnt,numbits,bitcnt);
			bytecnt += ((bitcnt+numbits)>>3);
			bitcnt = ((bitcnt+numbits)&7);
			if (bytecnt > blocklen-3)
			{
				giffilbuffer[0] = giffilbuffer[bytecnt];
				giffilbuffer[1] = giffilbuffer[bytecnt+1];
				i = blocklen-bytecnt;
				blocklen = (short)gifdata[gifdatacnt++];
				if (gifdatacnt+blocklen < GIFBUFLEN)
				{
					for(m=0;m<blocklen;m++)
						giffilbuffer[i+m] = gifdata[gifdatacnt+m];
					gifdatacnt += blocklen;
				}
				else
				{
					k = GIFBUFLEN-gifdatacnt;
					for(m=0;m<k;m++)
						giffilbuffer[i+m] = gifdata[gifdatacnt+m];
					read(fil,&gifdata[0],(unsigned)GIFBUFLEN);
					for(m=k;m<blocklen;m++)
						giffilbuffer[i+m] = gifdata[m-k];
					gifdatacnt = blocklen-k;
				}
				bytecnt = 0;
				blocklen += i;
			}
			if (currstr == numbitgoal)
				if (numbits < 12)
				{
					numbits++;
					numbitgoal <<= 1;
				}
			if (dat == numcols)
			{
				currstr = firstring;
				numbits = (globalflag&7)+2;
				numbitgoal = (numcols<<1);
			}
			else
			{
				gifprefix[currstr] = dat;
				k = 0;
				while (dat > numcols+1)
				{
					giftempstack[k++] = gifsuffix[dat];
					dat = gifprefix[dat];
				}
				giftempstack[k++] = gifprefix[dat];
				gifsuffix[currstr-1] = dat;
				gifsuffix[currstr] = dat;
				for(i=k-1;i>=0;i--)
				{
					if (y < 256)
					{
						outp(0x3c4,0x2); outp(0x3c5,1<<(x&3));
						drawpixel((((y*panxdim)+x)>>2)+0xa0000,giftempstack[i]);
					}
					x++;
					if (x >= xres)
					{
						x = 0;
						y++;
					}
				}
				currstr++;
			}
		}
	}
	close(fil);
	return(0);
}

drawmainscreen()
{
	long i, j, lin, templong, dat;

	outp(0x3c4,2); outp(0x3c5,15);
	clearbuf(0xa0000,16384,0L);

	inp(0x3da); outp(0x3c0,0x30); outp(0x3c0,113);  //FIX FOR LINE COMPARE
	outp(0x3d4,0x11); outp(0x3d5,inp(0x3d5)&(255-128)); //Unlock indeces 0-7
	outp(0x3d4,21); outp(0x3d5,142);

	lin = 399 - 32;
	outp(0x3d4,0x18); outp(0x3d5,lin&255);
	outp(0x3d4,0x7); outp(0x3d5,(inp(0x3d5)&239)|((lin&256)>>4));
	outp(0x3d4,0x9); outp(0x3d5,(inp(0x3d5)&191)|((lin&512)>>3));

	outp(0x3c8,0);
	for(i=0;i<768;i++)
		outp(0x3c9,palette[i]);
	for(i=0;i<32;i++)
		for(j=0;j<8;j++)
		{
			templong = (j*2*panxdim)+(i*3+224);
			dat = (j<<5)+i;
			drawchainpixel(templong,dat);
			drawchainpixel(templong+1,dat);
			drawchainpixel(templong+2,dat);
			drawchainpixel(templong+panxdim,dat);
			drawchainpixel(templong+panxdim+1,dat);
			drawchainpixel(templong+panxdim+2,dat);
		}

	showall(buf);
	drawdot((buf[(d<<10)+c]+16)&255);
	drawcolordot((col+16)&255);
}

drawxorbox(long x1, long y1, long x2, long y2, char col)
{
	long i, p, dat, temp;

	dat = (long)col;
	dat += (dat<<8);
	if (x1 > x2) temp = x1, x1 = x2, x2 = temp;
	if (y1 > y2) temp = y1, y1 = y2, y2 = temp;

	outp(0x3c4,2); outp(0x3c5,1<<(x1&3));
	outp(0x3ce,4); outp(0x3cf,x1&3);
	p = 0xa0000 + (((y1+1)*panxdim+x1)>>2);
	for(i=y1+1;i<y2;i++)
	{
		drawpixel(p,getpixel(p)^col);
		p += (panxdim>>2);
	}

	outp(0x3c4,2); outp(0x3c5,1<<(x2&3));
	outp(0x3ce,4); outp(0x3cf,x2&3);
	p = 0xa0000 + (((y1+1)*panxdim+x2)>>2);
	for(i=y1+1;i<y2;i++)
	{
		drawpixel(p,getpixel(p)^col);
		p += (panxdim>>2);
	}

	for(i=x1;i<=x2;i++)
	{
		outp(0x3c4,2); outp(0x3c5,1<<(i&3));
		outp(0x3ce,4); outp(0x3cf,i&3);

		p = 0xa0000 + ((y1*panxdim+i)>>2);
		drawpixel(p,getpixel(p)^col);

		p = 0xa0000 + ((y2*panxdim+i)>>2);
		drawpixel(p,getpixel(p)^col);
	}

	return(0);
}

drawmaskedbox(long x1, long y1, long x2, long y2, char col, char mask1, char mask2)
{
	long i, j, p, pinc, dat, temp;

	dat = (long)col; dat += (dat<<8); dat += (dat<<16);
	if (x1 > x2) temp = x1, x1 = x2, x2 = temp;
	if (y1 > y2) temp = y1, y1 = y2, y2 = temp;

	pinc = (panxdim>>1);

	outp(0x3c4,2);
	outp(0x3c5,mask1);
	for(j=y1;j<y2;j+=2)
	{
		p = 0xa0000 + ((j*panxdim+x1)>>2);
		for(i=((x2-x1)>>4);i>0;i--) { drawpixelses(p,dat); p += 4; }
	}

	outp(0x3c5,mask2);
	for(j=(y1+1);j<y2;j+=2)
	{
		p = 0xa0000 + ((j*panxdim+x1)>>2);
		for(i=((x2-x1)>>4);i>0;i--) { drawpixelses(p,dat); p += 4; }
	}

	return(0);
}

getpaletteconversion()
{
	long i, j, totintens1, totintens2, brichang, c1, c2, c3, bestcol, dadist;
	long zx;

	totintens1 = 0;
	totintens2 = 0;
	for(i=0;i<768;i++)
	{
		totintens1 += palette[i];
		totintens2 += palette2[i];
	}
	brichang = (totintens1-totintens2)/768;
	for(i=0;i<768;i+=3)
	{
		c1 = palette2[i]+brichang;
		c2 = palette2[i+1]+brichang;
		c3 = palette2[i+2]+brichang;
		bestcol = 0;
		dadist = 65536;
		for(j=0;j<768;j+=3)
		{
			zx = 30*(c1-palette[j])*(c1-palette[j]);
			zx += 59*(c2-palette[j+1])*(c2-palette[j+1]);
			zx += 11*(c3-palette[j+2])*(c3-palette[j+2]);
			if (zx < dadist)
				dadist = zx, bestcol = j/3;
		}
		palookupe[i/3] = bestcol;
	}
	palookupe[255] = 255;
}

fillregion(long x, long y, char col, char bound)
{
	unsigned char tstat, bstat, tlast, blast;
	int i, leftz, z, zz, c;

	c = 1;
	xfillbuf[c] = x;
	yfillbuf[c] = y;
	if (buf[(y<<10)+x] == bound)
		return(-1);
	while (c > 0)
	{
		z = xfillbuf[c];
		zz = yfillbuf[c];
		c--;
		while ((buf[(zz<<10)+(z-1)] != bound) && (z > 0))
			z--;
		leftz = z;
		tlast = 0;
		blast = 0;
		while ((buf[(zz<<10)+z] != bound) && (z < xdim))
		{
			if (zz > 0)
			{
				tstat = buf[((zz-1)<<10)+z];
				if ((tstat != bound) && (tstat != col))
				{
					if (tlast == 0)
					{
						c++;
						xfillbuf[c] = z;
						yfillbuf[c] = zz-1;
						tlast = 1;
					}
				}
				else
					tlast = 0;
			}
			if (zz < ydim-1)
			{
				bstat = buf[((zz+1)<<10)+z];
				if ((bstat != bound) && (bstat != col))
				{
					if (blast == 0)
					{
						c++;
						xfillbuf[c] = z;
						yfillbuf[c] = zz+1;
						blast = 1;
					}
				}
				else
					blast = 0;
			}
			z++;
		}
		z--;
		while (z >= leftz)
		{
			if ((z >= 0) && (z < xdim))
				buf[(zz<<10)+z] = col;
			z--;
		}
	}
	return(0);
}

getpalookup()
{
	long i, j, k, dist, mindist, c1, c2, c3, closest;

	for(i=0;i<NUMPALOOKUPS;i++)
	{
		for(j=0;j<256;j++)
		{
			c1 = (palette[j+j+j+0]*((NUMPALOOKUPS-1)-i))/NUMPALOOKUPS;
			c2 = (palette[j+j+j+1]*((NUMPALOOKUPS-1)-i))/NUMPALOOKUPS;
			c3 = (palette[j+j+j+2]*((NUMPALOOKUPS-1)-i))/NUMPALOOKUPS;

			mindist = 32767, closest = -1;
			for(k=0;k<256;k++)
			{
				dist = (c1-palette[k+k+k+0])*(c1-palette[k+k+k+0]);
				dist += (c2-palette[k+k+k+1])*(c2-palette[k+k+k+1]);
				dist += (c3-palette[k+k+k+2])*(c3-palette[k+k+k+2]);
				if (dist < mindist)
				{
					mindist = dist;
					closest = k;
				}
			}

			palookup[i][j] = closest;
		}
	}
}

gettile(long tilenum)
{
	char ch, xorcol;
	long i, j, lin, num, otilenum, topleft, x1, y1, x2, y2;
	long movetilenum, movetilenum1, movetilenum2, templong;

	lin = 400;
	outp(0x3d4,0x18); outp(0x3d5,lin&255);
	outp(0x3d4,0x7); outp(0x3d5,(inp(0x3d5)&239)|((lin&256)>>4));
	outp(0x3d4,0x9); outp(0x3d5,(inp(0x3d5)&191)|((lin&512)>>3));
	num = 0;
	outp(0x3d4,0xc); outp(0x3d5,num>>10);
	outp(0x3d4,0xd); outp(0x3d5,(num>>2)&255);
	outp(0x3c0,inp(0x3c0)|0x13); outp(0x3c0,(num&3)<<1);

	panxdim = 512;
	outp(0x3d4,0x9); outp(0x3d5,inp(0x3d5) & ~1);
	outp(0x3d4,0x13); outp(0x3d5,panxdim>>3);

	otilenum = tilenum;
	movetilenum = -1;
	movetilenum1 = -1;
	movetilenum2 = -1;

	topleft = ((tilenum/(5<<gettilezoom))*(5<<gettilezoom))-(5<<gettilezoom);
	if (topleft < 0) topleft = 0;
	if (topleft > MAXTILES-(15<<(gettilezoom<<1))) topleft = MAXTILES-(15<<(gettilezoom<<1));
	drawtilescreen(topleft);

	xorcol = (rand()&255);
	x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
	y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
	x2 = x1+(64>>gettilezoom)-1;
	y2 = y1+(128>>gettilezoom)-1;
	drawxorbox(x1,y1,x2,y2,xorcol);

	ch = 0;
	while ((ch != 27) && (ch != 13))
	{
		x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
		y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
		x2 = x1+(64>>gettilezoom)-1;
		y2 = y1+(128>>gettilezoom)-1;
		while (kbhit() == 0)
		{
			limitrate();
			drawxorbox(x1,y1,x2,y2,xorcol);
			xorcol = (rand()&255);
			limitrate();
			drawxorbox(x1,y1,x2,y2,xorcol);
		}
		ch = getch();
		if ((ch == '*') && (gettilezoom < 2))
		{
			gettilezoom++;
			topleft = ((tilenum/(5<<gettilezoom))*(5<<gettilezoom))-(5<<gettilezoom);
			if (topleft < 0) topleft = 0;
			if (topleft > MAXTILES-(15<<(gettilezoom<<1))) topleft = MAXTILES-(15<<(gettilezoom<<1));
			drawtilescreen(topleft);

			x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
			y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
			x2 = x1+(64>>gettilezoom)-1;
			y2 = y1+(128>>gettilezoom)-1;
			drawxorbox(x1,y1,x2,y2,xorcol);
		}
		if ((ch == '/') && (gettilezoom > 0))
		{
			gettilezoom--;
			topleft = ((tilenum/(5<<gettilezoom))*(5<<gettilezoom))-(5<<gettilezoom);
			if (topleft < 0) topleft = 0;
			if (topleft > MAXTILES-(15<<(gettilezoom<<1))) topleft = MAXTILES-(15<<(gettilezoom<<1));
			drawtilescreen(topleft);

			x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
			y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
			x2 = x1+(64>>gettilezoom)-1;
			y2 = y1+(128>>gettilezoom)-1;
			drawxorbox(x1,y1,x2,y2,xorcol);
		}

		if (ch == 32)
		{
			if (tilesreported != 0)     //Toggle registered / shareware
			{
				picanm[tilenum] ^= 0x80000000;
				allasksave = 1;
				asksave = 1;
				drawtilescreen(topleft);

				x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
				y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
				x2 = x1+(64>>gettilezoom)-1;
				y2 = y1+(128>>gettilezoom)-1;
				drawxorbox(x1,y1,x2,y2,xorcol);
			}
			else                        //Swap walls
			{
				if (movetilenum == -1)
					movetilenum = tilenum;
				else
				{
					if (swapwalls(tilenum,movetilenum) == 1)
					{
						  //Redraw screen
						drawtilescreen(topleft);

						x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
						y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
						x2 = x1+(64>>gettilezoom)-1;
						y2 = y1+(128>>gettilezoom)-1;
						drawxorbox(x1,y1,x2,y2,xorcol);
					}
					movetilenum = -1;
					movetilenum1 = -1;
					movetilenum2 = -1;
				}
			}
		}
		if (ch == '1') movetilenum1 = tilenum;
		if (ch == '2') movetilenum2 = tilenum;
		if (ch == '3')
		{
			if ((movetilenum1 >= 0) && (movetilenum2 >= 0))
			{
				if (movetilenum1 > movetilenum2)
					templong = movetilenum1, movetilenum1 = movetilenum2, movetilenum2 = templong;

				j = 0;
				for(i=movetilenum1;i<=movetilenum2;i++)
					j |= swapwalls(i,tilenum+i-movetilenum1);

				if (j != 0)
				{
						//Redraw screen
					drawtilescreen(topleft);

					x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
					y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
					x2 = x1+(64>>gettilezoom)-1;
					y2 = y1+(128>>gettilezoom)-1;
					drawxorbox(x1,y1,x2,y2,xorcol);
				}
			}
			movetilenum = -1;
			movetilenum1 = -1;
			movetilenum2 = -1;
		}

		if (ch == 0)
		{
			ch = getch();

			x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
			y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
			x2 = x1+(64>>gettilezoom)-1;
			y2 = y1+(128>>gettilezoom)-1;
			drawxorbox(x1,y1,x2,y2,xorcol);

			if (ch == 19)  //ALT-R (tile frequency report)
			{
				tilesreported ^= 1;
				if (tilesreported == 1) reportmaps();

					//Redraw screen
				drawtilescreen(topleft);
			}
			if (ch == 32)    //Alt-D    DELETE ALL REGISTERED TILES!!!
			{
				drawmaskedbox(0,0,319,7,blackcol,0xff,0xff);
				printext256(0L,0L,whitecol,-2,"DELETE ALL REGISTERED TILES NOW?");
				while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'))
					ch = getch();
				if ((ch == 'y') || (ch == 'Y'))
				{
					drawmaskedbox(0,0,319,15,blackcol,0xff,0xff);
					printext256(0L,0L,whitecol,-2,"DELETING!!!  Now you've done it!");
					printext256(0L,0L,whitecol,-2,"(Have a nice day)");
					for(i=0;i<MAXTILES;i++)
						if (picanm[i]&0x80000000)
						{
							picanm[i] &= ~0x80000000;
							if (tilesizx[i]|tilesizy[i])
							{
								loadpics(i);
								if (resizetile(i,(long)tilesizx[i],(long)tilesizy[i],0L,0L) != -1)
								{
									tilesizx[i] = 0;
									tilesizy[i] = 0;
								}
							}
						}
					allasksave = 1;
					savepics();
				}
				drawtilescreen(topleft);
			}
			if (ch == 22)    //Alt-U
			{
				if ((movetilenum1 >= 0) && (movetilenum2 >= 0))
				{
					if (movetilenum1 > movetilenum2)
						templong = movetilenum1, movetilenum1 = movetilenum2, movetilenum2 = templong;

					updatescript(movetilenum1,movetilenum2);

					outp(0x3c8,0);
					for(i=0;i<768;i++)
						outp(0x3c9,palette[i]);

					drawtilescreen(topleft);
				}
			}
			if (ch == 82)   //Insert tile (localtile trick - fast!)
			{
				loadpics(tilenum);

				for(i=localtileend[curtilefile];i>=tilenum+1;i--)
				{
					tilesizx[i] = tilesizx[i-1];
					tilesizy[i] = tilesizy[i-1];
					picanm[i] = picanm[i-1];
					waloff[i] = waloff[i-1];

					tilookup[i] = tilookup[i-1];

					for(j=0;j<17;j++)
						names[i][j] = names[i-1][j];
				}
				tilesizx[tilenum] = 0;
				tilesizy[tilenum] = 0;
				picanm[tilenum] = 0;
				for(j=0;j<17;j++)
					names[tilenum][j] = 0;

				needtosavenames = 1;
				mustsavelocals = 1;
				allasksave = 1;
				savepics();

				drawtilescreen(topleft);
			}
			if (ch == 83)   //Delete tile (localtile trick - fast!)
			{
				if (tilesreported == 0)
				{
					loadpics(tilenum);

					if (resizetile(tilenum,(long)tilesizx[tilenum],(long)tilesizy[tilenum],0L,0L) != -1)
					{
						for(i=tilenum;i<=localtileend[curtilefile];i++)
						{
							tilesizx[i] = tilesizx[i+1];
							tilesizy[i] = tilesizy[i+1];
							picanm[i] = picanm[i+1];
							waloff[i] = waloff[i+1];

							tilookup[i] = tilookup[i+1];

							for(j=0;j<17;j++)
							  names[i][j] = names[i+1][j];
						}
						tilesizx[localtileend[curtilefile]] = 0;
						tilesizy[localtileend[curtilefile]] = 0;
						picanm[localtileend[curtilefile]] = 0;
						for(j=0;j<17;j++)
							names[localtileend[curtilefile]][j] = 0;

						needtosavenames = 1;
						mustsavelocals = 1;
						allasksave = 1;
						savepics();
					}
					drawtilescreen(topleft);
				}
				else
				{
					loadpics(tilenum);

					if (resizetile(tilenum,(long)tilesizx[tilenum],(long)tilesizy[tilenum],0L,0L) != -1)
					{
						tilesizx[tilenum] = 0;
						tilesizy[tilenum] = 0;
						allasksave = 1;
						savepics();
					}
					drawtilescreen(topleft);
				}
			}
			if ((ch == 75) && (tilenum > 0)) tilenum--;
			if ((ch == 77) && (tilenum < MAXTILES-1)) tilenum++;
			if ((ch == 72) && (tilenum >= (5<<gettilezoom))) tilenum-=(5<<gettilezoom);
			if ((ch == 80) && (tilenum < MAXTILES-(5<<gettilezoom))) tilenum+=(5<<gettilezoom);
			if ((ch == 73) && (tilenum >= (5<<gettilezoom)))
			{
				tilenum-=(15<<(gettilezoom<<1));
				if (tilenum < 0) tilenum = 0;
			}
			if ((ch == 81) && (tilenum < MAXTILES-(5<<gettilezoom)))
			{
				tilenum+=(15<<(gettilezoom<<1));
				if (tilenum >= MAXTILES) tilenum = MAXTILES-1;
			}
			if (tilenum < topleft)
			{
				while (tilenum < topleft)
					topleft -= (5<<gettilezoom);
				if (topleft < 0) topleft = 0;
				drawtilescreen(topleft);
			}
			if (tilenum >= topleft+(15<<(gettilezoom<<1)))
			{
				while (tilenum >= topleft+(15<<(gettilezoom<<1)))
					topleft += (5<<gettilezoom);
				if (topleft > MAXTILES-(15<<(gettilezoom<<1))) topleft = MAXTILES-(15<<(gettilezoom<<1));
				drawtilescreen(topleft);
			}

			x1 = ((tilenum-topleft)%(5<<gettilezoom))*(64>>gettilezoom);
			y1 = ((tilenum-topleft)/(5<<gettilezoom))*(128>>gettilezoom);
			x2 = x1+(64>>gettilezoom)-1;
			y2 = y1+(128>>gettilezoom)-1;
			drawxorbox(x1,y1,x2,y2,xorcol);
		}
	}

	panxdim = 1024;
	outp(0x3d4,0x9); outp(0x3d5,inp(0x3d5) | 1);
	outp(0x3d4,0x13); outp(0x3d5,panxdim>>3);

	if (ch != 13) tilenum = otilenum;
	return(tilenum);
}

drawtilescreen(long pictopleft)
{
	long i, j, k, vidpos, vidpos2, wallnum, xdime, ydime, cnt, scaledown;
	long dat, dat2, ocurtilefile;
	char *picptr, buffer[16];

	outp(0x3c4,2); outp(0x3c5,0xf);
	clearbuf(0xa0000,(512L*400L)>>4,0L); //clear frame first

	loadpics(pictopleft);

	for(cnt=0;cnt<(15<<(gettilezoom<<1));cnt++)  //draw the (5*3)<<gettilezoom grid
	{
		wallnum = cnt+pictopleft;

		ocurtilefile = curtilefile;
		loadpics(wallnum);
		if (ocurtilefile != curtilefile)
		{
			i = (cnt%(5<<gettilezoom))*(64>>gettilezoom);
			j = (cnt/(5<<gettilezoom))*(128>>gettilezoom);
			if (i > 2)
			{
				drawxorbox(i-2,j,i-1,j+(128>>gettilezoom)-1,whitecol);
				drawxorbox(0,j+(128>>gettilezoom)-2,i-3,j+(128>>gettilezoom)-1,whitecol);
			}
			if (j > 2)
			{
				if (i < 2) i = 2;   //Safety precaution
				drawxorbox(i-2,j-2,319,j-1,whitecol);
			}
		}

		if ((tilesizx[wallnum] != 0) && (tilesizy[wallnum] != 0))
		{
			picptr = (char *)(waloff[wallnum]);
			xdime = tilesizx[wallnum];
			ydime = tilesizy[wallnum];

			vidpos = (cnt/(5<<gettilezoom))*(panxdim*(128>>gettilezoom))+(cnt%(5<<gettilezoom))*(64>>gettilezoom);
			if ((xdime <= (64>>gettilezoom)) && (ydime <= (64>>gettilezoom)))
			{
				for(i=0;i<xdime;i++)
				{
					outp(0x3c4,2); outp(0x3c5,1<<(vidpos&3));
					vidpos2 = (vidpos>>2)+0xa0000;
					for(j=0;j<ydime;j++)
					{
						dat = *picptr++;
						drawpixel(vidpos2,dat);
						drawpixel(vidpos2+(panxdim>>2),dat);
						vidpos2 += (panxdim>>1);
					}
					vidpos++;
				}
			}
			else                          //if 1 dimension > (64>>gettilezoom)
			{
				if (xdime > ydime)
					scaledown = ((xdime+(63>>gettilezoom))>>(6-gettilezoom));
				else
					scaledown = ((ydime+(63>>gettilezoom))>>(6-gettilezoom));

				for(i=0;i<xdime;i+=scaledown)
				{
					picptr = (char *)(waloff[wallnum]) + ydime*i;

					outp(0x3c4,2); outp(0x3c5,1<<(vidpos&3));
					vidpos2 = (vidpos>>2)+0xa0000;

					j = 0;
					while (j < ydime)
					{
						dat = *picptr; drawpixel(vidpos2,dat);
						picptr += (scaledown>>1);

						if (j+(scaledown>>1) >= ydime)
							break;

						dat2 = *picptr; drawpixel(vidpos2+(panxdim>>2),dat2);
						picptr += scaledown-(scaledown>>1);

						vidpos2 += (panxdim>>1);
						j += scaledown;
					}
					vidpos++;
				}
			}
		}

		if (tilesreported == 1)
		{
			i = (cnt%(5<<gettilezoom))*(64>>gettilezoom);
			j = (cnt/(5<<gettilezoom))*(128>>gettilezoom);
			if (picanm[wallnum]&0x80000000)
			{
				drawmaskedbox(i,j,i+(64>>gettilezoom),j+(128>>gettilezoom),whitecol,0x55,0xaa);
				printext256(i+(32>>gettilezoom)-11,j+(64>>gettilezoom)-3,blackcol,-1,"Pay");
				printext256(i+(32>>gettilezoom)-12,j+(64>>gettilezoom)-4,whitecol,-1,"Pay");
			}
			sprintf(buffer,"%ld",tilookup2[wallnum]);
			printext256(i,j,whitecol,blackcol,buffer);
		}
	}
	return(0);
}

loadnames()
{
	char buffer[160], firstch, ch;
	short int fil, i, num, buffercnt;

	if ((fil = open("names.h",O_BINARY|O_RDWR,S_IREAD)) == -1)
		return(-1);

	do
	{
		i = read(fil,&firstch,1);
	}
	while ((firstch != '#') && (i > 0));

	while ((firstch == '#') || (firstch == '/'))
	{
		do
		{
			read(fil,&ch,1);
		}
		while (ch > 32);

		buffercnt = 0;
		do
		{
			read(fil,&ch,1);
			if (ch > 32)
				buffer[buffercnt++] = ch;
		}
		while (ch > 32);

		num = 0;
		do
		{
			read(fil,&ch,1);
			if ((ch >= 48) && (ch <= 57))
				num = num*10+(ch-48);
		}
		while (ch != 13);
		for(i=0;i<buffercnt;i++)
			names[num][i] = buffer[i];
		names[num][buffercnt] = 0;

		if (read(fil,&firstch,1) <= 0)
			firstch = 0;
		if (firstch == 10)
			read(fil,&firstch,1);
	}
	close(fil);
	return(0);
}

savenames()
{
	char buffer[160];
	short int fil, i, j;

	if (needtosavenames == 0)
		return;

	i = 0;
	while (curpath[i] != 0) i++;
	curpath[i] = 92;
	curpath[i+1] = 0;
	chdir(curpath);
	curpath[i] = 0;

	if ((fil = open("names.h",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		return;

	strcpy(&buffer,"//Be careful when changing this file - it is parsed by Editart and Build.");
	buffer[73] = 13, buffer[74] = 10, buffer[75] = 0;
	write(fil,&buffer[0],75);

	strcpy(&buffer,"#define ");
	for(i=0;i<MAXTILES;i++)
		if (names[i][0] != 0)
		{
			j = 8;
			while (names[i][j-8] != 0)
			{
				buffer[j] = names[i][j-8];
				j++;
			}
			buffer[j++] = 32;

			if (i >= 10000) buffer[j++] = ((i/10000)%10)+48;
			if (i >= 1000) buffer[j++] = ((i/1000)%10)+48;
			if (i >= 100) buffer[j++] = ((i/100)%10)+48;
			if (i >= 10) buffer[j++] = ((i/10)%10)+48;
			buffer[j++] = (i%10)+48;

			buffer[j++] = 13;
			buffer[j++] = 10;
			write(fil,&buffer[0],j);
		}

	close(fil);
	return;
}

initmenupaths(char *filename)
{
	long i;

	strcpy(&curpath,filename);
	i = 0;
	while ((i < 160) && (curpath[i] != 0)) i++;
	while ((i > 0) && (curpath[i] != 92)) i--;
	curpath[i] = 0;
	strcpy(&menupath,curpath);
}

getfilenames(char *kind)
{
	short type;
	struct find_t fileinfo;

	if (strcmp(kind,"SUBD") == 0)
	{
		strcpy(kind,"*.*");
		if (_dos_findfirst(kind,_A_SUBDIR,&fileinfo) != 0)
			return(-1);
		type = 1;
	}
	else
	{
		if (_dos_findfirst(kind,_A_NORMAL,&fileinfo) != 0)
			return(-1);
		type = 0;
	}
	do
	{
		if ((type == 0) || ((fileinfo.attrib&16) > 0))
			if ((fileinfo.name[0] != '.') || (fileinfo.name[1] != 0))
			{
				if (menunamecnt < 256)
				{
					strcpy(menuname[menunamecnt],fileinfo.name);
					menuname[menunamecnt][16] = type;
					menunamecnt++;
				}
				else
					printmessage("Too many files! (max=256)");
			}
	}
	while (_dos_findnext(&fileinfo) == 0);

	return(0);
}

sortfilenames()
{
	char sortbuffer[17];
	long i, j, k, m;

	for(i=1;i<menunamecnt;i++)
		for(j=0;j<i;j++)
		{
			k = 0;
			while ((menuname[i][k] == menuname[j][k]) && (menuname[i][k] != 0) && (menuname[j][k] != 0))
				k++;
			if (menuname[i][k] < menuname[j][k])
			{
				memcpy(&sortbuffer[0],&menuname[i][0],sizeof(menuname[0]));
				memcpy(&menuname[i][0],&menuname[j][0],sizeof(menuname[0]));
				memcpy(&menuname[j][0],&sortbuffer[0],sizeof(menuname[0]));
			}
		}
}

menuselect()
{
	long newhighlight, i, j, topplc;
	char ch, buffer[42];

	printmessage("Select with arrows&enter.");

	chdir(menupath);
	menunamecnt = 0;
	getfilenames("SUBD");
	getfilenames("*.BMP");
	getfilenames("*.PCX");
	getfilenames("*.GIF");
	sortfilenames();
	if (menunamecnt == 0)
		return(-2);

	cleartopbox();
	if (menuhighlight < 0) menuhighlight = 0;
	if (menuhighlight >= menunamecnt) menuhighlight = menunamecnt-1;

	newhighlight = menuhighlight;
	do
	{
		if (menunamecnt <= 22)
		{
			topplc = 0;
		}
		else
		{
			topplc = newhighlight-11;
			if (topplc < 0) topplc = 0;
			if (topplc > menunamecnt-23) topplc = menunamecnt-23;
		}

		for(i=0;i<menunamecnt;i++)
		{
			for(j=0;j<24;j++)
				buffer[j] = 32;
			buffer[24] = 0;
			if (i == newhighlight)
			{
				buffer[0] = '-';
				buffer[1] = '-';
				buffer[2] = '>';
			}
			j = 0;
			while (menuname[i][j] != 0)
			{
				buffer[j+4] = menuname[i][j];
				j++;
			}
			if ((i-topplc >= 0) && (i-topplc <= 22))
			{
				if (menuname[i][16] == 1)
				{
					printext256(panx,((i-topplc)<<3)+16L+pany-panyoff,greencol,blackcol,buffer);
				}
				else
				{
					printext256(panx,((i-topplc)<<3)+16L+pany-panyoff,whitecol,blackcol,buffer);
				}
			}
		}
		ch = getch();
		if (ch == 0)
		{
			ch = getch();
			if ((ch == 75) || (ch == 72))
			{
				newhighlight--;
				if (newhighlight < 0)
					newhighlight = menunamecnt-1;
			}
			if ((ch == 77) || (ch == 80))
			{
				newhighlight++;
				if (newhighlight >= menunamecnt)
					newhighlight = 0;
			}
		}
		if ((ch == 13) && (menuname[newhighlight][16] == 1))
		{
			if ((menuname[newhighlight][0] == '.') && (menuname[newhighlight][1] == '.'))
			{
				i = 0;
				while ((i < 160) && (menupath[i] != 0)) i++;
				while ((i > 0) && (menupath[i] != 92)) i--;
				menupath[i] = 0;
			}
			else
			{
				strcat(&menupath,"\\");
				strcat(&menupath,menuname[newhighlight]);
			}
			chdir(menuname[newhighlight]);
			menunamecnt = 0;
			getfilenames("SUBD");
			getfilenames("*.BMP");
			getfilenames("*.PCX");
			getfilenames("*.GIF");
			sortfilenames();
			newhighlight = 0;
			ch = 0;
			cleartopbox();
		}
	}
	while ((ch != 13) && (ch != 27));
	if (ch == 13)
	{
		menuhighlight = newhighlight;
		return(newhighlight);
	}
	cleartopbox();
	return(-1);
}

printext256(long xpos, long ypos, short col, short backcol, char *name)
{
	char ch, damask;
	short shift;
	long p, startp, z, zz, zzz;

	outp(0x3c4,2);
	startp = ((ypos*panxdim+xpos)>>2)+0xa0000;

	z = 0;
	while (name[z] != 0)
	{
		ch = name[z];
		z++;

		if ((xpos&3) == 0)
		{
			p = startp;
			for(zz=0;zz<8;zz++)
			{
				if (backcol >= 0)
				{
					outp(0x3c5,15-reversemask[textfont[ch][zz]>>4]);
					drawpixel(p,backcol);
					outp(0x3c5,15-reversemask[textfont[ch][zz]&15]);
					drawpixel(p+1,backcol);
				}
				if (backcol == -2)
				{
					damask = reversemask[textfont[ch][zz]>>4];
					for(zzz=0;zzz<4;zzz++)
					{
						if ((damask&(1<<zzz)) > 0)
						{
							outp(0x3c5,1<<zzz);
							outp(0x3ce,4); outp(0x3cf,zzz);
							drawpixel(p,getpixel(p)^col);
						}
					}
					damask = reversemask[textfont[ch][zz]&15];
					for(zzz=0;zzz<4;zzz++)
					{
						if ((damask&(1<<zzz)) > 0)
						{
							outp(0x3c5,1<<zzz);
							outp(0x3ce,4); outp(0x3cf,zzz);
							drawpixel(p+1,getpixel(p+1)^col);
						}
					}
				}
				else
				{
					outp(0x3c5,reversemask[textfont[ch][zz]>>4]);
					drawpixel(p,col);
					outp(0x3c5,reversemask[textfont[ch][zz]&15]);
					drawpixel(p+1,col);
				}
				p += (panxdim>>2);
			}
		}
		else
		{
			shift = (xpos&3);

			p = startp;
			for(zz=0;zz<8;zz++)
			{
				if (backcol >= 0)
				{
					outp(0x3c5,(16-(1<<shift)) - (reversemask[(textfont[ch][zz]>>(shift+4))&reversemask[16-(1<<shift)]]));
					drawpixel(p,backcol);
					outp(0x3c5,15-reversemask[(textfont[ch][zz]>>shift)&15]);
					drawpixel(p+1,backcol);
					outp(0x3c5,((1<<shift)-1) - (reversemask[(textfont[ch][zz]<<(4-shift))&reversemask[(1<<shift)-1]]));
					drawpixel(p+2,backcol);
				}
				if (backcol == -2)
				{
					damask = reversemask[(textfont[ch][zz]>>(shift+4))&reversemask[16-(1<<shift)]];
					for(zzz=0;zzz<4;zzz++)
					{
						if ((damask&(1<<zzz)) > 0)
						{
							outp(0x3c5,1<<zzz);
							outp(0x3ce,4); outp(0x3cf,zzz);
							drawpixel(p,getpixel(p)^col);
						}
					}
					damask = reversemask[(textfont[ch][zz]>>shift)&15];
					for(zzz=0;zzz<4;zzz++)
					{
						if ((damask&(1<<zzz)) > 0)
						{
							outp(0x3c5,1<<zzz);
							outp(0x3ce,4); outp(0x3cf,zzz);
							drawpixel(p+1,getpixel(p+1)^col);
						}
					}
					damask = reversemask[(textfont[ch][zz]<<(4-shift))&reversemask[(1<<shift)-1]];
					for(zzz=0;zzz<4;zzz++)
					{
						if ((damask&(1<<zzz)) > 0)
						{
							outp(0x3c5,1<<zzz);
							outp(0x3ce,4); outp(0x3cf,zzz);
							drawpixel(p+2,getpixel(p+2)^col);
						}
					}
				}
				else
				{
					outp(0x3c5,reversemask[(textfont[ch][zz]>>(shift+4))&reversemask[16-(1<<shift)]]);
					drawpixel(p,col);
					outp(0x3c5,reversemask[(textfont[ch][zz]>>shift)&15]);
					drawpixel(p+1,col);
					outp(0x3c5,reversemask[(textfont[ch][zz]<<(4-shift))&reversemask[(1<<shift)-1]]);
					drawpixel(p+2,col);
				}
				p += (panxdim>>2);
			}
		}

		startp += 2;
	}
	return(0);
}

printmessage(char name[82])
{
	cleartext();
	printext256(0L,8L,yellowcol,browncol,name);
}

screencapture()
{
	char filename[15], *ptr;
	long fil, i, bufplc, p, col, ncol, leng, x, y, capturecount;

	menunamecnt = 0;
	getfilenames("CAPT????.PCX");
	sortfilenames();

	if (menunamecnt >= 256)
	{
		printmessage("MAXIMUM FILES is 256!");
		return(-1);
	}

	capturecount = 0;
	if (menunamecnt > 0)
	{
		capturecount = (menuname[menunamecnt-1][7]-48);
		capturecount += (menuname[menunamecnt-1][6]-48)*10;
		capturecount += (menuname[menunamecnt-1][5]-48)*100;
		capturecount += (menuname[menunamecnt-1][4]-48)*1000;
		capturecount++;
	}

	strcpy(filename,"captxxxx.pcx");
	filename[4] = ((capturecount/1000)%10)+48;
	filename[5] = ((capturecount/100)%10)+48;
	filename[6] = ((capturecount/10)%10)+48;
	filename[7] = (capturecount%10)+48;
	if ((fil=open(filename,O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IWRITE))==-1)
		return(-1);

	pcxheader[8] = ((xdim-1)&255); pcxheader[9] = (((xdim-1)>>8)&255);
	pcxheader[10] = ((ydim-1)&255); pcxheader[11] = (((ydim-1)>>8)&255);
	pcxheader[12] = (xdim&255); pcxheader[13] = ((xdim>>8)&255);
	pcxheader[14] = (ydim&255); pcxheader[15] = ((ydim>>8)&255);
	pcxheader[66] = (xdim&255); pcxheader[67] = ((xdim>>8)&255);

	write(fil,&pcxheader[0],128);

	bufplc = 0;
	x = 0; y = 0;
	while (y < ydim)
	{
		outp(97,inp(97)|3);

		col = buf[(y<<10)+x];
		x++; if (x == xdim) x = 0, y++;
		ncol = buf[(y<<10)+x];

		leng = 1;

		while ((ncol == col) && (x != 0) && (leng < 63))
		{
			leng++;

			x++; if (x == xdim) x = 0, y++;
			if (y > ydim) break;
			ncol = buf[(y<<10)+x];
		}

		outp(97,inp(97)&252);

		if ((leng > 1) || (col >= 0xc0))
		{
			tempbuf[bufplc++] = (leng|0xc0);
			if (bufplc == 256)
			{
				bufplc = 0;
				if (write(fil,&tempbuf[0],256) < 256)
					{ close(fil); capturecount--; return(-1); }
			}
		}
		tempbuf[bufplc++] = col;
		if (bufplc == 256)
		{
			bufplc = 0;
			if (write(fil,&tempbuf[0],256) < 256)
				{ close(fil); capturecount--; return(-1); }
		}
	}

	tempbuf[bufplc++] = 0xc;
	if (bufplc == 256)
	{
		bufplc = 0;
		if (write(fil,&tempbuf[0],256) < 256)
			{ close(fil); capturecount--; return(-1); }
	}

	outp(0x3c7,0);
	for(i=0;i<768;i++)
	{
		tempbuf[bufplc++] = (inp(0x3c9)<<2);
		if (bufplc == 256)
		{
			bufplc = 0;
			if (write(fil,&tempbuf[0],256) < 256)
				{ close(fil); capturecount--; return(-1); }
		}
	}
	if (bufplc > 0)
		if (write(fil,&tempbuf[0],bufplc) < bufplc)
			{ close(fil); return(-1); }
	close(fil);
	return(0);
}

resizetile(long picnume, long oldx, long oldy, long newx, long newy)
{
	char *charptr1, *charptr2;
	long *longptr1, *longptr2;
	long i, j, dat, templong;

	templong = newx*newy-oldx*oldy;

	if (totpicsiz+templong > totpicmem)
		return(-1);

	j = 0L;
	for(i=localtilestart[curtilefile];i<picnume;i++)
		j += tilesizx[i]*tilesizy[i];
	if (templong < 0)
	{
		longptr1 = (long *)(FP_OFF(pic)+j-templong);
		longptr2 = (long *)(FP_OFF(pic)+j);
		i = j-templong;
		while (i < totpicsiz-3)
		{
			dat = *longptr1++;
			*longptr2++ = dat;
			i += 4;
		}
		charptr1 = (char *)(longptr1);
		charptr2 = (char *)(longptr2);
		while (i < totpicsiz)
		{
			dat = *charptr1++;
			*charptr2++ = dat;
			i++;
		}

		if ((newx|newy) == 0)
			picanm[picnume] = 0L;
	}
	else if (templong > 0)
	{
			//Be careful with long pointers!  The offset of 3 is because
			//long pointers still point to the lowest of the 4 bytes

		longptr1 = (long *)(FP_OFF(pic)+totpicsiz-1 - 3);
		longptr2 = (long *)(FP_OFF(pic)+totpicsiz-1+templong - 3);
		i = j;
		while (i < totpicsiz-3)
		{
			dat = *longptr1--;
			*longptr2-- = dat;
			i += 4;
		}
		charptr1 = (char *)(longptr1);
		charptr2 = (char *)(longptr2);
		charptr1 += 3;
		charptr2 += 3;
		while (i < totpicsiz)
		{
			dat = *charptr1--;
			*charptr2-- = dat;
			i++;
		}
	}
	for(j=picnume+1;j<=localtileend[curtilefile];j++)
		waloff[j] += templong;
	totpicsiz += templong;
	return(0);
}

gettextcolors()
{
	long i, j, whitedist, browndist, yellowdist, greendist, blackdist, dist;
	long r, g, b, dr, dg, db;

	whitedist = 0x7fffffff;
	browndist = 0x7fffffff;
	yellowdist = 0x7fffffff;
	greendist = 0x7fffffff;
	blackdist = 0x7fffffff;
	j = 0;
	for(i=0;i<256;i++)
	{
		r = palette[j+0]; g = palette[j+1]; b = palette[j+2];

		dr = r-48; dg = g-48; db = b-48; dist = dr*dr+dg*dg+db*db;
		if (dist < whitedist) whitedist = dist, whitecol = i;

		dr = r-32; dg = g-16; db = b-12; dist = dr*dr+dg*dg+db*db;
		if (dist < browndist) browndist = dist, browncol = i;

		dr = r-48; dg = g-48; db = b-50; dist = dr*dr+dg*dg+db*db;
		if (dist < yellowdist) yellowdist = dist, yellowcol = i;

		dr = r-32; dg = g-63; db = b-32; dist = dr*dr+dg*dg+db*db;
		if (dist < greendist) greendist = dist, greencol = i;

		dr = r-0; dg = g-0; db = b-0; dist = dr*dr+dg*dg+db*db;
		if (dist < blackdist) blackdist = dist, blackcol = i;

		j += 3;
	}
}

gettrans(char dat1, char dat2)
{
	char retcol;
	long ravg, gavg, bavg;
	long tripledat1, tripledat2, closest, zx, triplezx, dar, dag, dab, dist;

	tripledat1 = ((long)dat1<<1)+(long)dat1;
	tripledat2 = ((long)dat2<<1)+(long)dat2;

	ravg = ((palette[tripledat1+0] + palette[tripledat2+0])>>1);
	gavg = ((palette[tripledat1+1] + palette[tripledat2+1])>>1);
	bavg = ((palette[tripledat1+2] + palette[tripledat2+2])>>1);

	retcol = 0;
	closest = 0x7fffffff;
	triplezx = 0;
	for(zx=0;zx<256;zx++)
	{
		dar = (palette[triplezx+0]-ravg) * 30;
		dag = (palette[triplezx+1]-gavg) * 59;
		dab = (palette[triplezx+2]-bavg) * 11;
		dist = dar*dar + dag*dag + dab*dab;
		if (dist < closest)
		{
			closest = dist;
			retcol = zx;
		}
		triplezx += 3;
	}

	return(retcol);
}

convalloc32(long size)
{
	 union REGS r;

	 r.x.eax = 0x0100;           //DPMI allocate DOS memory
	 r.x.ebx = ((size+15)>>4);   //Number of paragraphs requested
	 int386(0x31,&r,&r);

	 if (r.x.cflag != 0)         //Failed
		 return ((long)0);
	 return ((long)((r.x.eax&0xffff)<<4));   //Returns full 32-bit offset
}

swapwalls(long swapwall1, long swapwall2)
{
	char tempchar, *charptr1, *charptr2, *bakcharptr1, *bakcharptr2;
	long i, j, templong, numbytes1, numbytes2, numbytes3;

	outp(67,182); outp(66,16); outp(66,16);

	if (swapwall1 == swapwall2)
		return(0);

	if (swapwall1 > swapwall2)
		templong = swapwall1, swapwall1 = swapwall2, swapwall2 = templong;

	// charptr1  numbytes1    numbytes3     charptr2  numbytes2
	//  ³       /                ³             ³     /
	//  ÚÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄ¿
	//  ³   TILE1    ³ -----walls between----- ³ TILE2 ³
	//  ÀÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÙ

		//Swap memory
	numbytes1 = tilesizx[swapwall1]*tilesizy[swapwall1];
	numbytes2 = tilesizx[swapwall2]*tilesizy[swapwall2];

	charptr1 = (char *)FP_OFF(pic);
	for(i=localtilestart[curtilefile];i<swapwall1;i++)
		charptr1 += tilesizx[i]*tilesizy[i];

	numbytes3 = 0;
	for(i=swapwall1;i<swapwall2;i++)
		numbytes3 += tilesizx[i]*tilesizy[i];

	charptr2 = charptr1 + numbytes3;

	numbytes3 -= numbytes1;     //num bytes in between

	if (numbytes1 >= numbytes2)
	{
		outp(97,inp(97)|3);
		bakcharptr1 = charptr1;
		for(i=0;i<numbytes1;i++)
			buf[i] = *bakcharptr1++;

		outp(97,inp(97)&252);
		bakcharptr1 = charptr1;
		bakcharptr2 = charptr2;
		for(i=0;i<numbytes2;i++)
			*bakcharptr1++ = *bakcharptr2++;

		outp(97,inp(97)|3);
		bakcharptr2 = charptr1+numbytes1;
		for(i=0;i<numbytes3;i++)
			*bakcharptr1++ = *bakcharptr2++;

		outp(97,inp(97)&252);
		for(i=0;i<numbytes1;i++)
			*bakcharptr1++ = buf[i];
	}
	else
	{

		outp(97,inp(97)|3);
		bakcharptr1 = charptr2;
		for(i=0;i<numbytes2;i++)
			buf[i] = *bakcharptr1++;

		outp(97,inp(97)&252);
		bakcharptr1 = charptr2+numbytes2-1;
		bakcharptr2 = charptr1+numbytes1-1;
		for(i=0;i<numbytes1;i++)
			*bakcharptr1-- = *bakcharptr2--;

		outp(97,inp(97)|3);
		bakcharptr2 = charptr2-1;
		for(i=0;i<numbytes3;i++)
			*bakcharptr1-- = *bakcharptr2--;

		outp(97,inp(97)&252);
		bakcharptr1 = charptr1;
		for(i=0;i<numbytes2;i++)
			*bakcharptr1++ = buf[i];
	}

		//Swap attributes
	templong = tilookup[swapwall1]; tilookup[swapwall1] = tilookup[swapwall2]; tilookup[swapwall2] = templong;

	templong = tilesizx[swapwall1]; tilesizx[swapwall1] = tilesizx[swapwall2]; tilesizx[swapwall2] = templong;
	templong = tilesizy[swapwall1]; tilesizy[swapwall1] = tilesizy[swapwall2]; tilesizy[swapwall2] = templong;
	templong = picanm[swapwall1]; picanm[swapwall1] = picanm[swapwall2]; picanm[swapwall2] = templong;

	needtosavenames = 1;
	for(i=0;i<17;i++)
	{
		tempchar = names[swapwall1][i];
		names[swapwall1][i] = names[swapwall2][i];
		names[swapwall2][i] = tempchar;
	}

	for(i=swapwall1+1;i<=swapwall2;i++)
		waloff[i] = waloff[i-1]+(tilesizx[i-1]*tilesizy[i-1]);

	allasksave = 1;
	return(1);
}

updatemaps()
{
	char ch;
	long i, j, k, fil, bad, mapversion;
	short type, mapnumsectors, mapnumwalls, mapnumsprites;
	struct find_t fileinfo;

	i = 0;
	while (curpath[i] != 0) i++;
	curpath[i] = 92;
	curpath[i+1] = 0;
	chdir(curpath);
	curpath[i] = 0;

	bad = 0;
	for(i=0;i<MAXTILES;i++)
		if (tilookup[i] != i)
		{
			bad = 1;
			break;
		}

	if (bad == 1)
	{
		printf("Some tiles have been moved around.\n");
		printf("Update tilenum's to *.MAP? (Y/N)\n");
		do
		{
			ch = getch();
		}
		while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N') && (ch != 27));
		if ((ch == 'y') || (ch == 'Y'))
		{
			for(i=0;i<MAXTILES;i++)
				tilookup2[tilookup[i]] = i;

			if (_dos_findfirst("*.MAP",_A_NORMAL,&fileinfo) == 0)
			{
				do
				{
					printf("Updating %s...\n",fileinfo.name);

					if ((fil = open(fileinfo.name,O_BINARY|O_RDWR,S_IREAD|S_IWRITE)) != -1)
					{
						read(fil,&mapversion,4);
						if (mapversion == 0x00000007)       //Build map format
						{
							lseek(fil,4 + 4+4+4+2+2,SEEK_SET);
							read(fil,&mapnumsectors,2);
							for(i=0;i<mapnumsectors;i++)
							{
								read(fil,&mapsector,sizeof(struct sectortype));
								lseek(fil,-sizeof(struct sectortype),SEEK_CUR);
								mapsector.ceilingpicnum = tilookup2[mapsector.ceilingpicnum];
								mapsector.floorpicnum = tilookup2[mapsector.floorpicnum];
								write(fil,&mapsector,sizeof(struct sectortype));
							}
							read(fil,&mapnumwalls,2);
							for(i=0;i<mapnumwalls;i++)
							{
								read(fil,&mapwall,sizeof(struct walltype));
								lseek(fil,-sizeof(struct walltype),SEEK_CUR);
								mapwall.picnum = tilookup2[mapwall.picnum];
								if (mapwall.cstat&48)  //1-way or masked wall
									mapwall.overpicnum = tilookup2[mapwall.overpicnum];
								write(fil,&mapwall,sizeof(struct walltype));
							}
							read(fil,&mapnumsprites,2);
							for(i=0;i<mapnumsprites;i++)
							{
								read(fil,&mapsprite,sizeof(struct spritetype));
								lseek(fil,-sizeof(struct spritetype),SEEK_CUR);
								mapsprite.picnum = tilookup2[mapsprite.picnum];
								write(fil,&mapsprite,sizeof(struct spritetype));
							}
						}
						else if ((mapversion>>16) == 1)  //2Draw map format
						{
							read(fil,&nummaps,2);
							read(fil,&numsprites[0],MAXMAPS<<1);

							i = 4+2+(MAXMAPS<<1);                        //Global header
							for(j=0;j<nummaps;j++)
							{
								lseek(fil,i+4+4+4+2,SEEK_SET);

									//used xfillbuf becuase 64*64 shorts
								read(fil,&xfillbuf,8192);
								for(k=0;k<4096;k++)
									xfillbuf[k] = tilookup2[xfillbuf[k]];
								lseek(fil,-8192,SEEK_CUR);
								write(fil,&xfillbuf,8192);

								for(k=0;k<numsprites[j];k++)
								{
										//2draw sprite structure:
										//   2+4+4+4+(spritepicnum(2))+2

									read(fil,&xfillbuf,18);
									xfillbuf[7] = tilookup2[xfillbuf[7]];
									lseek(fil,-18,SEEK_CUR);
									write(fil,&xfillbuf,18);
								}

								i += (4+4+4+2+8192+(numsprites[j]*18));   //Individual map header
							}
						}
						else if ((mapversion>>16) == 2)  //Polytex map format
						{
						}
						close(fil);
					}

				}
				while (_dos_findnext(&fileinfo) == 0);
			}
			printf("MAPS UPDATED.\n");
			printf("Don't forget to recompile - NAMES.H may have changed!\n");
		}
		else
			printf("MAPS NOT UPDATED.\n");
	}
}

updatepalette()
{
	char ch, *ptr;
	long i, j, k, m, fil;

	for(i=0;i<768;i++)          //Swap palette and palette2
	{
		j = palette[i];
		palette[i] = palette2[i];
		palette2[i] = j;
	}
	getpaletteconversion();

	getpalookup();
	if ((fil = open("palette.dat",O_BINARY|O_CREAT|O_WRONLY,S_IWRITE)) != -1)
	{
		write(fil,&palette[0],768);
		write(fil,&palookup[0][0],NUMPALOOKUPS*256);
		gettextcolors();
		close(fil);
	}

	for(m=0;m<numtilefiles;m++)
	{
		loadpics(localtilestart[m]);
		for(i=localtilestart[m];i<=localtileend[m];i++)
			if ((tilesizx[i] > 0) && (tilesizy[i] > 0))
			{
				loadwall(buf,i);
				k = tilesizx[i]*tilesizy[i];
				ptr = (char *)waloff[i];
				for(j=0;j<k;j++)
				{
					ch = *ptr;
					*ptr = palookupe[ch];
					ptr++;
				}
				savewall(buf,i);
			}
		allasksave = 1;
		savepics();
	}
	loadpics(picnum);
	loadwall(buf,picnum);

	for(i=0;i<768;i++)
		palette2[i] = palette[i];

	asksave = 1;
}

selectbox(long *dax1, long *day1, long *dax2, long *day2)
{
	char ch, xorcol, *ptr;
	long i, x1, y1, x2, y2, fil, templong, lin, num, updatx, updaty;

	lin = 400;
	outp(0x3d4,0x18); outp(0x3d5,lin&255);
	outp(0x3d4,0x7); outp(0x3d5,(inp(0x3d5)&239)|((lin&256)>>4));
	outp(0x3d4,0x9); outp(0x3d5,(inp(0x3d5)&191)|((lin&512)>>3));
	num = 0;
	outp(0x3d4,0xc); outp(0x3d5,num>>10);
	outp(0x3d4,0xd); outp(0x3d5,(num>>2)&255);
	outp(0x3c0,inp(0x3c0)|0x13); outp(0x3c0,(num&3)<<1);

	if (xres > 1024) xres = 1024;
	if (yres > 256) yres = 256;
	if (xdim < 1) xdim = 1;
	if (ydim < 1) ydim = 1;

	if (*dax1 == 0x80000000)
	{
		x1 = ((xres-xdim)>>1); x2 = x1+xdim;
		y1 = ((yres-ydim)>>1); y2 = y1+ydim;
		if (x1 < 0) x1 = 0;
		if (y1 < 0) y1 = 0;
		if (x2 > xres) x2 = xres;
		if (y2 > yres) y2 = yres;
	}
	else
	{
		x1 = *dax1;
		y1 = *day1;
		x2 = *dax2;
		y2 = *day2;
		if ((x2 <= x1) && (y2 <= y1))
			if ((x1 > 4) && (y1 > 4) && (x1 < xres-4) && (y1 < xres-4))
			{
				x1 -= 4;
				y1 -= 4;
				x2 = x1+8;
				y2 = y1+8;
			}
	}
	updatx = ((x1+x2)>>1); updaty = ((y1+y2)>>1);

	xorcol = (rand()&255);

	drawxorbox(x1,y1,x2-1,y2-1,xorcol);
	ch = 0;
	while ((ch != 13) && (ch != ' ') && (ch != 27) && (ch != 'o') && (ch != 'O'))
	{
		if (kbhit() == 0)
		{
			drawxorbox(x1,y1,x2-1,y2-1,xorcol);
			limitrate();
			xorcol = (rand()&255);
			drawxorbox(x1,y1,x2-1,y2-1,xorcol);
			limitrate();
		}
		else
		{
			ch = getch();

			if ((ch == 'p') || (ch == 'P'))
			{
				drawxorbox(x1,y1,x2-1,y2-1,xorcol);
				printext256(panxforu,panyforu,xorcol,-2,"Make this the new build palette?");
				do
				{
					printext256(panxforu,panyforu,xorcol,-2,"Make this the new build palette?");
					limitrate();
					xorcol = (rand()&255);
					printext256(panxforu,panyforu,xorcol,-2,"Make this the new build palette?");
					limitrate();

					if (kbhit() != 0)
						ch = getch();
				}
				while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));
				printext256(panxforu,panyforu,xorcol,-2,"Make this the new build palette?");
				drawxorbox(x1,y1,x2-1,y2-1,xorcol);

				if ((ch == 'y') || (ch == 'Y'))
					updatepalette();
			}

			if (ch == 0)
			{
				drawxorbox(x1,y1,x2-1,y2-1,xorcol);

				ch = getch();
				if (bstatus == 0)
				{
					if ((ch == 75) && (x1 > 0)) x1--, x2--;
					if ((ch == 77) && (x2 < xres)) x1++, x2++;
					if ((ch == 72) && (y1 > 0)) y1--, y2--;
					if ((ch == 80) && (y2 < yres)) y1++, y2++;
					updatx = x1; updaty = y1;
				}
				else
				{
					if ((ch == 75) && (x2 > x1+1)) x2--;
					if ((ch == 77) && (x2 < xres)) x2++;
					if ((ch == 72) && (y2 > y1+1)) y2--;
					if ((ch == 80) && (y2 < yres)) y2++;
					updatx = x2-1; updaty = y2-1;
				}

				drawxorbox(x1,y1,x2-1,y2-1,xorcol);
			}
		}
		if (moustat == 1)
		{
			readmouse();
			if ((mousx != 0) || (mousy != 0))
			{
				drawxorbox(x1,y1,x2-1,y2-1,xorcol);

				if (bstatus == 0)
				{
					x1 += mousx; x2 += mousx;
					if (x1 < 0) templong = -x1, x1 += templong, x2 += templong;
					if (x2 > xres) templong = x2-xres, x1 -= templong, x2 -= templong;
					y1 += mousy; y2 += mousy;
					if (y1 < 0) templong = -y1, y1 += templong, y2 += templong;
					if (y2 > yres) templong = y2-yres, y1 -= templong, y2 -= templong;
					updatx = x1; updaty = y1;
				}
				else
				{
					x2 += mousx;
					if (x2 < x1+1) x2 = x1+1;
					if (x2 > xres) x2 = xres;
					y2 += mousy;
					if (y2 < y1+1) y2 = y1+1;
					if (y2 > yres) y2 = yres;
					updatx = x2-1; updaty = y2-1;
				}

				drawxorbox(x1,y1,x2-1,y2-1,xorcol);
			}
		}
		updatepanningforumode(updatx,updaty);
	}
	drawxorbox(x1,y1,x2-1,y2-1,xorcol);

	if ((ch == 13) || (ch == ' ') || (ch == 'o') || (ch == 'O'))
	{
		if (ch == ' ')
			for(i=0;i<256;i++)
				palookupe[i] = i;

		if ((ch == 'o') || (ch == 'O'))
		{
			*dax1 = ((x1+x2)>>1);
			*day1 = ((y1+y2)>>1);
			*dax2 = ((x1+x2)>>1);
			*day2 = ((y1+y2)>>1);
		}
		else
		{
			*dax1 = x1;
			*day1 = y1;
			*dax2 = x2;
			*day2 = y2;
		}
		return(1);
	}
	return(0);
}

updatescript(long picnumrangestart, long picnumrangeend)
{
	char buffer[160], dafilename[160];
	long i, j, k, datilenum, x1, y1, x2, y2;

	if (capfil == -1)
	{
		capfil = open("capfil.txt",O_BINARY|O_RDWR,S_IREAD);
		if (capfil == -1) return;
	}

	lseek(capfil,0L,SEEK_SET);
	while (eof(capfil) == 0)
	{
		i = 0;
		do
		{
			read(capfil,&buffer[i],1);
			if ((i == 0) && ((buffer[i] == 10) || (buffer[i] == 13)))
				i--;
			i++;
		} while (((buffer[i-1] != 10) && (buffer[i-1] != 13)) && (eof(capfil) == 0));
		buffer[i] = 0;

		j = 0; k = 0;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&datilenum);

		j = k+1; k = j;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%s",&dafilename[0]);

		j = k+1; k = j;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&x1);

		j = k+1; k = j;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&y1);

		j = k+1; k = j;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&x2);

		j = k+1; k = j;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&y2);

		x2 += x1;
		y2 += y1;

		if ((datilenum >= picnumrangestart) && (datilenum <= picnumrangeend) && (i > 4))
		{
			i = 0;
			while ((i < 160) && (dafilename[i] != 0)) i++;
			while ((i > 0) && (dafilename[i] != 92)) i--;
			dafilename[i] = 0;
			chdir(dafilename);
			loadpicture(&dafilename[i+1]);

			loadpics(datilenum);

			outp(67,182); outp(66,16); outp(66,16);
			outp(97,inp(97)|3);
			captureit(x1,y1,x2,y2,datilenum,dafilename,0);
			outp(97,inp(97)&252);

			asksave = 2, savewall(buf,datilenum);
		}
	}
	chdir(curpath);
}

loadtileofscript(long tilenume, char *filespec, long *x1, long *y1, long *x2, long *y2)
{
	char buffer[160];
	long i, j, k, datilenum;

	if (capfil == -1)
	{
		capfil = open("capfil.txt",O_BINARY|O_RDWR,S_IREAD);
		if (capfil == -1) return(0);
	}

	lseek(capfil,0L,SEEK_SET);
	while (eof(capfil) == 0)
	{
		i = 0;
		do
		{
			read(capfil,&buffer[i],1);
			if ((i == 0) && ((buffer[i] == 10) || (buffer[i] == 13)))
				i--;
			i++;
		} while (((buffer[i-1] != 10) && (buffer[i-1] != 13)) && (eof(capfil) == 0));
		buffer[i] = 0;

		j = 0; k = 0;
		while ((buffer[k] != ',') && (k < i)) k++;
		buffer[k] = 0;
		sscanf(&buffer[j],"%ld",&datilenum);

		if (datilenum == tilenume)
		{
			j = k+1; k = j;
			while ((buffer[k] != ',') && (k < i)) k++;
			buffer[k] = 0;
			sscanf(&buffer[j],"%s",filespec);

			j = k+1; k = j;
			while ((buffer[k] != ',') && (k < i)) k++;
			buffer[k] = 0;
			sscanf(&buffer[j],"%ld",x1);

			j = k+1; k = j;
			while ((buffer[k] != ',') && (k < i)) k++;
			buffer[k] = 0;
			sscanf(&buffer[j],"%ld",y1);

			j = k+1; k = j;
			while ((buffer[k] != ',') && (k < i)) k++;
			buffer[k] = 0;
			sscanf(&buffer[j],"%ld",x2);

			j = k+1; k = j;
			while ((buffer[k] != ',') && (k < i)) k++;
			buffer[k] = 0;
			sscanf(&buffer[j],"%ld",y2);

			*x2 = (*x2) + (*x1);
			*y2 = (*y2) + (*y1);

			chdir(curpath);
			return(1);
		}
	}
	chdir(curpath);
	return(0);
}

captureit(long x1, long y1, long x2, long y2, long datilenum, char *dafilename, char capfilmode)
{
	char buffer[160];
	long i, j, k, vidpos, bad, dafileng, daothertilenum;

	if (capfilmode == 1)
	{
		if (capfil == -1)
			capfil = open("capfil.txt",O_BINARY|O_RDWR,S_IREAD);
		if (capfil != -1)
		{
			lseek(capfil,0L,SEEK_SET);
			dafileng = read(capfil,&buf[0],524288);
			if (dafileng < 0) dafileng = 0;
			close(capfil);

			j = 0;
			while (j < dafileng)
			{
				i = j; daothertilenum = 0;
				while ((buf[i] >= 48) && (buf[i] <= 57) && (i < dafileng))
					daothertilenum = (daothertilenum*10)+(buf[i++]-48);
				if (daothertilenum >= datilenum) break;
				while (((buf[j] != 10) && (buf[j] != 13)) && (j < dafileng)) j++;
				while (((buf[j] == 10) || (buf[j] == 13)) && (j < dafileng)) j++;
			}
			k = j;
			while (k < dafileng)
			{
				i = k; daothertilenum = 0;
				while ((buf[i] >= 48) && (buf[i] <= 57) && (i < dafileng))
					daothertilenum = (daothertilenum*10)+(buf[i++]-48);
				if (daothertilenum > datilenum) break;
				while (((buf[k] != 10) && (buf[k] != 13)) && (k < dafileng)) k++;
				while (((buf[k] == 10) || (buf[k] == 13)) && (k < dafileng)) k++;
			}

			capfil = open("capfil.txt",O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IWRITE);

			if (j > 0) write(capfil,&buf[0],j);
			sprintf(buffer,"%ld,%s,%ld,%ld,%ld,%ld\r\n",datilenum,dafilename,x1,y1,x2-x1,y2-y1);
			write(capfil,buffer,strlen(buffer));
			if (dafileng > k) write(capfil,&buf[k],dafileng-k);

			close(capfil);
			capfil = -1;
		}
		else
		{
			capfil = open("capfil.txt",O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IWRITE);

			sprintf(buffer,"%ld,%s,%ld,%ld,%ld,%ld\r\n",datilenum,dafilename,x1,y1,x2-x1,y2-y1);
			write(capfil,buffer,strlen(buffer));

			close(capfil);
			capfil = -1;
		}
	}

	if ((x2 <= x1) || (y2 <= y1))
	{
		x2 = x1+1; y2 = y1+1;

		outp(0x3ce,4);

		do
		{
			bad = 0;

			if (x1 > 0)
			{
				for(j=0;j<y2-y1;j++)
				{
					vidpos = (y1+j)*panxdim+(x1-1);

					outp(0x3cf,vidpos&3);
					if (getpixel((vidpos>>2)+0xa0000) != 255)
					{
						x1--, bad = 1;
						break;
					}
				}
			}
			if (x2 < xres-1)
			{
				for(j=0;j<y2-y1;j++)
				{
					vidpos = (y1+j)*panxdim+(x2+1);

					outp(0x3cf,vidpos&3);
					if (getpixel((vidpos>>2)+0xa0000) != 255)
					{
						x2++, bad = 1;
						break;
					}
				}
			}
			if (y1 > 0)
			{
				for(i=0;i<x2-x1;i++)
				{
					vidpos = (y1-1)*panxdim+(x1+i);

					outp(0x3cf,vidpos&3);
					if (getpixel((vidpos>>2)+0xa0000) != 255)
					{
						y1--, bad = 1;
						break;
					}
				}
			}
			if (y2 < yres-1)
			{
				for(i=0;i<x2-x1;i++)
				{
					vidpos = (y2+1)*panxdim+(x1+i);

					outp(0x3cf,vidpos&3);
					if (getpixel((vidpos>>2)+0xa0000) != 255)
					{
						y2++, bad = 1;
						break;
					}
				}
			}
		} while (bad == 1);

		x2++;
		y2++;
	}

	xdim = x2-x1; c = (xdim>>1);
	ydim = y2-y1; d = (ydim>>1);

	outp(0x3ce,4);
	for(j=0;j<ydim;j++)
	{
		vidpos = (y1+j)*panxdim+x1;
		for(i=0;i<xdim;i++)
		{
			outp(0x3cf,vidpos&3);
			buf[((j+pany)<<10)+(i+panx)] = palookupe[getpixel((vidpos>>2)+0xa0000)];
			vidpos++;
		}
	}
	asksave = 1;
}

reportmaps()
{
	char ch;
	long i, j, k, fil, bad, mapversion;
	short type, mapnumsectors, mapnumwalls, mapnumsprites;
	struct find_t fileinfo;

	i = 0;
	while (curpath[i] != 0) i++;
	curpath[i] = 92;
	curpath[i+1] = 0;
	chdir(curpath);
	curpath[i] = 0;

	for(i=MAXTILES-1;i>=0;i--)
		tilookup2[i] = 0;

	if (_dos_findfirst("*.MAP",_A_NORMAL,&fileinfo) == 0)
	{
		do
		{
			if ((fil = open(fileinfo.name,O_BINARY|O_RDWR,S_IREAD)) != -1)
			{
				read(fil,&mapversion,4);
				if (mapversion == 0x00000007)       //Build map format
				{
					lseek(fil,4 + 4+4+4+2+2,SEEK_SET);
					read(fil,&mapnumsectors,2);
					for(i=0;i<mapnumsectors;i++)
					{
						read(fil,&mapsector,sizeof(struct sectortype));
						tilookup2[mapsector.ceilingpicnum]++;
						tilookup2[mapsector.floorpicnum]++;
					}
					read(fil,&mapnumwalls,2);
					for(i=0;i<mapnumwalls;i++)
					{
						read(fil,&mapwall,sizeof(struct walltype));
						tilookup2[mapwall.picnum]++;
						if (mapwall.cstat&48) tilookup2[mapwall.overpicnum]++;
					}
					read(fil,&mapnumsprites,2);
					for(i=0;i<mapnumsprites;i++)
					{
						read(fil,&mapsprite,sizeof(struct spritetype));
						tilookup2[mapsprite.picnum]++;
					}
				}
				else if ((mapversion>>16) == 1)  //2Draw map format
				{
					read(fil,&nummaps,2);
					read(fil,&numsprites[0],MAXMAPS<<1);

					i = 4+2+(MAXMAPS<<1);                        //Global header
					for(j=0;j<nummaps;j++)
					{
						lseek(fil,i+4+4+4+2,SEEK_SET);

							//used xfillbuf becuase 64*64 shorts
						read(fil,&xfillbuf,8192);
						for(k=0;k<4096;k++)
							xfillbuf[k] = tilookup2[xfillbuf[k]];

						for(k=0;k<numsprites[j];k++)
						{
							//2draw sprite structure:
							//   2+4+4+4+(spritepicnum(2))+2

							read(fil,&xfillbuf,18);
							xfillbuf[7] = tilookup2[xfillbuf[7]];
						}

						i += (4+4+4+2+8192+(numsprites[j]*18));   //Individual map header
					}
				}
				else if ((mapversion>>16) == 2)  //Polytex map format
				{
				}
				close(fil);
			}
		} while (_dos_findnext(&fileinfo) == 0);
	}
}
