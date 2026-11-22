@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
set L=C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\LUA-VENDOR\mashedmod\deps\lua-4.0
cd /d %~dp0
cl /nologo /O2 /W3 /I"%L%\include" /Fet_vs.exe luatest.c "%L%\lua_4_0.lib" /link /NODEFAULTLIB:libc.lib > link_vs.log 2>&1
echo vs link rc=%errorlevel%
cl /nologo /O2 /W3 /I"%L%\include" /Fet_ddk.exe luatest.c "%L%\lua_4_0_ddk.lib" /link /NODEFAULTLIB:libc.lib legacy_stdio_definitions.lib > link_ddk.log 2>&1
echo ddk link rc=%errorlevel%
%~dp0t_vs.exe
echo vs run rc=%errorlevel%
%~dp0t_ddk.exe
echo ddk run rc=%errorlevel%
