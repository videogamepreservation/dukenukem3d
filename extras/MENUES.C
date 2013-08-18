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
#include "mouse.h"
#include "animlib.h"

extern char inputloc;
extern int recfilep;
extern char vgacompatible;
short probey=0,lastprobey=0,last_menu,globalskillsound=-1;
short sh,onbar,buttonstat,deletespot;
short last_zero,last_fifty,last_threehundred = 0;
static char fileselect = 1, menunamecnt, menuname[256][17], curpath[80], menupath[80];

/* Error codes */
#define eTenBnNotInWindows 3801
#define eTenBnBadGameIni 3802
#define eTenBnBadTenIni 3803
#define eTenBnBrowseCancel 3804
#define eTenBnBadTenInst 3805

int  tenBnStart(void);
void tenBnSetBrowseRtn(char *(*rtn)(char *str, int len));
void tenBnSetExitRtn(void (*rtn)(void));
void tenBnSetEndRtn(void (*rtn)(void));

void dummyfunc(void)
{
}

void dummymess(int i,char *c)
{
}

void TENtext(void)
{
    long dacount,dalastcount;

    puts("\nDuke Nukem 3D has been licensed exclusively to TEN (Total");
    puts("Entertainment Network) for wide-area networked (WAN) multiplayer");
    puts("games.\n");

    puts("The multiplayer code within Duke Nukem 3D has been highly");
    puts("customized to run best on TEN, where you'll experience fast and");
    puts("stable performance, plus other special benefits.\n");

    puts("We do not authorize or recommend the use of Duke Nukem 3D with");
    puts("gaming services other than TEN.\n");

    puts("Duke Nukem 3D is protected by United States copyright law and");
    puts("international treaty.\n");

    puts("For the best online multiplayer gaming experience, please call TEN");
    puts("at 800-8040-TEN, or visit TEN's Web Site at www.ten.net.\n");

    puts("Press any key to continue.\n");

    _bios_timeofday(0,&dacount);

    while( _bios_keybrd(1) == 0 )
    {
        _bios_timeofday(0,&dalastcount);
        if( (dacount+240) < dalastcount ) break;
    }
}

void cmenu(short cm)
{
    current_menu = cm;

    if( (cm >= 1000 && cm <= 1009) )
        return;

    if( cm == 0 )
        probey = last_zero;
    else if(cm == 50)
        probey = last_fifty;
    else if(cm >= 300 && cm < 400)
        probey = last_threehundred;
    else if(cm == 110)
        probey = 1;
    else probey = 0;
    lastprobey = -1;
}


void savetemp(char *fn,long daptr,long dasiz)
{
    FILE *fp;

    if ((fp = fopen(fn,"wb")) == (FILE *)NULL)
       return;

    fwrite((char *)daptr,dasiz,1,fp);

    fclose(fp);
}

void getangplayers(short snum)
{
    short i,a;

    for(i=connecthead;i>=0;i=connectpoint2[i])
    {
        if(i != snum)
        {
            a = ps[snum].ang+getangle(ps[i].posx-ps[snum].posx,ps[i].posy-ps[snum].posy);
            a = a-1024;
            rotatesprite(
                (320<<15) + (((sintable[(a+512)&2047])>>7)<<15),
                (320<<15) - (((sintable[a&2047])>>8)<<15),
                klabs(sintable[((a>>1)+768)&2047]<<2),0,APLAYER,0,ps[i].palookup,0,0,0,xdim-1,ydim-1);
        }
    }
}

loadpheader(char spot,int32 *vn,int32 *ln,int32 *psk,int32 *nump)
{

     long i;
	 char *fn = "game0.sav";
	 long fil;
     long bv;

	 fn[4] = spot+'0';

     if ((fil = kopen4load(fn,0)) == -1) return(-1);

     walock[MAXTILES-3] = 255;

     kdfread(&bv,4,1,fil);
     if(bv != BYTEVERSION)
     {
        FTA(114,&ps[myconnectindex]);
        kclose(fil);
        return 1;
     }

     kdfread(nump,sizeof(int32),1,fil);

     kdfread(tempbuf,19,1,fil);
	 kdfread(vn,sizeof(int32),1,fil);
	 kdfread(ln,sizeof(int32),1,fil);
     kdfread(psk,sizeof(int32),1,fil);

     if (waloff[MAXTILES-3] == 0) allocache(&waloff[MAXTILES-3],160*100,&walock[MAXTILES-3]);
     tilesizx[MAXTILES-3] = 100; tilesizy[MAXTILES-3] = 160;
     kdfread((char *)waloff[MAXTILES-3],160,100,fil);

	 kclose(fil);

	 return(0);
}


loadplayer(signed char spot)
{
     short k;
     char *fn = "game0.sav";
     char *mpfn = "gameA_00.sav";
     char *fnptr, scriptptrs[MAXSCRIPTSIZE];
     long fil, bv, i, j, x;
     int32 nump;

     if(spot < 0)
     {
        multiflag = 1;
        multiwhat = 0;
        multipos = -spot-1;
        return -1;
     }

     if( multiflag == 2 && multiwho != myconnectindex )
     {
         fnptr = mpfn;
         mpfn[4] = spot + 'A';

         if(ud.multimode > 9)
         {
             mpfn[6] = (multiwho/10) + '0';
             mpfn[7] = (multiwho%10) + '0';
         }
         else mpfn[7] = multiwho + '0';
     }
     else
     {
        fnptr = fn;
        fn[4] = spot + '0';
     }

     if ((fil = kopen4load(fnptr,0)) == -1) return(-1);

     ready2send = 0;

     kdfread(&bv,4,1,fil);
     if(bv != BYTEVERSION)
     {
        FTA(114,&ps[myconnectindex]);
        kclose(fil);
        ototalclock = totalclock;
        ready2send = 1;
        return 1;
     }

     kdfread(&nump,sizeof(nump),1,fil);
     if(nump != numplayers)
     {
        kclose(fil);
        ototalclock = totalclock;
        ready2send = 1;
        FTA(124,&ps[myconnectindex]);
        return 1;
     }

     if(numplayers > 1)
     {
         pub = NUMPAGES;
         pus = NUMPAGES;
         vscrn();
         drawbackground();
         menutext(160,100,0,0,"LOADING...");
         nextpage();
    }

     waitforeverybody();

	 FX_StopAllSounds();
     clearsoundlocks();
	 MUSIC_StopSong();

     if(numplayers > 1)
         kdfread(&buf,19,1,fil);
     else
         kdfread(&ud.savegame[spot][0],19,1,fil);

//     music_changed = (music_select != (ud.volume_number*11) + ud.level_number);

	 kdfread(&ud.volume_number,sizeof(ud.volume_number),1,fil);
	 kdfread(&ud.level_number,sizeof(ud.level_number),1,fil);
	 kdfread(&ud.player_skill,sizeof(ud.player_skill),1,fil);

	 ud.m_level_number = ud.level_number;
	 ud.m_volume_number = ud.volume_number;
	 ud.m_player_skill = ud.player_skill;

		 //Fake read because lseek won't work with compression
     walock[MAXTILES-3] = 1;
     if (waloff[MAXTILES-3] == 0) allocache(&waloff[MAXTILES-3],160*100,&walock[MAXTILES-3]);
     tilesizx[MAXTILES-3] = 100; tilesizy[MAXTILES-3] = 160;
     kdfread((char *)waloff[MAXTILES-3],160,100,fil);

	 kdfread(&numwalls,2,1,fil);
     kdfread(&wall[0],sizeof(walltype),MAXWALLS,fil);
	 kdfread(&numsectors,2,1,fil);
     kdfread(&sector[0],sizeof(sectortype),MAXSECTORS,fil);
	 kdfread(&sprite[0],sizeof(spritetype),MAXSPRITES,fil);
	 kdfread(&headspritesect[0],2,MAXSECTORS+1,fil);
	 kdfread(&prevspritesect[0],2,MAXSPRITES,fil);
	 kdfread(&nextspritesect[0],2,MAXSPRITES,fil);
	 kdfread(&headspritestat[0],2,MAXSTATUS+1,fil);
	 kdfread(&prevspritestat[0],2,MAXSPRITES,fil);
	 kdfread(&nextspritestat[0],2,MAXSPRITES,fil);
	 kdfread(&numcyclers,sizeof(numcyclers),1,fil);
	 kdfread(&cyclers[0][0],12,MAXCYCLERS,fil);
     kdfread(ps,sizeof(ps),1,fil);
     kdfread(po,sizeof(po),1,fil);
	 kdfread(&numanimwalls,sizeof(numanimwalls),1,fil);
	 kdfread(&animwall,sizeof(animwall),1,fil);
	 kdfread(&msx[0],sizeof(long),sizeof(msx)/sizeof(long),fil);
	 kdfread(&msy[0],sizeof(long),sizeof(msy)/sizeof(long),fil);
     kdfread((short *)&spriteqloc,sizeof(short),1,fil);
     kdfread((short *)&spriteqamount,sizeof(short),1,fil);
     kdfread((short *)&spriteq[0],sizeof(short),spriteqamount,fil);
	 kdfread(&mirrorcnt,sizeof(short),1,fil);
	 kdfread(&mirrorwall[0],sizeof(short),64,fil);
     kdfread(&mirrorsector[0],sizeof(short),64,fil);
     kdfread(&show2dsector[0],sizeof(char),MAXSECTORS>>3,fil);
     kdfread(&actortype[0],sizeof(char),MAXTILES,fil);
     kdfread(&boardfilename[0],sizeof(boardfilename),1,fil);

     kdfread(&numclouds,sizeof(numclouds),1,fil);
     kdfread(&clouds[0],sizeof(short)<<7,1,fil);
     kdfread(&cloudx[0],sizeof(short)<<7,1,fil);
     kdfread(&cloudy[0],sizeof(short)<<7,1,fil);

     kdfread(&scriptptrs[0],1,MAXSCRIPTSIZE,fil);
     kdfread(&script[0],4,MAXSCRIPTSIZE,fil);
     for(i=0;i<MAXSCRIPTSIZE;i++)
        if( scriptptrs[i] )
     {
         j = (long)script[i]+(long)&script[0];
         script[i] = j;
     }

     kdfread(&actorscrptr[0],4,MAXTILES,fil);
     for(i=0;i<MAXTILES;i++)
         if(actorscrptr[i])
     {
        j = (long)actorscrptr[i]+(long)&script[0];
        actorscrptr[i] = (long *)j;
     }

     kdfread(&scriptptrs[0],1,MAXSPRITES,fil);
     kdfread(&hittype[0],sizeof(struct weaponhit),MAXSPRITES,fil);

     for(i=0;i<MAXSPRITES;i++)
     {
        j = (long)(&script[0]);
        if( scriptptrs[i]&1 ) T2 += j;
        if( scriptptrs[i]&2 ) T5 += j;
        if( scriptptrs[i]&4 ) T6 += j;
     }

	 kdfread(&lockclock,sizeof(lockclock),1,fil);
     kdfread(&pskybits,sizeof(pskybits),1,fil);
     kdfread(&pskyoff[0],sizeof(pskyoff[0]),MAXPSKYTILES,fil);

	 kdfread(&animatecnt,sizeof(animatecnt),1,fil);
	 kdfread(&animatesect[0],2,MAXANIMATES,fil);
	 kdfread(&animateptr[0],4,MAXANIMATES,fil);
     for(i = animatecnt-1;i>=0;i--) animateptr[i] = (long *)((long)animateptr[i]+(long)(&sector[0]));
	 kdfread(&animategoal[0],4,MAXANIMATES,fil);
	 kdfread(&animatevel[0],4,MAXANIMATES,fil);

	 kdfread(&earthquaketime,sizeof(earthquaketime),1,fil);
     kdfread(&ud.from_bonus,sizeof(ud.from_bonus),1,fil);
     kdfread(&ud.secretlevel,sizeof(ud.secretlevel),1,fil);
     kdfread(&ud.respawn_monsters,sizeof(ud.respawn_monsters),1,fil);
     ud.m_respawn_monsters = ud.respawn_monsters;
     kdfread(&ud.respawn_items,sizeof(ud.respawn_items),1,fil);
     ud.m_respawn_items = ud.respawn_items;
     kdfread(&ud.respawn_inventory,sizeof(ud.respawn_inventory),1,fil);
     ud.m_respawn_inventory = ud.respawn_inventory;

     kdfread(&ud.god,sizeof(ud.god),1,fil);
     kdfread(&ud.auto_run,sizeof(ud.auto_run),1,fil);
     kdfread(&ud.crosshair,sizeof(ud.crosshair),1,fil);
     kdfread(&ud.monsters_off,sizeof(ud.monsters_off),1,fil);
     ud.m_monsters_off = ud.monsters_off;
     kdfread(&ud.last_level,sizeof(ud.last_level),1,fil);
     kdfread(&ud.eog,sizeof(ud.eog),1,fil);

     kdfread(&ud.coop,sizeof(ud.coop),1,fil);
     ud.m_coop = ud.coop;
     kdfread(&ud.marker,sizeof(ud.marker),1,fil);
     ud.m_marker = ud.marker;
     kdfread(&ud.ffire,sizeof(ud.ffire),1,fil);
     ud.m_ffire = ud.ffire;

     kdfread(&camsprite,sizeof(camsprite),1,fil);
     kdfread(&connecthead,sizeof(connecthead),1,fil);
     kdfread(connectpoint2,sizeof(connectpoint2),1,fil);
     kdfread(&numplayersprites,sizeof(numplayersprites),1,fil);
     kdfread((short *)&frags[0][0],sizeof(frags),1,fil);

     kdfread(&randomseed,sizeof(randomseed),1,fil);
     kdfread(&global_random,sizeof(global_random),1,fil);
     kdfread(&parallaxyscale,sizeof(parallaxyscale),1,fil);

     kclose(fil);

     if(ps[myconnectindex].over_shoulder_on != 0)
     {
         cameradist = 0;
         cameraclock = 0;
         ps[myconnectindex].over_shoulder_on = 1;
     }

     screenpeek = myconnectindex;

     clearbufbyte(gotpic,sizeof(gotpic),0L);
     clearsoundlocks();
	 cacheit();

     music_select = (ud.volume_number*11) + ud.level_number;
     playmusic(&music_fn[0][music_select][0]);

     ps[myconnectindex].gm = MODE_GAME;
	 ud.recstat = 0;

     if(ps[myconnectindex].jetpack_on)
         spritesound(DUKE_JETPACK_IDLE,ps[myconnectindex].i);

     restorepalette = 1;
     setpal(&ps[myconnectindex]);
     vscrn();

     FX_SetReverb(0);

     if(ud.lockout == 0)
     {
         for(x=0;x<numanimwalls;x++)
             if( wall[animwall[x].wallnum].extra >= 0 )
                 wall[animwall[x].wallnum].picnum = wall[animwall[x].wallnum].extra;
     }
     else
     {
         for(x=0;x<numanimwalls;x++)
             switch(wall[animwall[x].wallnum].picnum)
         {
             case FEMPIC1:
                 wall[animwall[x].wallnum].picnum = BLANKSCREEN;
                 break;
             case FEMPIC2:
             case FEMPIC3:
                 wall[animwall[x].wallnum].picnum = SCREENBREAK6;
                 break;
         }
     }

     numinterpolations = 0;
     startofdynamicinterpolations = 0;

     k = headspritestat[3];
     while(k >= 0)
     {
        switch(sprite[k].lotag)
        {
            case 31:
                setinterpolation(&sector[sprite[k].sectnum].floorz);
                break;
            case 32:
                setinterpolation(&sector[sprite[k].sectnum].ceilingz);
                break;
            case 25:
                setinterpolation(&sector[sprite[k].sectnum].floorz);
                setinterpolation(&sector[sprite[k].sectnum].ceilingz);
                break;
            case 17:
                setinterpolation(&sector[sprite[k].sectnum].floorz);
                setinterpolation(&sector[sprite[k].sectnum].ceilingz);
                break;
            case 0:
            case 5:
            case 6:
            case 11:
            case 14:
            case 15:
            case 16:
            case 26:
            case 30:
                setsectinterpolate(k);
                break;
        }

        k = nextspritestat[k];
     }

     for(i=numinterpolations-1;i>=0;i--) bakipos[i] = *curipos[i];
     for(i = animatecnt-1;i>=0;i--)
         setinterpolation(animateptr[i]);

     show_shareware = 0;
     everyothertime = 0;

     clearbufbyte(playerquitflag,MAXPLAYERS,0x01010101);

     resetmys();

     ready2send = 1;

     flushpackets();
     clearfifo();
     waitforeverybody();

     resettimevars();

     return(0);
}

