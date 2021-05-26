del main.gb
C:\c_code\gbdk\bin\lcc.exe -Wl-j -Wm-yS -o main.gb ..\main.c ..\tiles\tile_detectivewalk.c ..\tiles\cig_shine.c ..\tiles\smoke.c
@REM -Wl-j -Wm-yS = force generating the *.sym file, so you will see names in the BGB debugger.