// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

All .C files in this directory are separate programs. Do not link them to
each other.

To best view my source, set your "Tab Stops" to 3!

-------------------  Brief description of utility files  --------------------

BACKMAP6.C: Converts Build MAP format from 7 to 6

BACKMAP5.C: Converts Build MAP format from 6 to 5

CONVMAP6.C: Converts Build MAP format from 5 to 6

CONVMAP7.C: Converts Build MAP format from 6 to 7

TRANSPAL.C: Palette generation utility
   NOTE: This program requires PRAGMAS.H. It is located inside SRC.ZIP.
   NOTE: This program requires PALETTE.DAT to be in the directory. You
         can extract it from STUFF.DAT by using my KEXTRACT utility.

EDITART.C: ART collection utility and editor
   NOTE: This program requires PALETTE.DAT, TABLES.DAT to be in the
         directory you run it from. You can extract these files from
         STUFF.DAT by using my KEXTRACT utility.

KGROUP.C: Use to combine files into a .GRP file (Note: "STUFF.DAT" in my
   game directory is actually a .GRP file even though it has a different
   file extension)

KEXTRACT.C: Use to extract files from a .GRP file

WAD2MAP.C: Doom to Build converter utility. This converts the maps.
   Note: this program requires "pragmas.h". It is located inside SRC.ZIP.

WAD2ART.C: Doom to Build converter utility. This converts the textures.

SETUP.C: The Build setup program. This code should also work in a 16-bit
   compiler. (That's how I got SETUP.EXE so small!)

-----------------------------------------------------------------------------
-Ken S. (web page: http://www.advsys.net/ken)
