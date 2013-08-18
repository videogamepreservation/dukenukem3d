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

// Floor Over Floor

// If standing in sector with SE42
// then draw viewing to SE41 and raise all =hi SE43 cielings.

// If standing in sector with SE43
// then draw viewing to SE40 and lower all =hi SE42 floors.

// If standing in sector with SE44
// then draw viewing to SE40.

// If standing in sector with SE45
// then draw viewing to SE41.

#define FOFTILE 13
#define FOFTILEX 32
#define FOFTILEY 32
char fofon=0;
long tempsectorz[MAXSECTORS];
long tempsectorpicnum[MAXSECTORS];
//short tempcursectnum;

SE40_Draw(int spnum)
{
 int i=0,j=0,k=0;
 int floor1=0,floor2=0,ok=0,fofmode=0;
 long offx,offy;

 if(!fofon) return;
 if(sprite[spnum].ang!=512) return;

 i = FOFTILE;    //Effect TILE
 if (!(gotpic[i>>3]&(1<<(i&7)))) return;
 gotpic[i>>3] &= ~(1<<(i&7));

 floor1=spnum;

 if(sprite[spnum].lotag==42) fofmode=40;
 if(sprite[spnum].lotag==43) fofmode=41;
 if(sprite[spnum].lotag==44) fofmode=40;
 if(sprite[spnum].lotag==45) fofmode=41;

// fofmode=sprite[spnum].lotag-2;

// sectnum=sprite[j].sectnum;
// sectnum=cursectnum;
 ok++;

/*  recursive?
 for(j=0;j<MAXSPRITES;j++)
 {
  if(
     sprite[j].sectnum==sectnum &&
     sprite[j].picnum==1 &&
     sprite[j].lotag==110
    ) { DrawFloorOverFloor(j); break;}
 }
*/

 if(ok==0) { Message("no fof",RED); return; }

 for(j=0;j<MAXSPRITES;j++)
 {
  if(
     sprite[j].picnum==1 &&
     sprite[j].lotag==fofmode &&
     sprite[j].hitag==sprite[floor1].hitag
    ) { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
 }
 if(ok==1) { Message("no floor1",RED); return; }

 if(fofmode==40) k=41; else k=40;

 for(j=0;j<MAXSPRITES;j++)
 {
  if(
     sprite[j].picnum==1 &&
     sprite[j].lotag==k &&
     sprite[j].hitag==sprite[floor1].hitag
    ) {floor2=j; ok++; break;}
 }

 if(ok==2) { Message("no floor2",RED); return; }

 for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
 {
  if(sprite[j].picnum==1 &&
     sprite[j].lotag==k+2 &&
     sprite[j].hitag==sprite[floor1].hitag
    )
    {
     if(k==40)
     {tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
      sector[sprite[j].sectnum].floorz+=(((ps[screenpeek].posz-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
      tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
      sector[sprite[j].sectnum].floorpicnum=13;
     }
     if(k==41)
     {tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
      sector[sprite[j].sectnum].ceilingz+=(((ps[screenpeek].posz-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
      tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
      sector[sprite[j].sectnum].ceilingpicnum=13;
     }
    }
 }

 i=floor1;
 offx=ps[screenpeek].posx-sprite[i].x;
 offy=ps[screenpeek].posy-sprite[i].y;
 i=floor2;
 drawrooms(offx+sprite[i].x,offy+sprite[i].y,ps[screenpeek].posz,ps[screenpeek].ang,horiz,sprite[i].sectnum);
 ExtAnalyzeSprites();
 drawmasks();

 for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
 {
  if(sprite[j].picnum==1 &&
     sprite[j].lotag==k+2 &&
     sprite[j].hitag==sprite[floor1].hitag
    )
    {
     if(k==40)
     {sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
      sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
     }
     if(k==41)
     {sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
      sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
     }
    }// end if
 }// end for

} // end SE40
