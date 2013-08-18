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

//UPDATE THIS FILE OVER THE OLD GETSPRITESCORE/COMPUTERGETINPUT FUNCTIONS
getspritescore(long snum, long dapicnum)
{
	switch(dapicnum)
	{
		case FIRSTGUNSPRITE: return(5);
		case CHAINGUNSPRITE: return(50);
		case RPGSPRITE: return(200);
		case FREEZESPRITE: return(25);
		case SHRINKERSPRITE: return(80);
		case HEAVYHBOMB: return(60);
		case TRIPBOMBSPRITE: return(50);
		case SHOTGUNSPRITE: return(120);
		case DEVISTATORSPRITE: return(120);

		case FREEZEAMMO: if (ps[snum].ammo_amount[FREEZE_WEAPON] < max_ammo_amount[FREEZE_WEAPON]) return(10); else return(0);
		case AMMO: if (ps[snum].ammo_amount[SHOTGUN_WEAPON] < max_ammo_amount[SHOTGUN_WEAPON]) return(10); else return(0);
		case BATTERYAMMO: if (ps[snum].ammo_amount[CHAINGUN_WEAPON] < max_ammo_amount[CHAINGUN_WEAPON]) return(20); else return(0);
		case DEVISTATORAMMO: if (ps[snum].ammo_amount[DEVISTATOR_WEAPON] < max_ammo_amount[DEVISTATOR_WEAPON]) return(25); else return(0);
		case RPGAMMO: if (ps[snum].ammo_amount[RPG_WEAPON] < max_ammo_amount[RPG_WEAPON]) return(50); else return(0);
		case CRYSTALAMMO: if (ps[snum].ammo_amount[SHRINKER_WEAPON] < max_ammo_amount[SHRINKER_WEAPON]) return(10); else return(0);
		case HBOMBAMMO: if (ps[snum].ammo_amount[HANDBOMB_WEAPON] < max_ammo_amount[HANDBOMB_WEAPON]) return(30); else return(0);
		case SHOTGUNAMMO: if (ps[snum].ammo_amount[SHOTGUN_WEAPON] < max_ammo_amount[SHOTGUN_WEAPON]) return(25); else return(0);

		case COLA: if (sprite[ps[snum].i].extra < 100) return(10); else return(0);
		case SIXPAK: if (sprite[ps[snum].i].extra < 100) return(30); else return(0);
		case FIRSTAID: if (ps[snum].firstaid_amount < 100) return(100); else return(0);
		case SHIELD: if (ps[snum].shield_amount < 100) return(50); else return(0);
		case STEROIDS: if (ps[snum].steroids_amount < 400) return(30); else return(0);
		case AIRTANK: if (ps[snum].scuba_amount < 6400) return(30); else return(0);
		case JETPACK: if (ps[snum].jetpack_amount < 1600) return(100); else return(0);
		case HEATSENSOR: if (ps[snum].heat_amount < 1200) return(10); else return(0);
		case ACCESSCARD: return(1);
		case BOOTS: if (ps[snum].boot_amount < 200) return(50); else return(0);
		case ATOMICHEALTH: if (sprite[ps[snum].i].extra < max_player_health) return(50); else return(0);
		case HOLODUKE: if (ps[snum].holoduke_amount < 2400) return(30); else return(0);
	}
	return(0);
}

static long fdmatrix[12][12] =
{
 //KNEE PIST SHOT CHAIN RPG PIPE SHRI DEVI WALL FREE HAND EXPA
	 128,  -1,  -1,  -1, 128,  -1,  -1,  -1, 128,  -1, 128,  -1,   //KNEE
	1024,1024,1024,1024,2560, 128,2560,2560,1024,2560,2560,2560,   //PIST
	 512, 512, 512, 512,2560, 128,2560,2560,1024,2560,2560,2560,   //SHOT
	 512, 512, 512, 512,2560, 128,2560,2560,1024,2560,2560,2560,   //CHAIN
	2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,   //RPG
	 512, 512, 512, 512,2048, 512,2560,2560, 512,2560,2560,2560,   //PIPE
	 128, 128, 128, 128,2560, 128,2560,2560, 128, 128, 128, 128,   //SHRI
	1536,1536,1536,1536,2560,1536,1536,1536,1536,1536,1536,1536,   //DEVI
	  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   //WALL
	 128, 128, 128, 128,2560, 128,2560,2560, 128, 128, 128, 128,   //FREE
	2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,2560,   //HAND
	 128, 128, 128, 128,2560, 128,2560,2560, 128, 128, 128, 128,   //EXPA
};

