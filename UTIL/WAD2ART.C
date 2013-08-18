// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <string.h>

#define MAXWADS 4096

static long artversion, localtilestart, localtileend;
static long fil1, fil2;
static char wadata[MAXWADS][8];
static long wadplc[MAXWADS], wadlen[MAXWADS], numwads;
static long xoffses[1024], ylookup[256], picanm[MAXWADS];
static short tilesizx[MAXWADS], tilesizy[MAXWADS];
static char pal[768], palookup[8192];
static char screen[65536], tempbuf[131072];
static long frameplace;

#pragma aux setvmode =\
	"int 0x10",\
	parm [eax]\

#pragma aux drawpixel =\
	"mov byte ptr [edi], al",\
	parm [edi][eax]\

#pragma aux clearbuf =\
	"rep stosd",\
	parm [edi][ecx][eax]\

#pragma aux copybuf =\
	"rep movsd",\
	parm [esi][edi][ecx]\

main(short argc, char **argv)
{
	long i, j, endoffile;
	char wadfile[80];

	printf("Wad2Art!                                       Copyright 1995 by Ken Silverman\n");

	if (argc != 2)
	{
		printf("Command line parameters: Wad2Art [Doom IWAD file]\n");
		printf("   Creates TILES000.ART, PALETTE.DAT, and NAMES.H in current directory.\n");
		printf("   Ex: wad2art c:\\doom\\doom.wad\n");
		exit(0);
	}

	strcpy(wadfile,argv[1]);
	if (strchr(wadfile,'.') == 0) strcat(wadfile,".wad");
	if ((fil1 = open(wadfile,O_BINARY|O_RDWR,S_IREAD)) == -1)
		{ printf("Wad not found\n"); exit(0); }
	if ((fil2 = open("tiles000.art",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		{ printf("Can't open art file\n"); exit(0); }

	frameplace = FP_OFF(screen);

	j = 0;
	for(i=0;i<256;i++) { ylookup[i] = j; j += 320; }

	printf("Loading wad header...\n");
	loadwadheader();
	setvmode(0x13);
	lseek(fil2,16+(numwads<<3),SEEK_SET);
	for(i=0;i<numwads;i++)
		{ tilesizx[i] = 0; tilesizy[i] = 0; picanm[i] = 0L; }

	printf("Saving names.h\n");
	savenames();
	printf("Converting palette\n");
	convpalette();

	printf("Saving tiles000.art\n");
	showart("L_START");
	showart("S_START");
	showart("P_START");
	showart("F_START");

	printf("Saving tiles000.art header\n");
	artversion = 1; localtilestart = 0; localtileend = numwads-1;

	endoffile = tell(fil2);
	lseek(fil2,0,SEEK_SET);
	write(fil2,&artversion,4);
	write(fil2,&numwads,4);
	write(fil2,&localtilestart,4);
	write(fil2,&localtileend,4);
	write(fil2,&tilesizx[0],numwads<<1);
	write(fil2,&tilesizy[0],numwads<<1);
	write(fil2,&picanm[0],numwads<<2);
	lseek(fil2,endoffile,SEEK_SET);

	close(fil2);
	close(fil1);

	setvmode(0x3);
	printf("Congratulations!  Your disk actually had enough space this time!\n");
}

loadwadheader()
{
	long i, j;

	read(fil1,&tempbuf[0],12);
	numwads = ((long)tempbuf[4])+(((long)tempbuf[5])<<8)+(((long)tempbuf[6])<<16)+(((long)tempbuf[7])<<24);
	i = ((long)tempbuf[8])+(((long)tempbuf[9])<<8)+(((long)tempbuf[10])<<16)+(((long)tempbuf[11])<<24);
	lseek(fil1,i,SEEK_SET);
	read(fil1,&tempbuf[0],numwads*16);
	j = 0;
	for(i=0;i<numwads;i++)
	{
		wadplc[i] = ((long)tempbuf[j])+(((long)tempbuf[j+1])<<8)+(((long)tempbuf[j+2])<<16)+(((long)tempbuf[j+3])<<24);
		j += 4;
		wadlen[i] = ((long)tempbuf[j])+(((long)tempbuf[j+1])<<8)+(((long)tempbuf[j+2])<<16)+(((long)tempbuf[j+3])<<24);
		j += 4;
		wadata[i][0] = tempbuf[j+0]; wadata[i][1] = tempbuf[j+1];
		wadata[i][2] = tempbuf[j+2]; wadata[i][3] = tempbuf[j+3];
		wadata[i][4] = tempbuf[j+4]; wadata[i][5] = tempbuf[j+5];
		wadata[i][6] = tempbuf[j+6]; wadata[i][7] = tempbuf[j+7];
		j += 8;
	}
}

convpalette()
{
	long i, fil3;
	short danumshades;

	i = 0;
	while (strnicmp(wadata[i],"PLAYPAL",7) != 0) i++;
	lseek(fil1,wadplc[i],SEEK_SET);
	read(fil1,pal,768);
	outp(0x3c8,0);
	for(i=0;i<768;i++)
	{
		pal[i] >>= 2;
		outp(0x3c9,pal[i]);
	}

	i = 0;
	while (strnicmp(wadata[i],"COLORMAP",8) != 0) i++;
	lseek(fil1,wadplc[i],SEEK_SET);
	read(fil1,palookup,8192);

	if ((fil3 = open("palette.dat",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		{ printf("Cannot save palette.dat\n"); exit(0); }
	write(fil3,pal,768);
	danumshades = 32;
	write(fil3,&danumshades,2);
	write(fil3,palookup,8192);
	close(fil3);
}

saveart (short tilenum, short xlen, short ylen)
{
	long i, x, p, pend;

	pend = ylookup[ylen];
	copybuf(frameplace,0xa0000,pend>>2);

	tilesizx[tilenum] = xlen;
	tilesizy[tilenum] = ylen;
	i = 0;
	for(x=0;x<xlen;x++)
		for(p=x;p<pend;p+=320)
			tempbuf[i++] = screen[p];
	if (write(fil2,&tempbuf[0],i) < 0)
	{
		setvmode(0x3);
		printf("NOT ENOUGH DISK SPACE!\n");
		exit(0);
	}
}

savenames()
{
	char buffer[160];
	long fil3, i, j;

	if ((fil3 = open("names.h",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		return;

	strcpy(&buffer,"//Be careful when changing this file - it is parsed by Editart and Build.");
	buffer[73] = 13, buffer[74] = 10, buffer[75] = 0;
	write(fil3,&buffer[0],75);

	strcpy(&buffer,"#define ");
	for(i=0;i<numwads;i++)
		if (wadata[i][0] != 0)
		{
			j = 8;
			while ((j < 16) && (wadata[i][j-8] != 0))
				{ buffer[j] = wadata[i][j-8]; j++; }
			buffer[j++] = 32;

			if (i >= 10000) buffer[j++] = ((i/10000)%10)+48;
			if (i >= 1000) buffer[j++] = ((i/1000)%10)+48;
			if (i >= 100) buffer[j++] = ((i/100)%10)+48;
			if (i >= 10) buffer[j++] = ((i/10)%10)+48;
			buffer[j++] = (i%10)+48;

			buffer[j++] = 13;
			buffer[j++] = 10;
			write(fil3,&buffer[0],j);
		}

	close(fil3);
	return;
}

showart (char *part)
{
	char yoff, ylen;
	short xsiz, ysiz;
	long i, j, z, zx, zzx, x, y, p, pend, junk, curplc;

	curplc = -1;
	if ((strnicmp(part,"L_START",7) == 0) || (strnicmp(part,"S_START",7) == 0) || (strnicmp(part,"P_START",7) == 0))
	{
		if (strnicmp(part,"L_START",7) == 0)
			z = 462;
		else
		{
			z = 0;
			while (strnicmp(wadata[z],part,7) != 0) z++;
			z++;
		}
	 
		do
		{
			if (strnicmp(wadata[z],"P1_START",8) == 0) z++;
			if (strnicmp(wadata[z],"P1_END",6) == 0) z++;
			if (strnicmp(wadata[z],"P2_START",8) == 0) z++;
			if (strnicmp(wadata[z],"S_START",7) == 0) break;
			if (strnicmp(wadata[z],"S_END",5) == 0) break;
			if (strnicmp(wadata[z],"P_END",5) == 0) break;
		  
			if (curplc != wadplc[z]) lseek(fil1,wadplc[z],SEEK_SET);
			read(fil1,&tempbuf[0],wadlen[z]);
			curplc = wadplc[z]+wadlen[z];

			xsiz = (long)tempbuf[0]+(((long)tempbuf[1])<<8);
			ysiz = (long)tempbuf[2]+(((long)tempbuf[3])<<8);
			if ((xsiz <= 0) || (ysiz <= 0) || (xsiz > 320) || (ysiz > 200)) goto skipit;
			i = 8;
			for(zx=0;zx<xsiz;zx++)
			{
				xoffses[zx] = ((long)tempbuf[i])+(((long)tempbuf[i+1])<<8)+(((long)tempbuf[i+2])<<16)+(((long)tempbuf[i+3])<<24);
				i += 4;
			}

			clearbuf(frameplace,ylookup[ysiz]>>2,0xffffffff);

			for(x=0;x<xsiz;x++)
			{
				i = xoffses[x];
				yoff = tempbuf[i++];
				while (yoff != 255)
				{
					p = ylookup[yoff]+x;
					pend = p+ylookup[tempbuf[i]];
					i += 2;
					for(;p<pend;p+=320)
						screen[p] = tempbuf[i++];
					i++;
					yoff = tempbuf[i++];
				}
			}

			saveart(z,xsiz,ysiz);
skipit:
			z++;
		} while (z < numwads);
	}
	else
	{
		z = 0;
		while (strnicmp(wadata[z],part,7) != 0) z++;
		z++;
	 
		while (1)
		{
			if (strnicmp(wadata[z],"F1_START",8) == 0) z++;
			if (strnicmp(wadata[z],"F1_END",6) == 0) z++;
			if (strnicmp(wadata[z],"F_END",5) == 0) break;

			if (wadlen[z] == 4096)
			{
				if (curplc != wadplc[z]) lseek(fil1,wadplc[z],SEEK_SET);
				read(fil1,&tempbuf[0],4096);
				curplc = wadplc[z]+4096;
				i = 0;
				for(x=0;x<64;x++)
					for(p=x;p<320*64;p+=320)
						screen[p] = tempbuf[i++];

				saveart(z,64,64);
			}

			z++;
		}
	}
}