saveplayer(signed char spot)
{
     long i, j;
	 char *fn = "game0.sav";
     char *mpfn = "gameA_00.sav";
     char *fnptr,scriptptrs[MAXSCRIPTSIZE];
	 FILE *fil;
     long bv = BYTEVERSION;

     if(spot < 0)
     {
        multiflag = 1;
        multiwhat = 1;
        multipos = -spot-1;
        return -1;
     }

     waitforeverybody();

     if( multiflag == 2 && multiwho != myconnectindex )
     {
         fnptr = mpfn;
         mpfn[4] = spot + 'A';

         if(ud.multimode > 9)
         {
             mpfn[6] = (multiwho/10) + '0';
             mpfn[7] = multiwho + '0';
         }
         else mpfn[7] = multiwho + '0';
     }
     else
     {
        fnptr = fn;
        fn[4] = spot + '0';
     }

     if ((fil = fopen(fnptr,"wb")) == 0) return(-1);

     ready2send = 0;

     dfwrite(&bv,4,1,fil);
     dfwrite(&ud.multimode,sizeof(ud.multimode),1,fil);

	 dfwrite(&ud.savegame[spot][0],19,1,fil);
	 dfwrite(&ud.volume_number,sizeof(ud.volume_number),1,fil);
     dfwrite(&ud.level_number,sizeof(ud.level_number),1,fil);
	 dfwrite(&ud.player_skill,sizeof(ud.player_skill),1,fil);
     dfwrite((char *)waloff[MAXTILES-1],160,100,fil);

	 dfwrite(&numwalls,2,1,fil);
     dfwrite(&wall[0],sizeof(walltype),MAXWALLS,fil);
	 dfwrite(&numsectors,2,1,fil);
     dfwrite(&sector[0],sizeof(sectortype),MAXSECTORS,fil);
	 dfwrite(&sprite[0],sizeof(spritetype),MAXSPRITES,fil);
	 dfwrite(&headspritesect[0],2,MAXSECTORS+1,fil);
	 dfwrite(&prevspritesect[0],2,MAXSPRITES,fil);
	 dfwrite(&nextspritesect[0],2,MAXSPRITES,fil);
	 dfwrite(&headspritestat[0],2,MAXSTATUS+1,fil);
	 dfwrite(&prevspritestat[0],2,MAXSPRITES,fil);
	 dfwrite(&nextspritestat[0],2,MAXSPRITES,fil);
	 dfwrite(&numcyclers,sizeof(numcyclers),1,fil);
	 dfwrite(&cyclers[0][0],12,MAXCYCLERS,fil);
     dfwrite(ps,sizeof(ps),1,fil);
     dfwrite(po,sizeof(po),1,fil);
	 dfwrite(&numanimwalls,sizeof(numanimwalls),1,fil);
	 dfwrite(&animwall,sizeof(animwall),1,fil);
	 dfwrite(&msx[0],sizeof(long),sizeof(msx)/sizeof(long),fil);
	 dfwrite(&msy[0],sizeof(long),sizeof(msy)/sizeof(long),fil);
     dfwrite(&spriteqloc,sizeof(short),1,fil);
     dfwrite(&spriteqamount,sizeof(short),1,fil);
     dfwrite(&spriteq[0],sizeof(short),spriteqamount,fil);
	 dfwrite(&mirrorcnt,sizeof(short),1,fil);
	 dfwrite(&mirrorwall[0],sizeof(short),64,fil);
	 dfwrite(&mirrorsector[0],sizeof(short),64,fil);
     dfwrite(&show2dsector[0],sizeof(char),MAXSECTORS>>3,fil);
     dfwrite(&actortype[0],sizeof(char),MAXTILES,fil);
     dfwrite(&boardfilename[0],sizeof(boardfilename),1,fil);

     dfwrite(&numclouds,sizeof(numclouds),1,fil);
     dfwrite(&clouds[0],sizeof(short)<<7,1,fil);
     dfwrite(&cloudx[0],sizeof(short)<<7,1,fil);
     dfwrite(&cloudy[0],sizeof(short)<<7,1,fil);

     for(i=0;i<MAXSCRIPTSIZE;i++)
     {
          if( (long)script[i] >= (long)(&script[0]) && (long)script[i] < (long)(&script[MAXSCRIPTSIZE]) )
          {
                scriptptrs[i] = 1;
                j = (long)script[i] - (long)&script[0];
                script[i] = j;
          }
          else scriptptrs[i] = 0;
     }

     dfwrite(&scriptptrs[0],1,MAXSCRIPTSIZE,fil);
     dfwrite(&script[0],4,MAXSCRIPTSIZE,fil);

     for(i=0;i<MAXSCRIPTSIZE;i++)
        if( scriptptrs[i] )
     {
        j = script[i]+(long)&script[0];
        script[i] = j;
     }

     for(i=0;i<MAXTILES;i++)
         if(actorscrptr[i])
     {
        j = (long)actorscrptr[i]-(long)&script[0];
        actorscrptr[i] = (long *)j;
     }
     dfwrite(&actorscrptr[0],4,MAXTILES,fil);
     for(i=0;i<MAXTILES;i++)
         if(actorscrptr[i])
     {
         j = (long)actorscrptr[i]+(long)&script[0];
         actorscrptr[i] = (long *)j;
     }

     for(i=0;i<MAXSPRITES;i++)
     {
        scriptptrs[i] = 0;

        if(actorscrptr[PN] == 0) continue;

        j = (long)&script[0];

        if(T2 >= j && T2 < (long)(&script[MAXSCRIPTSIZE]) )
        {
            scriptptrs[i] |= 1;
            T2 -= j;
        }
        if(T5 >= j && T5 < (long)(&script[MAXSCRIPTSIZE]) )
        {
            scriptptrs[i] |= 2;
            T5 -= j;
        }
        if(T6 >= j && T6 < (long)(&script[MAXSCRIPTSIZE]) )
        {
            scriptptrs[i] |= 4;
            T6 -= j;
        }
    }

    dfwrite(&scriptptrs[0],1,MAXSPRITES,fil);
    dfwrite(&hittype[0],sizeof(struct weaponhit),MAXSPRITES,fil);

    for(i=0;i<MAXSPRITES;i++)
    {
        if(actorscrptr[PN] == 0) continue;
        j = (long)&script[0];

        if(scriptptrs[i]&1)
            T2 += j;
        if(scriptptrs[i]&2)
            T5 += j;
        if(scriptptrs[i]&4)
            T6 += j;
    }

	 dfwrite(&lockclock,sizeof(lockclock),1,fil);
     dfwrite(&pskybits,sizeof(pskybits),1,fil);
     dfwrite(&pskyoff[0],sizeof(pskyoff[0]),MAXPSKYTILES,fil);
	 dfwrite(&animatecnt,sizeof(animatecnt),1,fil);
	 dfwrite(&animatesect[0],2,MAXANIMATES,fil);
	 for(i = animatecnt-1;i>=0;i--) animateptr[i] = (long *)((long)animateptr[i]-(long)(&sector[0]));
	 dfwrite(&animateptr[0],4,MAXANIMATES,fil);
	 for(i = animatecnt-1;i>=0;i--) animateptr[i] = (long *)((long)animateptr[i]+(long)(&sector[0]));
	 dfwrite(&animategoal[0],4,MAXANIMATES,fil);
	 dfwrite(&animatevel[0],4,MAXANIMATES,fil);

	 dfwrite(&earthquaketime,sizeof(earthquaketime),1,fil);
	 dfwrite(&ud.from_bonus,sizeof(ud.from_bonus),1,fil);
     dfwrite(&ud.secretlevel,sizeof(ud.secretlevel),1,fil);
     dfwrite(&ud.respawn_monsters,sizeof(ud.respawn_monsters),1,fil);
     dfwrite(&ud.respawn_items,sizeof(ud.respawn_items),1,fil);
     dfwrite(&ud.respawn_inventory,sizeof(ud.respawn_inventory),1,fil);
     dfwrite(&ud.god,sizeof(ud.god),1,fil);
     dfwrite(&ud.auto_run,sizeof(ud.auto_run),1,fil);
     dfwrite(&ud.crosshair,sizeof(ud.crosshair),1,fil);
     dfwrite(&ud.monsters_off,sizeof(ud.monsters_off),1,fil);
     dfwrite(&ud.last_level,sizeof(ud.last_level),1,fil);
     dfwrite(&ud.eog,sizeof(ud.eog),1,fil);
     dfwrite(&ud.coop,sizeof(ud.coop),1,fil);
     dfwrite(&ud.marker,sizeof(ud.marker),1,fil);
     dfwrite(&ud.ffire,sizeof(ud.ffire),1,fil);
     dfwrite(&camsprite,sizeof(camsprite),1,fil);
     dfwrite(&connecthead,sizeof(connecthead),1,fil);
     dfwrite(connectpoint2,sizeof(connectpoint2),1,fil);
     dfwrite(&numplayersprites,sizeof(numplayersprites),1,fil);
     dfwrite((short *)&frags[0][0],sizeof(frags),1,fil);

     dfwrite(&randomseed,sizeof(randomseed),1,fil);
     dfwrite(&global_random,sizeof(global_random),1,fil);
     dfwrite(&parallaxyscale,sizeof(parallaxyscale),1,fil);

	 fclose(fil);

     if(ud.multimode < 2)
     {
         strcpy(&fta_quotes[122],"GAME SAVED");
         FTA(122,&ps[myconnectindex]);
     }

     ready2send = 1;

     waitforeverybody();

     ototalclock = totalclock;

     return(0);
}

#define LMB (buttonstat&1)
#define RMB (buttonstat&2)

ControlInfo minfo;

long mi;

int probe(int x,int y,int i,int n)
{
    short centre, s;

    s = 1+(CONTROL_GetMouseSensitivity()>>4);

    if( ControllerType == 1 && CONTROL_MousePresent )
    {
        CONTROL_GetInput( &minfo );
        mi += minfo.dz;
    }

    else minfo.dz = minfo.dyaw = 0;

    if( x == (320>>1) )
        centre = 320>>2;
    else centre = 0;

    if(!buttonstat)
    {
        if( KB_KeyPressed( sc_UpArrow ) || KB_KeyPressed( sc_PgUp ) || KB_KeyPressed( sc_kpad_8 ) ||
            mi < -8192 )
        {
            mi = 0;
            KB_ClearKeyDown( sc_UpArrow );
            KB_ClearKeyDown( sc_kpad_8 );
            KB_ClearKeyDown( sc_PgUp );
            sound(KICK_HIT);

            probey--;
            if(probey < 0) probey = n-1;
            minfo.dz = 0;
        }
        if( KB_KeyPressed( sc_DownArrow ) || KB_KeyPressed( sc_PgDn ) || KB_KeyPressed( sc_kpad_2 )
            || mi > 8192 )
        {
            mi = 0;
            KB_ClearKeyDown( sc_DownArrow );
            KB_ClearKeyDown( sc_kpad_2 );
            KB_ClearKeyDown( sc_PgDn );
            sound(KICK_HIT);
            probey++;
            minfo.dz = 0;
        }
    }

    if(probey >= n)
        probey = 0;

    if(centre)
    {
//        rotatesprite(((320>>1)+(centre)+54)<<16,(y+(probey*i)-4)<<16,65536L,0,SPINNINGNUKEICON+6-((6+(totalclock>>3))%7),sh,0,10,0,0,xdim-1,ydim-1);
//        rotatesprite(((320>>1)-(centre)-54)<<16,(y+(probey*i)-4)<<16,65536L,0,SPINNINGNUKEICON+((totalclock>>3)%7),sh,0,10,0,0,xdim-1,ydim-1);

        rotatesprite(((320>>1)+(centre>>1)+70)<<16,(y+(probey*i)-4)<<16,65536L,0,SPINNINGNUKEICON+6-((6+(totalclock>>3))%7),sh,0,10,0,0,xdim-1,ydim-1);
        rotatesprite(((320>>1)-(centre>>1)-70)<<16,(y+(probey*i)-4)<<16,65536L,0,SPINNINGNUKEICON+((totalclock>>3)%7),sh,0,10,0,0,xdim-1,ydim-1);
    }
    else
        rotatesprite((x-tilesizx[BIGFNTCURSOR]-4)<<16,(y+(probey*i)-4)<<16,65536L,0,SPINNINGNUKEICON+(((totalclock>>3))%7),sh,0,10,0,0,xdim-1,ydim-1);

    if( KB_KeyPressed(sc_Space) || KB_KeyPressed( sc_kpad_Enter ) || KB_KeyPressed( sc_Enter ) || (LMB && !onbar) )
    {
        if(current_menu != 110)
            sound(PISTOL_BODYHIT);
        KB_ClearKeyDown( sc_Enter );
        KB_ClearKeyDown( sc_Space );
        KB_ClearKeyDown( sc_kpad_Enter );
        return(probey);
    }
    else if( KB_KeyPressed( sc_Escape ) || (RMB) )
    {
        onbar = 0;
        KB_ClearKeyDown( sc_Escape );
        sound(EXITMENUSOUND);
        return(-1);
    }
    else
    {
        if(onbar == 0) return(-probey-2);
        if ( KB_KeyPressed( sc_LeftArrow ) || KB_KeyPressed( sc_kpad_4 ) || ((buttonstat&1) && minfo.dyaw < -128 ) )
            return(probey);
        else if ( KB_KeyPressed( sc_RightArrow ) || KB_KeyPressed( sc_kpad_6 ) || ((buttonstat&1) && minfo.dyaw > 128 ) )
            return(probey);
        else return(-probey-2);
    }
}

