del /S /Q *.sdf *.exp *.ilk *.pdb
rd /S /Q ipch
cd OS_Scheduler_Simulator
rd /S /Q Debug
rd /S /Q Release
cd ..
for /D %%F in (algo_*) DO CD %%F & rd /S /Q Debug & rd /S /Q Release & CD ..
