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

//STEP 1: If you want the transluscent fade out effects to work, you will
//   need to CHANGE ALLLLLLLLLLLLLLL GAMETEXTS TO HAVE THE EXTRA DABITS
//   PARAMETER!!!!!!  Call GAMETEXT with dabits as 2+8+16 everywhere
//   except for the new transluscence fade-out stuff I did

int gametext(int x,int y,char *t,char s,short dabits)
	...
	rotatesprite(x<<16,y<<16,65536L,0,ac,s,0,dabits,0,0,xdim-1,ydim-1);


//STEP 2: Remove user_quote_time,user_quote,typebuflen,typebuf from
//   DUKE3D.H&GLOBAL.C and stick this in GAME.C:
#define MAXUSERQUOTES 4
long quotebot, quotebotgoal;
short user_quote_time[MAXUSERQUOTES];
char user_quote[MAXUSERQUOTES][128];
char typebuflen,typebuf[41];

adduserquote(char *daquote)
{
	long i;

	for(i=MAXUSERQUOTES-1;i>0;i--)
	{
		strcpy(user_quote[i],user_quote[i-1]);
		user_quote_time[i] = user_quote_time[i-1];
	}
	strcpy(user_quote[0],daquote);
	user_quote_time[0] = 180;
	pub = NUMPAGES;
}

//STEP 3: In GAME.C, make all references of user_quote call the
//   adduserquote function instead, for example, look at this case:

Getpackets:
				 case 4:
					 strcpy(recbuf,packbuf+1);
					 recbuf[packbufleng-1] = 0;

					 adduserquote(recbuf);
					 sound(EXITMENUSOUND);

					 pus = NUMPAGES;
					 pub = NUMPAGES;

					 break;

  //FULLY REPLACE THIS FUNCTION
void operatefta(void)
{
	 long i, j, k;

	 if(ud.screen_size > 0) j = 200-45; else j = 200-8;
	 quotebot = min(quotebot,j);
	 quotebotgoal = min(quotebotgoal,j);
	 if(ps[myconnectindex].gm&MODE_TYPE) j -= 8;
	 quotebotgoal = j; j = quotebot;
	 for(i=0;i<MAXUSERQUOTES;i++)
	 {
		 k = user_quote_time[i]; if (k <= 0) break;

			  if (k > 16) gametext(320>>1,j,user_quote[i],0,2+8+16);
		 else if (k > 8) gametext(320>>1,j,user_quote[i],0,2+8+16+1);
						else gametext(320>>1,j,user_quote[i],0,2+8+16+1+32);
		 j -= 8;
	 }

	 if (ps[screenpeek].fta <= 1) return;

	 if (ud.coop != 1 && ud.screen_size > 0 && ud.multimode > 1)
	 {
		 j = 0; k = 8;
		 for(i=connecthead;i>=0;i=connectpoint2[i])
			 if (i > j) j = i;

		 if (j >= 4 && j <= 8) k += 8;
		 else if (j > 8 && j <= 12) k += 16;
		 else if (j > 12) k += 24;
	 }
	 else k = 0;

	 if (ps[screenpeek].ftq == 115 || ps[screenpeek].ftq == 116)
	 {
		 k = quotebot;
		 for(i=0;i<MAXUSERQUOTES;i++)
		 {
			 if (user_quote_time[i] <= 0) break;
			 k -= 8;
		 }
		 k -= 4;
	 }

	 j = ps[screenpeek].fta;
		  if (j > 16) gametext(320>>1,k,fta_quotes[ps[screenpeek].ftq],0,2+8+16);
	 else if (j > 8) gametext(320>>1,k,fta_quotes[ps[screenpeek].ftq],0,2+8+16+1);
					else gametext(320>>1,k,fta_quotes[ps[screenpeek].ftq],0,2+8+16+1+32);
}

	//FULLY REPLACE THIS FUNCTION
