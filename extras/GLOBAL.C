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

#include "duke3d.h"

char *mymembuf;
char MusicPtr[72000];

short global_random;
short neartagsector, neartagwall, neartagsprite;

long gc,neartaghitdist,lockclock,max_player_health,max_armour_amount,max_ammo_amount[MAX_WEAPONS];

// long temp_data[MAXSPRITES][6];
struct weaponhit hittype[MAXSPRITES];
short spriteq[1024],spriteqloc,spriteqamount=64,moustat;
struct animwalltype animwall[MAXANIMWALLS];
short numanimwalls;
long *animateptr[MAXANIMATES], animategoal[MAXANIMATES], animatevel[MAXANIMATES], animatecnt;
// long oanimateval[MAXANIMATES];
short animatesect[MAXANIMATES];
long msx[2048],msy[2048];
short cyclers[MAXCYCLERS][6],numcyclers;

char fta_quotes[NUMOFFIRSTTIMEACTIVE][64];

unsigned char tempbuf[2048], packbuf[576];

char buf[80];

short camsprite;
short mirrorwall[64], mirrorsector[64], mirrorcnt;

int current_menu;

char betaname[80];

char level_names[44][33],level_file_names[44][128];
long partime[44],designertime[44];
char volume_names[4][33];
char skill_names[5][33];

volatile long checksume;
long soundsiz[NUM_SOUNDS];

short soundps[NUM_SOUNDS],soundpe[NUM_SOUNDS],soundvo[NUM_SOUNDS];
char soundm[NUM_SOUNDS],soundpr[NUM_SOUNDS];
char sounds[NUM_SOUNDS][14];

short title_zoom;

fx_device device;

SAMPLE Sound[ NUM_SOUNDS ];
SOUNDOWNER SoundOwner[NUM_SOUNDS][4];

char numplayersprites,loadfromgrouponly,earthquaketime;

long fricxv,fricyv;
struct player_orig po[MAXPLAYERS];
struct player_struct ps[MAXPLAYERS];
struct user_defs ud;

char pus, pub;
char syncstat, syncval[MAXPLAYERS][MOVEFIFOSIZ];
long syncvalhead[MAXPLAYERS], syncvaltail, syncvaltottail;

input sync[MAXPLAYERS], loc;
input recsync[RECSYNCBUFSIZ];
long avgfvel, avgsvel, avgavel, avghorz, avgbits;


input inputfifo[MOVEFIFOSIZ][MAXPLAYERS];
input recsync[RECSYNCBUFSIZ];

long movefifosendplc;

  //Multiplayer syncing variables
short screenpeek;
long movefifoend[MAXPLAYERS];


    //Game recording variables

char playerreadyflag[MAXPLAYERS],ready2send;
char playerquitflag[MAXPLAYERS];
long vel, svel, angvel, horiz, ototalclock, respawnactortime=768, respawnitemtime=768, groupfile;

long script[MAXSCRIPTSIZE],*scriptptr,*insptr,*labelcode,labelcnt;
long *actorscrptr[MAXTILES],*parsing_actor;
char *label,*textptr,error,warning,killit_flag;
char *music_pointer;
char actortype[MAXTILES];


char display_mirror,typebuflen,typebuf[41];

char music_fn[4][11][13],music_select;
char env_music_fn[4][13];
char rtsplaying;


short weaponsandammosprites[15] = {
        RPGSPRITE,
        CHAINGUNSPRITE,
        DEVISTATORAMMO,
        RPGAMMO,
        RPGAMMO,
        JETPACK,
        SHIELD,
        FIRSTAID,
        STEROIDS,
        RPGAMMO,
        RPGAMMO,
        RPGSPRITE,
        RPGAMMO,
        FREEZESPRITE,
        FREEZEAMMO
    };

long impact_damage;

	//GLOBAL.C - replace the end "my's" with this
long myx, omyx, myxvel, myy, omyy, myyvel, myz, omyz, myzvel;
short myhoriz, omyhoriz, myhorizoff, omyhorizoff;
short myang, omyang, mycursectnum, myjumpingcounter,frags[MAXPLAYERS][MAXPLAYERS];

char myjumpingtoggle, myonground, myhardlanding, myreturntocenter;
signed char multiwho, multipos, multiwhat, multiflag;

long fakemovefifoplc,movefifoplc;
long myxbak[MOVEFIFOSIZ], myybak[MOVEFIFOSIZ], myzbak[MOVEFIFOSIZ];
long myhorizbak[MOVEFIFOSIZ],dukefriction = 0xcc00, show_shareware;

short myangbak[MOVEFIFOSIZ];
char myname[32],camerashitable,freezerhurtowner=0,lasermode;
char networkmode = 255, movesperpacket = 1,gamequit = 0,playonten = 0,everyothertime;
long numfreezebounces=3,rpgblastradius,pipebombblastradius,tripbombblastradius,shrinkerblastradius,morterblastradius,bouncemineblastradius,seenineblastradius;
STATUSBARTYPE sbar;

long myminlag[MAXPLAYERS], mymaxlag, otherminlag, bufferjitter = 1;
short numclouds,clouds[128],cloudx[128],cloudy[128];
long cloudtotalclock = 0,totalmemory = 0;
long numinterpolations = 0, startofdynamicinterpolations = 0;
long oldipos[MAXINTERPOLATIONS];
long bakipos[MAXINTERPOLATIONS];
long *curipos[MAXINTERPOLATIONS];

