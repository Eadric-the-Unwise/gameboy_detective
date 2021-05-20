del main.gb
C:\c_code\projects\detective\detective_metasprite\build\gb_map_converter\gb_map_converter.exe -i ..\maps\dk_bkg1.png -n dk_bkg1 -o ..\maps

C:\c_code\gbdk\bin\lcc.exe -Wl-j -Wm-yS -o main.gb ..\main.c ..\tiles\tile_detectivewalk.c ..\tiles\cig_shine.c ..\tiles\smoke.c ..\maps\dk_bkg1_map.c ..\maps\dk_bkg1_tiles.c
@REM -Wl-j -Wm-yS = force generating the *.sym file, so you will see names in the BGB debugger.

@REM -n level_1 is the name you want the c/h files and variables to use, it defaults to the png file name
@REM -o ..\maps is the output folder for the c/h files
@REM you will need to use the full path to gb_map_converter.exe
@REM there's a few other arguments, like -t to create a _tiles.png file if you're trying to debug a problem