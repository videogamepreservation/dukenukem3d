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

// function.c

// file created by makehead.exe
// this header contains default key assignments, as well as
// default button assignments and game function names


char * gamefunctions[] =
   {
   "Move_Forward",
   "Move_Backward",
   "Turn_Left",
   "Turn_Right",
   "Strafe",
   "Strafe_Left",
   "Strafe_Right",
   "TurnAround",
   "Jump",
   "Crouch",
   "Fire",
   "Open",
   "Look_Up",
   "Look_Down",
   "Aim_Up",
   "Aim_Down",
   "Run",
   "SendMessage",
   "Weapon_1",
   "Weapon_2",
   "Weapon_3",
   "Weapon_4",
   "Weapon_5",
   "Weapon_6",
   "Weapon_7",
   "Weapon_8",
   "Weapon_9",
   "Weapon_10",
   "Pause",
   "Map",
   "Look_Left",
   "Look_Right",
   "Gamma_Correction",
   "Escape",
   "Shrink_Screen",
   "Enlarge_Screen",
   "ScreenPeek",
   "Transparent_Mode",
   "AutoRun",
   "Virtual_Mode",
   "Center_View",
   "Holster_Weapon",
   "Inventory_Left",
   "Inventory_Right",
   "Holo_Duke",
   "Jetpack",
   "Powerup_Toggle",
   "Inventory",
   "Remote_Ridicule_1",
   "Remote_Ridicule_2",
   "Remote_Ridicule_3",
   "Remote_Ridicule_4",
   "Remote_Ridicule_5",
   "Remote_Ridicule_6",
   "Remote_Ridicule_7",
   "Remote_Ridicule_8",
   "Remote_Ridicule_9",
   "Remote_Ridicule_10",
   "Cross_Hair_Toggle",
   };

#define NUMKEYENTRIES 59

static char * keydefaults[] =
   {
   "Move_Forward", "Up", "Kpad8", 
   "Move_Backward", "Down", "Kpad2", 
   "Turn_Left", "Left", "Kpad4", 
   "Turn_Right", "Right", "Kpad6", 
   "Strafe", "LAlt", "RAlt", 
   "Strafe_Left", ",", "", 
   "Strafe_Right", ".", "", 
   "TurnAround", "BakSpc", "", 
   "Jump", "A", "", 
   "Crouch", "Z", "", 
   "Fire", "LCtrl", "RCtrl", 
   "Open", "Space", "", 
   "Look_Up", "PgUp", "", 
   "Look_Down", "PgDn", "", 
   "Aim_Up", "Home", "", 
   "Aim_Down", "End", "", 
   "Run", "LShift", "RShift", 
   "SendMessage", "T", "", 
   "Weapon_1", "1", "", 
   "Weapon_2", "2", "", 
   "Weapon_3", "3", "", 
   "Weapon_4", "4", "", 
   "Weapon_5", "5", "", 
   "Weapon_6", "6", "", 
   "Weapon_7", "7", "", 
   "Weapon_8", "8", "", 
   "Weapon_9", "9", "", 
   "Weapon_10", "0", "", 
   "Pause", "Pause", "", 
   "Map", "Tab", "", 
   "Look_Left", "[", "", 
   "Look_Right", "]", "", 
   "Gamma_Correction", "F11", "", 
   "Escape", "Escape", "", 
   "Shrink_Screen", "-", "Kpad-", 
   "Enlarge_Screen", "=", "Kpad+", 
   "ScreenPeek", "K", "", 
   "Transparent_Mode", "W", "", 
   "AutoRun", "CapLck", "", 
   "Virtual_Mode", "V", "", 
   "Center_View", "KPad5", "", 
   "Holster_Weapon", "KpdEnt", "", 
   "Inventory_Left", "[", "", 
   "Inventory_Right", "]", "", 
   "Holo_Duke", "H", "", 
   "Jetpack", "J", "", 
   "Powerup_Toggle", "X", "", 
   "Inventory", "Enter", "", 
   "Remote_Ridicule_1", "F1", "", 
   "Remote_Ridicule_2", "F2", "", 
   "Remote_Ridicule_3", "F3", "", 
   "Remote_Ridicule_4", "F4", "", 
   "Remote_Ridicule_5", "F5", "", 
   "Remote_Ridicule_6", "F6", "", 
   "Remote_Ridicule_7", "F7", "", 
   "Remote_Ridicule_8", "F8", "", 
   "Remote_Ridicule_9", "F9", "", 
   "Remote_Ridicule_10", "F10", "", 
   "Cross_Hair_Toggle", "C", "", 
   };


static char * mousedefaults[] =
   {
   "Fire",
   "Strafe",
   "Move_Forward",
   };


static char * mouseclickeddefaults[] =
   {
   "",
   "Open",
   "",
   };


static char * joystickdefaults[] =
   {
   "",
   "",
   "",
   "",
   };


static char * joystickclickeddefaults[] =
   {
   "",
   "",
   "",
   "",
   };
