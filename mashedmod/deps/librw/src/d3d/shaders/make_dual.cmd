@echo off
REM MASHED LOCAL PATCH (P9, rtex 2026-10-16). Same fxc invocations as make_matfx.cmd; FXC
REM defaults to the Windows SDK copy that ships with the MSVC Build Tools install.
if "%FXC%"=="" set FXC=C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\fxc.exe
"%FXC%" /nologo /T vs_2_0 /Fh matfx_dual_amb_VS.h matfx_dual_VS.hlsl
"%FXC%" /nologo /T vs_2_0 /DDIRECTIONALS /Fh matfx_dual_amb_dir_VS.h matfx_dual_VS.hlsl
"%FXC%" /nologo /T vs_2_0 /DDIRECTIONALS /DPOINTLIGHTS /DSPOTLIGHTS /Fh matfx_dual_all_VS.h matfx_dual_VS.hlsl

"%FXC%" /nologo /T ps_2_0 /DTEX /Fh matfx_dual_tex_PS.h matfx_dual_PS.hlsl
"%FXC%" /nologo /T ps_2_0 /DTEX /DUV0 /Fh matfx_dual_tex_uv0_PS.h matfx_dual_PS.hlsl
"%FXC%" /nologo /T ps_2_0 /Fh matfx_dual_PS.h matfx_dual_PS.hlsl
