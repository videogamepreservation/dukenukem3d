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

void moveweapons(void)
{
    short i, j, k, nexti, p, q, tempsect;
    long dax,day,daz, x, l, ll, x1, y1;
    unsigned long qq;
    spritetype *s;

    i = headspritestat[4];
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if(s->sectnum < 0) KILLIT(i);

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        switch(s->picnum)
        {
            case RADIUSEXPLOSION:
            case KNEE:
                KILLIT(i);
            case TONGUE:
                T1 = sintable[(T2)&2047]>>9;
                T2 += 32;
                if(T2 > 2047) KILLIT(i);

                if(sprite[s->owner].statnum == MAXSTATUS)
                    if(badguy(&sprite[s->owner]) == 0)
                        KILLIT(i);

                s->ang = sprite[s->owner].ang;
                s->x = sprite[s->owner].x;
                s->y = sprite[s->owner].y;
                if(sprite[s->owner].picnum == APLAYER)
                    s->z = sprite[s->owner].z-(34<<8);
                for(k=0;k<T1;k++)
                {
                    q = EGS(s->sectnum,
                        s->x+((k*sintable[(s->ang+512)&2047])>>9),
                        s->y+((k*sintable[s->ang&2047])>>9),
                        s->z+((k*ksgn(s->zvel))*klabs(s->zvel/12)),TONGUE,-40+(k<<1),
                        8,8,0,0,0,i,5);
                    sprite[q].cstat = 128;
                    sprite[q].pal = 8;
                }
                q = EGS(s->sectnum,
                    s->x+((k*sintable[(s->ang+512)&2047])>>9),
                    s->y+((k*sintable[s->ang&2047])>>9),
                    s->z+((k*ksgn(s->zvel))*klabs(s->zvel/12)),INNERJAW,-40,
                    32,32,0,0,0,i,5);
                sprite[q].cstat = 128;
                if( T2 > 512 && T2 < (1024) )
                    sprite[q].picnum = INNERJAW+1;

                goto BOLT;

            case FREEZEBLAST:
                if(s->yvel < 1)
                {
                    j = spawn(i,TRANSPORTERSTAR);
                    sprite[j].pal = 1;
                    sprite[j].xrepeat = 32;
                    sprite[j].yrepeat = 32;
                    KILLIT(i);
                }
            case SHRINKSPARK:
            case RPG:
            case FIRELASER:
            case SPIT:
            case COOLEXPLOSION1:

                if( s->picnum == COOLEXPLOSION1 )
                    if( Sound[WIERDSHOT_FLY].num == 0 )
                        spritesound(WIERDSHOT_FLY,i);

                p = -1;

                if(s->picnum == RPG && sector[s->sectnum].lotag == 2)
                {
                    k = s->xvel>>1;
                    ll = s->zvel>>1;
                }
                else
                {
                    k = s->xvel;
                    ll = s->zvel;
                }

                dax = s->x; day = s->y; daz = s->z;

                getglobalz(i);
                qq = CLIPMASK1;

                switch(s->picnum)
                {
                    case RPG:
                        if(hittype[i].picnum != BOSS2 && s->xrepeat >= 10 && sector[s->sectnum].lotag != 2)
                        {
                            j = spawn(i,SMALLSMOKE);
                            sprite[j].z += (1<<8);
                        }
                        break;
                }

                j = movesprite(i,
                    (k*(sintable[(s->ang+512)&2047]))>>14,
                    (k*(sintable[s->ang&2047]))>>14,ll,qq);

                if(s->picnum == RPG && s->yvel >= 0)
                    if( FindDistance2D(s->x-sprite[s->yvel].x,s->y-sprite[s->yvel].y) < 256 )
                        j = 49152|s->yvel;

                if(s->sectnum < 0) { KILLIT(i); }

                if( (j&49152) != 49152)
                    if(s->picnum != FREEZEBLAST)
                {
                    if(s->z < hittype[i].ceilingz)
                    {
                        j = 16384|(s->sectnum);
                        s->zvel = -1;
                    }
                    else if(s->z > hittype[i].floorz)
                    {
                        j = 16384|(s->sectnum);
                        if(sector[s->sectnum].lotag != 1)
                            s->zvel = 1;
                    }
                }

                if(s->picnum == FIRELASER)
                {
                    for(k=-3;k<2;k++)
                    {
                        x = EGS(s->sectnum,
                            s->x+((k*sintable[(s->ang+512)&2047])>>9),
                            s->y+((k*sintable[s->ang&2047])>>9),
                            s->z+((k*ksgn(s->zvel))*klabs(s->zvel/24)),FIRELASER,-40+(k<<2),
                            s->xrepeat,s->yrepeat,0,0,0,s->owner,5);

                        sprite[x].cstat = 128;
                        sprite[x].pal = s->pal;
                    }
                }
                else if(s->picnum == SPIT) if(s->zvel < 6144)
                    s->zvel += gc-112;

                if( j != 0 )
                {
                    if(s->picnum == COOLEXPLOSION1)
                    {
                        if( (j&49152) == 49152 && sprite[j&(MAXSPRITES-1)].picnum != APLAYER)
                            goto BOLT;
                        s->xvel = 0;
                        s->zvel = 0;
                    }

                    if( (j&49152) == 49152 )
                    {
                        j &= (MAXSPRITES-1);

                        if(s->picnum == FREEZEBLAST && sprite[j].pal == 1 )
                            if( badguy(&sprite[j]) || sprite[j].picnum == APLAYER )
                        {
                            j = spawn(i,TRANSPORTERSTAR);
                            sprite[j].pal = 1;
                            sprite[j].xrepeat = 32;
                            sprite[j].yrepeat = 32;

                            KILLIT(i);
                        }

                        checkhitsprite(j,i);

                        if(sprite[j].picnum == APLAYER)
                        {
                            p = sprite[j].yvel;
                            spritesound(PISTOL_BODYHIT,j);

                            if(s->picnum == SPIT)
                            {
                                ps[p].horiz += 32;
                                ps[p].return_to_center = 8;

                                if(ps[p].loogcnt == 0)
                                {
                                    if(Sound[DUKE_LONGTERM_PAIN].num < 1)
                                        spritesound(DUKE_LONGTERM_PAIN,ps[p].i);

                                    j = 3+(TRAND&3);
                                    ps[p].numloogs = j;
                                    ps[p].loogcnt = 24*4;
                                    for(x=0;x < j;x++)
                                    {
                                        ps[p].loogiex[x] = TRAND%xdim;
                                        ps[p].loogiey[x] = TRAND%ydim;
                                    }
                                }
                            }
                        }
                    }
                    else if( (j&49152) == 32768 )
                    {
                        j &= (MAXWALLS-1);

                        if(s->picnum != RPG && s->picnum != FREEZEBLAST && s->picnum != SPIT && ( wall[j].overpicnum == MIRROR || wall[j].picnum == MIRROR ) )
                        {
                            k = getangle(
                                    wall[wall[j].point2].x-wall[j].x,
                                    wall[wall[j].point2].y-wall[j].y);
                            s->ang = ((k<<1) - s->ang)&2047;
                            s->owner = i;
                            spawn(i,TRANSPORTERSTAR);
                            goto BOLT;
                        }
                        else
                        {
                            setsprite(i,dax,day,daz);
                            checkhitwall(i,j,s->x,s->y,s->z,s->picnum);

                            if(s->picnum == FREEZEBLAST)
                            {
                                if( wall[j].overpicnum != MIRROR && wall[j].picnum != MIRROR )
                                    s->yvel--;

                                k = getangle(
                                    wall[wall[j].point2].x-wall[j].x,
                                    wall[wall[j].point2].y-wall[j].y);
                                s->ang = ((k<<1) - s->ang)&2047;
                                goto BOLT;
                            }
                        }
                    }
                    else if( (j&49152) == 16384)
                    {
                        setsprite(i,dax,day,daz);

                        if(s->zvel < 0)
                        {
                            if( sector[s->sectnum].ceilingstat&1 )
                                if(sector[s->sectnum].ceilingpal == 0)
                                    KILLIT(i);

                            checkhitceiling(s->sectnum);
                        }

                        if(s->picnum == FREEZEBLAST)
                        {
                            bounce(i);
                            ssp(i,qq);
                            s->yvel--;
                            goto BOLT;
                        }
                    }

                    if(s->picnum != SPIT)
                    {
                        if(s->picnum == RPG)
                        {
                            k = spawn(i,EXPLOSION2);
                            sprite[k].x = dax;
                            sprite[k].y = day;
                            sprite[k].z = daz;

                            if(s->xrepeat < 10)
                            {
                                sprite[k].xrepeat = 6;
                                sprite[k].yrepeat = 6;
                            }
                            else if( (j&49152) == 16384)
                            {
                                if( s->zvel > 0)
                                    spawn(i,EXPLOSION2BOT);
                                else { sprite[k].cstat |= 8; sprite[k].z += (48<<8); }
                            }
                        }
                        else if(s->picnum == SHRINKSPARK)
                        {
                            spawn(i,SHRINKEREXPLOSION);
                            spritesound(SHRINKER_HIT,i);
                            hitradius(i,shrinkerblastradius,0,0,0,0);
                        }
                        else if( s->picnum != COOLEXPLOSION1 && s->picnum != FREEZEBLAST && s->picnum != FIRELASER)
                        {
                            k = spawn(i,EXPLOSION2);
                            sprite[k].xrepeat = sprite[k].yrepeat = s->xrepeat>>1;
                            if( (j&49152) == 16384)
                            {
                                if( s->zvel < 0)
                                    { sprite[k].cstat |= 8; sprite[k].z += (72<<8); }
                            }
                        }
                        if( s->picnum == RPG )
                        {
                            spritesound(RPG_EXPLODE,i);

                            if(s->xrepeat >= 10)
                            {
                                x = s->extra;
                                hitradius( i,rpgblastradius, x>>2,x>>1,x-(x>>2),x);
                            }
                            else
                            {
                                x = s->extra+(global_random&3);
                                hitradius( i,(rpgblastradius>>1),x>>2,x>>1,x-(x>>2),x);
                            }
                        }
                    }
                    if(s->picnum != COOLEXPLOSION1) KILLIT(i);
                }
                if(s->picnum == COOLEXPLOSION1)
                {
                    s->shade++;
                    if(s->shade >= 40) KILLIT(i);
                }
                else if(s->picnum == RPG && sector[s->sectnum].lotag == 2 && s->xrepeat >= 10 && rnd(184))
                    spawn(i,WATERBUBBLE);

                goto BOLT;


            case SHOTSPARK1:
                p = findplayer(s,&x);
                execute(i,p,x);
                goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}
