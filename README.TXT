===================================================================
Duke Nukem 3D (v1.5 CD Version) Source Code Release - April 1, 2003
===================================================================

LEGAL STUFF
-----------

"Duke Nukem" is a registered trademark of Apogee Software, Ltd. (a.k.a. 3D Realms).
"Duke Nukem 3D" copyright 1996 - 2003 3D Realms.  All trademarks and copyrights reserved.

This is the complete source code for Duke Nukem 3D version 1.5, buildable as detailed in the next section.

The code is licensed under the terms of the GPL (gnu public license).  You should read the entire license (filename "GNU.TXT" in this archive), so you understand your rights and what you can and cannot do with the source code from this release.

All of the Duke Nukem 3D data files remain copyrighted and licensed by 3D Realms under the original terms.  You cannot redistribute our data or data files from the original game.  You can use our data for personal entertainment or educational purposes.  If you need the data, you can order a Duke Nukem 3D CD from our store on our website.

Please do not contact us for possible commercial exploitation of Duke Nukem 3D -- we will not be interested.

Please note that this is being released without any kind of support from Apogee Software, Ltd / 3D Realms Entertainment.  We cannot help in getting this running and we do not guarantee that you will be able to get it to work, nor do we guarantee that it won't blow up your computer if you do try and use it.  Use at your own risk.

SPECIAL THANKS
--------------

First, we'd like to thank all the fans of 3D Realms and Duke Nukem.  We wouldn't be where we are without you and without your support.

Second, we'd like to thank Charlie Wiederhold for putting this release together.  We had decided some time ago to release the source code, but people are busy and it always seemed to be low priority.  A couple of weeks ago, Charlie got a source archive from Scott, and came back a few days later with it all compiling.

Thanks also to Ken Silverman and Jim Dose for allowing us to include some of their source in this build, so we have it all in one archive.

Finally, we'd like to thank the whole Duke Nukem Forever team.  These guys are all working incredibly hard on the next installment of Duke Nukem.

Enjoy the source code.  We can't wait to see what people do with it.  And we really look forward to being able to play the game under XP, with sound, internet play, hardware acceleration and whatever else you're clever enough to put in :)

George Broussard & Scott Miller
3D Realms

======================================================
HOW TO COMPILE THE SOURCE CODE (by Charlie Wiederhold)
======================================================

This source code release was compiled on March 20th, 2003 using the materials in this archive.

Duke was originally written with the Watcom 10.0 C/C++ compiler. This code has been updated to allow it to compile under the free version of the Watcom compiler available from their webpage (http://www.openwatcom.org).

It was compiled under both Open Watcom 1.0 and Watcom 11.0c for Windows.  Both are free to download, and I didn't notice much of a difference between the two when using them. This means, thankfully, that anybody can work with this code right away without trying to find an out of production compiler or wait for someone to port it to other modern compilers.

Step 1) Install Watcom C/C++ onto your system.

Step 2) When you install, make sure that you select DOS 32 Bit as one of your target environments to build for.

Step 3) Choose a place you want to work from and unzip the contents of this .ZIP file into that directory.

From here you have two choices on how to work. You can use the command line compiler that comes with Watcom, or you can use the IDE (basically the development studio that manages your files, options for compiling, editing files, debugger, etc). Whichever one you choose depends on what you are comfortable with.

IF YOU USE THE COMMAND LINE COMPILER
------------------------------------

- In a Command Prompt, go into the Source directory where you should find a MAKEFILE. and a MAKEFILE.LNK file.
- At the Command Prompt type "wmake" or "wmake makefile" without the quotes.
- This should compile and create a DUKE3D.EXE file which you can then copy into the directory with your Duke 3D data and run.

IF YOU USE THE IDE
------------------

- Start up the Watcom IDE and go to File -> Open Project.
- Find the directory where you've unzipped the Source files into, and you should see a DUKE3D.WPJ, select this and hit "OK".
- Click the "Make Current Target" button, or press "F4" or go to the menu Targets -> Make. You'll see lots of Warnings as it compiles, that's normal.
- This will create a DUKE3D.EXE file in the same directory where the DUKE3D.WPJ was located, which can then be copied in the directory with your Duke 3D data and run.

ONCE YOU HAVE DUKE3D.EXE COMPILED
---------------------------------

