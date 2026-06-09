@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /std:c++17 /I ..\..\..\..\mashedmod\src\mashed_re ^
   menustr_test.cpp ^
   ..\..\..\..\mashedmod\src\mashed_re\D3d9Render\MenuStringTable.cpp ^
   ..\..\..\..\mashedmod\src\mashed_re\Piz\PizReader.cpp ^
   /Fe:menustr_test.exe
