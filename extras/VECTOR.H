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

#ifndef __vector_h
#define __vector_h
#ifdef __cplusplus
extern "C" {
#endif


typedef struct vect
   {
   appfloat x,y,z;
   }fpvector_t;

#define EQTOL 0.001
#define ISAPPXEQ(a,b) (fabs((a)-(b)) < EQTOL)

#define FP_VECTORS_EQUAL(v1,v2) (ISAPPXEQ((v1)->x,(v2)->x) && ISAPPXEQ((v1)->y,(v2)->y) && ISAPPXEQ((v1)->z,(v2)->z))

#define ASSIGN_VECTOR(v,nx,ny,nz) \
   {                              \
   (v).x = (nx);                    \
   (v).y = (ny);                    \
   (v).z = (nz);                    \
   }

//#define FP_2x2Det(x1,y1,x2,y2) ((x1)*(y2) - (x2)*(y1))


#define FP_DotProduct(v1,v2) \
   (((v1)->x*(v2)->x) + ((v1)->y*(v2)->y) + ((v1)->z*(v2)->z))


appfloat FP_VectorLength(fpvector_t*v);
appfloat FP_TripleProduct(fpvector_t*vec1,fpvector_t *vec2, fpvector_t *vec3);
appfloat FP_2x2Det(appfloat x1, appfloat y1, appfloat x2, appfloat y2);
void FP_VectorAdd(fpvector_t *result, fpvector_t *v1, fpvector_t*v2);
void FP_VectorSub(fpvector_t *result, fpvector_t *v1, fpvector_t*v2);
void FP_ScaleVector(fpvector_t *result, fpvector_t *v, appfloat scale);
void FP_NormalizeVector(fpvector_t *v);
void FP_DecomposeVector(fpvector_t*,fpvector_t*,appfloat *);
void FP_CrossProduct(fpvector_t *result, fpvector_t *v1, fpvector_t*v2);
fpvector_t FP_RotateVAV(fpvector_t *vector, fpvector_t* rotvector, appfloat cosang,
                        appfloat sinang);

//extern fpvector_t _IVEC,_JVEC,_KVEC;

#ifdef __cplusplus
};
#endif
#endif
