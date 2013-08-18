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
#include <stdlib.h>
#include <string.h>
#include "duke3d.h"


#ifdef RBG_DEBUG


#ifdef RBG_DEBUG
   char  rbg_names[MAXTILES][17];
   long  rbg_loop_count = 0L;
   FILE *rbg_outfile;
   char  rbg_outname[13] = "debug.dat";
   void  rbg_loadnames( void );
   void  rbg_debug( void );
#endif


void rbg_debug()

{
 int  xyz, abc;

 if ( (rbg_loop_count%200L) == 0L )
    {
     if ( rbg_loop_count > 0L )
        fclose( rbg_outfile );

     if ( ( rbg_outfile = fopen( rbg_outname, "w" ) ) == NULL )
         printf( "ERROR: Could not open \"%s\" for writing.\n",
                 rbg_outname );
    }

 rbg_loop_count++;

 xyz = headspritestat[5];
 while ( xyz >= 0 )
    {
     if ( xyz == headspritestat[5] )
        fprintf( rbg_outfile, "%6ld ", rbg_loop_count );
     else
        fprintf( rbg_outfile, "      " );
     fprintf( rbg_outfile, "stat[%3d]:%5d |%s|\n", sprite[sprite[xyz].owner].picnum,
             sprite[xyz].picnum, rbg_names[sprite[xyz].picnum] );
     xyz = nextspritestat[xyz];
    }
}











void rbg_loadnames()
{
    char buffer[80], firstch, ch;
    short int fil, i, num, buffercnt;

    if ((fil = open("names.h",O_BINARY|O_RDWR,S_IREAD)) == -1)
    {
        //skipnames = 1;
        return;
    }
    do
    {
        i = read(fil,&firstch,1);
    }
    while ((firstch != '#') && (i > 0));

    while ((firstch == '#') || (firstch == '/'))
    {
        do
        {
            read(fil,&ch,1);
        }
        while (ch > 32);

        buffercnt = 0;
        do
        {
            read(fil,&ch,1);
            if (ch > 32)
                buffer[buffercnt++] = ch;
        }
        while (ch > 32);

        num = 0;
        do
        {
            read(fil,&ch,1);
            if ((ch >= 48) && (ch <= 57))
                num = num*10+(ch-48);
        }
        while (ch != 13);
        for(i=0;i<buffercnt;i++)
            rbg_names[num][i] = buffer[i];
        rbg_names[num][buffercnt] = 0;

        if (read(fil,&firstch,1) <= 0)
            firstch = 0;
        if (firstch == 10)
            read(fil,&firstch,1);
    }
    close(fil);
    return;
}




#endif
