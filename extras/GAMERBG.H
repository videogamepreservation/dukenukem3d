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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include <io.h>
#include <fcntl.H>
#include <time.h>
#include <ctype.h>
#include "pragmas.h"

#include "build.h"


#define RBG_DEBUG  1

#ifdef RBG_DEBUG
   char  rbg_names[MAXTILES][17];
   long  rbg_loop_count = 0L;
   FILE *rbg_outfile;
   char  rbg_outname[13] = "debug.dat";
   void  rbg_loadnames( void );
   void  rbg_debug( void );
#endif




#define FOURSLEIGHT (2<<8)


#include "types.h"
#include "file_lib.h"
#include "develop.h"
#include "gamedefs.h"
#include "keyboard.h"
#include "util_lib.h"
#include "function.h"
#include "fx_man.h"
#include "config.h"
#include "sounds.h"
#include "control.h"
#include "soundefs.h"


#include "task_man.h"
#include "music.h"
#include "sndcards.h"

#include "names.h"

#define TICRATE (120)
#define TICSPERFRAME (TICRATE/26)

#define GC (TICSPERFRAME*50)

#define NUM_SOUNDS 192

#pragma aux sgn =\
	"add ebx, ebx",\
	"sbb eax, eax",\
	"cmp eax, ebx",\
	"adc eax, 0",\
	parm [ebx]\

#define    ALT_IS_PRESSED ( KB_KeyPressed( sc_RightAlt ) || KB_KeyPressed( sc_LeftAlt ) )


#define    BLACK 0
#define    DARKBLUE 1
#define    DARKGREEN 2
#define    DARKCYAN 3
#define    DARKRED 4
#define    DARKPURPLE 5
#define    BROWN 6
#define    LIGHTGRAY 7

#define    DARKGRAY 8
#define    BLUE 9
#define    GREEN 10
#define    CYAN 11
#define    RED 12
#define    PURPLE 13
#define    YELLOW 14
#define    WHITE 15

#define    PHEIGHT (38<<8)

#define P(X) printf("%ld\n",X);

#define SAVE_EXT ".SAV"
#define LEVEL_NAME "E_L_.MAP"
#define MODE_MENU   1
#define MODE_DEMO   2
#define MODE_GAME   4
#define MODE_EOL    8
#define MODE_TYPE  16
#define MODE_END  128

#define MAXANIMWALLS 256
#define NUMOFFIRSTTIMEACTIVE 128
#define MAXPSKYTILES 256
#define MAXCYCLERS 256
#define MAXSCRIPTSIZE 15600
#define MAXANIMATES 512

#define SP  sprite[i].yvel
#define SX  sprite[i].x
#define SY  sprite[i].y
#define SZ  sprite[i].z
#define SS  sprite[i].shade
#define PN  sprite[i].picnum
#define SA  sprite[i].ang
#define SV  sprite[i].xvel
#define ZV  sprite[i].zvel
#define RX  sprite[i].xrepeat
#define RY  sprite[i].yrepeat
#define OW  sprite[i].owner
#define CS  sprite[i].cstat
#define SH  sprite[i].extra
#define CX  sprite[i].xoffset
#define CY  sprite[i].yoffset
#define CD  sprite[i].clipdist
#define PL  sprite[i].pal
#define SLT  sprite[i].lotag
#define SHT  sprite[i].hitag
#define SECT sprite[i].sectnum

#define TRAND krand()
// (randomseed+=8675309L)&65535)

#define T1  hittype[i].temp_data[0]
#define T2  hittype[i].temp_data[1]
#define T3  hittype[i].temp_data[2]
#define T4  hittype[i].temp_data[3]
#define T5  hittype[i].temp_data[4]
#define T6  hittype[i].temp_data[5]

#define ESCESCAPE if(KB_KeyPressed( sc_Escape ) ) gameexit("");

#define PSUS ps[p].us=numpages
#define PUS p->us=numpages


