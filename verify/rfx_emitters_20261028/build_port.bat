@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d D:\MashedWT\D5-RFX-EMITTERS_data
cl /nologo /EHsc /std:c++17 /O2 /W3 C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D5-RFX-EMITTERS\verify\rfx_emitters_20261028\emit_port.cpp C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D5-RFX-EMITTERS\mashedmod\src\mashed_re\Render\RfxEmitter.cpp /Fe:emit_port.exe
