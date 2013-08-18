// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#define NUMOPTIONS 8
#define NUMKEYS 19

#define NUMENUS 14
static int menuoffs[NUMENUS], menuleng[NUMENUS] = {8,9,4,11,13,15,8,7,2,3,14,6,14,19};
#define MAINMENU 0
#define GRAPHMENU 1
#define CHAINXDIMMENU 2
#define CHAINYDIMMENU 3
#define VESA2MENU 4
#define DIGIMENU 5
#define DIGIOPTIONMENU 6
#define DIGIHZMENU 7
#define MUSICMENU 8
#define INPUTMENU 9
#define COMMENU 10
#define COMSPEEDMENU 11
#define COMIRQMENU 12
#define KEYMENU 13

static unsigned char screeninfo[4000];
static unsigned char *strtable[135] =
{
	"Graphics Mode",
	"Digitized Sound",
	"Music",
	"Input Devices",
	"Communications",
	"Ä",
	"Ignore Changes and Quit",
	"Save Changes and Quit",

	"Chain mode (Mode X)       ³ VGA compatible ³ 256*200 to 400*540",
	"VESA mode                 ³ VESA 1.2 / 2.0 ³ 320*200 to 1600*1200",
	"Screen-Buffer mode        ³ VGA compatible ³ 320*200 only",
	"Ä",
	"Specially-optimized TSENG ET4000 mode        320*200 only",
	"Specially-optimized Paradise WD90Cxx mode    320*200 only",
	"Specially-optimized S3 chipset mode          320*200 only",
	"Ä",
	"Red-Blue Stereo vision (VGA compatible)      320*200 only",

	"256 * X",
	"320 * X",
	"360 * X",
	"400 * X",

	"X * 200",
	"X * 240",
	"X * 256",
	"X * 270",
	"X * 300",
	"X * 350",
	"X * 360",
	"X * 400",
	"X * 480",
	"X * 512",
	"X * 540",

	" 320 * 200",
	" 360 * 200",
	" 320 * 240",
	" 360 * 240",
	" 320 * 400",
	" 360 * 400",
	" 640 * 350",
	" 640 * 400",
	" 640 * 480",
	" 800 * 600",
	"1024 * 768",
	"1280 * 1024",
	"1600 * 1200",

	"No digitized sound",
	"Ä",
	"Sound Blaster",
	"Pro Audio Spectrum",
	"Sound Man 16",
	"Adlib",
	"General Midi",
	"Sound Canvas",
	"Sound Blaster AWE32",
	"Wave Blaster",
	"Sound Scape",
	"Gravis Ultrasound",
	"Sound Source",
	"Tandy Sound source",
	"PC speaker",

	"  Mono,  8, lo",
	"  Mono,  8, HI",
	"  Mono, 16, lo",
	"  Mono, 16, HI",
	"Stereo,  8, lo",
	"Stereo,  8, HI",
	"Stereo, 16, lo",
	"Stereo, 16, HI",

	"  6000hz ",
	"  8000hz ",
	" 11025hz ",
	" 16000hz ",
	" 22050hz ",
	" 32000hz ",
	" 44100hz ",

	"Music OFF",
	"Music ON",

	"Keyboard only",
	"Keyboard & Mouse",
	"Keyboard & Spaceplayer",

	"None",
	"Ä",
	"COM1",
	"COM2",
	"COM3",
	"COM4",
	"Ä",
	"IPX network - 2 Players",
	"IPX network - 3 Players",
	"IPX network - 4 Players",
	"IPX network - 5 Players",
	"IPX network - 6 Players",
	"IPX network - 7 Players",
	"IPX network - 8 Players",

	"2400 bps",
	"4800 bps",
	"9600 bps",
	"14400 bps",
	"19200 bps",
	"28800 bps",

	"IRQ2",
	"IRQ3",
	"IRQ4",
	"IRQ5",
	"IRQ6",
	"IRQ7",
	"IRQ8",
	"IRQ9",
	"IRQ10",
	"IRQ11",
	"IRQ12",
	"IRQ13",
	"IRQ14",
	"IRQ15",

	"Move FORWARD                                                ",
	"Move BACKWARD                                               ",
	"Turn LEFT                                                   ",
	"Turn RIGHT                                                  ",
	"RUN                                                         ",
	"STRAFE (walk sideways)                                      ",
	"SHOOT!                                                      ",
	"OPEN / CLOSE / USE                                          ",
	"Stand HIGH                                                  ",
	"Stand LOW                                                   ",
	"Look Up                                                     ",
	"Look Down                                                   ",
	"Strafe Left                                                 ",
	"Strafe Right                                                ",
	"2D/3D flip                                                  ",
	"Player view flip                                            ",
	"2D Zoom in                                                  ",
	"2D Zoom out                                                 ",
	"Message typing                                              ",
};