#define ADDAMMO(P,N,W) {if(ps[P].ammo_amount[1]==0 && W != 0)defaultpowerup(&ps[P],&ps[P].ammo_amount[0]); ps[P].ammo_amount[W] += N;ps[P].us=numpages;}
#define ADDCAMMO(P,N,W) {if(ps[P].ammo_c_amount[1]==0 && W != 0)defaultpowerup(&ps[P],&ps[P].ammo_c_amount[0]); ps[P].ammo_c_amount[W] +=N;ps[P].us=numpages;}
#define ADDBATAMMO(P,N,W) {if(ps[P].ammo_bat_amount[1]==0 && W != 0)defaultpowerup(&ps[P],&ps[P].ammo_bat_amount[0]); ps[P].ammo_bat_amount[W] +=N;ps[P].us=numpages;}
#define ADDHAMMO(P,N,W) {if(ps[P].ammo_h_amount[1]==0 && W!=0)defaultpowerup(&ps[P],&ps[P].ammo_h_amount[0]); ps[P].ammo_h_amount[W]+=N;ps[P].us=numpages;}
#define ADDFAMMO(P,N,W) {if(ps[P].ammo_f_amount[1]==0 && W!=0)defaultpowerup(&ps[P],&ps[P].ammo_f_amount[0]); ps[P].ammo_f_amount[W]+=N;ps[P].us=numpages;}
#define ADDRAMMO(P,N,W) {if(ps[P].ammo_r_amount[1]==0 && W!=0)defaultpowerup(&ps[P],&ps[P].ammo_r_amount[0]); ps[P].ammo_r_amount[W]+=N;ps[P].us=numpages;}
#define ADDHLAMMO(P,N,W) {if(ps[P].ammo_hl_amount[1]==0 && W != 0)defaultpowerup(&ps[P],&ps[P].ammo_hl_amount[0]); ps[P].ammo_hl_amount[W] +=N;ps[P].us=numpages;}



#define IFWITHIN(B,E) if((PN)>=(B) && (PN)<=(E))
#define KILLIT(KX) {deletesprite(KX);goto BOLT;}

#define IFMOVING if(ssp(i,0))
#define IFHIT j=ifhitbyweapon(i);if(j >= 0)
#define IFHITSECT j=ifhitsectors(s->sectnum);if(j >= 0)

#define AFLAMABLE(X) (X==TREE1||X==TREE2||X==BOX||X==TIRE)


#define IFSKILL1 if(player_skill<1)
#define IFSKILL2 if(player_skill<2)
#define IFSKILL3 if(player_skill<3)
#define IFSKILL4 if(player_skill<4)

#define rnd(X) ((TRAND>>8)>=(255-(X)))

typedef struct
{
    long x,y,z;
    short i;
    int voice;
    char num;
} SOUNDOWNER;

#define __USRHOOKS_H

enum USRHOOKS_Errors
   {
   USRHOOKS_Warning = -2,
   USRHOOKS_Error   = -1,
   USRHOOKS_Ok      = 0
   };


typedef struct
{
    char *ptr;
    char lock;
    int  length;
} SAMPLE;

static struct animwalltype
{
        short wallnum;
        long tag;
};
extern struct animwalltype animwall[MAXANIMWALLS];
extern short numanimwalls;

extern char *mymembuf;
extern char typebuflen,typebuf[30];
extern char MusicPtr[60000];
extern long msx[2048],msy[2048];
extern short cyclers[MAXCYCLERS][6],numcyclers;

// KILL QUOTES diring playback.
// Make cops hittable more,
// Title in between.


typedef struct user_defs
{
    char fta_on,respawn_on,tickrate,coords,screen_tilting;
    char god,monsters_off,warp_on,coop,shadows;
    char displaytext, randomplayerloc, giveallkeys, clip,detail,airduct_off;
    char user_name[MAXPLAYERS][21];
    char ridecule[10][40];
    char savegame[10][22];

    short pause_on,screen_size,reverb_amount;
    short camerasprite,last_camsprite,brightness;
    short jute_mnum,jute_snum;

    long const_visibility,uw_framerate;
    long camera_time;

    int32 entered_name;

};

typedef struct player_orig
{
    long ox,oy,oz;
    short oa,os;
};

