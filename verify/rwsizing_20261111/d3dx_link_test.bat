@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
cl /nologo /EHsc /MT /I"%~dp0inc" t.cpp /link /LIBPATH:"C:\Users\maria\Desktop\Proyectos\Mashed\tools\dxsdk9\sum2003\Lib" d3dx9.lib d3d9.lib user32.lib gdi32.lib advapi32.lib ole32.lib winmm.lib legacy_stdio_definitions.lib > t_build.txt 2>&1
echo exit=%errorlevel% >> t_build.txt
if exist t.exe t.exe >> t_build.txt 2>&1
