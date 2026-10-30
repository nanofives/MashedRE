# RESULT D5-DEPS-IMPORTS (static, text-only; no build, no run, no Ghidra)

Exe inspected: `mashedmod/build/lan_release/mashed_re.exe` (the LAN release build, label "Mashed RE c36bbf631 (2026-10-30)"),
size 2,843,648, sha256 `25c9ad6460487b8ac216f811f84f3ccd7fd64e2bff5646e48249ac04b8957ce0`, **sha8 `25c9ad64`**.
`mashedmod/build/lan_build` holds only the source archive, no exe, so the staged release exe was used. The older
`mashedmod/build/mashed_re.exe` (sha8 `01fa6bdd`, 2026-10-24) was not used.
Tool: pefile 2024.8.26 via `py -3.12`. Raw dump: `D:/MashedWT/D5-DEPS-IMPORTS_data/imports.txt`.

PE: i386 (0x14c), linker 14.44, subsystem WINDOWS (2), DllCharacteristics 0x8100 (NX, TS-aware, no ASLR), has TLS directory,
sections .text .rdata .data .fptable .reloc, no resource directory. **Delay-load imports: none.**

## CRT linkage
Static (`/MT` default: `build.bat` line 229 passes neither `/MD` nor `/MDd`). Import table has no VCRUNTIME140, MSVCP140,
UCRTBASE or api-ms-win-crt-* entry, and a byte scan of the exe finds none of those names. The CRT code in the exe calls
KERNEL32 only (FlsAlloc, FindFirstFileExW, GetConsoleMode, etc.). **No Visual C++ redistributable is needed.**

## Static imports (7 DLLs)
| DLL | Class | Functions |
|---|---|---|
| KERNEL32.dll (128) | Windows system | file (CreateFileA/W, ReadFile, WriteFile, FindFirstFileExW, GetFileAttributesA, SetFilePointerEx), threads/sync (CreateThread, SRW locks, TLS, FLS, critical sections, CreateSemaphoreA, Thread32First/Next, OpenThread, SuspendThread, ResumeThread, GetThreadContext), process (GetModuleFileNameA/W, GetProcAddress, LoadLibraryExA/W, GetModuleHandleA/W/ExA/ExW, SetCurrentDirectoryA, GetEnvironmentVariableA), memory (VirtualAlloc/Protect/Query, Heap*), resources (FindResourceA, LoadResource, LockResource, SizeofResource), timing (QueryPerformanceCounter/Frequency, GetTickCount/64), exceptions (AddVectoredExceptionHandler, SetUnhandledExceptionFilter, RaiseException, RtlUnwind, RtlCaptureStackBackTrace), K32GetMappedFileNameA, plus CRT-start-up helpers |
| USER32.dll (28) | Windows system | CreateWindowExA, RegisterClassExA, PeekMessageA, DispatchMessageA, WaitMessage, SetWindowPos, SetWindowLongA, SetProcessDPIAware, EnumDisplayMonitors, GetMonitorInfoA, AdjustWindowRect, DrawTextW, wsprintfA, etc. |
| GDI32.dll (10) | Windows system | CreateFontW, CreateDIBSection, CreateCompatibleDC, SelectObject, SetBkMode, SetTextColor, GetTextExtentPoint32W, GdiFlush, DeleteDC, DeleteObject |
| ole32.dll (3) | Windows system | CoInitializeEx, CoCreateInstance, CoTaskMemFree |
| d3d9.dll (1) | Windows system DLL on Win10/11 (DirectX 9 runtime part of the OS) | Direct3DCreate9 |
| DINPUT8.dll (1) | Windows system | DirectInput8Create |
| DSOUND.dll (1) | Windows system | ordinal 11 (the `DirectSoundCreate8` export slot, unnamed in the table) |