static long goalx[MAXPLAYERS], goaly[MAXPLAYERS], goalz[MAXPLAYERS];
static long goalsect[MAXPLAYERS], goalwall[MAXPLAYERS], goalsprite[MAXPLAYERS];
static long goalplayer[MAXPLAYERS], clipmovecount[MAXPLAYERS];
short searchsect[MAXSECTORS], searchparent[MAXSECTORS];
void computergetinput(long snum, input *syn)
{
	long i, j, k, l, x1, y1, z1, x2, y2, z2, x3, y3, z3, dx, dy;
	long dist, daang, zang, fightdist, damyang, damysect;
	long startsect, endsect, splc, send, startwall, endwall;
	short dasect, dawall, daspr;
	struct player_struct *p;
	walltype *wal;

	p = &ps[snum];
	syn->fvel = 0;
	syn->svel = 0;
	syn->avel = 0;
	syn->horz = 0;
	syn->bits = 0;

	x1 = sprite[p->i].x;
	y1 = sprite[p->i].y;
	z1 = sprite[p->i].z;
	damyang = sprite[p->i].ang;
	damysect = sprite[p->i].sectnum;
	if ((numplayers >= 2) && (snum == myconnectindex))
		{ x1 = myx; y1 = myy; z1 = myz+PHEIGHT; damyang = myang; damysect = mycursectnum; }

	if (!(numframes&7))
	{
		if (!cansee(x1,y1,z1-(48<<8),damysect,x2,y2,z2-(48<<8),sprite[ps[goalplayer[snum]].i].sectnum))
			goalplayer[snum] = snum;
	}

	if ((goalplayer[snum] == snum) || (ps[goalplayer[snum]].dead_flag != 0))
	{
		j = 0x7fffffff;
		for(i=connecthead;i>=0;i=connectpoint2[i])
			if (i != snum)
			{
				dist = ksqrt((sprite[ps[i].i].x-x1)*(sprite[ps[i].i].x-x1)+(sprite[ps[i].i].y-y1)*(sprite[ps[i].i].y-y1));

				x2 = sprite[ps[i].i].x;
				y2 = sprite[ps[i].i].y;
				z2 = sprite[ps[i].i].z;
				if (!cansee(x1,y1,z1-(48<<8),damysect,x2,y2,z2-(48<<8),sprite[ps[i].i].sectnum))
					dist <<= 1;

				if (dist < j) { j = dist; goalplayer[snum] = i; }
			}
	}

	x2 = sprite[ps[goalplayer[snum]].i].x;
	y2 = sprite[ps[goalplayer[snum]].i].y;
	z2 = sprite[ps[goalplayer[snum]].i].z;

	if (p->dead_flag) syn->bits |= (1<<29);
	if ((p->firstaid_amount > 0) && (p->last_extra < 100))
		syn->bits |= (1<<16);

	for(j=headspritestat[4];j>=0;j=nextspritestat[j])
	{
		switch (sprite[j].picnum)
		{
			case TONGUE: k = 4; break;
			case FREEZEBLAST: k = 4; break;
			case SHRINKSPARK: k = 16; break;
			case RPG: k = 16; break;
			default: k = 0; break;
		}
		if (k)
		{
			x3 = sprite[j].x;
			y3 = sprite[j].y;
			z3 = sprite[j].z;
			for(l=0;l<=8;l++)
			{
				if (tmulscale11(x3-x1,x3-x1,y3-y1,y3-y1,(z3-z1)>>4,(z3-z1)>>4) < 3072)
				{
					dx = sintable[(sprite[j].ang+512)&2047];
					dy = sintable[sprite[j].ang&2047];
					if ((x1-x3)*dy > (y1-y3)*dx) i = -k*512; else i = k*512;
					syn->fvel -= mulscale17(dy,i);
					syn->svel += mulscale17(dx,i);
				}
				if (l < 7)
				{
					x3 += (mulscale14(sprite[j].xvel,sintable[(sprite[j].ang+512)&2047])<<2);
					y3 += (mulscale14(sprite[j].xvel,sintable[sprite[j].ang&2047])<<2);
					z3 += (sprite[j].zvel<<2);
				}
				else
				{
					hitscan(sprite[j].x,sprite[j].y,sprite[j].z,sprite[j].sectnum,
					 mulscale14(sprite[j].xvel,sintable[(sprite[j].ang+512)&2047]),
					 mulscale14(sprite[j].xvel,sintable[sprite[j].ang&2047]),
					 (long)sprite[j].zvel,
					 &dasect,&dawall,&daspr,&x3,&y3,&z3,CLIPMASK1);
				}
			}
		}
	}

	if ((ps[goalplayer[snum]].dead_flag == 0) &&
		((cansee(x1,y1,z1,damysect,x2,y2,z2,sprite[ps[goalplayer[snum]].i].sectnum)) ||
		 (cansee(x1,y1,z1-(24<<8),damysect,x2,y2,z2-(24<<8),sprite[ps[goalplayer[snum]].i].sectnum)) ||
		 (cansee(x1,y1,z1-(48<<8),damysect,x2,y2,z2-(48<<8),sprite[ps[goalplayer[snum]].i].sectnum))))
	{
		syn->bits |= (1<<2);

		if ((p->curr_weapon == HANDBOMB_WEAPON) && (!(rand()&7)))
			syn->bits &= ~(1<<2);

		if (p->curr_weapon == TRIPBOMB_WEAPON)
			syn->bits |= ((rand()%MAX_WEAPONS)<<8);

		if (p->curr_weapon == RPG_WEAPON)
		{
			hitscan(x1,y1,z1-PHEIGHT,damysect,sintable[(damyang+512)&2047],sintable[damyang&2047],
				(100-p->horiz-p->horizoff)*32,&dasect,&dawall,&daspr,&x3,&y3,&z3,CLIPMASK1);
			if ((x3-x1)*(x3-x1)+(y3-y1)*(y3-y1) < 2560*2560) syn->bits &= ~(1<<2);
		}


		fightdist = fdmatrix[p->curr_weapon][ps[goalplayer[snum]].curr_weapon];
		if (fightdist < 128) fightdist = 128;
		dist = ksqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); if (dist == 0) dist = 1;
		daang = getangle(x2+(ps[goalplayer[snum]].posxv>>14)-x1,y2+(ps[goalplayer[snum]].posyv>>14)-y1);
		zang = 100-((z2-z1)*8)/dist;
		fightdist = max(fightdist,(klabs(z2-z1)>>4));

		if (sprite[ps[goalplayer[snum]].i].yrepeat < 32)
			{ fightdist = 0; syn->bits &= ~(1<<2); }
		if (sprite[ps[goalplayer[snum]].i].pal == 1)
			{ fightdist = 0; syn->bits &= ~(1<<2); }

		if (dist < 256) syn->bits |= (1<<22);

		x3 = x2+((x1-x2)*fightdist/dist);
		y3 = y2+((y1-y2)*fightdist/dist);
		syn->fvel += (x3-x1)*2047/dist;
		syn->svel += (y3-y1)*2047/dist;

			//Strafe attack
		if (fightdist)
		{
			j = totalclock+snum*13468;
			i = sintable[(j<<6)&2047];
			i += sintable[((j+4245)<<5)&2047];
			i += sintable[((j+6745)<<4)&2047];
			i += sintable[((j+15685)<<3)&2047];
			dx = sintable[(sprite[ps[goalplayer[snum]].i].ang+512)&2047];
			dy = sintable[sprite[ps[goalplayer[snum]].i].ang&2047];
			if ((x1-x2)*dy > (y1-y2)*dx) i += 8192; else i -= 8192;
			syn->fvel += ((sintable[(daang+1024)&2047]*i)>>17);
			syn->svel += ((sintable[(daang+512)&2047]*i)>>17);
		}

		syn->avel = min(max((((daang+1024-damyang)&2047)-1024)>>1,-127),127);
		syn->horz = min(max((zang-p->horiz)>>1,-MAXHORIZ),MAXHORIZ);
		syn->bits |= (1<<23);
		return;
	}

	goalsect[snum] = -1;
	if (goalsect[snum] < 0)
	{
		goalwall[snum] = -1;
		startsect = sprite[p->i].sectnum;
		endsect = sprite[ps[goalplayer[snum]].i].sectnum;

		clearbufbyte(show2dsector,(MAXSECTORS+7)>>3,0L);
		searchsect[0] = startsect;
		searchparent[0] = -1;
		show2dsector[startsect>>3] |= (1<<(startsect&7));
		for(splc=0,send=1;splc<send;splc++)
		{
			startwall = sector[searchsect[splc]].wallptr;
			endwall = startwall + sector[searchsect[splc]].wallnum;
			for(i=startwall,wal=&wall[startwall];i<endwall;i++,wal++)
			{
				j = wal->nextsector; if (j < 0) continue;

				dx = ((wall[wal->point2].x+wal->x)>>1);
				dy = ((wall[wal->point2].y+wal->y)>>1);
				if ((getceilzofslope(j,dx,dy) > getflorzofslope(j,dx,dy)-(28<<8)) && ((sector[j].lotag < 15) || (sector[j].lotag > 22)))
					continue;
				if (getflorzofslope(j,dx,dy) < getflorzofslope(searchsect[splc],dx,dy)-(72<<8))
					continue;
				if ((show2dsector[j>>3]&(1<<(j&7))) == 0)
				{
					show2dsector[j>>3] |= (1<<(j&7));
					searchsect[send] = (short)j;
					searchparent[send] = (short)splc;
					send++;
					if (j == endsect)
					{
						clearbufbyte(show2dsector,(MAXSECTORS+7)>>3,0L);
						for(k=send-1;k>=0;k=searchparent[k])
							show2dsector[searchsect[k]>>3] |= (1<<(searchsect[k]&7));

						for(k=send-1;k>=0;k=searchparent[k])
							if (!searchparent[k]) break;

						goalsect[snum] = searchsect[k];
						startwall = sector[goalsect[snum]].wallptr;
						endwall = startwall+sector[goalsect[snum]].wallnum;
						x3 = y3 = 0;
						for(i=startwall;i<endwall;i++)
						{
							x3 += wall[i].x;
							y3 += wall[i].y;
						}
						x3 /= (endwall-startwall);
						y3 /= (endwall-startwall);

						startwall = sector[startsect].wallptr;
						endwall = startwall+sector[startsect].wallnum;
						l = 0; k = startwall;
						for(i=startwall;i<endwall;i++)
						{
							if (wall[i].nextsector != goalsect[snum]) continue;
							dx = wall[wall[i].point2].x-wall[i].x;
							dy = wall[wall[i].point2].y-wall[i].y;

							//if (dx*(y1-wall[i].y) <= dy*(x1-wall[i].x))
							//   if (dx*(y2-wall[i].y) >= dy*(x2-wall[i].x))
									if ((x3-x1)*(wall[i].y-y1) <= (y3-y1)*(wall[i].x-x1))
										if ((x3-x1)*(wall[wall[i].point2].y-y1) >= (y3-y1)*(wall[wall[i].point2].x-x1))
											{ k = i; break; }

							dist = ksqrt(dx*dx+dy*dy);
							if (dist > l) { l = dist; k = i; }
						}
						goalwall[snum] = k;
						daang = ((getangle(wall[wall[k].point2].x-wall[k].x,wall[wall[k].point2].y-wall[k].y)+1536)&2047);
						goalx[snum] = ((wall[k].x+wall[wall[k].point2].x)>>1)+(sintable[(daang+512)&2047]>>8);
						goaly[snum] = ((wall[k].y+wall[wall[k].point2].y)>>1)+(sintable[daang&2047]>>8);
						goalz[snum] = sector[goalsect[snum]].floorz-(32<<8);
						break;
					}
				}
			}

			for(i=headspritesect[searchsect[splc]];i>=0;i=nextspritesect[i])
				if (sprite[i].lotag == 7)
				{
					j = sprite[sprite[i].owner].sectnum;
					if ((show2dsector[j>>3]&(1<<(j&7))) == 0)
					{
						show2dsector[j>>3] |= (1<<(j&7));
						searchsect[send] = (short)j;
						searchparent[send] = (short)splc;
						send++;
						if (j == endsect)
						{
							clearbufbyte(show2dsector,(MAXSECTORS+7)>>3,0L);
							for(k=send-1;k>=0;k=searchparent[k])
								show2dsector[searchsect[k]>>3] |= (1<<(searchsect[k]&7));

							for(k=send-1;k>=0;k=searchparent[k])
								if (!searchparent[k]) break;

							goalsect[snum] = searchsect[k];
							startwall = sector[startsect].wallptr;
							endwall = startwall+sector[startsect].wallnum;
							l = 0; k = startwall;
							for(i=startwall;i<endwall;i++)
							{
								dx = wall[wall[i].point2].x-wall[i].x;
								dy = wall[wall[i].point2].y-wall[i].y;
								dist = ksqrt(dx*dx+dy*dy);
								if ((wall[i].nextsector == goalsect[snum]) && (dist > l))
									{ l = dist; k = i; }
							}
							goalwall[snum] = k;
							daang = ((getangle(wall[wall[k].point2].x-wall[k].x,wall[wall[k].point2].y-wall[k].y)+1536)&2047);
							goalx[snum] = ((wall[k].x+wall[wall[k].point2].x)>>1)+(sintable[(daang+512)&2047]>>8);
							goaly[snum] = ((wall[k].y+wall[wall[k].point2].y)>>1)+(sintable[daang&2047]>>8);
							goalz[snum] = sector[goalsect[snum]].floorz-(32<<8);
							break;
						}
					}
				}
			if (goalwall[snum] >= 0) break;
		}
	}

	if ((goalsect[snum] < 0) || (goalwall[snum] < 0))
	{
		if (goalsprite[snum] < 0)
		{
			for(k=0;k<4;k++)
			{
				i = (rand()%numsectors);
				for(j=headspritesect[i];j>=0;j=nextspritesect[j])
				{
					if ((sprite[j].xrepeat <= 0) || (sprite[j].yrepeat <= 0)) continue;
					if (getspritescore(snum,sprite[j].picnum) <= 0) continue;
					if (cansee(x1,y1,z1-(32<<8),damysect,sprite[j].x,sprite[j].y,sprite[j].z-(4<<8),i))
						{ goalx[snum] = sprite[j].x; goaly[snum] = sprite[j].y; goalz[snum] = sprite[j].z; goalsprite[snum] = j; break; }
				}
			}
		}
		x2 = goalx[snum];
		y2 = goaly[snum];
		dist = ksqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); if (!dist) return;
		daang = getangle(x2-x1,y2-y1);
		syn->fvel += (x2-x1)*2047/dist;
		syn->svel += (y2-y1)*2047/dist;
		syn->avel = min(max((((daang+1024-damyang)&2047)-1024)>>3,-127),127);
	}
	else
		goalsprite[snum] = -1;

	x3 = p->posx; y3 = p->posy; z3 = p->posz; dasect = p->cursectnum;
	i = clipmove(&x3,&y3,&z3,&dasect,p->posxv,p->posyv,164L,4L<<8,4L<<8,CLIPMASK0);
	if (!i)
	{
		x3 = p->posx; y3 = p->posy; z3 = p->posz+(24<<8); dasect = p->cursectnum;
		i = clipmove(&x3,&y3,&z3,&dasect,p->posxv,p->posyv,164L,4L<<8,4L<<8,CLIPMASK0);
	}
	if (i)
	{
		clipmovecount[snum]++;

		j = 0;
		if ((i&0xc000) == 32768)  //Hit a wall (49152 for sprite)
			if (wall[i&(MAXWALLS-1)].nextsector >= 0)
			{
				if (getflorzofslope(wall[i&(MAXWALLS-1)].nextsector,p->posx,p->posy) <= p->posz+(24<<8)) j |= 1;
				if (getceilzofslope(wall[i&(MAXWALLS-1)].nextsector,p->posx,p->posy) >= p->posz-(24<<8)) j |= 2;
			}
		if ((i&0xc000) == 49152) j = 1;
		if (j&1) if (clipmovecount[snum] == 4) syn->bits |= (1<<0);
		if (j&2) syn->bits |= (1<<1);

			//Strafe attack
		daang = getangle(x2-x1,y2-y1);
		if ((i&0xc000) == 32768)
			daang = getangle(wall[wall[i&(MAXWALLS-1)].point2].x-wall[i&(MAXWALLS-1)].x,wall[wall[i&(MAXWALLS-1)].point2].y-wall[i&(MAXWALLS-1)].y);
		j = totalclock+snum*13468;
		i = sintable[(j<<6)&2047];
		i += sintable[((j+4245)<<5)&2047];
		i += sintable[((j+6745)<<4)&2047];
		i += sintable[((j+15685)<<3)&2047];
		syn->fvel += ((sintable[(daang+1024)&2047]*i)>>17);
		syn->svel += ((sintable[(daang+512)&2047]*i)>>17);

		if ((clipmovecount[snum]&31) == 2) syn->bits |= (1<<29);
		if ((clipmovecount[snum]&31) == 17) syn->bits |= (1<<22);
		if (clipmovecount[snum] > 32) { goalsect[snum] = -1; goalwall[snum] = -1; clipmovecount[snum] = 0; }

		goalsprite[snum] = -1;
	}
	else
		clipmovecount[snum] = 0;

	if ((goalsect[snum] >= 0) && (goalwall[snum] >= 0))
	{
		x2 = goalx[snum];
		y2 = goaly[snum];
		dist = ksqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); if (!dist) return;
		daang = getangle(x2-x1,y2-y1);
		if ((goalwall[snum] >= 0) && (dist < 4096))
			daang = ((getangle(wall[wall[goalwall[snum]].point2].x-wall[goalwall[snum]].x,wall[wall[goalwall[snum]].point2].y-wall[goalwall[snum]].y)+1536)&2047);
		syn->fvel += (x2-x1)*2047/dist;
		syn->svel += (y2-y1)*2047/dist;
		syn->avel = min(max((((daang+1024-damyang)&2047)-1024)>>3,-127),127);
	}
}
