// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <dos.h>
#include <conio.h>

#define MAXFILES 4096

static char buf[65536];

static long numfiles;
static char filespec[MAXFILES][128], filelist[MAXFILES][16];
static long fileleng[MAXFILES];

main(short argc, char **argv)
{
	long i, j, k, l, fil, fil2;
	char stuffile[16], filename[128];

	if (argc < 3)
	{
		printf("KGROUP [grouped file][@file or filespec...]           by Kenneth Silverman\n");
		printf("   This program collects many files into 1 big uncompressed file called a\n");
		printf("   group file\n");
		printf("   Ex: kgroup stuff.dat *.art *.map *.k?? palette.dat tables.dat\n");
		printf("      (stuff.dat is the group file, the rest are the files to add)\n");
		exit(0);
	}

	numfiles = 0;
	for(i=argc-1;i>1;i--)
	{
		strcpy(filename,argv[i]);
		if (filename[0] == '@')
		{
			if ((fil = open(&filename[1],O_BINARY|O_RDONLY,S_IREAD)) != -1)
			{
				l = read(fil,buf,65536);
				j = 0;
				while ((j < l) && (buf[j] <= 32)) j++;
				while (j < l)
				{
					k = j;
					while ((k < l) && (buf[k] > 32)) k++;

					buf[k] = 0;
					findfiles(&buf[j]);
					j = k+1;

					while ((j < l) && (buf[j] <= 32)) j++;
				}
				close(fil);
			}
		}
		else
			findfiles(filename);
	}

	strcpy(stuffile,argv[1]);

	if ((fil = open(stuffile,O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
	{
		printf("Error: %s could not be opened\n",stuffile);
		exit(0);
	}
	strcpy(filename,"KenSilverman");
	write(fil,filename,12);
	write(fil,&numfiles,4);
	write(fil,filelist,numfiles<<4);

	for(i=0;i<numfiles;i++)
	{
		printf("Adding %s...\n",filespec[i]);
		if ((fil2 = open(filespec[i],O_BINARY|O_RDONLY,S_IREAD)) == -1)
		{
			printf("Error: %s not found\n",filespec[i]);
			close(fil);
			exit(0);
		}
		for(j=0;j<fileleng[i];j+=65536)
		{
			k = min(fileleng[i]-j,65536);
			read(fil2,buf,k);
			if (write(fil,buf,k) < k)
			{
				close(fil2);
				close(fil);
				printf("OUT OF HD SPACE!  Press any key to continue.\n");
				getch();
				exit(0);
			}
		}
		close(fil2);
	}
	close(fil);
	printf("Saved to %s.\n",stuffile);
}

findfiles(char *dafilespec)
{
	struct find_t fileinfo;
	long i, daspeclen;
	char daspec[128];

	strcpy(daspec,dafilespec);
	for(daspeclen=0;daspec[daspeclen]!=0;daspeclen++);
	while ((daspec[daspeclen] != '\\') && (daspeclen > 0)) daspeclen--;
	if (daspeclen > 0) daspec[daspeclen++] = '\\';

	if (_dos_findfirst(dafilespec,_A_NORMAL,&fileinfo) != 0) return;
	do
	{
		strcpy(&filelist[numfiles][0],fileinfo.name);
		fileleng[numfiles] = fileinfo.size;
		filelist[numfiles][12] = (char)(fileleng[numfiles]&255);
		filelist[numfiles][13] = (char)((fileleng[numfiles]>>8)&255);
		filelist[numfiles][14] = (char)((fileleng[numfiles]>>16)&255);
		filelist[numfiles][15] = (char)((fileleng[numfiles]>>24)&255);

		strcpy(&filespec[numfiles][0],daspec);
		strcpy(&filespec[numfiles][daspeclen],fileinfo.name);

		numfiles++;
		if (numfiles > MAXFILES)
		{
			printf("FATAL ERROR: TOO MANY FILES SELECTED! (MAX is 4096)\n");
			exit(0);
		}
	} while (_dos_findnext(&fileinfo) == 0);
}
