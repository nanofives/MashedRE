@echo off
REM usage: cxxcheck.bat <file.c>  -- compiles the file as C++ with VS2022 (port toolchain), MATCH_PORT defined
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /c /TP /W1 /O2 /EHa /DMATCH_PORT /Fo"%TEMP%\cxxcheck_%~n1.obj" "%~1"