int menutext(int x,int y,short s,short p,char *t)
{
    short i, ac, centre;

    y -= 12;

    i = centre = 0;

    if( x == (320>>1) )
    {
        while( *(t+i) )
        {
            if(*(t+i) == ' ')
            {
                centre += 5;
                i++;
                continue;
            }
            ac = 0;
            if(*(t+i) >= '0' && *(t+i) <= '9')
                ac = *(t+i) - '0' + BIGALPHANUM-10;
            else if(*(t+i) >= 'a' && *(t+i) <= 'z')
                ac = toupper(*(t+i)) - 'A' + BIGALPHANUM;
            else if(*(t+i) >= 'A' && *(t+i) <= 'Z')
                ac = *(t+i) - 'A' + BIGALPHANUM;
            else switch(*(t+i))
            {
                case '-':
                    ac = BIGALPHANUM-11;
                    break;
                case '.':
                    ac = BIGPERIOD;
                    break;
                case '\'':
                    ac = BIGAPPOS;
                    break;
                case ',':
                    ac = BIGCOMMA;
                    break;
                case '!':
                    ac = BIGX;
                    break;
                case '?':
                    ac = BIGQ;
                    break;
                case ';':
                    ac = BIGSEMI;
                    break;
                case ':':
                    ac = BIGSEMI;
                    break;
                default:
                    centre += 5;
                    i++;
                    continue;
            }

            centre += tilesizx[ac]-1;
            i++;
        }
    }

    if(centre)
        x = (320-centre-10)>>1;

    while(*t)
    {
        if(*t == ' ') {x+=5;t++;continue;}
        ac = 0;
        if(*t >= '0' && *t <= '9')
            ac = *t - '0' + BIGALPHANUM-10;
        else if(*t >= 'a' && *t <= 'z')
            ac = toupper(*t) - 'A' + BIGALPHANUM;
        else if(*t >= 'A' && *t <= 'Z')
            ac = *t - 'A' + BIGALPHANUM;
        else switch(*t)
        {
            case '-':
                ac = BIGALPHANUM-11;
                break;
            case '.':
                ac = BIGPERIOD;
                break;
            case ',':
                ac = BIGCOMMA;
                break;
            case '!':
                ac = BIGX;
                break;
            case '\'':
                ac = BIGAPPOS;
                break;
            case '?':
                ac = BIGQ;
                break;
            case ';':
                ac = BIGSEMI;
                break;
            case ':':
                ac = BIGCOLIN;
                break;
            default:
                x += 5;
                t++;
                continue;
        }

        rotatesprite(x<<16,y<<16,65536L,0,ac,s,p,10+16,0,0,xdim-1,ydim-1);

        x += tilesizx[ac];
        t++;
    }
    return (x);
}

int menutextc(int x,int y,short s,short p,char *t)
{
    short i, ac, centre;

    s += 8;
    y -= 12;

    i = centre = 0;

//    if( x == (320>>1) )
    {
        while( *(t+i) )
        {
            if(*(t+i) == ' ')
            {
                centre += 5;
                i++;
                continue;
            }
            ac = 0;
            if(*(t+i) >= '0' && *(t+i) <= '9')
                ac = *(t+i) - '0' + BIGALPHANUM+26+26;
            if(*(t+i) >= 'a' && *(t+i) <= 'z')
                ac = *(t+i) - 'a' + BIGALPHANUM+26;
            if(*(t+i) >= 'A' && *(t+i) <= 'Z')
                ac = *(t+i) - 'A' + BIGALPHANUM;

            else switch(*t)
            {
                case '-':
                    ac = BIGALPHANUM-11;
                    break;
                case '.':
                    ac = BIGPERIOD;
                    break;
                case ',':
                    ac = BIGCOMMA;
                    break;
                case '!':
                    ac = BIGX;
                    break;
                case '?':
                    ac = BIGQ;
                    break;
                case ';':
                    ac = BIGSEMI;
                    break;
                case ':':
                    ac = BIGCOLIN;
                    break;
            }

            centre += tilesizx[ac]-1;
            i++;
        }
    }

    x -= centre>>1;

    while(*t)
    {
        if(*t == ' ') {x+=5;t++;continue;}
        ac = 0;
        if(*t >= '0' && *t <= '9')
            ac = *t - '0' + BIGALPHANUM+26+26;
        if(*t >= 'a' && *t <= 'z')
            ac = *t - 'a' + BIGALPHANUM+26;
        if(*t >= 'A' && *t <= 'Z')
            ac = *t - 'A' + BIGALPHANUM;
        switch(*t)
        {
            case '-':
                ac = BIGALPHANUM-11;
                break;
            case '.':
                ac = BIGPERIOD;
                break;
            case ',':
                ac = BIGCOMMA;
                break;
            case '!':
                ac = BIGX;
                break;
            case '?':
                ac = BIGQ;
                break;
            case ';':
                ac = BIGSEMI;
                break;
            case ':':
                ac = BIGCOLIN;
                break;
        }

        rotatesprite(x<<16,y<<16,65536L,0,ac,s,p,10+16,0,0,xdim-1,ydim-1);

        x += tilesizx[ac];
        t++;
    }
    return (x);
}


void bar(int x,int y,short *p,short dainc,char damodify,short s, short pa)
{
    short xloc;
    char rev;

    if(dainc < 0) { dainc = -dainc; rev = 1; }
    else rev = 0;
    y-=2;

    if(damodify)
    {
        if(rev == 0)
        {
            if( KB_KeyPressed( sc_LeftArrow ) || KB_KeyPressed( sc_kpad_4 ) || ((buttonstat&1) && minfo.dyaw < -256 ) ) // && onbar) )
            {
                KB_ClearKeyDown( sc_LeftArrow );
                KB_ClearKeyDown( sc_kpad_4 );

                *p -= dainc;
                if(*p < 0)
                    *p = 0;
                sound(KICK_HIT);
            }
            if( KB_KeyPressed( sc_RightArrow ) || KB_KeyPressed( sc_kpad_6 ) || ((buttonstat&1) && minfo.dyaw > 256 ) )//&& onbar) )
            {
                KB_ClearKeyDown( sc_RightArrow );
                KB_ClearKeyDown( sc_kpad_6 );

                *p += dainc;
                if(*p > 63)
                    *p = 63;
                sound(KICK_HIT);
            }
        }
        else
        {
            if( KB_KeyPressed( sc_RightArrow ) || KB_KeyPressed( sc_kpad_6 ) || ((buttonstat&1) && minfo.dyaw > 256 ))//&& onbar ))
            {
                KB_ClearKeyDown( sc_RightArrow );
                KB_ClearKeyDown( sc_kpad_6 );

                *p -= dainc;
                if(*p < 0)
                    *p = 0;
                sound(KICK_HIT);
            }
            if( KB_KeyPressed( sc_LeftArrow ) || KB_KeyPressed( sc_kpad_4 ) || ((buttonstat&1) && minfo.dyaw < -256 ))// && onbar) )
            {
                KB_ClearKeyDown( sc_LeftArrow );
                KB_ClearKeyDown( sc_kpad_4 );

                *p += dainc;
                if(*p > 64)
                    *p = 64;
                sound(KICK_HIT);
            }
        }
    }

    xloc = *p;

    rotatesprite( (x+22)<<16,(y-3)<<16,65536L,0,SLIDEBAR,s,pa,10,0,0,xdim-1,ydim-1);
    if(rev == 0)
        rotatesprite( (x+xloc+1)<<16,(y+1)<<16,65536L,0,SLIDEBAR+1,s,pa,10,0,0,xdim-1,ydim-1);
    else
        rotatesprite( (x+(65-xloc) )<<16,(y+1)<<16,65536L,0,SLIDEBAR+1,s,pa,10,0,0,xdim-1,ydim-1);
}

#define SHX(X) 0
// ((x==X)*(-sh))
#define PHX(X) 0
// ((x==X)?1:2)
#define MWIN(X) rotatesprite( 320<<15,200<<15,X,0,MENUSCREEN,-16,0,10+64,0,0,xdim-1,ydim-1)
#define MWINXY(X,OX,OY) rotatesprite( ( 320+(OX) )<<15, ( 200+(OY) )<<15,X,0,MENUSCREEN,-16,0,10+64,0,0,xdim-1,ydim-1)


int32 volnum,levnum,plrskl,numplr;
short lastsavedpos = -1;

void dispnames(void)
{
    short x, c = 160;

    c += 64;
    for(x = 0;x <= 108;x += 12)
    rotatesprite((c+91-64)<<16,(x+56)<<16,65536L,0,TEXTBOX,24,0,10,0,0,xdim-1,ydim-1);

    rotatesprite(22<<16,97<<16,65536L,0,WINDOWBORDER2,24,0,10,0,0,xdim-1,ydim-1);
    rotatesprite(180<<16,97<<16,65536L,1024,WINDOWBORDER2,24,0,10,0,0,xdim-1,ydim-1);
    rotatesprite(99<<16,50<<16,65536L,512,WINDOWBORDER1,24,0,10,0,0,xdim-1,ydim-1);
    rotatesprite(103<<16,144<<16,65536L,1024+512,WINDOWBORDER1,24,0,10,0,0,xdim-1,ydim-1);

    minitext(c,48,ud.savegame[0],2,10+16);
    minitext(c,48+12,ud.savegame[1],2,10+16);
    minitext(c,48+12+12,ud.savegame[2],2,10+16);
    minitext(c,48+12+12+12,ud.savegame[3],2,10+16);
    minitext(c,48+12+12+12+12,ud.savegame[4],2,10+16);
    minitext(c,48+12+12+12+12+12,ud.savegame[5],2,10+16);
    minitext(c,48+12+12+12+12+12+12,ud.savegame[6],2,10+16);
    minitext(c,48+12+12+12+12+12+12+12,ud.savegame[7],2,10+16);
    minitext(c,48+12+12+12+12+12+12+12+12,ud.savegame[8],2,10+16);
    minitext(c,48+12+12+12+12+12+12+12+12+12,ud.savegame[9],2,10+16);

}

getfilenames(char kind[6])
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
				strcpy(menuname[menunamecnt],fileinfo.name);
				menuname[menunamecnt][16] = type;
				menunamecnt++;
			}
	}
	while (_dos_findnext(&fileinfo) == 0);

	return(0);
}