extern char numplayersprites;
extern char player_skill,level_number,volume_number;
typedef struct player_struct
{
    long zoom,exitx,exity,loogiex[64],loogiey[64],numloogs,loogcnt;
    long posx, posy, posz, horiz;
    long oposx,oposy,oposz,pyoff;
    long posxv,posyv,poszv,last_pissed_time;
    long player_par,visibility;
    long bobcounter,weapon_sway;
    long pals_time,flaming,crack_time;

    short ang,oang,angvel,cursectnum,look_ang,last_extra,steroids_shade;
    short ammo_c_amount[2],ammo_f_amount[2],ammo_amount[2],ammo_bat_amount[2],ammo_h_amount[2],ammo_r_amount[2],ammo_hl_amount[2];
    short holoduke_amount,newowner,hurt_delay,hbomb_hold_delay,alchocol_amount;
    short jumping_counter,airleft,last_weapon,knee_incs,access_incs;
    short fta[NUMOFFIRSTTIMEACTIVE],ftq,access_wallnum,access_spritenum;
    short kickback_pic,current_weapon,got_access,weapon_ang,firstaid_amount;
    short somethingonplayer,on_crane,i,one_parallax_sectnum;
    short over_shoulder_on,random_club_frame,fist_incs;
    short frag,one_eighty_count,cheat_phase;
    short dummyplayersprite,extra_extra8;
    short heat_amount;

    char gm,on_warping_sector,footprintcount,overhead_on,weapon_flash;
    char hbomb_on,jumping_toggle,onmovingsector,rapid_fire_hold,on_ground;
    char name[13],inven_icon,loogtype,crosshair;

    char jetpack_on,on_waterbridge,lastrandomspot,harley_mode;
    char bodysuit_on,scuba_on,us,had_bodysuit,footprintpal;

    short rotscrnang,dead_flag,show_empty_weapon,steroid_doses,last_steroids_amount;
    short scuba_amount,jetpack_amount,steroids_amount,shield_amount;
    short bodysuit_amount,holoduke_on,pycount;
    short transporter_hold,last_full_weapon,footprintshade;

    char gotweapon3,gotweapon4,gotweapon5,gotweapon6,gotweapon7,gotweapon8,gotweapon9,gotweapon10;
    char togweapon2,togweapon3,togweapon4,togweapon5,togweapon7,togweapon8,togweapon9,last_overhead;
    char toggle_key_flag,knuckle_incs,weapon_pos,select_dir;
    char walking_snd_toggle,interface_toggle_flag, palookup, hard_landing;
    char max_secret_rooms,secret_rooms,fire_flag,pals[3],show_help;
    char max_squashable,squashable,everything_transluscent;
    char max_actors_killed,actors_killed,auto_run,return_to_center;
};

extern unsigned char tempbuf[1596];
extern long max_player_health,max_armour_amount,max_ammo_amount;

extern long impact_damage;

#define MOVEFIFOSIZ 256
#define MOVFIFOSIZ 256

extern char syncvalplc, othersyncvalplc;
extern char syncvalend, othersyncvalend;
extern long syncvalcnt, othersyncvalcnt;
extern long syncval[MOVEFIFOSIZ], othersyncval[MOVEFIFOSIZ];

// extern long temp_data[MAXSPRITES][6];
extern short spriteq[64],spriteqloc;
extern struct player_struct ps[MAXPLAYERS];
extern struct player_orig po[MAXPLAYERS];
extern struct user_defs ud;
extern short int moustat;
extern short int global_random;
extern long scaredfallz;
extern char buf[80]; //My own generic input buffer

extern short bonus_xy[3][16];

extern char fta_quotes[NUMOFFIRSTTIMEACTIVE][64];
extern char *mainmem;
extern char scantoasc[128],ready2send;
extern char scantoascwithshift[128];

//MED
//extern char *SoundCardNames[ NumSoundCards + 1 ];
//extern int   SoundCards[ NumSoundCards + 1 ];
extern char diz[500];

extern long chainnumpages;