void typemode(void)
{
	 short ch, hitstate, i, j;

	 if( ps[myconnectindex].gm&MODE_SENDTOWHOM )
	 {
		  if(sendmessagecommand != -1 || ud.multimode < 3)
		  {
				tempbuf[0] = 4;
				tempbuf[1] = 0;
				recbuf[0]  = 0;

				if(ud.multimode < 3)
					 sendmessagecommand = 2;

				strcat(recbuf,ud.user_name[myconnectindex]);
				strcat(recbuf,": ");
				strcat(recbuf,typebuf);
				j = strlen(recbuf);
				recbuf[j] = 0;
				strcat(tempbuf+1,recbuf);

				if(sendmessagecommand >= ud.multimode)
				{
					 for(ch=connecthead;ch >= 0;ch=connectpoint2[ch])
						  if (ch != myconnectindex)
								sendpacket(ch,tempbuf,j+1);

					 adduserquote(recbuf);
					 quotebot += 8;
					 quotebotgoal = quotebot;
				}
				else if(sendmessagecommand >= 0)
					 sendpacket(sendmessagecommand,tempbuf,j+1);

				sendmessagecommand = -1;
				ps[myconnectindex].gm &= ~(MODE_TYPE|MODE_SENDTOWHOM);
		  }
		  else if(sendmessagecommand == -1)
		  {
				j = 50;
				gametext(320>>1,j,"SEND MESSAGE TO...",0,2+8+16); j += 8;
				for(i=0;i<ud.multimode;i++)
				{
					 if (i == myconnectindex)
					 {
						 minitextshade((320>>1)-40+1,j+1,"A/ENTER - ALL",26,0,2+8+16);
						 minitext((320>>1)-40,j,"A/ENTER - ALL",0,2+8+16); j += 7;
					 }
					 else
					 {
						 sprintf(buf,"      %ld - %s",i+1,ud.user_name[i]);
						 minitextshade((320>>1)-40-6+1,j+1,buf,26,0,2+8+16);
						 minitext((320>>1)-40-6,j,buf,0,2+8+16); j += 7;
					 }
				}
				minitextshade((320>>1)-40-4+1,j+1,"    ESC - Abort",26,0,2+8+16);
				minitext((320>>1)-40-4,j,"    ESC - Abort",0,2+8+16); j += 7;

				//sprintf(buf,"PRESS 1-%ld FOR INDIVIDUAL PLAYER.",ud.multimode);
				//gametext(320>>1,j,buf,0,2+8+16); j += 8;
				//gametext(320>>1,j,"'A' OR 'ENTER' FOR ALL PLAYERS",0,2+8+16); j += 8;
				//gametext(320>>1,j,"ESC ABORTS",0,2+8+16); j += 8;

				if (ud.screen_size > 0) j = 200-45; else j = 200-8;
				gametext(320>>1,j,typebuf,0,2+8+16);

				if( KB_KeyWaiting() )
				{
					 i = KB_GetCh();
					 if(i == 'A' || i == 'a' || i == 13)
						  sendmessagecommand = ud.multimode;
					 else if(i >= '1' || i <= (ud.multimode + '1') )
						  sendmessagecommand = i - '1';
					 else
					 {
						  ps[myconnectindex].gm &= ~(MODE_TYPE|MODE_SENDTOWHOM);
						  sendmessagecommand = -1;
						  typebuf[0] = 0;
					 }

					 KB_ClearKeyDown(sc_1);
					 KB_ClearKeyDown(sc_2);
					 KB_ClearKeyDown(sc_3);
					 KB_ClearKeyDown(sc_4);
					 KB_ClearKeyDown(sc_5);
					 KB_ClearKeyDown(sc_6);
					 KB_ClearKeyDown(sc_7);
					 KB_ClearKeyDown(sc_8);
					 KB_ClearKeyDown(sc_A);
					 KB_ClearKeyDown(sc_Escape);
					 KB_ClearKeyDown(sc_Enter);
				}
		  }
	 }
	 else
	 {
		  if(ud.screen_size > 0) j = 200-45; else j = 200-8;
		  hitstate = strget(320>>1,j,typebuf,30,1);

		  if(hitstate == 1)
		  {
				KB_ClearKeyDown(sc_Enter);
				ps[myconnectindex].gm |= MODE_SENDTOWHOM;
		  }
		  else if(hitstate == -1)
				ps[myconnectindex].gm &= ~(MODE_TYPE|MODE_SENDTOWHOM);
		  else pub = NUMPAGES;
	 }
}

//domovethings - in middle, where you used to decrement user_quote_time,
//replace with this:
	 for(i=0;i<MAXUSERQUOTES;i++)
		 if (user_quote_time[i])
		 {
			 user_quote_time[i]--;
			 if (!user_quote_time[i]) pub = NUMPAGES;
		 }
	 if ((klabs(quotebotgoal-quotebot) <= 16) && (ud.screen_size <= 8))
		 quotebot += ksgn(quotebotgoal-quotebot);
	 else
		 quotebot = quotebotgoal;