- If you own Duke 3D version 1.5, you are set... simply copy your new .EXE into the directory and run it.
- If you own Duke 3D version 1.4 or 1.3d, follow the same steps below except don't download the shareware data.
- If you own neither versions: Download the shareware version of Duke 3D from http://www.3drealms.com (go to Downloads).
- In the directory you have Duke 3D installed now, copy the four files in the TESTDATA directory into your Duke 3D directory. Now you should be able to play the game well enough to test, though unfortunately there will still be some minor issues. Your best results will come from owning a copy of Duke 3D version 1.5, which can still be purchased from the 3D Realms website.

This should be enough to get you started. Unfortunately nobody at 3D Realms will be able to devote the time to answer any support questions if you do encounter problems. However, go to http://www.3drealms.com/forums.html and you will find people discussing the source code in the Duke 3D Source category and able to answer questions. I will try to answer extremely common questions there shortly after the release, but I promise, within a very short time the community will outgrow my knowledge/understanding of this source and be better suited to answer any questions itself.

MISC NOTES ABOUT THIS RELEASE
-----------------------------

- All changes I made from the original are indicated by a "// CTW" style comment. In all but one case I commented out the original code so that it would still be there. I made as few changes as possible since the fun for people is working on the Duke 3D original code, not my personal rewrite/interpretation.

- Unfortunately the source to the SETUP.EXE and SETMAIN.EXE programs used for creating setup configuration files for Duke 3D is nowhere to be found, so if you need a setup utility to create custom .CFG files you'll need to write your own. If we find the source code we'll update this release to include that as well. The code to read .CFG files is within the Duke 3D CONFIG.C source itself, so it gives you something to start with.

- This source includes the Build Engine data (.OBJ files) needed for compiling. The Build Engine is a seperate entity from Duke 3D, though Duke 3D is built upon the Build Engine.

- The BUILDENGINE.ZIP file contains all of the data you need from Ken Silverman's released version to compile the Build Engine itself. Instructions for doing this are contained within that file. I have tested this with the free versions of Watcom and it works. More information about this code at Ken's webpage here: http://www.advsys.net/ken/buildsrc/default.htm

- The AUDIOLIB.ZIP file contains all of the data you need from Jim Dose's sound engine that was used in both Rise of the Triad and Duke 3D to compile the actual Audio Library itself. Instructions for doing this are contained in that file. I have not tested this personally to see if it compiles under the free version of Watcom.

- Sound will be sketchy if you are on any modern system. It does work, but only as well as the original Duke 3D's does.

- Networking works. Scott Miller and I fired up a quick game and it worked fine. The only catch is you need to disable sound for this to work in modern systems. No testing over a modem was done.

- The original .DMO demo files do not work. However recording new ones and playing those back does work, sometimes. I've disabled playback of demos in the source for stability when people first try to run their compiled version. It shouldn't be too hard to find how to re-enable it for those who want to go in and fix it.

- If you would like to play Duke 3D in high res Vesa modes instead of 320x200, download Ken's utility here: http://www.advsys.net/ken/build.htm

- The files in the EXTRAS folder are there purely for curiosity and educational purposes. Things like Allen Blum's experiments with room over room can be found in there, as well as older versions of the source files that ultimately made it into the game. None of these are necessary and are purely, well, extra.

- Duke 3D used DOS/4GW for it's DOS Extender. Watcom 1.0/11.0c comes with a couple free DOS Extenders, however you will need to bind it to the extender in order to distribute the EXE you create to other computers where it wasn't compiled. How you do this depends on which extender you choose to use. I trust that once you get to the point of distributing an EXE you can figure out how to bind it to the extender. Since DOS/4GW was a commercial licensed product, we can't distribute the resources we used to do this.

- All references to TEN (Total Entertainment Network) have been commented out of the version that will be compiled, but left in for the curious to look at.

- This page was an invaluable learning tool for me when working on preparing the source code for release: http://www.clipx.net/ ... The actual information I used can be found here: http://www.clipx.net/ng/zcpp/index.php

- I started working on this having no knowledge of Watcom, DOS based C programming, etc. and got it up and going within a night (and a half...ish) after work hours. So if you have a basic understanding of programming, you shouldn't have much trouble getting this up and running.

- The first things I think everyone would like to see done is an update to the sound engine and an update to the networking to allow play over the internet. But there is no end to the changes people can make, so everyone at 3D Realms looks forward to what people will do now that the source code is finally out there. It's been a long time coming (too long probably), but here you go, have fun!

Charlie Wiederhold
3D Realms