static unsigned char *keytable[] =
{
	"-","ESC","1","2","3","4","5","6","7","8","9","0","-","=","BACKSPC","TAB",
	"Q","W","E","R","T","Y","U","I","O","P","[","]","ENTER","L-CTRL","A","S",
	"D","F","G","H","J","K","L",";","'","`","L-SHIFT","|","Z","X","C","V",
	"B","N","M",",",".","/","R-SHIFT","KP *","L-ALT","SPACEBAR","CAPSLOCK","F1","F2","F3","F4","F5",
	"F6","F7","F8","F9","F10","NUMLOCK","SCROLL","KP 7","KP 8","KP 9","KP -","KP 4","KP 5","KP 6","KP +","KP 1",
	"KP 2","KP 3","KP 0","KP .","-","-","-","F11","F12","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","KP ENTER","R-CTRL","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","KP /","-","-","R-ALT","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","HOME","UP","PAGEUP","-","LEFT","-","RIGHT","-","END",
	"DOWN","PAGEDOWN","INSERT","DELETE","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-",
	"-","-","-","-","-","-","-","-","-","-","-","-","-","-","-","-"
};

static unsigned char defaultkey[NUMKEYS] =
{
	0xc8,0xd0,0xcb,0xcd,0x2a,0x9d,0x1d,0x39,
	0x1e,0x2c,0xd1,0xc9,0x33,0x34,
	0x9c,0x1c,0xd,0xc,0xf,
};
volatile unsigned char readch, oldreadch, extended, keystatus[256];

static unsigned char option[NUMOPTIONS], keys[NUMKEYS];

void (_interrupt _far *oldkeyhandler)();
void _interrupt _far keyhandler(void);

#if defined(__386__)

#pragma aux printchrasm =\
	"add edi, 0xb8000"\
	"rep stosw"\
	parm [edi][ecx][eax]\

#pragma aux savescreen =\
	"mov esi, 0xb8000"\
	"mov edi, offset screeninfo"\
	"mov ecx, 1000"\
	"rep movsd"\
	modify [ecx esi edi]\

#pragma aux restorescreen =\
	"mov esi, offset screeninfo"\
	"mov edi, 0xb8000"\
	"mov ecx, 1000"\
	"rep movsd"\
	modify [ecx esi edi]\

#else

void printchrasm (int rdi, int rcx, int rax)
{
	_asm
	{
		cld
		mov di, 0b800h
		mov es, di
		mov di, rdi
		mov cx, rcx
		mov ax, rax
		rep stosw
	}
}

void savescreen ()
{
	_asm
	{
		cld
		mov di, offset screeninfo
		mov cx, 2000
		mov si, seg screeninfo
		mov es, si
		push ds
		mov si, 0b800h
		mov ds, si
		xor si, si
		rep movsw
		pop ds
	}
}

void restorescreen ()
{
	_asm
	{
		cld
		mov si, offset screeninfo
		mov cx, 2000
		mov di, 0b800h
		mov es, di
		xor di, di
		rep movsw
	}
}

#endif