extern fx_device device;
extern SAMPLE Sound[ NUM_SOUNDS ];
extern SOUNDOWNER SoundOwner[NUM_SOUNDS][4];

// extern signed char clubframes[6][20];

extern char *user_quote;
extern short user_quote_time;

extern char playerreadyflag[MAXPLAYERS];
extern char sounds[NUM_SOUNDS][14];

extern long script[MAXSCRIPTSIZE],*scriptptr,*insptr,*labelcode,labelcnt;
extern char *label,*textptr,error,warning,killit_flag;
extern long *actorscrptr[MAXSPRITES],*parsing_actor;
extern char *music_pointer;
// extern char *keyw[];

extern char ipath[80],opath[80];

extern char music_fn[4][10][13],music_select;
extern char env_music_fn[10][13];
extern short camsprite;

extern char gotz;
extern char inspace(short sectnum);

typedef struct weaponhit
{
    char cgg;
    short picnum,ang,extra,owner,movflag;
    short tempang,whattospawn,dispicnum;
    short timetosleep;
    long floorz,ceilingz,lastvx,lastvy,bposx,bposy,bposz;
    long temp_data[6];
};

extern struct weaponhit hittype[1536];

typedef struct synctype
{
    short syncvel, syncsvel, syncangvel;
    unsigned long syncbits;
} synctype;

extern struct synctype sync[MAXPLAYERS],fsync[MAXPLAYERS],osync[MAXPLAYERS];
extern struct synctype recsync[16384];

extern short numplayers, myconnectindex;
extern short connecthead, connectpoint2[MAXPLAYERS];   //Player linked list variables (indeces, not connection numbers)
extern short screenpeek;

extern long reccnt, recstat;       //Can only record 1-player game

extern int current_menu;
extern long tempwallptr,animatecnt;
extern long lockclock,frameplace;
extern char display_mirror,dummy,loadfromgrouponly;

extern short locvel, olocvel;
extern short locsvel, olocsvel;
extern short locangvel, olocangvel;

extern unsigned long locbits, olocbits;
extern long movefifoplc, movefifoend;
extern long ototalclock, gotlastpacketclock;

extern long *animateptr[MAXANIMATES], animategoal[MAXANIMATES];
extern long animatevel[MAXANIMATES];
extern long oanimateval[MAXANIMATES];
extern short neartagsector, neartagwall, neartagsprite;
extern long neartaghitdist;
extern short animatesect[MAXANIMATES];
extern unsigned char palette[768];
extern long movefifoplc, movefifoend,vel,svel,angvel;
extern short mirrorwall[64], mirrorsector[64], mirrorcnt;

extern void TestCallBack(unsigned long);

// #define NUMOPTIONS 8
#define NUMKEYS 19

#define MOVEFIFOSIZ 256
extern short baksyncvel[MOVEFIFOSIZ][MAXPLAYERS];
extern short baksyncsvel[MOVEFIFOSIZ][MAXPLAYERS];
extern short baksyncangvel[MOVEFIFOSIZ][MAXPLAYERS];
extern long baksyncbits[MOVEFIFOSIZ][MAXPLAYERS];

extern short otherp;
extern long otherx;

//extern char option[NUMOPTIONS];
//extern char keys[NUMKEYS];
extern long frameplace, chainplace, chainnumpages;
extern char palette[768];

//extern volatile char keystatus[256], keyfifo[KEYFIFOSIZ], keyfifoplc, keyfifoend;
//extern volatile char readch, oldreadch, extended, keytemp,checksume;
extern volatile long checksume;

#include "funct.h"

extern char screencapt,rotatedummy;
extern char soundp[NUM_SOUNDS],soundpr[NUM_SOUNDS],soundm[NUM_SOUNDS];
extern long soundsiz[NUM_SOUNDS];
extern char level_names[24][33];

// extern long bposx[MAXSPRITES],bposy[MAXSPRITES],bposz[MAXSPRITES];
extern int32 SoundToggle,MusicToggle;
extern char everyothertime;
extern char restorepalette;

extern long animdelay;
extern char multimode;
extern long cachecount;
