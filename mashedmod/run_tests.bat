@echo off
REM Combined standalone unit-test regression gate. Builds + RUNS the harnesses and
REM gates on each program's EXIT CODE (0 = pass, 1 = fail) plus a build-success
REM check — a crash/build error can't false-pass. Runnable from any cwd (uses %~dp0).
REM Test exes/objs go to the gitignored build\ dir.
REM
REM   exit 0 = ALL GREEN ; exit 1 = any build error or test failure.
REM Groups:
REM   pure-logic (CI-safe, no assets): navsm, ruleengine, racemodes
REM   asset-dependent (needs original\TOASTART\...): menustr, badges
setlocal
set ROOT=%~dp0
set SRC=%ROOT%src\mashed_re
set OUT=%ROOT%build
set HDIR=%ROOT%..\re\analysis\standalone_menu_sm\harvest
set NAVSM=%HDIR%\build_navsm_test.bat
set RC=0
if not exist "%OUT%" mkdir "%OUT%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul

echo ============ [1/5] navsm (menu state machine) ============
call "%NAVSM%"
if errorlevel 1 (echo   navsm: FAIL & set RC=1) else (echo   navsm: GREEN)

pushd "%SRC%"

echo ============ [2/5] ruleengine_test (race rule engine) ============
cl /nologo /EHsc /std:c++17 /I.. Race\tests\ruleengine_test.cpp Race\RuleEngine.cpp /Fo"%OUT%\\" /Fe"%OUT%\ruleengine_test.exe" >nul
if errorlevel 1 (echo   ruleengine: BUILD FAILED & set RC=1) else (
    "%OUT%\ruleengine_test.exe"
    if errorlevel 1 (echo   ruleengine: FAIL & set RC=1) else (echo   ruleengine: GREEN)
)

echo ============ [3/5] racemodes_test (mode-^>rule map) ============
cl /nologo /EHsc /std:c++17 /I.. Race\tests\racemodes_test.cpp Race\RaceModes.cpp /Fo"%OUT%\\" /Fe"%OUT%\racemodes_test.exe" >nul
if errorlevel 1 (echo   racemodes: BUILD FAILED & set RC=1) else (
    "%OUT%\racemodes_test.exe"
    if errorlevel 1 (echo   racemodes: FAIL & set RC=1) else (echo   racemodes: GREEN)
)
popd

echo ============ [4/5] menustr_test (USA.DAT text table; asset-dependent) ============
cl /nologo /EHsc /std:c++17 /I "%SRC%" "%HDIR%\menustr_test.cpp" "%SRC%\D3d9Render\MenuStringTable.cpp" "%SRC%\Piz\PizReader.cpp" /Fo"%OUT%\\" /Fe"%OUT%\menustr_test.exe" >nul
if errorlevel 1 (echo   menustr: BUILD FAILED & set RC=1) else (
    pushd "%ROOT%.."
    "%OUT%\menustr_test.exe" >nul
    if errorlevel 1 (echo   menustr: FAIL & set RC=1) else (echo   menustr: GREEN)
    popd
)

echo ============ [5/5] badges_test (BADGES.TXD decode; asset-dependent) ============
cl /nologo /EHsc /std:c++17 /I "%SRC%" "%HDIR%\badges_test.cpp" "%SRC%\Txd\TxdDecoder.cpp" "%SRC%\Piz\PizReader.cpp" /Fo"%OUT%\\" /Fe"%OUT%\badges_test.exe" >nul
if errorlevel 1 (echo   badges: BUILD FAILED & set RC=1) else (
    pushd "%HDIR%"
    "%OUT%\badges_test.exe" >nul
    if errorlevel 1 (echo   badges: FAIL & set RC=1) else (echo   badges: GREEN)
    popd
)

echo.
if %RC%==0 (echo ===== ALL STANDALONE TESTS GREEN =====) else (echo ===== TESTS RED =====)
exit /b %RC%
