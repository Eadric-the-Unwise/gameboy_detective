del main.gb
C:\c_code\projects\detective\detective_metasprite\build\gb_map_converter\gb_map_converter.exe -i ..\maps\bkg_submap.png -n bkg_submap -o ..\maps

C:\c_code\gbdk\bin\lcc.exe -Wl-j -Wm-yS -o main.gb ..\main.c ..\tiles\tile_detectivewalk.c ..\tiles\cig_shine.c ..\tiles\smoke.c ..\maps\bkg_apartment_map.c ..\maps\bkg_apartment_tiles.c ..\maps\bkg_apartment_scroll_map.c ..\maps\bkg_apartment_scroll_tiles.c ..\maps\bkg_submap_map.c ..\maps\bkg_submap_tiles.c ..\tiles\bkg_apartment_lamp_top.c ..\tiles\bkg_apartment_lamp_bot.c ..\tiles\apartment_smoke.c
@REM -Wl-j -Wm-yS = force generating the *.sym file, so you will see names in the BGB debugger.

@REM -n level_1 is the name you want the c/h files and variables to use, it defaults to the png file name
@REM -o ..\maps is the output folder for the c/h files
@REM you will need to use the full path to gb_map_converter.exe
@REM there's a few other arguments, like -t to create a _tiles.png file if you're trying to debug a problem