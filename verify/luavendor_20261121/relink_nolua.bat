@echo off
REM LUA-VENDOR control: relink mashed_re.exe from the same objects with the build.bat link line minus "%LUA_LIB%",
REM to D:\MashedWT\LUA-VENDOR_data\nolua\, then pe_compare.py compares it with build\mashed_re.exe.
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
set ROOT=%~dp0..\..\mashedmod\
set OUT=%ROOT%build
set SRC=%ROOT%src\mashed_re
set D=D:\MashedWT\LUA-VENDOR_data\nolua
if not exist "%D%" mkdir "%D%"
set QHULL_LIB=%ROOT%deps\qhull-2002.1\qhull_2002_1.lib
set LIBRW_LIB=%ROOT%deps\librw\librw_d3d9.lib
pushd "%SRC%"
cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /Fe"%D%\mashed_re.exe" @"%OUT%\obj\exe\link.rsp" ^
    "%OUT%\mashed_re.res" ^
    "%OUT%\QhullBridge_exe.obj" ^
    "%OUT%\RwBridge_exe.obj" ^
    "%OUT%\RwRasterBridge_exe.obj" ^
    "%OUT%\RwSceneBuild_exe.obj" ^
    "%OUT%\RwRaceSubmit_exe.obj" ^
    "%OUT%\RwRaceFx_exe.obj" ^
    "%OUT%\RwApiBind_exe.obj" ^
    /link /SUBSYSTEM:WINDOWS /BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO ^
    /MAP:"%D%\mashed_re.map" ^
    /DELAYLOAD:d3d9.dll /DELAYLOAD:dinput8.dll ^
    user32.lib d3d9.lib gdi32.lib "%QHULL_LIB%" "%LIBRW_LIB%"
set RC=%errorlevel%
popd
echo relink rc=%RC%
exit /b %RC%
