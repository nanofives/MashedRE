@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
cl /nologo /c /EHsc /DRW_D3D9 /DNDEBUG /I"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RW-SIZING\mashedmod\deps\librw" /d1reportAllClassLayout lrw.cpp /Fo%TEMP%\lrwprobe.obj
