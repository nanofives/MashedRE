@echo off
REM ===========================================================================
REM Vendored librw (MIT, aap) -> static lib for Mashed RE. Gate D2 / lane M3-E1'.
REM
REM librw is adopted as a RENDERING PIPELINE, not as a file-format library. Our
REM own loaders (Track\TrackWorld.cpp, Track\DffModel.cpp, Txd\TxdDecoder.cpp)
REM already parse Mashed's BSP / DFF / TXD; we construct rw:: objects in memory
REM and let librw draw them. librw's stream layer is unused, which is why
REM upstream's "BSP is not supported at all" does not block this lane.
REM See re\analysis\LIBRW_SIZING_2026-08.md sections 3.2 and 3.4.
REM
REM RW_D3D9 selects the D3D9 backend, but ALL platform subdirs must still be
REM compiled. src\engine.cpp:233-238 calls ps2::/xbox::/d3d8::/d3d9::/wdgl::/gl3::
REM registerPlatformPlugins() UNCONDITIONALLY; the per-platform .cpp files guard
REM their bodies with #ifdef but still define the stub symbols those calls bind to.
REM Omitting src\ps2 and src\gl gives 30 LNK2019 unresolved externals at exe link
REM (measured 2026-07-31). Upstream does the same thing -- premake5.lua:126-127 is
REM `files { "src/*.*" }` + `files { "src/*/*.*" }`, i.e. every subdir.
REM src\gl\glad\ is the ONE exception: premake5.lua:129 adds it only for *gl3
REM platforms, and it needs GLFW/SDL headers we do not have. Excluded here.
REM
REM NOT x87/IA32 (unlike deps\qhull-2002.1\build_qhull.bat): qhull is compiled
REM /arch:IA32 to chase bit-identity with the original's 2004 x87 build. librw is
REM NOT a reimplementation of anything in MASHED.exe -- gate D2 accepts
REM BEHAVIOURAL parity with documented visual deltas, explicitly not bit-parity.
REM So there is no rounding contract to honour here and we take the default
REM codegen.
REM
REM No fxc / DirectX SDK needed: src\d3d\shaders\*.h are pre-compiled blobs
REM committed upstream.
REM ===========================================================================
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set HERE=%~dp0
set OBJ=%HERE%obj
set OUTLIB=%HERE%librw_d3d9.lib

if not exist "%OBJ%" mkdir "%OBJ%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

REM /MT to match the exe/asi targets (neither passes /MD, so both default to /MT).
REM /wd4996 CRT deprecation; /wd4244 narrowing; /wd4838 the three known
REM D3DFMT_UNKNOWN enum->uint32 narrowings in src\d3d\xbox.cpp:542-544; /wd4267
REM three size_t->unsigned short narrowings in src\lodepng\lodepng.cpp:729,753,770.
REM All five are upstream's, in code we do not maintain -- silenced so a clean
REM build stays clean and a NEW warning is visible.
REM LODEPNG_NO_COMPILE_CPP matches upstream (premake5.lua:125) -- drops lodepng's
REM C++ wrapper, which we do not use.
set CFLAGS=/nologo /c /EHsc /O2 /MT /W3 /DRW_D3D9 /DNDEBUG /DLODEPNG_NO_COMPILE_CPP /wd4996 /wd4244 /wd4838 /wd4267

cl %CFLAGS% /I "%HERE%." /I "%HERE%src" /Fo"%OBJ%\\" ^
    "%HERE%src\*.cpp" "%HERE%src\d3d\*.cpp" "%HERE%src\gl\*.cpp" ^
    "%HERE%src\ps2\*.cpp" "%HERE%src\lodepng\*.cpp"
if errorlevel 1 (echo [ERROR] librw compile failed & exit /b 1)

lib /nologo /OUT:"%OUTLIB%" "%OBJ%\*.obj"
if errorlevel 1 (echo [ERROR] lib failed & exit /b 1)

echo === librw_d3d9.lib OK ===
dir /b "%OUTLIB%"
endlocal
