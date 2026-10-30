# RESULT D5-DEPS-LOCAL-FOLLOWUP (text only, no boot, no exe run)

Verdict: **GREEN**. Covers ROADMAP.md line 3010 (D5 checklist item "Runtime dependencies listed (CRT, D3D9, DirectShow for video) and checked on a clean machine"): the list half is closed here, the clean-machine half stays with Mariano (see the needs-Mariano list). No source, script, build input or ROADMAP text changed. Gates not run (no exe, no sim change).

Source: `verify/d5depslocal_20261030/RESULT_D5-DEPS-LOCAL.md` (cited below as SRC, with line numbers). Exe audited: `mashed_re.exe` sha256 `25c9ad6460487b8ac216f811f84f3ccd7fd64e2bff5646e48249ac04b8957ce0` (SRC line 6). Module list: 96 unique paths in `modules.txt`, sha256 `19a21245e6e3f88c4917f90e7a1a88e6db9c6ce74985ce6f24741ed526c370be` (SRC line 10), raw copy in `D:/MashedWT/D5-DEPS-LOCAL_data/` (SRC line 36).

## Runtime-dependency table (for release notes)

All DLLs below ship with Windows. The release folder ships no DLL and no `.asi` (SRC line 7). The 32-bit copies in `C:\Windows\SysWOW64` are the ones used (SRC lines 13 and 17).

| # | Dependency | Kind | Needed for | Source of the DLL | Cite |
|---|---|---|---|---|---|
| 1 | KERNEL32.dll | static import | process basics | Windows | SRC line 13 (dumpbin list), line 17 |
| 2 | USER32.dll | static import | window, message loop | Windows | SRC line 13, line 17 |
| 3 | GDI32.dll | static import | GDI calls | Windows | SRC line 13, line 17 |
| 4 | d3d9.dll | static import | Direct3D 9 rendering | Windows (DirectX 9.0c runtime is part of Windows 10 and 11) | SRC line 13, line 17 |
| 5 | DSOUND.dll | static import | DirectSound audio | Windows | SRC line 13, line 17 |
| 6 | DINPUT8.dll | static import | DirectInput controllers | Windows | SRC line 13, line 17 |
| 7 | ole32.dll | static import | COM for the two rows below | Windows | SRC line 13, line 17 |
| 8 | quartz.dll | COM provider, loaded at run time | DirectShow filter graph (`{E436EBB3 prefix}`), menu MPEG movie | Windows, absent without Media Feature Pack on N/KN editions | SRC line 17, line 30, version table line 26 (10.0.26100.9711) |
| 9 | qedit.dll | COM provider, loaded at run time | SampleGrabber `{C1F400A0 prefix}` and NullRenderer `{C1F400A4 prefix}` | Windows, same N/KN caveat | SRC line 17, line 30, version table line 27 (10.0.26100.1) |
| 10 | WindowsCodecs.dll | COM provider, loaded at run time | WICImagingFactory `{CACAF262 prefix}`, PNG path | Windows | SRC line 17, line 30, version table line 28 (10.0.26100.9711) |

Rows 1 to 7 are the 7 static imports, rows 8 to 10 the 3 COM providers. Registration for 8 to 10 resolves under `HKLM\SOFTWARE\Classes\WOW6432Node\CLSID\<guid>\InprocServer32` to SysWOW64 (SRC line 30). All three loaded in the audited run, so the movie and PNG paths were exercised (SRC line 30).

### Not dependencies (checked absent)

| Item | Result | Cite |
|---|---|---|
| VCRUNTIME, MSVCP, api-ms-win-crt | not loaded, no Visual C++ redistributable needed | SRC line 19 |
| D3DX (d3dx9_*.dll) | not loaded, no DirectX End-User Runtime needed | SRC line 19 |
| XInput | not loaded | SRC line 19 |
| ucrtbase.dll, msvcrt.dll | loaded, but pulled in by system DLLs, the exe has no such import | SRC line 19 |
| NVIDIA user-mode driver DLLs, `nvspcap.dll`, Spanish language pack `.mui` files, `StaticCache.dat`, `SortDefault.nls` | loaded on the audit machine by OS or driver DLLs, not shipped and not required by the exe | SRC line 20 |
| Modules from the exe folder, `original\` or any user path | zero, other than the exe itself | SRC line 21 |

## Needs-Mariano list (items SRC marks Not covered, SRC lines 32 to 33)

Each item needs another PC or hardware. This session has none, so none is closed here.

| # | Item | Status | One-line manual test recipe |
|---|---|---|---|
| 1 | Clean PC (no dev tools, no prior DirectX or VC installs) | needs-Mariano | On a fresh Windows 10 or 11 install or a clean VM with a GPU, run `update.bat` in an empty folder, launch "Mashed (RE)", and confirm the main menu appears with no missing-DLL dialog. |
| 2 | N or KN edition plus Media Feature Pack | needs-Mariano | On a Windows N/KN install, launch once without the Media Feature Pack and note whether the menu movie and boot fail (rows 8 and 9 absent), then install the Media Feature Pack and confirm the movie plays. |
| 3 | Machine without the NVIDIA stack | needs-Mariano | On a PC with AMD or Intel graphics only, launch and confirm the main menu renders through the Windows d3d9 with no NVIDIA DLL involved. |
| 4 | Audio endpoint (DSOUND playback) | needs-Mariano | On a PC with a working speaker or headphone output, start a race and confirm music and engine sound play, since the audit PC had no endpoint and only the DSOUND load was seen (SRC line 33). |

Evidence note: the Windows edition on the audit machine was read from the registry `ProductName` as non-N/KN (SRC line 24), so item 2 has no local data at all.

## Files

Committed: this RESULT only. No raw data copied, all raw files stay at `D:/MashedWT/D5-DEPS-LOCAL_data/` (SRC line 36).