void _interrupt _far keyhandler()
{
	oldreadch = readch;
	_asm
	{
		in al, 60h
		mov readch, al
		in al, 61h
		or al, 80h
		out 61h, al
		and al, 7fh
		out 61h, al
	}
	if ((readch|1) == 0xe1)
		extended = 128;
	else
	{
		if (oldreadch != readch)
		{
			if ((readch&128) == 0)
			{
				if (keystatus[(readch&127)+extended] == 0)
					keystatus[(readch&127)+extended] = 1;
			}
			else
				keystatus[(readch&127)+extended] = 0;
		}
		extended = 0;
	}
	_asm
	{
		mov al, 20h
		out 20h, al
	}
}

main()
{
	int i, j;
	unsigned char ch, col;

	loadsetup();
	drawscreen();
	savescreen();

	j = 0;
	for(i=0;i<NUMENUS;i++)
	{
		menuoffs[i] = j;
		j += menuleng[i];
	}

	i = 0;
	while ((i >= 0) && (i <= 4))
	{
		restorescreen();
		i = menu("Main Menu",MAINMENU,i);
		restorescreen();

		switch(i)
		{
			case 0:
				if ((j = menu("Graphics Mode Selection",GRAPHMENU,option[0])) >= 0)
				{
					option[0] = j;
					if (option[0] == 0)
					{
						restorescreen();
						if ((j = menu("Select X dimension",CHAINXDIMMENU,option[6]&15)) >= 0)
							option[6] = ((option[6]&0xf0)|(j&0x0f));

						restorescreen();
						if ((j = menu("Select Y dimension",CHAINYDIMMENU,(option[6]>>4)&15)) >= 0)
							option[6] = ((option[6]&0x0f)|((j<<4)&0xf0));
					}
					else if (option[0] == 1)
					{
						restorescreen();
						if ((j = menu("Select VESA2 mode",VESA2MENU,option[6]&15)) >= 0)
							option[6] = ((option[6]&0xf0)|(j&0x0f));
					}
				}
				break;
			case 1:
				if ((j = menu("Digitized Sound Selection",DIGIMENU,option[1])) >= 0)
				{
					option[1] = j;
					if (option[1] != 0)
					{
						restorescreen();
						if ((j = menu("Select sound options",DIGIOPTIONMENU,option[7]&15)) >= 0)
							option[7] = ((option[7]&0xf0)|(j&0x0f));

						restorescreen();
						if ((j = menu("Select playback rate",DIGIHZMENU,(option[7]>>4)&15)) >= 0)
							option[7] = ((option[7]&0x0f)|((j<<4)&0xf0));
					}
				}
				break;
			case 2:
				if ((j = menu("Music Selection",MUSICMENU,option[2])) >= 0)
					option[2] = j;
				break;
			case 3:
				if ((j = menu("Input Device Selection",INPUTMENU,option[3])) >= 0)
				{
					option[3] = j;
					definekeys();
				}
				break;
			case 4:
				if ((j = menu("Communications Selection",COMMENU,option[4])) >= 0)
				{
					option[4] = j;

					if ((j >= 1) && (j <= 4))
					{
						restorescreen();
						if ((j = menu("COM Speed Selection",COMSPEEDMENU,option[5]&15)) >= 0)
							option[5] = ((option[5]&0xf0)|(j&0x0f));

						restorescreen();
						if ((j = menu("COM IRQ Selection",COMIRQMENU,(option[5]>>4)&15)) >= 0)
							option[5] = ((option[5]&0x0f)|((j<<4)&0xf0));
					}
				}
				break;
			case 6:
				savesetup();
				break;
		}
	}

	_asm
	{
		mov ax, 3
		int 10h
	}
}

loadsetup()
{
	int fil, i, j;

	if ((fil = open("setup.dat",O_BINARY|O_RDONLY,S_IREAD)) == -1)
	{
		for(i=0;i<NUMOPTIONS;i++) option[i] = 0;
		for(i=0;i<NUMKEYS;i++) keys[i] = 0;
	}
	read(fil,&option[0],NUMOPTIONS);
	read(fil,&keys[0],NUMKEYS);

	if (keys[0] == 0)
		for(i=0;i<NUMKEYS;i++)
			keys[i] = defaultkey[i];

	close(fil);
}

