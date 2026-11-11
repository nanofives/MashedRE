@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
cl /nologo /c /TP /I"C:\Users\maria\Desktop\Proyectos\Mashed\re\prior_art\renderware\gta-reversed-modern\source\game_sa\RenderWare\rw" %* rw3.cpp /Fo%TEMP%\rw3probe.obj
