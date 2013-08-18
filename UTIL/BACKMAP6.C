// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <dos.h>

#define NEWMAPVERSION 6
#define MAXMENUFILES 1024

#define MAXSECTORS 1024
#define MAXWALLS 8192
#define MAXSPRITES 4096

#define MAXSTATUS 1024
#define MAXTILES 4096

static char menuname[MAXMENUFILES][32];
static long menunamecnt;

#pragma pack(push,1);

typedef struct
{
	unsigned short wallptr, wallnum;
	short ceilingpicnum, floorpicnum;
	short ceilingheinum, floorheinum;
	long ceilingz, floorz;
	signed char ceilingshade, floorshade;
	char ceilingxpanning, floorxpanning;
	char ceilingypanning, floorypanning;
	char ceilingstat, floorstat;
	char ceilingpal, floorpal;
	char visibility;
	short lotag, hitag, extra;
} newsectortype;

typedef struct
{
	long x, y;
	short point2, nextsector, nextwall;
	short picnum, overpicnum;
	signed char shade;
	char pal;
	short cstat;
	unsigned char xrepeat, yrepeat, xpanning, ypanning;
	short lotag, hitag, extra;
} newwalltype;

typedef struct
{
	long x, y, z;
	short cstat;
	signed char shade;
	char pal, clipdist;
	unsigned char xrepeat, yrepeat;
	signed char xoffset, yoffset;
	short picnum, ang, xvel, yvel, zvel, owner;
	short sectnum, statnum;
	short lotag, hitag, extra;
} newspritetype;

	//40 bytes
typedef struct
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
} sectortype;

	//32 bytes
typedef struct
{
	long x, y;
	short point2, nextwall, nextsector, cstat;
	short picnum, overpicnum;
	signed char shade;
	char pal, xrepeat, yrepeat, xpanning, ypanning;
	short lotag, hitag, extra;
} walltype;

	//44 bytes
typedef struct
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
} spritetype;

#pragma pack(pop);

//**************************************************************************

static sectortype sector[MAXSECTORS];
static walltype wall[MAXWALLS];
static spritetype sprite[MAXSPRITES];

static newsectortype newsector[MAXSECTORS];
static newwalltype newwall[MAXWALLS];
static newspritetype newsprite[MAXSPRITES];

static long posx, posy, posz, mapversion;
static short ang, cursectnum, numsectors, numwalls, numsprites;

main(int argc, char **argv)
{
	long i;

	if (argc != 2)
	{
		printf("BACKMAP%ld [filespec]                                            by Ken Silverman\n",NEWMAPVERSION);
		printf("This BACKMAP%ld.EXE converts map version %ld to map version %ld\n",NEWMAPVERSION,NEWMAPVERSION+1,NEWMAPVERSION);
		exit(0);
	}
	menunamecnt = 0;
	getfilenames(argv[1]);
	sortfilenames();

	if (menunamecnt == 0)
	{
		printf("BACKMAP%ld [filespec]                                            by Ken Silverman\n",NEWMAPVERSION);
		printf("This BACKMAP%ld.EXE converts map version %ld to map version %ld\n",NEWMAPVERSION,NEWMAPVERSION+1,NEWMAPVERSION);
		printf("File not found\n");
		exit(0);
	}

	printf("Converting map version %ld to map version %ld\n",NEWMAPVERSION+1,NEWMAPVERSION);

	for(i=0;i<menunamecnt;i++)
		convmap(menuname[i]);
}