void sortfilenames()
{
	char sortbuffer[17];
	long i, j, k;

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

long quittimer = 0;

void menus(void)
{
    short c,x;
    volatile long l;
    int tenerr;

    getpackets();

    if(ControllerType == 1 && CONTROL_MousePresent)
    {
        if(buttonstat != 0 && !onbar)
        {
            x = MOUSE_GetButtons()<<3;
            if( x ) buttonstat = x<<3;
            else buttonstat = 0;
        }
        else
            buttonstat = MOUSE_GetButtons();
    }
    else buttonstat = 0;

    if( (ps[myconnectindex].gm&MODE_MENU) == 0 )
    {
        walock[MAXTILES-3] = 1;
        return;
    }

    ps[myconnectindex].gm &= (0xff-MODE_TYPE);
    ps[myconnectindex].fta = 0;

    x = 0;

    sh = 4-(sintable[(totalclock<<4)&2047]>>11);

    if(!(current_menu >= 1000 && current_menu <= 2999 && current_menu >= 300 && current_menu <= 369))
        vscrn();

    switch(current_menu)
    {
        case 25000:
            gametext(160,90,"SELECT A SAVE SPOT BEFORE",0,2+8+16);
            gametext(160,90+9,"YOU QUICK RESTORE.",0,2+8+16);

            x = probe(186,124,0,0);
            if(x >= -1)
            {
                if(ud.multimode < 2 && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }
                ps[myconnectindex].gm &= ~MODE_MENU;
            }
            break;

        case 20000:
            x = probe(326,190,0,0);
            gametext(160,50-8,"YOU ARE PLAYING THE SHAREWARE",0,2+8+16);
            gametext(160,59-8,"VERSION OF DUKE NUKEM 3D.  WHILE",0,2+8+16);
            gametext(160,68-8,"THIS VERSION IS REALLY COOL, YOU",0,2+8+16);
            gametext(160,77-8,"ARE MISSING OVER 75% OF THE TOTAL",0,2+8+16);
            gametext(160,86-8,"GAME, ALONG WITH OTHER GREAT EXTRAS",0,2+8+16);
            gametext(160,95-8,"AND GAMES, WHICH YOU'LL GET WHEN",0,2+8+16);
            gametext(160,104-8,"YOU ORDER THE COMPLETE VERSION AND",0,2+8+16);
            gametext(160,113-8,"GET THE FINAL TWO EPISODES.",0,2+8+16);

            gametext(160,113+8,"PLEASE READ THE 'HOW TO ORDER' ITEM",0,2+8+16);
            gametext(160,122+8,"ON THE MAIN MENU IF YOU WISH TO",0,2+8+16);
            gametext(160,131+8,"UPGRADE TO THE FULL REGISTERED",0,2+8+16);
            gametext(160,140+8,"VERSION OF DUKE NUKEM 3D.",0,2+8+16);
            gametext(160,149+16,"PRESS ANY KEY...",0,2+8+16);

            if( x >= -1 ) cmenu(100);
            break;
        case 20001:
            x = probe(188,80+32+32,0,0);
            gametext(160,86-8,"You must be in Windows 95 to",0,2+8+16);
            gametext(160,86,"play on TEN",0,2+8+16);
            gametext(160,86+32,"PRESS ANY KEY...",0,2+8+16);
            if(x >= -1) cmenu(0);
            break;
        case 20002:
            x = probe(188,80+32+32+32,0,0);
            gametext(160,86-8,"MISSING FILE: TENGAME.INI.  PLEASE",0,2+8+16);
            gametext(160,86,"CONNECT TO TEN BY LAUNCHING THE",0,2+8+16);
            gametext(160,86+8,"CONNECT TO TEN SHORTCUT OR CONTACT",0,2+8+16);
            gametext(160,86+8+8,"CUSTOMER SUPPORT AT 1-800-8040-TEN.",0,2+8+16);
            gametext(160,86+8+8+32,"PRESS ANY KEY...",0,2+8+16);
            if(x >= -1) cmenu(0);
            break;
        case 20003:
            x = probe(188,80+32+32,0,0);
            gametext(160,86-8,"BAD TEN INSTALL:  PLEASE RE-INSTALL",0,2+8+16);
            gametext(160,86,"BAD TEN INSTALL:  PLEASE RE-INSTALL TEN",0,2+8+16);
            gametext(160,86+32,"PRESS ANY KEY...",0,2+8+16);
            if(x >= -1) cmenu(0);
            break;
        case 20005:
            x = probe(188,80+32+32,0,0);
            gametext(160,86-8,"GET THE LATEST TEN SOFTWARE AT",0,2+8+16);
            gametext(160,86,"HTTP://WWW.TEN.NET",0,2+8+16);
            gametext(160,86+32,"PRESS ANY KEY...",0,2+8+16);
            if(x >= -1) cmenu(0);
            break;

        case 15001:
        case 15000:

            gametext(160,90,"LOAD last game:",0,2+8+16);

            sprintf(tempbuf,"\"%s\"",ud.savegame[lastsavedpos]);
            gametext(160,99,tempbuf,0,2+8+16);

            gametext(160,99+9,"(Y/N)",0,2+8+16);

            if( KB_KeyPressed(sc_Escape) || KB_KeyPressed(sc_N) || RMB)
            {
                if(sprite[ps[myconnectindex].i].extra <= 0)
                {
                    enterlevel(MODE_GAME);
                    return;
                }

                KB_ClearKeyDown(sc_N);
                KB_ClearKeyDown(sc_Escape);

                ps[myconnectindex].gm &= ~MODE_MENU;
                if(ud.multimode < 2 && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }
            }

            if(  KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                KB_FlushKeyboardQueue();
                FX_StopAllSounds();

                if(ud.multimode > 1)
                {
                    loadplayer(-1-lastsavedpos);
                    ps[myconnectindex].gm = MODE_GAME;
                }
                else
                {
                    c = loadplayer(lastsavedpos);
                    if(c == 0)
                        ps[myconnectindex].gm = MODE_GAME;
                }
            }

            probe(186,124+9,0,0);

            break;

        case 10000:
        case 10001:

            c = 60;
            rotatesprite(160<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(160,24,0,0,"ADULT MODE");

            x = probe(60,50+16,16,2);
            if(x == -1) { cmenu(200); break; }

            menutext(c,50+16,SHX(-2),PHX(-2),"ADULT MODE");
            menutext(c,50+16+16,SHX(-3),PHX(-3),"ENTER PASSWORD");

            if(ud.lockout) menutext(c+160+40,50+16,0,0,"OFF");
            else menutext(c+160+40,50+16,0,0,"ON");

            if(current_menu == 10001)
            {
                gametext(160,50+16+16+16+16-12,"ENTER PASSWORD",0,2+8+16);
                x = strget((320>>1),50+16+16+16+16,buf,19, 998);

                if( x )
                {
                    if(ud.pwlockout[0] == 0 || ud.lockout == 0 )
                        strcpy(&ud.pwlockout[0],buf);
                    else if( strcmp(buf,&ud.pwlockout[0]) == 0 )
                    {
                        ud.lockout = 0;
                        buf[0] = 0;

                        for(x=0;x<numanimwalls;x++)
                            if( wall[animwall[x].wallnum].picnum != W_SCREENBREAK &&
                                wall[animwall[x].wallnum].picnum != W_SCREENBREAK+1 &&
                                wall[animwall[x].wallnum].picnum != W_SCREENBREAK+2 )
                                    if( wall[animwall[x].wallnum].extra >= 0 )
                                        wall[animwall[x].wallnum].picnum = wall[animwall[x].wallnum].extra;

                    }
                    current_menu = 10000;
                    KB_ClearKeyDown(sc_Enter);
                    KB_ClearKeyDown(sc_kpad_Enter);
                    KB_FlushKeyboardQueue();
                }
            }
            else
            {
                if(x == 0)
                {
                    if( ud.lockout == 1 )
                    {
                        if(ud.pwlockout[0] == 0)
                        {
                            ud.lockout = 0;
                            for(x=0;x<numanimwalls;x++)
                            if( wall[animwall[x].wallnum].picnum != W_SCREENBREAK &&
                                wall[animwall[x].wallnum].picnum != W_SCREENBREAK+1 &&
                                wall[animwall[x].wallnum].picnum != W_SCREENBREAK+2 )
                                    if( wall[animwall[x].wallnum].extra >= 0 )
                                        wall[animwall[x].wallnum].picnum = wall[animwall[x].wallnum].extra;
                        }
                        else
                        {
                            buf[0] = 0;
                            current_menu = 10001;
                            inputloc = 0;
                            KB_FlushKeyboardQueue();
                        }
                    }
                    else
                    {
                        ud.lockout = 1;

                        for(x=0;x<numanimwalls;x++)
                            switch(wall[animwall[x].wallnum].picnum)
                            {
                                case FEMPIC1:
                                    wall[animwall[x].wallnum].picnum = BLANKSCREEN;
                                    break;
                                case FEMPIC2:
                                case FEMPIC3:
                                    wall[animwall[x].wallnum].picnum = SCREENBREAK6;
                                    break;
                            }
                    }
                }

                else if(x == 1)
                {
                    current_menu = 10001;
                    inputloc = 0;
                    KB_FlushKeyboardQueue();
                }
            }

            break;

        case 1000:
        case 1001:
        case 1002:
        case 1003:
        case 1004:
        case 1005:
        case 1006:
        case 1007:
        case 1008:
        case 1009:

            rotatesprite(160<<16,200<<15,65536L,0,MENUSCREEN,16,0,10+64,0,0,xdim-1,ydim-1);
            rotatesprite(160<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(160,24,0,0,"LOAD GAME");
            rotatesprite(101<<16,97<<16,65536,512,MAXTILES-3,-32,0,4+10+64,0,0,xdim-1,ydim-1);

            dispnames();

            sprintf(tempbuf,"PLAYERS: %-2d                      ",numplr);
            gametext(160,158,tempbuf,0,2+8+16);

            sprintf(tempbuf,"EPISODE: %-2d / LEVEL: %-2d / SKILL: %-2d",1+volnum,1+levnum,plrskl);
            gametext(160,170,tempbuf,0,2+8+16);

            gametext(160,90,"LOAD game:",0,2+8+16);
            sprintf(tempbuf,"\"%s\"",ud.savegame[current_menu-1000]);
            gametext(160,99,tempbuf,0,2+8+16);
            gametext(160,99+9,"(Y/N)",0,2+8+16);

            if( KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                lastsavedpos = current_menu-1000;

                KB_FlushKeyboardQueue();
                if(ud.multimode < 2 && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }

                if(ud.multimode > 1)
                {
                    if( ps[myconnectindex].gm&MODE_GAME )
                    {
                        loadplayer(-1-lastsavedpos);
                        ps[myconnectindex].gm = MODE_GAME;
                    }
                    else
                    {
                        tempbuf[0] = 126;
                        tempbuf[1] = lastsavedpos;
                        for(x=connecthead;x>=0;x=connectpoint2[x])
                            if(x != myconnectindex)
                                sendpacket(x,tempbuf,2);

                        getpackets();

                        loadplayer(lastsavedpos);

                        multiflag = 0;
                    }
                }
                else
                {
                    c = loadplayer(lastsavedpos);
                    if(c == 0)
                        ps[myconnectindex].gm = MODE_GAME;
                }

                break;
            }
            if( KB_KeyPressed(sc_N) || KB_KeyPressed(sc_Escape) || RMB)
            {
                KB_ClearKeyDown(sc_N);
                KB_ClearKeyDown(sc_Escape);
                sound(EXITMENUSOUND);
                if(ps[myconnectindex].gm&MODE_DEMO) cmenu(300);
                else
                {
                    ps[myconnectindex].gm &= ~MODE_MENU;
                    if(ud.multimode < 2 && ud.recstat != 2)
                    {
                        ready2send = 1;
                        totalclock = ototalclock;
                    }
                }
            }

            probe(186,124+9,0,0);

            break;

        case 1500:

            if( KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                KB_FlushKeyboardQueue();
                cmenu(100);
            }
            if( KB_KeyPressed(sc_N) || KB_KeyPressed(sc_Escape) || RMB)
            {
                KB_ClearKeyDown(sc_N);
                KB_ClearKeyDown(sc_Escape);
                if(ud.multimode < 2 && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }
                ps[myconnectindex].gm &= ~MODE_MENU;
                sound(EXITMENUSOUND);
                break;
            }
            probe(186,124,0,0);
            gametext(160,90,"ABORT this game?",0,2+8+16);
            gametext(160,90+9,"(Y/N)",0,2+8+16);

            break;

        case 2000:
        case 2001:
        case 2002:
        case 2003:
        case 2004:
        case 2005:
        case 2006:
        case 2007:
        case 2008:
        case 2009:

            rotatesprite(160<<16,200<<15,65536L,0,MENUSCREEN,16,0,10+64,0,0,xdim-1,ydim-1);
            rotatesprite(160<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(160,24,0,0,"SAVE GAME");

            rotatesprite(101<<16,97<<16,65536L,512,MAXTILES-3,-32,0,4+10+64,0,0,xdim-1,ydim-1);
            sprintf(tempbuf,"PLAYERS: %-2d                      ",ud.multimode);
            gametext(160,158,tempbuf,0,2+8+16);

            sprintf(tempbuf,"EPISODE: %-2d / LEVEL: %-2d / SKILL: %-2d",1+ud.volume_number,1+ud.level_number,ud.player_skill);
            gametext(160,170,tempbuf,0,2+8+16);

            dispnames();

            gametext(160,90,"OVERWRITE previous SAVED game?",0,2+8+16);
            gametext(160,90+9,"(Y/N)",0,2+8+16);

            if( KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                KB_FlushKeyboardQueue();
                inputloc = strlen(&ud.savegame[current_menu-2000][0]);

                cmenu(current_menu-2000+360);

                KB_FlushKeyboardQueue();
                break;
            }
            if( KB_KeyPressed(sc_N) || KB_KeyPressed(sc_Escape) || RMB)
            {
                KB_ClearKeyDown(sc_N);
                KB_ClearKeyDown(sc_Escape);
                cmenu(351);
                sound(EXITMENUSOUND);
            }

            probe(186,124,0,0);

            break;

        case 990:
        case 991:
        case 992:
        case 993:
        case 994:
        case 995:
        case 996:
        case 997:

//            rotatesprite(c<<16,200<<15,65536L,0,MENUSCREEN,16,0,10+64,0,0,xdim-1,ydim-1);
//            rotatesprite(c<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
//            menutext(c,24,0,0,"CREDITS");

            if(KB_KeyPressed(sc_Escape)) { cmenu(0); break; }

            if( KB_KeyPressed( sc_LeftArrow ) ||
                KB_KeyPressed( sc_kpad_4 ) ||
                KB_KeyPressed( sc_UpArrow ) ||
                KB_KeyPressed( sc_PgUp ) ||
                KB_KeyPressed( sc_kpad_8 ) )
            {
                KB_ClearKeyDown(sc_LeftArrow);
                KB_ClearKeyDown(sc_kpad_4);
                KB_ClearKeyDown(sc_UpArrow);
                KB_ClearKeyDown(sc_PgUp);
                KB_ClearKeyDown(sc_kpad_8);

                sound(KICK_HIT);
                current_menu--;
                if(current_menu < 990) current_menu = 992;
            }
            else if(
                KB_KeyPressed( sc_PgDn ) ||
                KB_KeyPressed( sc_Enter ) ||
                KB_KeyPressed( sc_Space ) ||
                KB_KeyPressed( sc_kpad_Enter ) ||
                KB_KeyPressed( sc_RightArrow ) ||
                KB_KeyPressed( sc_DownArrow ) ||
                KB_KeyPressed( sc_kpad_2 ) ||
                KB_KeyPressed( sc_kpad_9 ) ||
                KB_KeyPressed( sc_kpad_6 ) )
            {
                KB_ClearKeyDown(sc_PgDn);
                KB_ClearKeyDown(sc_Enter);
                KB_ClearKeyDown(sc_RightArrow);
                KB_ClearKeyDown(sc_kpad_Enter);
                KB_ClearKeyDown(sc_kpad_6);
                KB_ClearKeyDown(sc_kpad_9);
                KB_ClearKeyDown(sc_kpad_2);
                KB_ClearKeyDown(sc_DownArrow);
                KB_ClearKeyDown(sc_Space);
                sound(KICK_HIT);
                current_menu++;
                if(current_menu > 992) current_menu = 990;
            }

            switch(current_menu)
            {
                case 990:
                case 991:
                case 992:
                   rotatesprite(160<<16,200<<15,65536L,0,2504+current_menu-990,0,0,10+64,0,0,xdim-1,ydim-1);

                   break;

            }
            break;

        case 0:
            c = (320>>1);
            rotatesprite(c<<16,28<<16,65536L,0,INGAMEDUKETHREEDEE,0,0,10,0,0,xdim-1,ydim-1);
#ifndef UK
            rotatesprite((c+100)<<16,36<<16,65536L,0,PLUTOPAKSPRITE+2,(sintable[(totalclock<<4)&2047]>>11),0,2+8,0,0,xdim-1,ydim-1);
#endif
            x = probe(c,67,16,7);
            if(x >= 0)
            {
                if( ud.multimode > 1 && x == 0 && ud.recstat != 2)
                {
                    if( movesperpacket == 4 && myconnectindex != connecthead )
                        break;

                    last_zero = 0;
                    cmenu( 600 );
                }
                else
                {
                    last_zero = x;
                    switch(x)
                    {
                        case 0:
                            cmenu(100);
                            break;
                        case 1:
                            if(movesperpacket == 4 || numplayers > 1)
                                break;

                            tenBnSetExitRtn(dummyfunc);
                            setDebugMsgRoutine(dummymess);
                            tenerr = tenBnStart();

                            switch(tenerr)
                            {
                                case eTenBnNotInWindows:
                                    cmenu(20001);
                                    break;
                                case eTenBnBadGameIni:
                                    cmenu(20002);
                                    break;
                                case eTenBnBadTenIni:
                                    cmenu(20003);
                                    break;
                                case eTenBnBrowseCancel:
                                    cmenu(20004);
                                    break;
                                case eTenBnBadTenInst:
                                    cmenu(20005);
                                    break;
                                default:
                                    playonten = 1;
                                    gameexit(" ");
                                    break;
                            }
                            break;
                        case 2: cmenu(200);break;
                        case 3:
                            if(movesperpacket == 4 && connecthead != myconnectindex)
                                break;
                            cmenu(300);
                            break;
                        case 4: KB_FlushKeyboardQueue();cmenu(400);break;
                        case 5: cmenu(990);break;
                        case 6: cmenu(500);break;
                    }
                }
            }

            if(KB_KeyPressed(sc_Q)) cmenu(500);

            if(x == -1)
            {
                ps[myconnectindex].gm &= ~MODE_MENU;
                if(ud.multimode < 2 && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }
            }

            if(movesperpacket == 4)
            {
                if( myconnectindex == connecthead )
                    menutext(c,67,SHX(-2),PHX(-2),"NEW GAME");
                else
                    menutext(c,67,SHX(-2),1,"NEW GAME");
            }
            else
                menutext(c,67,SHX(-2),PHX(-2),"NEW GAME");

            if(movesperpacket != 4 && numplayers < 2)
                menutext(c,67+16,SHX(-3),PHX(-3),"PLAY ON TEN");
            else
                menutext(c,67+16,SHX(-3),1,"PLAY ON TEN");

            menutext(c,67+16+16,SHX(-4),PHX(-4),"OPTIONS");

            if(movesperpacket == 4 && connecthead != myconnectindex)
                menutext(c,67+16+16+16,SHX(-5),1,"LOAD GAME");
            else menutext(c,67+16+16+16,SHX(-5),PHX(-5),"LOAD GAME");

#ifndef VOLUMEALL
            menutext(c,67+16+16+16+16,SHX(-6),PHX(-6),"HOW TO ORDER");
#else
            menutext(c,67+16+16+16+16,SHX(-6),PHX(-6),"HELP");
#endif
            menutext(c,67+16+16+16+16+16,SHX(-7),PHX(-7),"CREDITS");

            menutext(c,67+16+16+16+16+16+16,SHX(-8),PHX(-8),"QUIT");

            break;

        case 50:
            c = (320>>1);
            rotatesprite(c<<16,32<<16,65536L,0,INGAMEDUKETHREEDEE,0,0,10,0,0,xdim-1,ydim-1);
#ifndef UK
            rotatesprite((c+100)<<16,36<<16,65536L,0,PLUTOPAKSPRITE+2,(sintable[(totalclock<<4)&2047]>>11),0,2+8,0,0,xdim-1,ydim-1);
#endif
            x = probe(c,67,16,7);
            switch(x)
            {
                case 0:
                    if(movesperpacket == 4 && myconnectindex != connecthead)
                        break;
                    if(ud.multimode < 2 || ud.recstat == 2)
                        cmenu(1500);
                    else
                    {
                        cmenu(600);
                        last_fifty = 0;
                    }
                    break;
                case 1:
                    if(movesperpacket == 4 && connecthead != myconnectindex)
                        break;
                    if(ud.recstat != 2)
                    {
                        last_fifty = 1;
                        cmenu(350);
                        setview(0,0,xdim-1,ydim-1);
                    }
                    break;
                case 2:
                    if(movesperpacket == 4 && connecthead != myconnectindex)
                        break;
                    last_fifty = 2;
                    cmenu(300);
                    break;
                case 3:
                    last_fifty = 3;
                    cmenu(200);
                    break;
                case 4:
                    last_fifty = 4;
                    KB_FlushKeyboardQueue();
                    cmenu(400);
                    break;
                case 5:
                    if(numplayers < 2)
                    {
                        last_fifty = 5;
                        cmenu(501);
                    }
                    break;
                case 6:
                    last_fifty = 6;
                    cmenu(500);
                    break;
                case -1:
                    ps[myconnectindex].gm &= ~MODE_MENU;
                    if(ud.multimode < 2 && ud.recstat != 2)
                    {
                        ready2send = 1;
                        totalclock = ototalclock;
                    }
                    break;
            }

            if( KB_KeyPressed(sc_Q) )
                cmenu(500);

            if(movesperpacket == 4 && connecthead != myconnectindex)
            {
                menutext(c,67                  ,SHX(-2),1,"NEW GAME");
                menutext(c,67+16               ,SHX(-3),1,"SAVE GAME");
                menutext(c,67+16+16            ,SHX(-4),1,"LOAD GAME");
            }
            else
            {
                menutext(c,67                  ,SHX(-2),PHX(-2),"NEW GAME");
                menutext(c,67+16               ,SHX(-3),PHX(-3),"SAVE GAME");
                menutext(c,67+16+16            ,SHX(-4),PHX(-4),"LOAD GAME");
            }

            menutext(c,67+16+16+16         ,SHX(-5),PHX(-5),"OPTIONS");
#ifndef VOLUMEALL
            menutext(c,67+16+16+16+16      ,SHX(-6),PHX(-6),"HOW TO ORDER");
#else
            menutext(c,67+16+16+16+16      ,SHX(-6),PHX(-6)," HELP");
#endif
            if(numplayers > 1)
                menutext(c,67+16+16+16+16+16   ,SHX(-7),1,"QUIT TO TITLE");
            else menutext(c,67+16+16+16+16+16   ,SHX(-7),PHX(-7),"QUIT TO TITLE");
            menutext(c,67+16+16+16+16+16+16,SHX(-8),PHX(-8),"QUIT GAME");
            break;

        case 100:
            rotatesprite(160<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(160,24,0,0,"SELECT AN EPISODE");
#ifdef PLUTOPAK
            if(boardfilename[0])
                x = probe(160,60,20,5);
            else x = probe(160,60,20,4);
#else
            if(boardfilename[0])
                x = probe(160,60,20,4);
            else x = probe(160,60,20,3);
#endif
            if(x >= 0)
            {
#ifdef VOLUMEONE
                if(x > 0)
                    cmenu(20000);
                else
                {
                    ud.m_volume_number = x;
                    ud.m_level_number = 0;
                    cmenu(110);
                }
#endif

#ifndef VOLUMEONE
#ifndef PLUTOPAK

                if(x == 3 && boardfilename[0])
                {
                    ud.m_volume_number = 0;
                    ud.m_level_number = 7;
                }
#else
                if(x == 4 && boardfilename[0])
                {
                    ud.m_volume_number = 0;
                    ud.m_level_number = 7;
                }
#endif

                else
                {
                    ud.m_volume_number = x;
                    ud.m_level_number = 0;
                }
                cmenu(110);
#endif
            }
            else if(x == -1)
            {
                if(ps[myconnectindex].gm&MODE_GAME) cmenu(50);
                else cmenu(0);
            }

            menutext(160,60,SHX(-2),PHX(-2),volume_names[0]);

            c = 80;
#ifdef VOLUMEONE
            menutext(160,60+20,SHX(-3),1,volume_names[1]);
            menutext(160,60+20+20,SHX(-4),1,volume_names[2]);
#ifdef PLUTOPAK
            menutext(160,60+20+20,SHX(-5),1,volume_names[3]);
#endif
#else
            menutext(160,60+20,SHX(-3),PHX(-3),volume_names[1]);
            menutext(160,60+20+20,SHX(-4),PHX(-4),volume_names[2]);
#ifdef PLUTOPAK
            menutext(160,60+20+20+20,SHX(-5),PHX(-5),volume_names[3]);
            if(boardfilename[0])
            {

                menutext(160,60+20+20+20+20,SHX(-6),PHX(-6),"USER MAP");
                gametextpal(160,60+20+20+20+20+3,boardfilename,16+(sintable[(totalclock<<4)&2047]>>11),2);
            }
#else
            if(boardfilename[0])
            {
                menutext(160,60+20+20+20,SHX(-6),PHX(-6),"USER MAP");
                gametext(160,60+20+20+20+6,boardfilename,2,2+8+16);
            }
#endif

#endif
            break;

        case 110:
            c = (320>>1);
            rotatesprite(c<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(c,24,0,0,"SELECT SKILL");
            x = probe(c,70,19,4);
            if(x >= 0)
            {
                switch(x)
                {
                    case 0: globalskillsound = JIBBED_ACTOR6;break;
                    case 1: globalskillsound = BONUS_SPEECH1;break;
                    case 2: globalskillsound = DUKE_GETWEAPON2;break;
                    case 3: globalskillsound = JIBBED_ACTOR5;break;
                }

                sound(globalskillsound);

                ud.m_player_skill = x+1;
                if(x == 3) ud.m_respawn_monsters = 1;
                else ud.m_respawn_monsters = 0;

                ud.m_monsters_off = ud.monsters_off = 0;

                ud.m_respawn_items = 0;
                ud.m_respawn_inventory = 0;

                ud.multimode = 1;

                if(ud.m_volume_number == 3)
                {
                    flushperms();
                    setview(0,0,xdim-1,ydim-1);
                    clearview(0L);
                    nextpage();
                }

                newgame(ud.m_volume_number,ud.m_level_number,ud.m_player_skill);
                enterlevel(MODE_GAME);
            }
            else if(x == -1)
            {
                cmenu(100);
                KB_FlushKeyboardQueue();
            }

            menutext(c,70,SHX(-2),PHX(-2),skill_names[0]);
            menutext(c,70+19,SHX(-3),PHX(-3),skill_names[1]);
            menutext(c,70+19+19,SHX(-4),PHX(-4),skill_names[2]);
            menutext(c,70+19+19+19,SHX(-5),PHX(-5),skill_names[3]);
            break;

        case 200:

            rotatesprite(320<<15,10<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(320>>1,15,0,0,"OPTIONS");

            c = (320>>1)-120;

            onbar = (probey == 3 || probey == 4 || probey == 5);
            x = probe(c+6,31,15,10);

            if(x == -1)
                { if(ps[myconnectindex].gm&MODE_GAME) cmenu(50);else cmenu(0); }

            if(onbar == 0) switch(x)
            {
                case 0:
                    ud.detail = 1-ud.detail;
                    break;
                case 1:
                    ud.shadows = 1-ud.shadows;
                    break;
                case 2:
                    ud.screen_tilting = 1-ud.screen_tilting;
                    break;
                case 6:
                    if ( ControllerType == controltype_keyboardandmouse )
                        ud.mouseflip = 1-ud.mouseflip;
                    break;
                case 7:
                    cmenu(700);
                    break;
                case 8:
#ifndef AUSTRALIA
                    cmenu(10000);
#endif
                    break;
                case 9:
                    if( (ps[myconnectindex].gm&MODE_GAME) )
                    {
                        closedemowrite();
                        break;
                    }
                    ud.m_recstat = !ud.m_recstat;
                    break;
            }

            if(ud.detail) menutext(c+160+40,31,0,0,"HIGH");
            else menutext(c+160+40,31,0,0,"LOW");

            if(ud.shadows) menutext(c+160+40,31+15,0,0,"ON");
            else menutext(c+160+40,31+15,0,0,"OFF");

            switch(ud.screen_tilting)
            {
                case 0: menutext(c+160+40,31+15+15,0,0,"OFF");break;
                case 1: menutext(c+160+40,31+15+15,0,0,"ON");break;
                case 2: menutext(c+160+40,31+15+15,0,0,"FULL");break;
            }

            menutext(c,31,SHX(-2),PHX(-2),"DETAIL");
            menutext(c,31+15,SHX(-3),PHX(-3),"SHADOWS");
            menutext(c,31+15+15,SHX(-4),PHX(-4),"SCREEN TILTING");
            menutext(c,31+15+15+15,SHX(-5),PHX(-5),"SCREEN SIZE");

                bar(c+167+40,31+15+15+15,(short *)&ud.screen_size,-4,x==3,SHX(-5),PHX(-5));

            menutext(c,31+15+15+15+15,SHX(-6),PHX(-6),"BRIGHTNESS");
                bar(c+167+40,31+15+15+15+15,(short *)&ud.brightness,8,x==4,SHX(-6),PHX(-6));
                if(x==4) setbrightness(ud.brightness>>2,&ps[myconnectindex].palette[0]);

            if ( ControllerType == controltype_keyboardandmouse )
            {
                short sense;
                sense = CONTROL_GetMouseSensitivity()>>10;

                menutext(c,31+15+15+15+15+15,SHX(-7),PHX(-7),"MOUSE SENSITIVITY");
                bar(c+167+40,31+15+15+15+15+15,&sense,4,x==5,SHX(-7),PHX(-7));
                CONTROL_SetMouseSensitivity( sense<<10 );
                menutext(c,31+15+15+15+15+15+15,SHX(-7),PHX(-7),"MOUSE AIMING FLIP");

                if(ud.mouseflip) menutext(c+160+40,31+15+15+15+15+15+15,SHX(-7),PHX(-7),"ON");
                else menutext(c+160+40,31+15+15+15+15+15+15,SHX(-7),PHX(-7),"OFF");

            }
            else
            {
                short sense;
                sense = CONTROL_GetMouseSensitivity()>>10;

                menutext(c,31+15+15+15+15+15,SHX(-7),1,"MOUSE SENSITIVITY");
                bar(c+167+40,31+15+15+15+15+15,&sense,4,x==99,SHX(-7),1);
                menutext(c,31+15+15+15+15+15+15,SHX(-7),1,"MOUSE AIMING FLIP");

                if(ud.mouseflip) menutext(c+160+40,31+15+15+15+15+15+15,SHX(-7),1,"ON");
                else menutext(c+160+40,31+15+15+15+15+15+15,SHX(-7),1,"OFF");
            }

            menutext(c,31+15+15+15+15+15+15+15,SHX(-8),PHX(-8),"SOUNDS");
#ifndef AUSTRALIA
            menutext(c,31+15+15+15+15+15+15+15+15,SHX(-9),PHX(-9),"PARENTAL LOCK");
#else
            menutext(c,31+15+15+15+15+15+15+15+15,SHX(-9),1,"PARENTAL LOCK");
#endif
            if( (ps[myconnectindex].gm&MODE_GAME) && ud.m_recstat != 1 )
            {
                menutext(c,31+15+15+15+15+15+15+15+15+15,SHX(-10),1,"RECORD");
                menutext(c+160+40,31+15+15+15+15+15+15+15+15+15,SHX(-10),1,"OFF");
            }
            else
            {
                menutext(c,31+15+15+15+15+15+15+15+15+15,SHX(-10),PHX(-10),"RECORD");

                if(ud.m_recstat == 1)
                    menutext(c+160+40,31+15+15+15+15+15+15+15+15+15,SHX(-10),PHX(-10),"ON");
                else menutext(c+160+40,31+15+15+15+15+15+15+15+15+15,SHX(-10),PHX(-10),"OFF");
            }

            break;

        case 700:
            c = (320>>1)-120;
            rotatesprite(320<<15,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(320>>1,24,0,0,"SOUNDS");
            onbar = ( probey == 2 || probey == 3 );

            x = probe(c,50,16,7);
            switch(x)
            {
                case -1:
                    if(ps[myconnectindex].gm&MODE_GAME)
                    {
                        ps[myconnectindex].gm &= ~MODE_MENU;
                        if(ud.multimode < 2  && ud.recstat != 2)
                        {
                            ready2send = 1;
                            totalclock = ototalclock;
                        }
                    }

                    else cmenu(200);
                    break;
                case 0:
                    if (FXDevice != NumSoundCards)
                    {
                        SoundToggle = 1-SoundToggle;
                        if( SoundToggle == 0 )
                        {
                            FX_StopAllSounds();
                            clearsoundlocks();
                        }
                        onbar = 0;
                    }
                    break;
                case 1:

                    if(eightytwofifty == 0 || numplayers < 2)
                        if(MusicDevice != NumSoundCards)
                    {
                        MusicToggle = 1-MusicToggle;
                        if( MusicToggle == 0 ) MUSIC_Pause();
                        else
                        {
                            if(ud.recstat != 2 && ps[myconnectindex].gm&MODE_GAME)
                                playmusic(&music_fn[0][music_select][0]);
                            else playmusic(&env_music_fn[0][0]);

                            MUSIC_Continue();
                        }
                    }
                    onbar = 0;

                    break;
                case 4:
                    if(SoundToggle && (FXDevice != NumSoundCards)) VoiceToggle = 1-VoiceToggle;
                    onbar = 0;
                    break;
                case 5:
                    if(SoundToggle && (FXDevice != NumSoundCards)) AmbienceToggle = 1-AmbienceToggle;
                    onbar = 0;
                    break;
                case 6:
                    if(SoundToggle && (FXDevice != NumSoundCards))
                    {
                        ReverseStereo = 1-ReverseStereo;
                        FX_SetReverseStereo(ReverseStereo);
                    }
                    onbar = 0;
                    break;
                default:
                    onbar = 1;
                    break;
            }

            if(SoundToggle && FXDevice != NumSoundCards) menutext(c+160+40,50,0,(FXDevice == NumSoundCards),"ON");
            else menutext(c+160+40,50,0,(FXDevice == NumSoundCards),"OFF");

            if(MusicToggle && (MusicDevice != NumSoundCards) && (!eightytwofifty||numplayers<2))
                menutext(c+160+40,50+16,0,(MusicDevice == NumSoundCards),"ON");
            else menutext(c+160+40,50+16,0,(MusicDevice == NumSoundCards),"OFF");

            menutext(c,50,SHX(-2),(FXDevice == NumSoundCards),"SOUND");
            menutext(c,50+16+16,SHX(-4),(FXDevice==NumSoundCards)||SoundToggle==0,"SOUND VOLUME");
            {
                l = FXVolume;
                FXVolume >>= 2;
                bar(c+167+40,50+16+16,(short *)&FXVolume,4,(FXDevice!=NumSoundCards)&&x==2,SHX(-4),SoundToggle==0||(FXDevice==NumSoundCards));
                if(l != FXVolume)
                    FXVolume <<= 2;
                if(l != FXVolume)
                    FX_SetVolume( (short) FXVolume );
            }
            menutext(c,50+16,SHX(-3),(MusicDevice==NumSoundCards),"MUSIC");
            menutext(c,50+16+16+16,SHX(-5),(MusicDevice==NumSoundCards)||(numplayers > 1 && eightytwofifty)||MusicToggle==0,"MUSIC VOLUME");
            {
                l = MusicVolume;
                MusicVolume >>= 2;
                bar(c+167+40,50+16+16+16,
                    (short *)&MusicVolume,4,
                    (eightytwofifty==0||numplayers < 2) && (MusicDevice!=NumSoundCards) && x==3,SHX(-5),
                    (numplayers > 1 && eightytwofifty)||MusicToggle==0||(MusicDevice==NumSoundCards));
                MusicVolume <<= 2;
                if(l != MusicVolume)
                    Music_SetVolume( (short) MusicVolume );

            }
            menutext(c,50+16+16+16+16,SHX(-6),(FXDevice==NumSoundCards)||SoundToggle==0,"DUKE TALK");
            menutext(c,50+16+16+16+16+16,SHX(-7),(FXDevice==NumSoundCards)||SoundToggle==0,"AMBIENCE");

            menutext(c,50+16+16+16+16+16+16,SHX(-8),(FXDevice==NumSoundCards)||SoundToggle==0,"FLIP STEREO");

            if(VoiceToggle) menutext(c+160+40,50+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"ON");
            else menutext(c+160+40,50+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"OFF");

            if(AmbienceToggle) menutext(c+160+40,50+16+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"ON");
            else menutext(c+160+40,50+16+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"OFF");

            if(ReverseStereo) menutext(c+160+40,50+16+16+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"ON");
            else menutext(c+160+40,50+16+16+16+16+16+16,0,(FXDevice==NumSoundCards)||SoundToggle==0,"OFF");


            break;

        case 350:
            cmenu(351);
            screencapt = 1;
            displayrooms(myconnectindex,65536);
            savetemp("duke3d.tmp",waloff[MAXTILES-1],160*100);
            screencapt = 0;
            break;

        case 360:
        case 361:
        case 362:
        case 363:
        case 364:
        case 365:
        case 366:
        case 367:
        case 368:
        case 369:
        case 351:
        case 300:

            c = 320>>1;
            rotatesprite(c<<16,200<<15,65536L,0,MENUSCREEN,16,0,10+64,0,0,xdim-1,ydim-1);
            rotatesprite(c<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);

            if(current_menu == 300) menutext(c,24,0,0,"LOAD GAME");
            else menutext(c,24,0,0,"SAVE GAME");

            if(current_menu >= 360 && current_menu <= 369 )
            {
                sprintf(tempbuf,"PLAYERS: %-2d                      ",ud.multimode);
                gametext(160,158,tempbuf,0,2+8+16);
                sprintf(tempbuf,"EPISODE: %-2d / LEVEL: %-2d / SKILL: %-2d",1+ud.volume_number,1+ud.level_number,ud.player_skill);
                gametext(160,170,tempbuf,0,2+8+16);

                x = strget((320>>1),184,&ud.savegame[current_menu-360][0],19, 999 );

                if(x == -1)
                {
            //        readsavenames();
                    ps[myconnectindex].gm = MODE_GAME;
                    if(ud.multimode < 2  && ud.recstat != 2)
                    {
                        ready2send = 1;
                        totalclock = ototalclock;
                    }
                    goto DISPLAYNAMES;
                }

                if( x == 1 )
                {
                    if( ud.savegame[current_menu-360][0] == 0 )
                    {
                        KB_FlushKeyboardQueue();
                        cmenu(351);
                    }
                    else
                    {
                        if(ud.multimode > 1)
                            saveplayer(-1-(current_menu-360));
                        else saveplayer(current_menu-360);
                        lastsavedpos = current_menu-360;
                        ps[myconnectindex].gm = MODE_GAME;

                        if(ud.multimode < 2  && ud.recstat != 2)
                        {
                            ready2send = 1;
                            totalclock = ototalclock;
                        }
                        KB_ClearKeyDown(sc_Escape);
                        sound(EXITMENUSOUND);
                    }
                }

                rotatesprite(101<<16,97<<16,65536,512,MAXTILES-1,-32,0,2+4+8+64,0,0,xdim-1,ydim-1);
                dispnames();
                rotatesprite((c+67+strlen(&ud.savegame[current_menu-360][0])*4)<<16,(50+12*probey)<<16,32768L-10240,0,SPINNINGNUKEICON+(((totalclock)>>3)%7),0,0,10,0,0,xdim-1,ydim-1);
                break;
            }

           last_threehundred = probey;

            x = probe(c+68,54,12,10);

          if(current_menu == 300)
          {
              if( ud.savegame[probey][0] )
              {
                  if( lastprobey != probey )
                  {
                     loadpheader(probey,&volnum,&levnum,&plrskl,&numplr);
                     lastprobey = probey;
                  }

                  rotatesprite(101<<16,97<<16,65536L,512,MAXTILES-3,-32,0,4+10+64,0,0,xdim-1,ydim-1);
                  sprintf(tempbuf,"PLAYERS: %-2d                      ",numplr);
                  gametext(160,158,tempbuf,0,2+8+16);
                  sprintf(tempbuf,"EPISODE: %-2d / LEVEL: %-2d / SKILL: %-2d",1+volnum,1+levnum,plrskl);
                  gametext(160,170,tempbuf,0,2+8+16);
              }
              else menutext(69,70,0,0,"EMPTY");
          }
          else
          {
              if( ud.savegame[probey][0] )
              {
                  if(lastprobey != probey)
                      loadpheader(probey,&volnum,&levnum,&plrskl,&numplr);
                  lastprobey = probey;
                  rotatesprite(101<<16,97<<16,65536L,512,MAXTILES-3,-32,0,4+10+64,0,0,xdim-1,ydim-1);
              }
              else menutext(69,70,0,0,"EMPTY");
              sprintf(tempbuf,"PLAYERS: %-2d                      ",ud.multimode);
              gametext(160,158,tempbuf,0,2+8+16);
              sprintf(tempbuf,"EPISODE: %-2d / LEVEL: %-2d / SKILL: %-2d",1+ud.volume_number,1+ud.level_number,ud.player_skill);
              gametext(160,170,tempbuf,0,2+8+16);
          }

            switch( x )
            {
                case -1:
                    if(current_menu == 300)
                    {
                        if( (ps[myconnectindex].gm&MODE_GAME) != MODE_GAME)
                        {
                            cmenu(0);
                            break;
                        }
                        else
                            ps[myconnectindex].gm &= ~MODE_MENU;
                    }
                    else
                        ps[myconnectindex].gm = MODE_GAME;

                    if(ud.multimode < 2 && ud.recstat != 2)
                    {
                        ready2send = 1;
                        totalclock = ototalclock;
                    }

                    break;
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    if( current_menu == 300)
                    {
                        if( ud.savegame[x][0] )
                            current_menu = (1000+x);
                    }
                    else
                    {
                        if( ud.savegame[x][0] != 0)
                            current_menu = 2000+x;
                        else
                        {
                            KB_FlushKeyboardQueue();
                            current_menu = (360+x);
                            ud.savegame[x][0] = 0;
                            inputloc = 0;
                        }
                    }
                    break;
            }

            DISPLAYNAMES:
            dispnames();
            break;

#ifndef VOLUMEALL
        case 400:
        case 401:
        case 402:
        case 403:

            c = 320>>1;

            if( KB_KeyPressed( sc_LeftArrow ) ||
                KB_KeyPressed( sc_kpad_4 ) ||
                KB_KeyPressed( sc_UpArrow ) ||
                KB_KeyPressed( sc_PgUp ) ||
                KB_KeyPressed( sc_kpad_8 ) )
            {
                KB_ClearKeyDown(sc_LeftArrow);
                KB_ClearKeyDown(sc_kpad_4);
                KB_ClearKeyDown(sc_UpArrow);
                KB_ClearKeyDown(sc_PgUp);
                KB_ClearKeyDown(sc_kpad_8);

                sound(KICK_HIT);
                current_menu--;
                if(current_menu < 400) current_menu = 403;
            }
            else if(
                KB_KeyPressed( sc_PgDn ) ||
                KB_KeyPressed( sc_Enter ) ||
                KB_KeyPressed( sc_kpad_Enter ) ||
                KB_KeyPressed( sc_RightArrow ) ||
                KB_KeyPressed( sc_DownArrow ) ||
                KB_KeyPressed( sc_kpad_2 ) ||
                KB_KeyPressed( sc_kpad_9 ) ||
                KB_KeyPressed( sc_Space ) ||
                KB_KeyPressed( sc_kpad_6 ) )
            {
                KB_ClearKeyDown(sc_PgDn);
                KB_ClearKeyDown(sc_Enter);
                KB_ClearKeyDown(sc_RightArrow);
                KB_ClearKeyDown(sc_kpad_Enter);
                KB_ClearKeyDown(sc_kpad_6);
                KB_ClearKeyDown(sc_kpad_9);
                KB_ClearKeyDown(sc_kpad_2);
                KB_ClearKeyDown(sc_DownArrow);
                KB_ClearKeyDown(sc_Space);
                sound(KICK_HIT);
                current_menu++;
                if(current_menu > 403) current_menu = 400;
            }

            if( KB_KeyPressed(sc_Escape) )
            {
                if(ps[myconnectindex].gm&MODE_GAME)
                    cmenu(50);
                else cmenu(0);
                return;
            }

            flushperms();
            rotatesprite(0,0,65536L,0,ORDERING+current_menu-400,0,0,10+16+64,0,0,xdim-1,ydim-1);

#else
        case 400:
        case 401:

            c = 320>>1;

            if( KB_KeyPressed( sc_LeftArrow ) ||
                KB_KeyPressed( sc_kpad_4 ) ||
                KB_KeyPressed( sc_UpArrow ) ||
                KB_KeyPressed( sc_PgUp ) ||
                KB_KeyPressed( sc_kpad_8 ) )
            {
                KB_ClearKeyDown(sc_LeftArrow);
                KB_ClearKeyDown(sc_kpad_4);
                KB_ClearKeyDown(sc_UpArrow);
                KB_ClearKeyDown(sc_PgUp);
                KB_ClearKeyDown(sc_kpad_8);

                sound(KICK_HIT);
                current_menu--;
                if(current_menu < 400) current_menu = 401;
            }
            else if(
                KB_KeyPressed( sc_PgDn ) ||
                KB_KeyPressed( sc_Enter ) ||
                KB_KeyPressed( sc_kpad_Enter ) ||
                KB_KeyPressed( sc_RightArrow ) ||
                KB_KeyPressed( sc_DownArrow ) ||
                KB_KeyPressed( sc_kpad_2 ) ||
                KB_KeyPressed( sc_kpad_9 ) ||
                KB_KeyPressed( sc_Space ) ||
                KB_KeyPressed( sc_kpad_6 ) )
            {
                KB_ClearKeyDown(sc_PgDn);
                KB_ClearKeyDown(sc_Enter);
                KB_ClearKeyDown(sc_RightArrow);
                KB_ClearKeyDown(sc_kpad_Enter);
                KB_ClearKeyDown(sc_kpad_6);
                KB_ClearKeyDown(sc_kpad_9);
                KB_ClearKeyDown(sc_kpad_2);
                KB_ClearKeyDown(sc_DownArrow);
                KB_ClearKeyDown(sc_Space);
                sound(KICK_HIT);
                current_menu++;
                if(current_menu > 401) current_menu = 400;
            }

            if( KB_KeyPressed(sc_Escape) )
            {
                if(ps[myconnectindex].gm&MODE_GAME)
                    cmenu(50);
                else cmenu(0);
                return;
            }

            flushperms();
            switch(current_menu)
            {
                case 400:
                    rotatesprite(0,0,65536L,0,TEXTSTORY,0,0,10+16+64, 0,0,xdim-1,ydim-1);
                    break;
                case 401:
                    rotatesprite(0,0,65536L,0,F1HELP,0,0,10+16+64, 0,0,xdim-1,ydim-1);
                    break;
            }

#endif

            break;

        case 500:
            c = 320>>1;

            gametext(c,90,"Are you sure you want to quit?",0,2+8+16);
            gametext(c,99,"(Y/N)",0,2+8+16);

            if( KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                KB_FlushKeyboardQueue();

                if( gamequit == 0 && ( numplayers > 1 ) )
                {
                    if(ps[myconnectindex].gm&MODE_GAME)
                    {
                        gamequit = 1;
                        quittimer = totalclock+120;
                    }
                    else
                    {
                        sendlogoff();
                        gameexit(" ");
                    }
                }
                else if( numplayers < 2 )
                    gameexit(" ");

                if( ( totalclock > quittimer ) && ( gamequit == 1) )
                    gameexit("Timed out.");
            }

            x = probe(186,124,0,0);
            if(x == -1 || KB_KeyPressed(sc_N) || RMB)
            {
                KB_ClearKeyDown(sc_N);
                quittimer = 0;
                if( ps[myconnectindex].gm&MODE_DEMO )
                    ps[myconnectindex].gm = MODE_DEMO;
                else
                {
                    ps[myconnectindex].gm &= ~MODE_MENU;
                    if(ud.multimode < 2  && ud.recstat != 2)
                    {
                        ready2send = 1;
                        totalclock = ototalclock;
                    }
                }
            }

            break;
        case 501:
            c = 320>>1;
            gametext(c,90,"Quit to Title?",0,2+8+16);
            gametext(c,99,"(Y/N)",0,2+8+16);

            if( KB_KeyPressed(sc_Space) || KB_KeyPressed(sc_Enter) || KB_KeyPressed(sc_kpad_Enter) || KB_KeyPressed(sc_Y) || LMB )
            {
                KB_FlushKeyboardQueue();
                ps[myconnectindex].gm = MODE_DEMO;
                if(ud.recstat == 1)
                    closedemowrite();
                cmenu(0);
            }

            x = probe(186,124,0,0);

            if(x == -1 || KB_KeyPressed(sc_N) || RMB)
            {
                ps[myconnectindex].gm &= ~MODE_MENU;
                if(ud.multimode < 2  && ud.recstat != 2)
                {
                    ready2send = 1;
                    totalclock = ototalclock;
                }
            }

            break;

        case 601:
            displayfragbar();
            rotatesprite(160<<16,29<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(320>>1,34,0,0,&ud.user_name[myconnectindex][0]);

            sprintf(tempbuf,"Waiting for master");
            gametext(160,50,tempbuf,0,2+8+16);
            gametext(160,59,"to select level",0,2+8+16);

            if( KB_KeyPressed(sc_Escape) )
            {
                KB_ClearKeyDown(sc_Escape);
                sound(EXITMENUSOUND);
                cmenu(0);
            }
            break;

        case 602:
            if(menunamecnt == 0)
            {
        //        getfilenames("SUBD");
                getfilenames("*.MAP");
                sortfilenames();
                if (menunamecnt == 0)
                    cmenu(600);
            }
        case 603:
            c = (320>>1) - 120;
            displayfragbar();
            rotatesprite(320>>1<<16,19<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(320>>1,24,0,0,"USER MAPS");
            for(x=0;x<menunamecnt;x++)
            {
                if(x == fileselect)
                    minitext(15 + (x/15)*54,32 + (x%15)*8,menuname[x],0,26);
                else minitext(15 + (x/15)*54,32 + (x%15)*8,menuname[x],16,26);
            }

            fileselect = probey;
            if( KB_KeyPressed( sc_LeftArrow ) || KB_KeyPressed( sc_kpad_4 ) || ((buttonstat&1) && minfo.dyaw < -256 ) )
            {
                KB_ClearKeyDown( sc_LeftArrow );
                KB_ClearKeyDown( sc_kpad_4 );
                probey -= 15;
                if(probey < 0) probey += 15;
                else sound(KICK_HIT);
            }
            if( KB_KeyPressed( sc_RightArrow ) || KB_KeyPressed( sc_kpad_6 ) || ((buttonstat&1) && minfo.dyaw > 256 ) )
            {
                KB_ClearKeyDown( sc_RightArrow );
                KB_ClearKeyDown( sc_kpad_6 );
                probey += 15;
                if(probey >= menunamecnt)
                    probey -= 15;
                else sound(KICK_HIT);
            }

            onbar = 0;
            x = probe(0,0,0,menunamecnt);

            if(x == -1) cmenu(600);
            else if(x >= 0)
            {
                tempbuf[0] = 8;
                tempbuf[1] = ud.m_level_number = 6;
                tempbuf[2] = ud.m_volume_number = 0;
                tempbuf[3] = ud.m_player_skill+1;

                if(ud.player_skill == 3)
                    ud.m_respawn_monsters = 1;
                else ud.m_respawn_monsters = 0;

                if(ud.m_coop == 0) ud.m_respawn_items = 1;
                else ud.m_respawn_items = 0;

                ud.m_respawn_inventory = 1;

                tempbuf[4] = ud.m_monsters_off;
                tempbuf[5] = ud.m_respawn_monsters;
                tempbuf[6] = ud.m_respawn_items;
                tempbuf[7] = ud.m_respawn_inventory;
                tempbuf[8] = ud.m_coop;
                tempbuf[9] = ud.m_marker;

                x = strlen(menuname[probey]);

                copybufbyte(menuname[probey],tempbuf+10,x);
                copybufbyte(menuname[probey],boardfilename,x+1);

                for(c=connecthead;c>=0;c=connectpoint2[c])
                    if(c != myconnectindex)
                        sendpacket(c,tempbuf,x+10);

                newgame(ud.m_volume_number,ud.m_level_number,ud.m_player_skill+1);
                enterlevel(MODE_GAME);
            }
            break;

        case 600:
            c = (320>>1) - 120;
            if((ps[myconnectindex].gm&MODE_GAME) != MODE_GAME)
                displayfragbar();
            rotatesprite(160<<16,26<<16,65536L,0,MENUBAR,16,0,10,0,0,xdim-1,ydim-1);
            menutext(160,31,0,0,&ud.user_name[myconnectindex][0]);

            x = probe(c,57-8,16,8);

            switch(x)
            {
                case -1:
                    ud.m_recstat = 0;
                    if(ps[myconnectindex].gm&MODE_GAME) cmenu(50);
                    else cmenu(0);
                    break;
                case 0:
                    ud.m_coop++;
                    if(ud.m_coop == 3) ud.m_coop = 0;
                    break;
                case 1:
#ifndef VOLUMEONE
                    ud.m_volume_number++;
#ifdef PLUTOPAK
                    if(ud.m_volume_number > 3) ud.m_volume_number = 0;
#else
                    if(ud.m_volume_number > 2) ud.m_volume_number = 0;
#endif
                    if(ud.m_volume_number == 0 && ud.m_level_number > 6)
                        ud.m_level_number = 0;
                    if(ud.m_level_number > 10) ud.m_level_number = 0;
#endif
                    break;
                case 2:
#ifndef ONELEVELDEMO
                    ud.m_level_number++;
#ifndef VOLUMEONE
                    if(ud.m_volume_number == 0 && ud.m_level_number > 6)
                        ud.m_level_number = 0;
#else
                    if(ud.m_volume_number == 0 && ud.m_level_number > 5)
                        ud.m_level_number = 0;
#endif
                    if(ud.m_level_number > 10) ud.m_level_number = 0;
#endif
                    break;
                case 3:
                    if(ud.m_monsters_off == 1 && ud.m_player_skill > 0)
                        ud.m_monsters_off = 0;

                    if(ud.m_monsters_off == 0)
                    {
                        ud.m_player_skill++;
                        if(ud.m_player_skill > 3)
                        {
                            ud.m_player_skill = 0;
                            ud.m_monsters_off = 1;
                        }
                    }
                    else ud.m_monsters_off = 0;

                    break;

                case 4:
                    if(ud.m_coop == 0)
                        ud.m_marker = !ud.m_marker;
                    break;

                case 5:
                    if(ud.m_coop == 1)
                        ud.m_ffire = !ud.m_ffire;
                    break;

                case 6:
#ifdef VOLUMEALL
                    if(boardfilename[0] == 0) break;

                    tempbuf[0] = 5;
                    tempbuf[1] = ud.m_level_number = 7;
                    tempbuf[2] = ud.m_volume_number = 0;
                    tempbuf[3] = ud.m_player_skill+1;

                    ud.level_number = ud.m_level_number;
                    ud.volume_number = ud.m_volume_number;

                    if( ud.m_player_skill == 3 ) ud.m_respawn_monsters = 1;
                    else ud.m_respawn_monsters = 0;

                    if(ud.m_coop == 0) ud.m_respawn_items = 1;
                    else ud.m_respawn_items = 0;

                    ud.m_respawn_inventory = 1;

                    tempbuf[4] = ud.m_monsters_off;
                    tempbuf[5] = ud.m_respawn_monsters;
                    tempbuf[6] = ud.m_respawn_items;
                    tempbuf[7] = ud.m_respawn_inventory;
                    tempbuf[8] = ud.m_coop;
                    tempbuf[9] = ud.m_marker;
                    tempbuf[10] = ud.m_ffire;

                    for(c=connecthead;c>=0;c=connectpoint2[c])
                    {
                        resetweapons(c);
                        resetinventory(c);

                        if(c != myconnectindex)
                            sendpacket(c,tempbuf,11);
                    }

                    newgame(ud.m_volume_number,ud.m_level_number,ud.m_player_skill+1);
                    enterlevel(MODE_GAME);

                    return;
#endif
                case 7:

                    tempbuf[0] = 5;
                    tempbuf[1] = ud.m_level_number;
                    tempbuf[2] = ud.m_volume_number;
                    tempbuf[3] = ud.m_player_skill+1;

                    if( ud.m_player_skill == 3 ) ud.m_respawn_monsters = 1;
                    else ud.m_respawn_monsters = 0;

                    if(ud.m_coop == 0) ud.m_respawn_items = 1;
                    else ud.m_respawn_items = 0;

                    ud.m_respawn_inventory = 1;

                    tempbuf[4] = ud.m_monsters_off;
                    tempbuf[5] = ud.m_respawn_monsters;
                    tempbuf[6] = ud.m_respawn_items;
                    tempbuf[7] = ud.m_respawn_inventory;
                    tempbuf[8] = ud.m_coop;
                    tempbuf[9] = ud.m_marker;
                    tempbuf[10] = ud.m_ffire;

                    for(c=connecthead;c>=0;c=connectpoint2[c])
                    {
                        resetweapons(c);
                        resetinventory(c);

                        if(c != myconnectindex)
                            sendpacket(c,tempbuf,11);
                    }

                    newgame(ud.m_volume_number,ud.m_level_number,ud.m_player_skill+1);
                    enterlevel(MODE_GAME);

                    return;

            }

            c += 40;

            if(ud.m_coop==1) gametext(c+70,57-7-9,"COOPERATIVE PLAY",0,2+8+16);
            else if(ud.m_coop==2) gametext(c+70,57-7-9,"DUKEMATCH (NO SPAWN)",0,2+8+16);
            else gametext(c+70,57-7-9,"DUKEMATCH (SPAWN)",0,2+8+16);

 #ifdef VOLUMEONE
            gametext(c+70,57+16-7-9,volume_names[ud.m_volume_number],0,2+8+16);
 #else
            gametext(c+70,57+16-7-9,volume_names[ud.m_volume_number],0,2+8+16);
 #endif

            gametext(c+70,57+16+16-7-9,&level_names[11*ud.m_volume_number+ud.m_level_number][0],0,2+8+16);

            if(ud.m_monsters_off == 0 || ud.m_player_skill > 0)
                gametext(c+70,57+16+16+16-7-9,skill_names[ud.m_player_skill],0,2+8+16);
            else gametext(c+70,57+16+16+16-7-9,"NONE",0,2+8+16);

            if(ud.m_coop == 0)
            {
                if(ud.m_marker)
                    gametext(c+70,57+16+16+16+16-7-9,"ON",0,2+8+16);
                else gametext(c+70,57+16+16+16+16-7-9,"OFF",0,2+8+16);
            }

            if(ud.m_coop == 1)
            {
                if(ud.m_ffire)
                    gametext(c+70,57+16+16+16+16+16-7-9,"ON",0,2+8+16);
                else gametext(c+70,57+16+16+16+16+16-7-9,"OFF",0,2+8+16);
            }

            c -= 44;

            menutext(c,57-9,SHX(-2),PHX(-2),"GAME TYPE");

#ifdef VOLUMEONE
            sprintf(tempbuf,"EPISODE %ld",ud.m_volume_number+1);
            menutext(c,57+16-9,SHX(-3),1,tempbuf);
#else
            sprintf(tempbuf,"EPISODE %ld",ud.m_volume_number+1);
            menutext(c,57+16-9,SHX(-3),PHX(-3),tempbuf);
#endif

#ifndef ONELEVELDEMO
            sprintf(tempbuf,"LEVEL %ld",ud.m_level_number+1);
            menutext(c,57+16+16-9,SHX(-4),PHX(-4),tempbuf);
#else
            sprintf(tempbuf,"LEVEL %ld",ud.m_level_number+1);
            menutext(c,57+16+16-9,SHX(-4),1,tempbuf);
#endif
            menutext(c,57+16+16+16-9,SHX(-5),PHX(-5),"MONSTERS");

            if(ud.m_coop == 0)
                menutext(c,57+16+16+16+16-9,SHX(-6),PHX(-6),"MARKERS");
            else
                menutext(c,57+16+16+16+16-9,SHX(-6),1,"MARKERS");

            if(ud.m_coop == 1)
                menutext(c,57+16+16+16+16+16-9,SHX(-6),PHX(-6),"FR. FIRE");
            else menutext(c,57+16+16+16+16+16-9,SHX(-6),1,"FR. FIRE");

#ifdef VOLUMEALL
            menutext(c,57+16+16+16+16+16+16-9,SHX(-7),boardfilename[0] == 0,"USER MAP");
            if( boardfilename[0] != 0 )
                gametext(c+70+44,57+16+16+16+16+16,boardfilename,0,2+8+16);
#else
            menutext(c,57+16+16+16+16+16+16-9,SHX(-7),1,"USER MAP");
#endif

            menutext(c,57+16+16+16+16+16+16+16-9,SHX(-8),PHX(-8),"START GAME");

            break;
    }

    if( (ps[myconnectindex].gm&MODE_MENU) != MODE_MENU)
    {
        vscrn();
        cameraclock = totalclock;
        cameradist = 65536L;
    }
}

void palto(char r,char g,char b,long e)
{
    int i;
    char temparray[768];

    for(i=0;i<768;i+=3)
    {
        temparray[i  ] =
            ps[myconnectindex].palette[i+0]+((((long)r-(long)ps[myconnectindex].palette[i+0])*(long)(e&127))>>6);
        temparray[i+1] =
            ps[myconnectindex].palette[i+1]+((((long)g-(long)ps[myconnectindex].palette[i+1])*(long)(e&127))>>6);
        temparray[i+2] =
            ps[myconnectindex].palette[i+2]+((((long)b-(long)ps[myconnectindex].palette[i+2])*(long)(e&127))>>6);
    }

    if( (e&128) == 0 )
        if ((vidoption != 1) || (vgacompatible == 1)) limitrate();

    setbrightness(ud.brightness>>2,temparray);
}


void drawoverheadmap(long cposx, long cposy, long czoom, short cang)
{
	long i, j, k, l, x1, y1, x2, y2, x3, y3, x4, y4, ox, oy, xoff, yoff;
	long dax, day, cosang, sinang, xspan, yspan, sprx, spry;
	long xrepeat, yrepeat, z1, z2, startwall, endwall, tilenum, daang;
	long xvect, yvect, xvect2, yvect2;
	short p;
	char col;
	walltype *wal, *wal2;
	spritetype *spr;

	xvect = sintable[(-cang)&2047] * czoom;
	yvect = sintable[(1536-cang)&2047] * czoom;
	xvect2 = mulscale16(xvect,yxaspect);
	yvect2 = mulscale16(yvect,yxaspect);

		//Draw red lines
	for(i=0;i<numsectors;i++)
	{
		if (!(show2dsector[i>>3]&(1<<(i&7)))) continue;

		startwall = sector[i].wallptr;
		endwall = sector[i].wallptr + sector[i].wallnum;

		z1 = sector[i].ceilingz; z2 = sector[i].floorz;

		for(j=startwall,wal=&wall[startwall];j<endwall;j++,wal++)
		{
			k = wal->nextwall; if (k < 0) continue;

			//if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;
			//if ((k > j) && ((show2dwall[k>>3]&(1<<(k&7))) > 0)) continue;

			if (sector[wal->nextsector].ceilingz == z1)
				if (sector[wal->nextsector].floorz == z2)
					if (((wal->cstat|wall[wal->nextwall].cstat)&(16+32)) == 0) continue;

			col = 139; //red
			if ((wal->cstat|wall[wal->nextwall].cstat)&1) col = 234; //magenta

			if (!(show2dsector[wal->nextsector>>3]&(1<<(wal->nextsector&7))))
				col = 24;
            else continue;

			ox = wal->x-cposx; oy = wal->y-cposy;
			x1 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
			y1 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

			wal2 = &wall[wal->point2];
			ox = wal2->x-cposx; oy = wal2->y-cposy;
			x2 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
			y2 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

			drawline256(x1,y1,x2,y2,col);
		}
	}

		//Draw sprites
	k = ps[screenpeek].i;
	for(i=0;i<numsectors;i++)
	{
		if (!(show2dsector[i>>3]&(1<<(i&7)))) continue;
		for(j=headspritesect[i];j>=0;j=nextspritesect[j])
			//if ((show2dsprite[j>>3]&(1<<(j&7))) > 0)
			{
                spr = &sprite[j];

                if (j == k || (spr->cstat&0x8000) || spr->cstat == 257 || spr->xrepeat == 0) continue;

				col = 71; //cyan
				if (spr->cstat&1) col = 234; //magenta

				sprx = spr->x;
				spry = spr->y;

                if( (spr->cstat&257) != 0) switch (spr->cstat&48)
				{
                    case 0: break;

						ox = sprx-cposx; oy = spry-cposy;
						x1 = dmulscale16(ox,xvect,-oy,yvect);
						y1 = dmulscale16(oy,xvect2,ox,yvect2);

						ox = (sintable[(spr->ang+512)&2047]>>7);
						oy = (sintable[(spr->ang)&2047]>>7);
						x2 = dmulscale16(ox,xvect,-oy,yvect);
						y2 = dmulscale16(oy,xvect,ox,yvect);

						x3 = mulscale16(x2,yxaspect);
						y3 = mulscale16(y2,yxaspect);

						drawline256(x1-x2+(xdim<<11),y1-y3+(ydim<<11),
										x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
						drawline256(x1-y2+(xdim<<11),y1+x3+(ydim<<11),
										x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
						drawline256(x1+y2+(xdim<<11),y1-x3+(ydim<<11),
										x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                        break;

					case 16:
                        if( spr->picnum == LASERLINE )
                        {
                            x1 = sprx; y1 = spry;
                            tilenum = spr->picnum;
                            xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
                            if ((spr->cstat&4) > 0) xoff = -xoff;
                            k = spr->ang; l = spr->xrepeat;
                            dax = sintable[k&2047]*l; day = sintable[(k+1536)&2047]*l;
                            l = tilesizx[tilenum]; k = (l>>1)+xoff;
                            x1 -= mulscale16(dax,k); x2 = x1+mulscale16(dax,l);
                            y1 -= mulscale16(day,k); y2 = y1+mulscale16(day,l);

                            ox = x1-cposx; oy = y1-cposy;
                            x1 = dmulscale16(ox,xvect,-oy,yvect);
                            y1 = dmulscale16(oy,xvect2,ox,yvect2);

                            ox = x2-cposx; oy = y2-cposy;
                            x2 = dmulscale16(ox,xvect,-oy,yvect);
                            y2 = dmulscale16(oy,xvect2,ox,yvect2);

                            drawline256(x1+(xdim<<11),y1+(ydim<<11),
										x2+(xdim<<11),y2+(ydim<<11),col);
                        }

                        break;

                    case 32:

						tilenum = spr->picnum;
						xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
						yoff = (long)((signed char)((picanm[tilenum]>>16)&255))+((long)spr->yoffset);
						if ((spr->cstat&4) > 0) xoff = -xoff;
						if ((spr->cstat&8) > 0) yoff = -yoff;

						k = spr->ang;
						cosang = sintable[(k+512)&2047]; sinang = sintable[k];
						xspan = tilesizx[tilenum]; xrepeat = spr->xrepeat;
						yspan = tilesizy[tilenum]; yrepeat = spr->yrepeat;

						dax = ((xspan>>1)+xoff)*xrepeat; day = ((yspan>>1)+yoff)*yrepeat;
						x1 = sprx + dmulscale16(sinang,dax,cosang,day);
						y1 = spry + dmulscale16(sinang,day,-cosang,dax);
						l = xspan*xrepeat;
						x2 = x1 - mulscale16(sinang,l);
						y2 = y1 + mulscale16(cosang,l);
						l = yspan*yrepeat;
						k = -mulscale16(cosang,l); x3 = x2+k; x4 = x1+k;
						k = -mulscale16(sinang,l); y3 = y2+k; y4 = y1+k;

						ox = x1-cposx; oy = y1-cposy;
						x1 = dmulscale16(ox,xvect,-oy,yvect);
						y1 = dmulscale16(oy,xvect2,ox,yvect2);

						ox = x2-cposx; oy = y2-cposy;
						x2 = dmulscale16(ox,xvect,-oy,yvect);
						y2 = dmulscale16(oy,xvect2,ox,yvect2);

						ox = x3-cposx; oy = y3-cposy;
						x3 = dmulscale16(ox,xvect,-oy,yvect);
						y3 = dmulscale16(oy,xvect2,ox,yvect2);

						ox = x4-cposx; oy = y4-cposy;
						x4 = dmulscale16(ox,xvect,-oy,yvect);
						y4 = dmulscale16(oy,xvect2,ox,yvect2);

						drawline256(x1+(xdim<<11),y1+(ydim<<11),
										x2+(xdim<<11),y2+(ydim<<11),col);

						drawline256(x2+(xdim<<11),y2+(ydim<<11),
										x3+(xdim<<11),y3+(ydim<<11),col);

						drawline256(x3+(xdim<<11),y3+(ydim<<11),
										x4+(xdim<<11),y4+(ydim<<11),col);

						drawline256(x4+(xdim<<11),y4+(ydim<<11),
										x1+(xdim<<11),y1+(ydim<<11),col);

						break;
				}
			}
	}

		//Draw white lines
	for(i=0;i<numsectors;i++)
	{
		if (!(show2dsector[i>>3]&(1<<(i&7)))) continue;

		startwall = sector[i].wallptr;
		endwall = sector[i].wallptr + sector[i].wallnum;

		k = -1;
		for(j=startwall,wal=&wall[startwall];j<endwall;j++,wal++)
		{
			if (wal->nextwall >= 0) continue;

			//if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;

			if (tilesizx[wal->picnum] == 0) continue;
			if (tilesizy[wal->picnum] == 0) continue;

			if (j == k)
				{ x1 = x2; y1 = y2; }
			else
			{
				ox = wal->x-cposx; oy = wal->y-cposy;
				x1 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
				y1 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);
			}

			k = wal->point2; wal2 = &wall[k];
			ox = wal2->x-cposx; oy = wal2->y-cposy;
			x2 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
			y2 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

			drawline256(x1,y1,x2,y2,24);
		}
	}

	 for(p=connecthead;p >= 0;p=connectpoint2[p])
	 {
          if(ud.scrollmode && p == screenpeek) continue;

          ox = sprite[ps[p].i].x-cposx; oy = sprite[ps[p].i].y-cposy;
		  daang = (sprite[ps[p].i].ang-cang)&2047;
		  if (p == screenpeek) { ox = 0; oy = 0; daang = 0; }
		  x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
		  y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

          if(p == screenpeek || ud.coop == 1 )
          {
                if(sprite[ps[p].i].xvel > 16 && ps[p].on_ground)
                    i = APLAYERTOP+((totalclock>>4)&3);
                else
                    i = APLAYERTOP;

                j = klabs(ps[p].truefz-ps[p].posz)>>8;
                j = mulscale(czoom*(sprite[ps[p].i].yrepeat+j),yxaspect,16);

                if(j < 22000) j = 22000;
                else if(j > (65536<<1)) j = (65536<<1);

                rotatesprite((x1<<4)+(xdim<<15),(y1<<4)+(ydim<<15),j,
                    daang,i,sprite[ps[p].i].shade,sprite[ps[p].i].pal,
                    (sprite[ps[p].i].cstat&2)>>1,windowx1,windowy1,windowx2,windowy2);
          }
	 }
}



void endanimsounds(long fr)
{
    switch(ud.volume_number)
    {
        case 0:break;
        case 1:
            switch(fr)
            {
                case 1:
                    sound(WIND_AMBIENCE);
                    break;
                case 26:
                    sound(ENDSEQVOL2SND1);
                    break;
                case 36:
                    sound(ENDSEQVOL2SND2);
                    break;
                case 54:
                    sound(THUD);
                    break;
                case 62:
                    sound(ENDSEQVOL2SND3);
                    break;
                case 75:
                    sound(ENDSEQVOL2SND4);
                    break;
                case 81:
                    sound(ENDSEQVOL2SND5);
                    break;
                case 115:
                    sound(ENDSEQVOL2SND6);
                    break;
                case 124:
                    sound(ENDSEQVOL2SND7);
                    break;
            }
            break;
        case 2:
            switch(fr)
            {
                case 1:
                    sound(WIND_REPEAT);
                    break;
                case 98:
                    sound(DUKE_GRUNT);
                    break;
                case 82+20:
                    sound(THUD);
                    sound(SQUISHED);
                    break;
                case 104+20:
                    sound(ENDSEQVOL3SND3);
                    break;
                case 114+20:
                    sound(ENDSEQVOL3SND2);
                    break;
                case 158:
                    sound(PIPEBOMB_EXPLODE);
                    break;
            }
            break;
    }
}

void logoanimsounds(long fr)
{
    switch(fr)
    {
        case 1:
            sound(FLY_BY);
            break;
        case 19:
            sound(PIPEBOMB_EXPLODE);
            break;
    }
}

void intro4animsounds(long fr)
{
    switch(fr)
    {
        case 1:
            sound(INTRO4_B);
            break;
        case 12:
        case 34:
            sound(SHORT_CIRCUIT);
            break;
        case 18:
            sound(INTRO4_5);
            break;
    }
}

void first4animsounds(long fr)
{
    switch(fr)
    {
        case 1:
            sound(INTRO4_1);
            break;
        case 12:
            sound(INTRO4_2);
            break;
        case 7:
            sound(INTRO4_3);
            break;
        case 26:
            sound(INTRO4_4);
            break;
    }
}

void intro42animsounds(long fr)
{
    switch(fr)
    {
        case 10:
            sound(INTRO4_6);
            break;
    }
}




void endanimvol41(long fr)
{
    switch(fr)
    {
        case 3:
            sound(DUKE_UNDERWATER);
            break;
        case 35:
            sound(VOL4ENDSND1);
            break;
    }
}

void endanimvol42(long fr)
{
    switch(fr)
    {
        case 11:
            sound(DUKE_UNDERWATER);
            break;
        case 20:
            sound(VOL4ENDSND1);
            break;
        case 39:
            sound(VOL4ENDSND2);
            break;
        case 50:
            FX_StopAllSounds();
            break;
    }
}

void endanimvol43(long fr)
{
    switch(fr)
    {
        case 1:
            sound(BOSS4_DEADSPEECH);
            break;
        case 40:
            sound(VOL4ENDSND1);
            sound(DUKE_UNDERWATER);
            break;
        case 50:
            sound(BIGBANG);
            break;
    }
}


long lastanimhack=0;
void playanm(char *fn,char t)
{
	char *animbuf, *palptr;
    long i, j, k, length=0, numframes=0;
    int32 handle=-1;

//    return;

    if(t != 7 && t != 9 && t != 10 && t != 11)
        KB_FlushKeyboardQueue();

    if( KB_KeyWaiting() )
    {
        FX_StopAllSounds();
        goto ENDOFANIMLOOP;
    }

	handle = kopen4load(fn,0);
	if(handle == -1) return;
	length = kfilelength(handle);

    walock[MAXTILES-3-t] = 219+t;

    if(anim == 0 || lastanimhack != (MAXTILES-3-t))
        allocache((long *)&anim,length+sizeof(anim_t),&walock[MAXTILES-3-t]);

    animbuf = (char *)(FP_OFF(anim)+sizeof(anim_t));

    lastanimhack = (MAXTILES-3-t);

    tilesizx[MAXTILES-3-t] = 200;
    tilesizy[MAXTILES-3-t] = 320;

	kread(handle,animbuf,length);
	kclose(handle);

	ANIM_LoadAnim (animbuf);
	numframes = ANIM_NumFrames();

	palptr = ANIM_GetPalette();
	for(i=0;i<256;i++)
	{
		j = (i<<2); k = j-i;
		tempbuf[j+0] = (palptr[k+2]>>2);
		tempbuf[j+1] = (palptr[k+1]>>2);
		tempbuf[j+2] = (palptr[k+0]>>2);
		tempbuf[j+3] = 0;
	}

	VBE_setPalette(0L,256L,tempbuf);

    ototalclock = totalclock + 10;

	for(i=1;i<numframes;i++)
	{
       while(totalclock < ototalclock)
       {
          if( KB_KeyWaiting() )
              goto ENDOFANIMLOOP;
          getpackets();
       }

       if(t == 10) ototalclock += 14;
       else if(t == 9) ototalclock += 10;
       else if(t == 7) ototalclock += 18;
       else if(t == 6) ototalclock += 14;
       else if(t == 5) ototalclock += 9;
       else if(ud.volume_number == 3) ototalclock += 10;
       else if(ud.volume_number == 2) ototalclock += 10;
       else if(ud.volume_number == 1) ototalclock += 18;
       else                           ototalclock += 10;

       waloff[MAXTILES-3-t] = FP_OFF(ANIM_DrawFrame(i));
       rotatesprite(0<<16,0<<16,65536L,512,MAXTILES-3-t,0,0,2+4+8+16+64, 0,0,xdim-1,ydim-1);
       nextpage();

       if(t == 8) endanimvol41(i);
       else if(t == 10) endanimvol42(i);
       else if(t == 11) endanimvol43(i);
       else if(t == 9) intro42animsounds(i);
       else if(t == 7) intro4animsounds(i);
       else if(t == 6) first4animsounds(i);
       else if(t == 5) logoanimsounds(i);
       else if(t < 4) endanimsounds(i);
	}

    ENDOFANIMLOOP:

    ANIM_FreeAnim ();
    walock[MAXTILES-3-t] = 1;
}