savesetup()
{
	int fil, i, j;

	if ((fil = open("setup.dat",O_BINARY|O_TRUNC|O_CREAT|O_WRONLY,S_IWRITE)) == -1)
	{
		printf("Cannot save options");
		exit(0);
	}
	write(fil,&option[0],NUMOPTIONS);
	write(fil,&keys[0],NUMKEYS);
	close(fil);
}

printchr(int x, int y, unsigned char character, unsigned char attribute, int len)
{
	int pos;

	pos = (y*80+x)<<1;
	printchrasm((int)pos,(int)len,((int)attribute<<8)+(int)character);
}

printstr(int x, int y, unsigned char string[81], unsigned char attribute)
{
	unsigned char character;
	int i, pos;

	pos = (y*80+x)<<1;
	i = 0;
	while (string[i] != 0)
	{
		character = string[i];
		printchrasm((int)pos,1,((int)attribute<<8)+(int)character);
		i++;
		pos+=2;
	}
}

drawscreen()
{
	int i;

	_asm
	{
		mov ax, 3
		int 10h
	}
	outp(0x3d4,0x0e); outp(0x3d5,255);
	printchr(0,0,(unsigned char)218,(unsigned char)78,1);
	printchr(1,0,(unsigned char)196,(unsigned char)78,78);
	printchr(79,0,(unsigned char)191,(unsigned char)78,1);
	for(i=1;i<24;i++)
	{
		printchr(0,i,(unsigned char)179,(unsigned char)78,1);
		printchr(1,i,(unsigned char)32,(unsigned char)78,78);
		printchr(79,i,(unsigned char)179,(unsigned char)78,1);
	}
	printchr(0,24,(unsigned char)192,(unsigned char)78,1);
	printchr(1,24,(unsigned char)196,(unsigned char)78,78);
	printchr(79,24,(unsigned char)217,(unsigned char)78,1);
	printstr(31,1,"BUILD Setup Screen",79);
	printstr(29,21,"ARROWS to move cursor.",71);
	printstr(32,22,"ENTER to select.",71);
	printstr(33,23,"ESC to cancel.",71);
}