## Runtime-loaded (not in the import table)
- `LoadLibraryExA("original\MASHED.exe", LOAD_LIBRARY_AS_DATAFILE-style)` at `mashedmod/src/mashed_re/exe_main.cpp:8571`: reads the icon resource only. Needs `original/MASHED.exe` (data only).
- `GetProcAddress(u32, "SetProcessDpiAwarenessContext")` at `exe_main.cpp:9407`: optional, user32, system.
- `GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationThread")` at `Harness/NegCtl.cpp:184` and `Core/ShadowTrack.h:325`: system, harness only.
- `GetProcAddress(hModule,"CorExitProcess")` at `Boot/Boot.cpp:78`: mscoree probe, optional.
- `LoadLibraryW` calls in `d3d9_shim/d3d9_shim.cpp:752` (real d3d9), `dinput8_shim/dinput8_shim.cpp:58/88/97` (asi) belong to the **dev shim DLLs, not the exe target** (`build.bat` exe link line 229-238 has no shim). Not shipped.

## COM (CoCreateInstance), all `CLSCTX_INPROC_SERVER`, inprocess system components
| Site | CLSID | Provider |
|---|---|---|
| `D3d9Render/MpegVideoTexture.cpp:50` | `CLSID_FilterGraph` (strmiids.lib), IID_IGraphBuilder, IID_IMediaControl (line 210) | quartz.dll, system |
| `MpegVideoTexture.cpp:55` | SampleGrabber `{C1F400A0-3F08-11D3-9F0B-006008039E37}`, ISampleGrabber `{6B652FFF-11FE-4FCE-92AD-0266B5D7C78F}` | qedit.dll, system on Win10/11 (deprecated, present in System32 and SysWOW64) |
| `MpegVideoTexture.cpp:71` | NullRenderer `{C1F400A4-3F08-11D3-9F0B-006008039E37}` | qedit.dll, system |
| `D3d9Render/PngLoader.cpp:32` | `CLSID_WICImagingFactory` (windowscodecs.lib, ole32) | windowscodecs.dll, system |
All four rely on build-time import libs (strmiids, windowscodecs, dxguid, psapi, ole32) that are static: none appears in the import table, so no extra DLL beyond the runtime COM providers above. DirectShow MPEG-1 decoding also needs the OS MPEG-1 splitter/decoder filters (quartz.dll), present on Windows 10/11 [not tested on a clean machine, that stays with D5-DEPS].

## Files opened beside the exe (relative to the exe's working directory)
Read: `original/TOASTART/**` (Common/Frontend.piz, Font36.piz, Perm.piz, SFX.piz, PANEL/Panel.piz, POWERUPS/Powerups.piz, FONT/English.dat, FONT/USA.dat, TRACKS/<name>.piz, VEHICLES/*.piz, PC/PC.TXD, PC/MOVIES/{frontend,empire,supersonic,renderware,intro,small}.mpg), `original/toastaudio/pc/audio/pcdics/permdict.rws`, `original/gamesave.bin` (seed, `exe_main.cpp:9895`), `original/MASHED.exe` (icon, `:8571`), `videocfg.bin`, `contcfg%d.bin`.
Written: `mashed_re.log`, `mashed_re_settings.bin` (`:1945/1974/1992`), `mashed_re_gamesave.bin`, `videocfg.bin`, `log/` (e.g. `log/r10b_sim.txt`), `frame_prof.log`, `wedge_trap.log` (debug knobs). Env override: `MASHED_ROOT`.
Matches the LAN release layout in `CLAUDE.md`.

## Classification
- **Windows system (nothing to ship):** KERNEL32, USER32, GDI32, ole32, d3d9, DINPUT8, DSOUND, plus runtime COM providers quartz.dll, qedit.dll, windowscodecs.dll, optional ntdll/mscoree probes.
- **Redistributable required:** none (static CRT). Not required: VC++ runtime, D3DX, DirectX end-user runtime, XInput (no XInput import).
- **Must ship:** `mashed_re.exe` and the `original/` tree (assets, `MASHED.exe` as data, `gamesave.bin` seed). No sibling DLLs.
- Caveat for D5-DEPS (clean-machine test): d3d9.dll and DSOUND are OS DLLs, but on N/KN Windows editions quartz/qedit (Media Feature Pack) may be absent. Not verified here.

## Verdict
GREEN, static table only, no source change. Gates not run (quiet hold).
