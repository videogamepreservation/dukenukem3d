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

#ifndef _clrmap_private_
#define _clrmap_private_
#ifdef __cplusplus
extern "C" {
#endif

#define GRAPHICSDISPLAY 0
typedef struct
   {
   double y,i,q;
   } yiq_t;

typedef struct
   {
   double x,y,z;
   } xyz_t;

typedef struct
   {
   double l,u,v;
   } luv_t;

typedef struct
   {
   double h,s,v;
   } hsv_t;

#define ColorDistortion(dy,di,dq) \
           (  \
              ((dy)*(dy)) +\
              ((di)*(di)) +\
              ((dq)*(dq)) \
           )
/*
#define ColorDistortion(dy,di,dq) \
           (  \
              (0.2125*(dy)*(dy)) +\
              (0.7154*(di)*(di)) +\
              (0.0721*(dq)*(dq)) \
           )
#define IntensityDistortion(dy,di,dq) \
           (  \
              (0.2989*(dy)*(dy)) +\
              (0.5866*(di)*(di)) +\
              (0.1144*(dq)*(dq)) \
           )
#define ColorDistortion(dy,di,dq) \
           (  \
              ((dy)*(dy)) +\
              ((di)*(di)) +\
              ((dq)*(dq)) \
           )
#define ColorDistortion(dy,di,dq) \
           (  \
              (0.2125*0.2125*(dy)*(dy)) +\
              (0.7154*0.7154*(di)*(di)) +\
              (0.0721*0.0721*(dq)*(dq)) \
           )
#define ColorDistortion(dy,di,dq) \
           (  \
              (0.2989*0.2989*(dy)*(dy)) +\
              (0.5866*0.5866*(di)*(di)) +\
              (0.1144*0.1144*(dq)*(dq)) \
           )
*/

#define MAXDISTORTION  (ColorDistortion(255.0,255.0,255.0))

#ifdef __cplusplus
};
#endif
#endif
