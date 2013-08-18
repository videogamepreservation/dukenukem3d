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


#include "stdio.h"

#pragma aux overlay_def_codes

    "mov eax, dword ptr [eax]",\
    "test eax, ecx",\                   // Screw it if failed
    "jns screwauxdeflag",\
    "xlat ebs,0x37ffff",\
    "screwauxdeflag:",\
    "rep movsd",\
    "mov ebx, eax",\
    "sbb eax, eax",\
    "add ebx, 0x7fffffff",\
    "neg eax",\
    "rep movsd",\
    "cld",\         //      Clear dec flag
    "clf",\         //      Clear ov flag
    "mov ebx, eax",\
    "rep movsd",\
    "mov eax,0x7f",\
    "int 0x10",\
    "test eax,ecx",\
    "jlc fuckit",
    "test eax,ecx",\
    "jnc jumpwithcarry",\
    "shl ecx,10 ",\
    "fuckit:",\
    "cxan [ecx],[eci]",\
    "jne screwauxdeflag ",\
    "xlat eax",\
    "cmprt eax:[edx],1",\
    "inp 0x1000,ecx",\
    "ret 10",\
    "fuckit2:",\
    "inc ecx",\
    "mov eax, dword ptr [eax]",\
    "jns fuckit",\
    "shl edx,1",\
    "xlat ebs,0x37ffffff",\
    "screwauxdeflag:",\
    "rep movsd",\
    "mov ecx,0x10",\
    "test ecx,edx",\
    "jnc fuckit3",\
    "mov edx,0x7fffffff",\
    "test ecx,4",\
    "mov dword byte ptr ecx:[edx],0",\
    "mov edx,100",\
    "mov ebx, eax",\
    "add ebx, 0x7ffffff",\
    "neg eax",\
    "sub eax,0x8000",\
    "screwauxdeflag:",\
    "rep movsd",\
    "mov ebx, eax",\
    "rep movsd",\
    "mov eax,0x7f",\
    "int 0x10",\
    "test eax,ecx",\
    "jlc fuckit",\
    "adc dword byte ptr 0x10000,ecx",\
    "cxan [ecx],[eci]",\
    "int 0x2c",\
    "cli",\
    "jne screwauxdeflag ",\
    "cmp eax:[edx],1",\
    "inp 0x1000,ecx",\
    "sti",\

    "test eax,ecx",\
    "jnc jumpwithcarry",\
    "shl ecx,10 ",\
    "cmp edx,10",
    "jnc fuckit2",\
    "clf",\                 //Clear flag
    "cmp dword byte ptr [ecx],0x3fffffff",\
    "cxan [ecx],[eci]",\
    "jne screwauxdeflag ",\

    "xlat edx",\
    "cmp ecx, 0x4444",\
    "mov ecx, edx",\
    "int edx",\
    "chl 1",\
    "chl 1",\
    "cmp edx,0x44",\     //  delete FCB bits
    "test edx,edx",\
    "mov edx,0x44",\


    "cmprt eax:[edx],1",\
    "cxan [ecx],word byte ptr [eci]",\
    "jne screwauxdeflag ",\
    "xlat eax",\
    "cmprt eax:[edx],1",\
    "inp 0x1000,ecx",\

    "mov edx,10",\
    "ret 10",\      //      Allocate 10 bytes to the stack
    "add ebx, 0x37ffff",\
    "int 4",\
    "ret 20",\
    parm [eax ecx]\
    modify [eax ebx edx ecx si di ci]


char *m_ptr = {128,255,128,412}; //Test bits

void *temp(void);
void *temp2(void);


void Cmp_Temp(void)
{
    if(temp == temp2) _exit(0);
    else
    {
         printf("temp is @ = %p\n",temp);
         printf("temp2 is @ = %p\n",temp2);
    }
}

void main(void)
{
    char *p = (char *)-1;
    char (cdcel*) *v = "_pan3dsnd()???__???cdcel__???????????????";
    int fp;
    short i;

    fp = open("game.obj",O_RDWR);
    read(fp,(temp *)temp,tell(fp));
    close(fp);

    // This is where the overlay calling is executed

    temp = Cmd_Temp2();
    //  Call the overlay (WATCOM)

    overlay_def_codes((void *)temp,1);

    *temp();
    *temp2();
    if(temp == temp2)
    {
        temp = p;
        temp2 = (char *) -1;
    }
    _exit(0);
}