menu(unsigned char *title, int menunum, int selection)
{
	unsigned char ch, col, buffer[80];
	int i, j, k, xdim, ydim, x1, y1, firstring, numstrings;

	firstring = menuoffs[menunum];
	numstrings = menuleng[menunum];

	if (selection < 0) selection = 0;
	if (selection > numstrings) selection = numstrings-1;

	xdim = 0;
	for(i=firstring;i<firstring+numstrings;i++)
	{
		k = strlen(&strtable[i][0]);
		if (k > xdim)
			xdim = strlen(&strtable[i][0]);
	}
	xdim += 4;
	ydim = numstrings+2;
	x1 = 39-(xdim>>1);
	y1 = 12-(ydim>>1);

	printstr((int)39-(strlen(title)>>1),(int)y1-1,title,32);

	buffer[0] = 218;
	for(i=1;i<xdim-1;i++)
		buffer[i] = 196;
	buffer[xdim-1] = 191;
	buffer[xdim] = 0;
	printstr(x1,y1,buffer,19);
	for(i=y1+1;i<y1+ydim-1;i++)
	{
		if (strtable[firstring+i-(y1+1)][0] == 196)
		{
			buffer[0] = 195;
			for(j=1;j<xdim-1;j++)
				buffer[j] = 196;
			buffer[xdim-1] = 180;
			buffer[xdim] = 0;
			printstr(x1,i,buffer,19);
			if (selection >= i-(y1+1))
				selection++;
		}
		else
		{
			buffer[0] = 179;
			for(j=1;j<xdim-1;j++)
				buffer[j] = 32;
			buffer[xdim-1] = 179;
			buffer[xdim] = 0;
			printstr(x1,i,buffer,19);
		}
	}
	buffer[0] = 192;
	for(i=1;i<xdim-1;i++)
		buffer[i] = 196;
	buffer[xdim-1] = 217;
	buffer[xdim] = 0;
	printstr(x1,y1+ydim-1,buffer,19);
	ch = 0;
	while ((ch != 13) && (ch != 32) && (ch != 27))
	{
		for(i=firstring;i<firstring+numstrings;i++)
		{
			if (i == selection+firstring)
				col = 75;
			else
				col = 19;
			if (strtable[i][0] != 196)
				printstr(x1+2,y1+1+i-firstring,&strtable[i][0],col);

		}
		ch = getch();
		if (ch == 0)
		{
			ch = getch();
			if ((ch == 72) || (ch == 75))
			{
				do
				{
					selection--;
					if (selection < 0)
						selection = numstrings-1;
				}
				while (strtable[firstring+selection][0] == 196);
			}
			if ((ch == 80) || (ch == 77))
			{
				do
				{
					selection++;
					if (selection >= numstrings)
						selection = 0;
				}
				while (strtable[firstring+selection][0] == 196);
			}
			if ((ch == 59) && (firstring == menuoffs[KEYMENU]))
				return(-256);
			ch = 0;
		}
		if ((ch == 32) && (firstring == menuoffs[KEYMENU]))
			return(-257);
	}
	for(i=ydim-1;i>=0;i--)
		if (strtable[firstring+i][0] == 196)
			if (selection >= i)
				selection--;
	if (ch == 27)
		return(-1-selection);
	else
		return(selection);
}

definekeys()
{
	int i, j, k, keypos;

	for(i=1;i<24;i++)
		printchr(1,i,(unsigned char)32,(unsigned char)0x40,78);

	printstr(1,23,"Change selected key by pressing ENTER, then the KEY. ESC returns to main menu.",71);
	printstr(26,24," Press F1 for default keys. ",71);

	keypos = 0;
	do
	{
			//Draw and select box
		j = menuoffs[KEYMENU];
		for(i=0;i<NUMKEYS;i++)
		{
			for(k=0;k<8;k++)
				strtable[j][k+50] = 32;

			k = 0;
			while (keytable[keys[i]][k] > 0)
			{
				strtable[j][k+50] = keytable[keys[i]][k];
				k++;
			}

			do
			{
				j++;
			} while (strtable[j][0] == 196);
		}

		i = menu("Custom Key Menu",KEYMENU,keypos);
		if (i == -256)                       //F1 - reset to defaults
		{
			for(i=0;i<NUMKEYS;i++)
				keys[i] = defaultkey[i];
		}
		else if (i >= 0)
		{
			keypos = i;
			keys[keypos] = getscancode(keys[keypos]);
		}

	} while ((i >= 0) || (i == -256));
}

getscancode(unsigned char scancode)
{
#if defined(__386__)
	unsigned char *ptr;
#else
	unsigned char far *ptr;
#endif
	int i, j;

	for(j=0;j<25;j++)
		for(i=0;i<57;i++)
		{
#if defined(__386__)
			ptr = (unsigned char *)(0xb8000+(((j*80)+i)<<1)+1);
#else
			ptr = (unsigned char far *)(0xb8000000+(((j*80)+i)<<1)+1);
#endif
			if (*ptr == 75)
				*ptr = 19;
		}

	for(i=0;i<256;i++)
		keystatus[i] = 0;

	oldkeyhandler = _dos_getvect(0x9);
	_disable(); _dos_setvect(0x9, keyhandler); _enable();

	i = 0;
	while (1)
	{
		if (i != 0xaa)
		{
			if (keystatus[i] != 0)
			{
				j = i;
				break;
			}
		}
		i = ((i+1)&255);
	}

	_disable(); _dos_setvect(0x9, oldkeyhandler); _enable();

	if (j > 1)
		return(j);
	else
		return(scancode);
}
