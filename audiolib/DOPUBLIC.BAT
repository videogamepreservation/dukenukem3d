call wmake.bat
copy audio_wf.lib      public
copy notes.txt         public
copy midi.txt          public
copy notes.txt         public
copy audio_wf.lib      public\include
copy source\fx_man.h   public\include
copy source\music.h    public\include
copy source\sndcards.h public\include
copy source\task_man.h public\include
cd public\timer
wmake
cd ..\..\public\ps
wmake
cd ..\..\public\pm
wmake
cd ..\..