convmap(char *filename)
{
	long i, j;

	if (strchr(filename,'.') == 0) strcat(filename,".map");
	if ((i = loadoldboard(filename)) < 0)
	{
		if (i == -1)
		{
			printf("%s not found.\n",filename);
		}
		if (i == -2)
		{
			if (mapversion == NEWMAPVERSION)
				printf("%s already map version %ld.\n",filename,NEWMAPVERSION);
			else
				printf("CANNOT CONVERT %s!\n",filename);
		}
		return;
	}

	for(i=0;i<numsectors;i++)
	{
		newsector[i].ceilingz = sector[i].ceilingz;
		newsector[i].floorz = sector[i].floorz;
		newsector[i].wallptr = sector[i].wallptr;
		newsector[i].wallnum = sector[i].wallnum;
		newsector[i].ceilingpicnum = sector[i].ceilingpicnum;
		newsector[i].ceilingheinum = (((long)sector[i].ceilingheinum)>>5);
		newsector[i].ceilingshade = sector[i].ceilingshade;
		newsector[i].ceilingpal = sector[i].ceilingpal;
		newsector[i].ceilingxpanning = sector[i].ceilingxpanning;
		newsector[i].ceilingypanning = sector[i].ceilingypanning;
		newsector[i].floorpicnum = sector[i].floorpicnum;
		newsector[i].floorheinum = (((long)sector[i].floorheinum)>>5);
		newsector[i].floorshade = sector[i].floorshade;
		newsector[i].floorpal = sector[i].floorpal;
		newsector[i].floorxpanning = sector[i].floorxpanning;
		newsector[i].floorypanning = sector[i].floorypanning;
		newsector[i].ceilingstat = sector[i].ceilingstat;
		if (newsector[i].ceilingheinum == 0) newsector[i].ceilingstat &= ~2;
		newsector[i].floorstat = sector[i].floorstat;
		if (newsector[i].floorheinum == 0) newsector[i].floorstat &= ~2;
		newsector[i].visibility = sector[i].visibility;
		newsector[i].lotag = sector[i].lotag;
		newsector[i].hitag = sector[i].hitag;
		newsector[i].extra = sector[i].extra;
	}
	for(i=0;i<numwalls;i++)
	{
		newwall[i].x = wall[i].x;
		newwall[i].y = wall[i].y;
		newwall[i].point2 = wall[i].point2;
		newwall[i].nextwall = wall[i].nextwall;
		newwall[i].nextsector = wall[i].nextsector;
		newwall[i].cstat = wall[i].cstat;
		newwall[i].picnum = wall[i].picnum;
		newwall[i].overpicnum = wall[i].overpicnum;
		newwall[i].shade = wall[i].shade;
		newwall[i].pal = wall[i].pal;
		newwall[i].xrepeat = wall[i].xrepeat;
		newwall[i].yrepeat = wall[i].yrepeat;
		newwall[i].xpanning = wall[i].xpanning;
		newwall[i].ypanning = wall[i].ypanning;
		newwall[i].lotag = wall[i].lotag;
		newwall[i].hitag = wall[i].hitag;
		newwall[i].extra = wall[i].extra;
	}
	for(i=0;i<numsprites;i++)
	{
		newsprite[i].x = sprite[i].x;
		newsprite[i].y = sprite[i].y;
		newsprite[i].z = sprite[i].z;
		newsprite[i].cstat = sprite[i].cstat;
		newsprite[i].picnum = sprite[i].picnum;
		newsprite[i].shade = sprite[i].shade;
		newsprite[i].pal = sprite[i].pal;
		newsprite[i].clipdist = sprite[i].clipdist;
		newsprite[i].xrepeat = sprite[i].xrepeat;
		newsprite[i].yrepeat = sprite[i].yrepeat;
		newsprite[i].xoffset = sprite[i].xoffset;
		newsprite[i].yoffset = sprite[i].yoffset;
		newsprite[i].sectnum = sprite[i].sectnum;
		newsprite[i].statnum = sprite[i].statnum;
		newsprite[i].ang = sprite[i].ang;
		newsprite[i].owner = sprite[i].owner;
		newsprite[i].xvel = sprite[i].xvel;
		newsprite[i].yvel = sprite[i].yvel;
		newsprite[i].zvel = sprite[i].zvel;
		newsprite[i].lotag = sprite[i].lotag;
		newsprite[i].hitag = sprite[i].hitag;
		newsprite[i].extra = sprite[i].extra;
	}

	mapversion = NEWMAPVERSION;
	if (savenewboard(filename) == 0)
		printf("%s successfully converted.\n",filename);
	else
		printf("%s NOT converted for some stupid reason.\n",filename);
}

loadoldboard(char *filename)
{
	long i, fil;

	if ((fil = open(filename,O_BINARY|O_RDWR,S_IREAD)) == -1)
		return(-1);

	read(fil,&mapversion,4);
	if (mapversion != NEWMAPVERSION+1)
	{
		close(fil);
		return(-2);
	}
	read(fil,&posx,4);
	read(fil,&posy,4);
	read(fil,&posz,4);
	read(fil,&ang,2);
	read(fil,&cursectnum,2);

	read(fil,&numsectors,2);
	read(fil,&sector[0],sizeof(sectortype)*numsectors);

	read(fil,&numwalls,2);
	read(fil,&wall[0],sizeof(walltype)*numwalls);

	read(fil,&numsprites,2);
	read(fil,&sprite[0],sizeof(spritetype)*numsprites);

	close(fil);
	return(0);
}

savenewboard(char *filename)
{
	long i, fil;

	if ((fil = open(filename,O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		return(-1);

		 //Version control
	write(fil,&mapversion,4);

		 //Starting position
	write(fil,&posx,4);
	write(fil,&posy,4);
	write(fil,&posz,4);
	write(fil,&ang,2);
	write(fil,&cursectnum,2);

		//Sectors
	write(fil,&numsectors,2);
	write(fil,&newsector[0],sizeof(newsectortype)*numsectors);

		//Walls
	write(fil,&numwalls,2);
	write(fil,&newwall[0],sizeof(newwalltype)*numwalls);

		//Sprites
	write(fil,&numsprites,2);
	write(fil,&newsprite[0],sizeof(newspritetype)*numsprites);

	close(fil);

	return(0);
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
				if (menunamecnt < MAXMENUFILES)
				{
					strcpy(menuname[menunamecnt],fileinfo.name);
					menuname[menunamecnt][16] = type;
					menunamecnt++;
				}
				//else
				//   printmessage("Too many files! (max=MAXMENUFILES)");
			}
	}
	while (_dos_findnext(&fileinfo) == 0);

	return(0);
}

sortfilenames()
{
	char sortbuffer[32];
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
