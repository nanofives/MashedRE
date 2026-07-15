@echo off
REM ===========================================================================
REM Vendored qhull-2002.1 (public domain) -> static lib for Mashed RE.
REM
REM The original MASHED.exe statically links qhull-2002.1 (banner "2002.1
REM 2002/8/20" @ 0x00625e04) embedded inside RenderWare Physics 3.7's
REM src/qhull/ tree, wrapped by RwpQHullWrapper.c == FUN_0057ca30. We vendor
REM the STOCK upstream qhull-2002.1 (the version banner in the binary is
REM unmodified, so RWP used stock sources) and reimplement only the game-side
REM RwpQHullWrapper bridge clean-room from our own decomp.
REM
REM BIT-IDENTITY LEVER: qhull is famously FP-rounding sensitive. The original
REM was built ~2004 with MSVC (x87). We compile the qhull TUs with /arch:IA32
REM (x87, NOT SSE2) + /fp:precise to reproduce the original's rounding. This is
REM the same "build x87, no /arch:SSE2" convention used for the RW-math leaves
REM (memory: project-wsa2-rwmath-bitident).
REM
REM Only the LIBRARY subset is compiled. The CLI drivers (qconvex/qdelaun/
REM qhalf/qvoronoi/rbox/unix) and examples (user_eg/user_eg2) each carry their
REM own main() and are EXCLUDED.
REM ===========================================================================
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set HERE=%~dp0
set SRC=%HERE%src
set OBJ=%HERE%obj
set OUTLIB=%HERE%qhull_2002_1.lib

if not exist "%OBJ%" mkdir "%OBJ%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

REM /arch:IA32 -> x87 code (match original 2004 build); /fp:precise -> no
REM contraction/reordering; /Ox -> optimize (qhull is heavy); /wd4996 silence
REM CRT deprecation; /D_CRT_SECURE_NO_WARNINGS for str*/fopen.
set CFLAGS=/nologo /c /arch:IA32 /fp:precise /Ox /W1 /wd4996 /wd4244 /wd4305 /D_CRT_SECURE_NO_WARNINGS

pushd "%SRC%"
cl %CFLAGS% /Fo"%OBJ%\\" ^
    geom.c geom2.c global.c io.c mem.c merge.c ^
    poly.c poly2.c qhull.c qset.c stat.c user.c
if errorlevel 1 (echo [ERROR] qhull compile failed & popd & exit /b 1)
popd

lib /nologo /OUT:"%OUTLIB%" "%OBJ%\geom.obj" "%OBJ%\geom2.obj" "%OBJ%\global.obj" "%OBJ%\io.obj" "%OBJ%\mem.obj" "%OBJ%\merge.obj" "%OBJ%\poly.obj" "%OBJ%\poly2.obj" "%OBJ%\qhull.obj" "%OBJ%\qset.obj" "%OBJ%\stat.obj" "%OBJ%\user.obj"
if errorlevel 1 (echo [ERROR] lib failed & exit /b 1)

echo === qhull_2002_1.lib OK ===
dir /b "%OUTLIB%"
endlocal
