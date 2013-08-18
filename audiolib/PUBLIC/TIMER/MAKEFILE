.OPTIMIZE

lib_dir		      = ..
prg_dir           = .
obj_dir           = $(prg_dir)\obj
main_dir          = $(prg_dir)\source
header_dir        = $(main_dir);$(lib_dir)\INCLUDE
tasm_include_dir  = /i$(main_dir) /i$(lib_dir)\source

lib_files         = $(lib_dir)\audio_wf.lib

.h   :              $(header_dir)
.c   :              $(header_dir)
.asm :              $(header_dir)
.obj :              $(obj_dir)

object_files        = timer.obj usrhooks.obj

tasm_options        = /zi /p $(tasm_include_dir)
#wcc_options         = /w4 /d2 /zm /i=$(header_dir)
wcc_options         = /w4 /d1 /omaxnet /zp4 /5r /zm /i=$(header_dir)
wlink_options       =

timer.exe: $(object_files) $(lib_files) makefile
   %create temp.lnk
   %append temp.lnk option quiet
   %append temp.lnk option eliminate
   %append temp.lnk system dos4g
   %append temp.lnk name $^&
   %append temp.lnk debug all
   %append temp.lnk libfile $(lib_files)
   for %i in ($(object_files)) do %append temp.lnk file $(obj_dir)\%i
   wlink $(wlink_options) @temp.lnk
   #wc2dbg demo.exe
   del temp.lnk

.asm.obj :
   tasm $(tasm_options) $[* $(obj_dir)\$^&

.c.obj :
   wcc386 $(wcc_options) $[* /fo=$(obj_dir)\$^&

demo.obj     : demo.c task_man.h

usrhooks.obj : usrhooks.c usrhooks.h
