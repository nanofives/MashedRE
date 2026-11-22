@echo off
REM ===========================================================================
REM Vendored Lua 4.0 (stock lua.org lua-4.0.tar.gz, sha256 b476abc7...0e71337) -> static libs for Mashed RE.
REM LUA-VENDOR (run 55, decisions L-97 / L-177). Evidence: verify/luavendor_20261121/RESULT_LUA-VENDOR.md.
REM
REM MASHED.exe links Lua 4.0: version string "Lua 4.0" at 0x005d8790, and the 4.0 form of every function that 4.0.1
REM changed and the game links (retstat 0x004be360, lua_newuserdata 0x004b7180, traceexec 0x004b93dc, protectedparser
REM 0x004b79da). Only luaC_checkGC 0x004ba570 is identical to the 4.0.1 compile (extern luaC_collectgarbage); the tree
REM here is NOT edited for it (RESULT section 2, [UNCERTAIN] in re/orchestrator/UNCERTAIN_REGISTER.md).
REM
REM Two libraries, one per build (L-125):
REM   lua_4_0.lib      VS2022 cl, x87 (/arch:IA32 /fp:precise), linked into mashed_re.exe by mashedmod\build.bat.
REM   lua_4_0_ddk.lib  XP DDK cl 13.00.9176 / c2 13.00.9178 with the recorded flags /Og /Oi /Ot /Oy /Ob1 /GF /G5
REM                    (verify/libidentity_20261030/RESULT_LIB-IDENTITY.md), linked by re\tools\ddkobj\ddkobj.py into
REM                    the object build (MASHED_DDKOBJ=1). Needs tools\vc7ddk (gitignored, main checkout).
REM Compiled: the core (src\*.c) plus src\lib\lauxlib.c and lmathlib.c, the two standard libraries the game links
REM ("randomseed" and "atan2" are in the image; no string of lbaselib, liolib, lstrlib or ldblib is: "dofile",
REM "readfrom", "strsub", "tinsert", "setlinehook" are absent). Those four stay in the tree, stock, not compiled.
REM ltests.c and the lua / luac drivers are not vendored.
REM
REM   build_lua.bat        both libraries (the DDK one is skipped with a note when tools\vc7ddk is absent)
REM   build_lua.bat vs     VS2022 library only
REM   build_lua.bat ddk    DDK library only (fails when tools\vc7ddk is absent)
REM ===========================================================================
setlocal
set HERE=%~dp0
set SRC=%HERE%src
set INC=%HERE%include
set WHAT=%~1
if "%WHAT%"=="" set WHAT=both

set LUA_SRCS=lapi.c lcode.c ldebug.c ldo.c lfunc.c lgc.c llex.c lmem.c lobject.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c
set LUA_LIBSRCS=lauxlib.c lmathlib.c
set LUA_OBJS=lapi.obj lcode.obj ldebug.obj ldo.obj lfunc.obj lgc.obj llex.obj lmem.obj lobject.obj lparser.obj lstate.obj lstring.obj ltable.obj ltm.obj lundump.obj lvm.obj lzio.obj lauxlib.obj lmathlib.obj

if /i "%WHAT%"=="ddk" goto ddk

REM ---------------------------------------------------------------- VS2022
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set OBJ=%HERE%obj
if not exist "%OBJ%" mkdir "%OBJ%"
if not defined VCINSTALLDIR (
    call %VCVARS% >nul
    if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)
)
set CFLAGS=/nologo /c /arch:IA32 /fp:precise /O2 /W1 /wd4996 /wd4244 /wd4018 /wd4113 /D_CRT_SECURE_NO_WARNINGS /I"%INC%" /I"%SRC%"
pushd "%SRC%"
cl %CFLAGS% /Fo"%OBJ%\\" %LUA_SRCS%
if errorlevel 1 (echo [ERROR] lua VS2022 compile failed & popd & exit /b 1)
popd
pushd "%SRC%\lib"
cl %CFLAGS% /Fo"%OBJ%\\" %LUA_LIBSRCS%
if errorlevel 1 (echo [ERROR] lua VS2022 lib compile failed & popd & exit /b 1)
popd
pushd "%OBJ%"
lib /nologo /OUT:"%HERE%lua_4_0.lib" %LUA_OBJS%
if errorlevel 1 (echo [ERROR] lib lua_4_0.lib failed & popd & exit /b 1)
popd
echo === lua_4_0.lib OK ===
if /i "%WHAT%"=="vs" goto done

:ddk
REM ---------------------------------------------------------------- XP DDK (object build)
set DDK=%MASHED_VC7DDK%
if not defined DDK if exist "%HERE%..\..\..\tools\vc7ddk\bin\cl.exe" set DDK=%HERE%..\..\..\tools\vc7ddk
if not defined DDK if exist "%HERE%..\..\..\..\..\tools\vc7ddk\bin\cl.exe" set DDK=%HERE%..\..\..\..\..\tools\vc7ddk
if not defined DDK (
    if /i "%WHAT%"=="ddk" (echo [ERROR] tools\vc7ddk not found ^(set MASHED_VC7DDK^) & exit /b 1)
    echo [note] tools\vc7ddk not found: lua_4_0_ddk.lib not built ^(only the object build needs it^)
    goto done
)
set DOBJ=%HERE%obj_ddk
if not exist "%DOBJ%" mkdir "%DOBJ%"
REM The DDK compiler sees only its own CRT headers and binaries (same environment as re\tools\match\period_cc.py).
set INCLUDE=%DDK%\inc\crt
set LIB=
set CL=
set _CL_=
set PATH=%DDK%\bin;%PATH%
set DFLAGS=/nologo /c /W0 /Og /Oi /Ot /Oy /Ob1 /GF /G5 /I"%INC%" /I"%SRC%"
pushd "%SRC%"
"%DDK%\bin\cl.exe" %DFLAGS% /Fo"%DOBJ%\\" %LUA_SRCS%
if errorlevel 1 (echo [ERROR] lua DDK compile failed & popd & exit /b 1)
popd
pushd "%SRC%\lib"
"%DDK%\bin\cl.exe" %DFLAGS% /Fo"%DOBJ%\\" %LUA_LIBSRCS%
if errorlevel 1 (echo [ERROR] lua DDK lib compile failed & popd & exit /b 1)
popd
pushd "%DOBJ%"
"%DDK%\bin\lib.exe" /nologo /OUT:"%HERE%lua_4_0_ddk.lib" %LUA_OBJS%
if errorlevel 1 (echo [ERROR] lib lua_4_0_ddk.lib failed & popd & exit /b 1)
popd
echo === lua_4_0_ddk.lib OK ===

:done
endlocal
exit /b 0
