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

//****************************************************************************
//
// common.h
//
// common defines for the setup program
//
//****************************************************************************

#ifndef _common_public_
#define _common_public_
#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
//
// DEFINES
//
//****************************************************************************

//
// Color Defines
//

#define MENUBACK_FOREGROUND COLOR_BLACK
#define MENUBACK_BACKGROUND COLOR_DARKGRAY

#define MENUBACKBORDER_FOREGROUND COLOR_BLACK
#define MENUBACKBORDER_BACKGROUND COLOR_GRAY

#define MENU_ACTIVE_FOREGROUND   COLOR_WHITE
#define MENU_INACTIVE_FOREGROUND COLOR_GRAY
#define MENU_DISPLAY_FOREGROUND  COLOR_LIGHTGREEN

#define MENU_SECTIONHEADER_FOREGROUND   COLOR_YELLOW

//
// Setup program defines
//

#define SETUPPROGRAMNAME ("Duke Nukem 3D Setup")
#define SETUPPROGRAMVERSION ("1.2")

#define GAMENAME "Duke Nukem 3D"
#define GAMELAUNCHER ("DUKE3D.EXE")
#define GAMETOTYPE ("DUKE3D")

#define MENUFOOTER "Esc Exits   Move  ды Selects\0"

#define COMMITLAUNCHER ("COMMIT.EXE")
#define COMMITFILENAME ("COMMIT.DAT")

#define MAXVOICES 8
#define SONGNAME ("Duke Nukem Theme Song")
//#define SOUNDSETUPLAUNCHER ("SNDSETUP.EXE")

// Default Socket Number

#define DEFAULTSOCKETNUMBER 0x8849

// Default RTS file

#define DEFAULTRTSFILE "DUKE.RTS"

// Default External Control file

#define DEFAULTCONTROLFILE "EXTERNAL.EXE"

// Default Help file

#define DEFAULTHELPFILE "DN3DHELP.EXE"

// RTS extension

#define RTSEXTENSION "RTS"

// Default Player name

#define DEFAULTPLAYERNAME "DUKE"

// Default Macros

#define MACRO1  "An inspiration for birth control."
#define MACRO2  "You're gonna die for that!"
#define MACRO3  "It hurts to be you."
#define MACRO4  "Lucky Son of a Bitch."
#define MACRO5  "Hmmm....Payback time."
#define MACRO6  "You bottom dwelling scum sucker."
#define MACRO7  "Damn, you're ugly."
#define MACRO8  "Ha ha ha...Wasted!"
#define MACRO9  "You suck!"
#define MACRO10 "AARRRGHHHHH!!!"

#ifdef __cplusplus
};
#endif
#endif
