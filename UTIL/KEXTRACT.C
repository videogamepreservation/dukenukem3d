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

static long numfiles, anyfiles4extraction;
static char marked4extraction[MAXFILES];
static char filelist[MAXFILES][16];
static long fileoffs[MAXFILES+1], fileleng[MAXFILES];

main(short argc, char **argv)
{
	long i, j, k, l, fil, fil2;
	char stuffile[16], filename[128];

	if (argc < 3)
	{
		printf("KEXTRACT [grouped file][@file or filespec...]           by Kenneth Silverman\n");
		printf("   This program extracts files from a previously grouped group file.\n");
		printf("   You can extract files using the ? and * wildcards.\n");
		printf("   Ex: kextract stuff.dat tiles000.art nukeland.map palette.dat\n");
		printf("      (stuff.dat is the group file, the rest are the files to extract)\n");
		exit(0);
	}

	strcpy(stuffile,argv[1]);

	if ((fil = open(stuffile,O_BINARY|O_RDWR,S_IREAD)) == -1)
	{
		printf("Error: %s could not be opened\n",stuffile);
		exit(0);
	}

	read(fil,buf,16);
	if ((buf[0] != 'K') || (buf[1] != 'e') || (buf[2] != 'n') ||
		 (buf[3] != 'S') || (buf[4] != 'i') || (buf[5] != 'l') ||
		 (buf[6] != 'v') || (buf[7] != 'e') || (buf[8] != 'r') ||
		 (buf[9] != 'm') || (buf[10] != 'a') || (buf[11] != 'n'))
	{
		close(fil);
		printf("Error: %s not a valid group file\n",fil);
		exit(0);
	}
	numfiles = ((long)buf[12])+(((long)buf[13])<<8)+(((long)buf[14])<<16)+(((long)buf[15])<<24);

	read(fil,filelist,numfiles<<4);

	j = 0;
	for(i=0;i<numfiles;i++)
	{
		k = ((long)filelist[i][12])+(((long)filelist[i][13])<<8)+(((long)filelist[i][14])<<16)+(((long)filelist[i][15])<<24);
		filelist[i][12] = 0;
		fileoffs[i] = j;
		j += k;
	}
	fileoffs[numfiles] = j;

	for(i=0;i<numfiles;i++) marked4extraction[i] = 0;

	anyfiles4extraction = 0;
	for(i=argc-1;i>1;i--)
	{
		strcpy(filename,argv[i]);
		if (filename[0] == '@')
		{
			if ((fil2 = open(&filename[1],O_BINARY|O_RDWR,S_IREAD)) != -1)
			{
				l = read(fil2,buf,65536);
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
				close(fil2);
			}
		}
		else
			findfiles(filename);
	}

	if (anyfiles4extraction == 0)
	{
		close(fil);
		printf("No files found in group file with those names\n");
		exit(0);
	}

	for(i=0;i<numfiles;i++)
	{
		if (marked4extraction[i] == 0) continue;

		fileleng[i] = fileoffs[i+1]-fileoffs[i];

		if ((fil2 = open(filelist[i],O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
		{
			printf("Error: Could not write to %s\n",filelist[i]);
			continue;
		}
		printf("Extracting %s...\n",filelist[i]);
		lseek(fil,fileoffs[i]+((numfiles+1)<<4),SEEK_SET);
		for(j=0;j<fileleng[i];j+=65536)
		{
			k = min(fileleng[i]-j,65536);
			read(fil,buf,k);
			if (write(fil2,buf,k) < k)
			{
				printf("Write error (drive full?)\n");
				close(fil2);
				close(fil);
				exit(0);
			}
		}
		close(fil2);
	}
	close(fil);
}

findfiles(char *dafilespec)
{
	long i, j, k;
	char ch1, ch2, bad, buf1[12], buf2[12];

	for(k=0;k<12;k++) buf1[k] = 32;
	j = 0;
	for(k=0;dafilespec[k];k++)
	{
		if (dafilespec[k] == '.') j = 8;
		buf1[j++] = dafilespec[k];
	}

	for(i=numfiles-1;i>=0;i--)
	{
		for(k=0;k<12;k++) buf2[k] = 32;
		j = 0;
		for(k=0;filelist[i][k];k++)
		{
			if (filelist[i][k] == '.') j = 8;
			buf2[j++] = filelist[i][k];
		}

		bad = 0;
		for(j=0;j<12;j++)
		{
			ch1 = buf1[j]; if ((ch1 >= 97) && (ch1 <= 123)) ch1 -= 32;
			ch2 = buf2[j]; if ((ch2 >= 97) && (ch2 <= 123)) ch2 -= 32;
			if (ch1 == '*')
			{
				if (j < 8) j = 8; else j = 12;
				continue;
			}
			if ((ch1 != '?') && (ch1 != ch2)) { bad = 1; break; }
		}
		if (bad == 0) { marked4extraction[i] = 1; anyfiles4extraction = 1; }
	}
}
