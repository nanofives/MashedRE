@echo off
REM Combined standalone unit-test regression gate. Builds + RUNS the pure-logic
REM harnesses and gates on each program's EXIT CODE (0 = pass, 1 = fail) plus a
REM build-success check — a crash/build error can't false-pass. Runnable from any
REM cwd (uses %~dp0). Test exes/objs go to the gitignored build\ dir.
REM
REM   exit 0 = ALL GREEN ; exit 1 = any build error or test failure.
REM Covers: menu-SM nav (navsm), race rule engine (ruleengine), mode->rule (racemodes).
setlocal
set ROOT=%~dp0
set SRC=%ROOT%src\mashed_re
set OUT=%ROOT%build
set NAVSM=%ROOT%..\re\analysis\standalone_menu_sm\harvest\build_navsm_test.bat
set RC=0
if not exist "%OUT%" mkdir "%OUT%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul

echo ============ [1/3] navsm (menu state machine) ============
call "%NAVSM%"
if errorlevel 1 (echo   navsm: FAIL & set RC=1) else (echo   navsm: GREEN)

pushd "%SRC%"

echo ============ [2/3] ruleengine_test (race rule engine) ============
cl /nologo /EHsc /std:c++17 /I.. Race\tests\ruleengine_test.cpp Race\RuleEngine.cpp /Fo"%OUT%\\" /Fe"%OUT%\ruleengine_test.exe" >nul
if errorlevel 1 (
    echo   ruleengine: BUILD FAILED & set RC=1
) else (
    "%OUT%\ruleengine_test.exe"
    if errorlevel 1 (echo   ruleengine: FAIL & set RC=1) else (echo   ruleengine: GREEN)
)

echo ============ [3/3] racemodes_test (mode-^>rule map) ============
cl /nologo /EHsc /std:c++17 /I.. Race\tests\racemodes_test.cpp Race\RaceModes.cpp /Fo"%OUT%\\" /Fe"%OUT%\racemodes_test.exe" >nul
if errorlevel 1 (
    echo   racemodes: BUILD FAILED & set RC=1
) else (
    "%OUT%\racemodes_test.exe"
    if errorlevel 1 (echo   racemodes: FAIL & set RC=1) else (echo   racemodes: GREEN)
)

popd
echo.
if %RC%==0 (echo ===== ALL STANDALONE LOGIC TESTS GREEN =====) else (echo ===== TESTS RED =====)
exit /b %RC%
