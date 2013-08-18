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

long floorspace(short sectnum)
{
    if( (sector[sectnum].floorstat&1) && sector[sectnum].ceilingpal == 0 )
    {
        switch(sector[sectnum].floorpicnum)
        {
            case MOONSKY1:
            case BIGORBIT1:
                return 1;
        }
    }
    return 0;
}

void addammo( short weapon,struct player_struct *p,short amount)
{
   p->ammo_amount[weapon] += amount;

   if( p->ammo_amount[weapon] > max_ammo_amount[weapon] )
        p->ammo_amount[weapon] = max_ammo_amount[weapon];
}

void addweapon( struct player_struct *p,short weapon)
{
    if ( p->gotweapon[weapon] == 0 ) p->gotweapon[weapon] = 1;

    p->random_club_frame = 0;

    if(p->holster_weapon == 0)
    {
        p->weapon_pos = -1;
        p->last_weapon = p->curr_weapon;
    }
    else
    {
        p->weapon_pos = 10;
        p->holster_weapon = 0;
        p->last_weapon = -1;
    }

    p->kickback_pic = 0;
    p->curr_weapon = weapon;

    switch(weapon)
    {
        case KNEE_WEAPON:
        case TRIPBOMB_WEAPON:
        case HANDREMOTE_WEAPON:
        case HANDBOMB_WEAPON:     break;
        case SHOTGUN_WEAPON:      spritesound(SHOTGUN_COCK,p->i);break;
        case PISTOL_WEAPON:       spritesound(INSERT_CLIP,p->i);break;
                    default:      spritesound(SELECT_WEAPON,p->i);break;
    }
}

void checkavailinven( struct player_struct *p )
{

    if(p->firstaid_amount > 0)
        p->inven_icon = 1;
    else if(p->steroids_amount > 0)
        p->inven_icon = 2;
    else if(p->holoduke_amount > 0)
        p->inven_icon = 3;
    else if(p->jetpack_amount > 0)
        p->inven_icon = 4;
    else if(p->heat_amount > 0)
        p->inven_icon = 5;
    else if(p->scuba_amount > 0)
        p->inven_icon = 6;
    else if(p->boot_amount > 0)
        p->inven_icon = 7;
    else p->inven_icon = 0;
}

void checkavailweapon( struct player_struct *p )
{
    short i,okay,check_shoot,check_bombs;

    if(p->ammo_amount[p->curr_weapon] > 0) return;
    okay = check_shoot = check_bombs = 0;

    switch(p->curr_weapon)
    {
        case PISTOL_WEAPON:
        case CHAINGUN_WEAPON:
        case SHOTGUN_WEAPON:
#ifndef VOLUMEONE
        case FREEZE_WEAPON:
        case CYCLOID_WEAPON:
        case SHRINKER_WEAPON:
#endif
        case RPG_WEAPON:
        case KNEE_WEAPON:
            check_shoot = 1;
            break;
        case HANDBOMB_WEAPON:
        case HANDREMOTE_WEAPON:
#ifndef VOLUMEONE
        case TRIPBOMB_WEAPON:
#endif
            check_bombs = 1;
            break;
    }

    CHECK_SHOOT:
    if(check_shoot)
    {
        for(i = p->curr_weapon+1; i < MAX_WEAPONS;i++)
            switch(i)
            {
                case PISTOL_WEAPON:
                case CHAINGUN_WEAPON:
                case SHOTGUN_WEAPON:
#ifndef VOLUMEONE
                case FREEZE_WEAPON:
                case SHRINKER_WEAPON:
                case CYCLOID_WEAPON:
#endif
                    if ( p->gotweapon[i] && p->ammo_amount[i] > 0 )
                    {
                        okay = i;
                        goto OKAY_HERE;
                    }
                    break;
            }

        for(i = p->curr_weapon-1; i > 0;i--)
            switch(i)
            {
                case PISTOL_WEAPON:
                case CHAINGUN_WEAPON:
                case SHOTGUN_WEAPON:
#ifndef VOLUMEONE
                case FREEZE_WEAPON:
                case CYCLOID_WEAPON:
                case SHRINKER_WEAPON:
#endif
                    if ( p->gotweapon[i] && p->ammo_amount[i] > 0 )
                    {
                        okay = i;
                        goto OKAY_HERE;
                    }
                    break;
            }

        if( p->gotweapon[RPG_WEAPON] && p->ammo_amount[RPG_WEAPON] > 0 )
        {
            okay = RPG_WEAPON;
            goto OKAY_HERE;
        }

        if(check_bombs == 0)
            check_bombs = 1;
        else
        {
            addweapon(p,KNEE_WEAPON);
            return;
        }
    }

    if(check_bombs)
    {
        for(i = p->curr_weapon-1; i > 0;i--)
            switch(i)
            {
                case HANDBOMB_WEAPON:
#ifndef VOLUMEONE
                case TRIPBOMB_WEAPON:
#endif
                    if ( p->gotweapon[i] && p->ammo_amount[i] > 0 )
                    {
                        okay = i;
                        goto OKAY_HERE;
                    }
                    break;
            }

        for(i = p->curr_weapon+1; i < MAX_WEAPONS;i++)
            switch(i)
            {
                case HANDBOMB_WEAPON:
#ifdef VOLUMEONE
                case TRIPBOMB_WEAPON:
#endif
                    if ( p->gotweapon[i] && p->ammo_amount[i] > 0 )
                    {
                        okay = i;
                        goto OKAY_HERE;
                    }
                    break;
            }

        if(check_shoot == 0)
        {
            check_shoot = 1;
            goto CHECK_SHOOT;
        }
        else
        {
            addweapon(p,KNEE_WEAPON);
            return;
        }
    }

    OKAY_HERE:

    if(okay)
    {
        p->last_weapon  = p->curr_weapon;
        p->random_club_frame = 0;
        p->curr_weapon  = okay;
        p->kickback_pic = 0;
        if(p->holster_weapon == 1)
        {
            p->holster_weapon = 0;
            p->weapon_pos = 10;
        }
        else p->weapon_pos   = -1;
        return;
    }
}
 
