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

#ifndef _fixed_public
#define _fixed_public
#ifdef __cplusplus
extern "C" {
#endif

#define FPSHIFT 16
#define FIXED1 (1l << FPSHIFT)

//***************************************************************************/
//
//  Fixed Point Macros
//
//***************************************************************************/

// Get rid of fraction and round up at the same time

#define RoundFixed( fixedval, bits )  \
   ( ( (fixedval) + ( 1 << ( (bits) - 1 ) ) ) >> (bits) )
//int32 RoundFixed ( fixed fixedval, int32 bits );


#define RoundFixed16( fixedval ) RoundFixed( fixedval, 16 )
//sdword RoundFixed16 ( fixed fixedval );

#define MakeFixed( fixedval, bits ) ( (fixedval) << (bits) )
//fixed MakeFixed ( int32 val, int32 bits );

#define MakeFixed16( fixedval ) MakeFixed ( fixedval, 16 )
//fixed MakeFixed16 ( int32 val );


//***************************************************************************
//
//  Fixed Point Functions
//
//***************************************************************************

fixed FixedMul(fixed a, fixed b);
fixed FixedMulShift(fixed a, fixed b, fixed shift);
fixed FixedDiv2(fixed a, fixed b);
fixed FixedDiv(fixed a, fixed b);
fixed FixedDiv24(fixed a, fixed b);
fixed fixedmul24(fixed a, fixed b);
fixed FixedScale(fixed orig, fixed factor, fixed divisor);
fixed FixedSqrtLP(fixed n);  // Low  Precision (8.8)
fixed FixedSqrtHP(fixed n);  // High Precision (8.16)
fixed FixedDivShift(fixed a, fixed b, fixed c);

#if defined(__FLAT__) || defined(__NT__)

#define FixedDiv( a, b ) FixedDiv2( a, b )

/*
#pragma aux FixedMul =               \
        "imul ebx",                  \
        "adc  edx,0h"                \
        "shrd eax,edx,16"            \
        parm    [eax] [ebx]          \
        value   [eax]                \
        modify exact [eax edx]
*/

#pragma aux FixedMul =               \
        "imul ebx",                  \
        "add  eax, 8000h"            \
        "adc  edx,0h"                \
        "shrd eax,edx,16"            \
        parm    [eax] [ebx]          \
        value   [eax]                \
        modify exact [eax edx]


#pragma aux FixedMulShift =          \
        "imul ebx",                  \
        "shrd eax,edx,cl"            \
        parm    [eax] [ebx] [ecx]    \
        value   [eax]                \
        modify exact [eax edx]

#pragma aux FixedDiv2 =              \
        "cdq",                       \
        "shld edx,eax,16",           \
        "sal eax,16",                \
        "idiv ebx"                   \
        parm    [eax] [ebx]          \
        value   [eax]                \
        modify exact [eax edx]

#pragma aux FixedDivShift =              \
        "cdq",                       \
        "shld edx,eax,cl",           \
        "sal eax,cl",                \
        "idiv ebx"                   \
        parm    [eax] [ebx] [ecx]    \
        value   [eax]                \
        modify exact [eax edx]


#pragma aux FixedDiv24 =              \
        "cdq",                       \
        "shld edx,eax,24",           \
        "sal eax,24",                \
        "idiv ebx"                   \
        parm    [eax] [ebx]          \
        value   [eax]                \
        modify exact [eax edx]


#pragma aux FixedScale =             \
        "imul ebx",                  \
        "idiv ecx"                   \
        parm    [eax] [ebx] [ecx]    \
        value   [eax]                \
        modify exact [eax edx]




/*
FUNCTION:
    Fixed32 FixedSqrtHP(Fixed32 n);
DESCRIPTION:
    This does a high-precision square root of a Fixed32.  It has
    8.16 bit accuracy.  For more speed use FixedSqrtLP().


FUNCTION:
    Fixed32 FixedSqrtLP(Fixed32 n);
DESCRIPTION:
    This does a low-precision square root of a Fixed32.  It has
    8.8 bit accuracy.  For more accuracy use FixedSqrtHP().
*/


#pragma aux FixedSqrtLP =            \
    "         xor eax, eax"          \
    "         mov ebx, 40000000h"    \
    "sqrtLP1: mov edx, ecx"          \
    "         sub edx, ebx"          \
    "         jl  sqrtLP2"           \
    "         sub edx, eax"          \
    "         jl  sqrtLP2"           \
    "         mov ecx,edx"           \
    "         shr eax, 1"            \
    "         or  eax, ebx"          \
    "         shr ebx, 2"            \
    "         jnz sqrtLP1"           \
    "         shl eax, 8"            \
    "         jmp sqrtLP3"           \
    "sqrtLP2: shr eax, 1"            \
    "         shr ebx, 2"            \
    "         jnz sqrtLP1"           \
    "         shl eax, 8"            \
    "sqrtLP3: nop"                   \
    parm caller [ecx]                \
    value [eax]                      \
    modify [eax ebx ecx edx];


#pragma aux FixedSqrtHP =            \
    "         xor eax, eax"          \
    "         mov ebx, 40000000h"    \
    "sqrtHP1: mov edx, ecx"          \
    "         sub edx, ebx"          \
    "         jb  sqrtHP2"           \
    "         sub edx, eax"          \
    "         jb  sqrtHP2"           \
    "         mov ecx,edx"           \
    "         shr eax, 1"            \
    "         or  eax, ebx"          \
    "         shr ebx, 2"            \
    "         jnz sqrtHP1"           \
    "         jz  sqrtHP5"           \
    "sqrtHP2: shr eax, 1"            \
    "         shr ebx, 2"            \
    "         jnz sqrtHP1"           \
    "sqrtHP5: mov ebx, 00004000h"    \
    "         shl eax, 16"           \
    "         shl ecx, 16"           \
    "sqrtHP3: mov edx, ecx"          \
    "         sub edx, ebx"          \
    "         jb  sqrtHP4"           \
    "         sub edx, eax"          \
    "         jb  sqrtHP4"           \
    "         mov ecx, edx"          \
    "         shr eax, 1"            \
    "         or  eax, ebx"          \
    "         shr ebx, 2"            \
    "         jnz sqrtHP3"           \
    "         jmp sqrtHP6"           \
    "sqrtHP4: shr eax, 1"            \
    "         shr ebx, 2"            \
    "         jnz sqrtHP3"           \
    "sqrtHP6: nop"                   \
    parm caller [ecx]                \
    value [eax]                      \
    modify [eax ebx ecx edx];

#endif  /* __FLAT__ || __NT__ */

#ifdef __NeXT__

#include <math.h>   // for sqrt

fixed FixedDiv(fixed a, fixed b);
fixed FixedDivShift(fixed a, fixed b, fixed c);
fixed FixedDiv24(fixed a, fixed b);
fixed fixedmul24(fixed a, fixed b);
fixed FixedMul(fixed a, fixed b);
fixed FixedMulShift(fixed a, fixed b, fixed c);

//#define FixedMulShift( a, b, shift ) \
//   ( ( fixed )( ( ( long long )( a ) * ( long long )( b ) + \
//        ( ( long long )1 << ( ( shift ) - 1 ) ) ) >> ( shift ) ) )


#define FixedDiv2( a, b ) FixedDiv( a, b )

#define FixedScale( orig, factor, divisor ) \
   ( ( fixed )( ( ( long long )( orig ) * \
        ( long long )( factor ) ) / ( long long )( divisor ) ) )

#define FixedSqrtHP( n ) \
   ( ( fixed )( sqrt( ( double )( n )/ FIXED1 ) * FIXED1 ) )

#define FixedSqrtLP( n ) \
   ( FixedSqrtHP( n ) )

#endif   /* __NeXT__ */

#if    defined(__MSDOS__) && !defined(__FLAT__)

#include <math.h>   // for sqrt

fixed FixedDiv(fixed a, fixed b);
fixed FixedDiv24(fixed a, fixed b);
fixed fixedmul24(fixed a, fixed b);
fixed FixedMul(fixed a, fixed b);

#define FixedMulShift( a, b, shift ) \
   ( ( fixed )( ( ( long )( a ) * ( long )( b ) + \
        ( ( long )1 << ( ( shift ) - 1 ) ) ) >> ( shift ) ) )


#define FixedDiv2( a, b ) FixedDiv( a, b )

#define FixedScale( orig, factor, divisor ) \
   ( ( fixed )( ( ( long )( orig ) * \
        ( long )( factor ) ) / ( long )( divisor ) ) )

#define FixedSqrtHP( n ) \
   ( ( fixed )( sqrt( ( double )( n )/ FIXED1 ) * FIXED1 ) )

#define FixedSqrtLP( n ) \
   ( FixedSqrtHP( n ) )

#endif   /* __286__ */

#ifdef __cplusplus
};
#endif
#endif  /* _fixed_public */
