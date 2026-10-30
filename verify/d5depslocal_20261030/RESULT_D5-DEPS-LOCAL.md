# RESULT D5-DEPS-LOCAL (standalone boot, module audit; no original-side run)

Verdict: **GREEN**. No source, script or build input changed. Gates not run (module audit only, no sim change).

## Setup
- Exe: `mashedmod/build/lan_release/mashed_re.exe`, sha256 `25c9ad6460487b8ac216f811f84f3ccd7fd64e2bff5646e48249ac04b8957ce0` (sha8 `25c9ad64`, the exe D5-DEPS-IMPORTS audited).
- Staged with `scripts/lan_release/stage_lan_release.py --app mashedmod/build/lan_release --install original --out D:/MashedWT/D5-DEPS-LOCAL_root`: 205 files, 428.4 MB. Root holds `mashed_re.exe`, `original/`, updater files. No `*.dll` and no `*.asi` beside the exe (checked).
- Launched with cwd = the root and an environment of only `SystemRoot`, `windir`, `TEMP`, `TMP` and `PATH=C:\Windows\System32`. Wrapped in `mashed_lock.py run --label D5-DEPS-LOCAL`.
- PID 22588 (spawned by the script, killed by the script in `finally`). 40 s after launch it was alive, and `mashed_re.log` ends with `B17-SUMMARY reached-main-loop chrome=YES thunks=6/6` plus the main-menu self-check strings (Championship, Time Trial, Options).
- Module list: psutil `memory_maps()` of that PID, 96 unique paths. Raw: `modules.txt` here (sha256 `19a21245e6e3f88c4917f90e7a1a88e6db9c6ce74985ce6f24741ed526c370be`). Runner script was in the scratchpad, not committed.

## Static imports: dumpbin /dependents agrees
`dumpbin.txt` lists exactly 7: USER32, d3d9, DSOUND, GDI32, DINPUT8, ole32, KERNEL32. Same set as RESULT_D5-DEPS-IMPORTS.md. All 7 appear in the loaded list from `C:\Windows\SysWOW64`.

## Loaded modules
- `D:\MashedWT\D5-DEPS-LOCAL_root\mashed_re.exe`: the exe itself, the only module outside `C:\Windows`.
- `C:\Windows\SysWOW64`: 7 static imports plus runtime COM providers `quartz.dll`, `qedit.dll`, `WindowsCodecs.dll`, matching the IMPORTS table. Everything else there is OS-side transitive load (combase, rpcrt4, winmm, devenum, msdmo, msvfw32, MMDevAPI, setupapi and so on).
- `C:\Windows\System32`: ntdll, wow64*, NLS data. WinSxS: comctl32 and GdiPlus (OS side-by-side, `C:\Windows\WinSxS`).
- No VCRUNTIME, MSVCP, api-ms-win-crt, D3DX, XInput. `ucrtbase.dll` and `msvcrt.dll` are OS copies pulled by system DLLs, not by the exe (the exe has no such import).
- Not under System32/SysWOW64 but not shipped by us, all loaded by OS DLLs: NVIDIA user-mode driver DLLs under `C:\Windows\System32\DriverStore\FileRepository\nv_dispig...` (nvd3dum, nvgpucomp32, nvldumd, NvMemMapStorage, nvppe, pulled in by d3d9), `SysWOW64\nvspcap.dll` (NVIDIA hook, system dir), `.mui` files from the Spanish language experience pack under `C:\Program Files\WindowsApps\...` (resource lookup), `C:\Windows\Fonts\StaticCache.dat`, `C:\Windows\Globalization\Sorting\SortDefault.nls`. These are this machine's GPU driver and language pack, not dependencies of the exe.
- Zero modules from the exe folder, from `original\` or from any other user path.

## quartz / qedit / windowscodecs on this machine
Machine: Windows 10 Pro 26H2, build 26300 (edition read from the registry `ProductName`; `Get-WindowsEdition` needs elevation and was not run). Not an N/KN edition.
| DLL | System32 (64-bit) | SysWOW64 (32-bit, what the exe uses) | Version (SysWOW64) | Signature |
|---|---|---|---|---|
| quartz.dll | 1,855,488 B | 1,541,120 B | 10.0.26100.9711 | Valid |
| qedit.dll | 679,936 B | 580,096 B | 10.0.26100.1 | Valid |
| windowscodecs.dll | 2,383,264 B | 1,998,456 B | 10.0.26100.9711 | Valid |
WOW6432Node COM registration resolves in `HKLM\SOFTWARE\Classes\WOW6432Node\CLSID\...\InprocServer32`: FilterGraph `{E436EBB3-...}` -> SysWOW64\quartz.dll, SampleGrabber `{C1F400A0-...}` and NullRenderer `{C1F400A4-...}` -> SysWOW64\qedit.dll, WICImagingFactory `{CACAF262-...}` -> SysWOW64\windowscodecs.dll. All three were also loaded in the run, so the MPEG menu movie path and PNG path were exercised.

## Not covered (needs Mariano, not in this row)
Clean-machine test on another PC, N/KN edition check (Media Feature Pack for quartz/qedit), a machine without the NVIDIA stack. This machine has no audio endpoint, so DSOUND was loaded but no playback was verified.

## Raw data
`D:/MashedWT/D5-DEPS-LOCAL_data/` : `modules.txt`, `dumpbin.txt`, `run.log`, `dump.bat`.
