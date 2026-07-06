@echo off
REM Regression harness for the standalone menu nav state machine (MenuNavSM).
REM Builds + RUNS navsm_test and gates on any "FAIL" line so it can be used as a
REM CI-style check after any MenuNavSM.{h,cpp} change. Covers: reversed push map,
REM state-gated SELECT routing, grey-out, slide-anim tick, save-driven state, the
REM 34-screen reachability BFS, WS-G2 mode-select wiring, and (D-11057) the
REM continue-cup chain (Piece 5) + config wrap-edit primitive (Piece 6).
REM
REM Usage:  build_navsm_test.bat        (exit 0 = all GREEN, 1 = a FAIL / did not
REM         run to completion / build error). Runnable from any cwd (pushd %~dp0).
REM Outputs (navsm_test.exe / *.obj / *.out.txt) are gitignored.
setlocal
pushd "%~dp0"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /std:c++17 ^
   navsm_test.cpp ^
   ..\..\..\..\mashedmod\src\mashed_re\Frontend\MenuNavSM.cpp ^
   /Fe:navsm_test.exe
if errorlevel 1 (echo [navsm_test] BUILD FAILED & popd & exit /b 1)

echo === RUN navsm_test ===
.\navsm_test.exe > navsm_test.out.txt 2>&1
type navsm_test.out.txt
REM Require the final test marker: a crash/early-exit must NOT pass as GREEN.
findstr /C:"Piece 6:" navsm_test.out.txt >nul
if errorlevel 1 (echo [navsm_test] RED - test did not run to completion & del navsm_test.out.txt >nul 2>&1 & popd & exit /b 1)
findstr /C:"FAIL" navsm_test.out.txt >nul
if not errorlevel 1 (echo [navsm_test] RED - one or more assertions FAILED & del navsm_test.out.txt >nul 2>&1 & popd & exit /b 1)
del navsm_test.out.txt >nul 2>&1
popd
echo [navsm_test] GREEN - all assertions passed
endlocal
