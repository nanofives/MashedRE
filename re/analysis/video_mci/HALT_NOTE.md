# Session U halt note — video_mci-20260508-0623

Session U searched for MCI video playback and halted: **MCI is not used**. Video is via DirectShow.

## What was found

### WINMM.DLL imports (complete)
`timeBeginPeriod`, `timeEndPeriod`, `timeGetTime`, `timeSetEvent`, `timeKillEvent` — all timer APIs.
No `mciSendString`, no `mciSendCommand`, no dynamic MCI load observed.

### .mpg strings
| Address      | Value |
|--------------|-------|
| `0x005cf8d4` | `"toastart\pc\movies\intro.mpg"` |
| `0x005cf8f4` | `"toastart\pc\movies\renderware.mpg"` |
| `0x005cf918` | `"toastart\pc\movies\supersonic.mpg"` |
| `0x005cf93c` | `"toastart\pc\movies\empire.mpg"` |
| `0x005cf95c` | `"toastart\pc\movies\frontend.mpg"` |
| `0x005cfcf0` | `"toastart\pc\movies\small.mpg"` |

Strings at `0x005cf8d4`–`0x005cf95c` form a 5-entry pointer table at `0x006147c4–0x006147e4`.

### DirectShow video candidates (fully decompiled, not yet catalogued)

**`FUN_004944c0`** (0x004944c0–0x0049481f) — parameterised player (int index into movie table):
- `CoCreateInstance` creates filter graph (IID at `0x005d0c4c`, interface IID at `0x005d0abc`), graph stored at `DAT_00771a30`
- `operator_new(0x170)` + `FUN_00493c00(0, &local_18)` — allocates and inits TEXTURERENDERER object
- `lstrcpyA(local_124, (&PTR_006147c4)[param_1])` — picks movie path by index
- `FUN_00493bc0` converts path to wide char
- `IFilterGraph::AddSourceFilter` (vtable+0x38) adds source filter as `L"SOURCE"`, output interface at `local_1c`
- vtable+0x2c finds `L"Output"` pin into `local_20`
- vtable+0x30 renders source output pin
- QueryInterface calls for IIDs at `0x005d0bcc` (→ `DAT_00771a34`), `0x005d0bbc` (→ `DAT_00771a2c`), `0x005cfac4` (→ `DAT_00771a28`), `0x005cfad4` (→ `DAT_00771a20`)
- vtable+0x1c on `DAT_00771a34` — Run/Pause call
- On success: sets `DAT_00771a00 = 0`, `DAT_006147d8 = 1`; returns 0x0
- On failure paths: calls `FUN_00493f00(error_string, hr)`

**`FUN_00494c80`** (0x00494c80–0x00494ed8) — no-parameter variant, hardcoded to `small.mpg`:
- Calls `FUN_004944b0()` twice before DirectShow setup
- Same graph pattern; graph globals are `DAT_00771a38`/`DAT_00771a44`/`DAT_00771a48`/`DAT_00771a40`/`DAT_00771a3c`
- Calls `FUN_004a1790()` twice on both success and failure paths (cleanup/uninit)
- Does NOT call vtable+0x1c; goes directly to return 0x0 on success

### Callees to investigate in a future session
- `FUN_00493c00` — TEXTURERENDERER constructor/init (called from both)
- `FUN_00493bc0` — ANSI-to-wide-char path converter (called from both)
- `FUN_00493f00` — error log/printf wrapper (called from both)
- `FUN_004a1790` — called twice on every exit path of FUN_00494c80 (cleanup)
- `FUN_004944b0` — called twice before graph setup in FUN_00494c80

## Recommended follow-up session

Bucket: `re/analysis/video_directshow/`
Subsystem: `util`
Seed RVAs: `0x004944c0`, `0x00494c80`
Callee depth-1: `0x00493c00`, `0x00493bc0`, `0x00493f00`, `0x004a1790`, `0x004944b0`
ID ranges: allocate fresh U/D/S ranges in the next batch file.

No tracker rows written. No STUBS/UNCERTAINTIES/DEFERRED entries created.
Slot Mashed_pool2 was released cleanly.
