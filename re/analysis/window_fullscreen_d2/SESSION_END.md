# SESSION_END — window_fullscreen_d2-20260508-1346

## Slot held
Mashed_pool15 (pre-assigned, no lock file, opened read-only)

## Binary anchor
MASHED.exe SHA-256 = `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` ✓

## D1 uncertainties resolved

| ID | Status | Resolution |
|---|---|---|
| U-1327 | RESOLVED | DAT_00911f98 written by FUN_004c7a70 at 0x004c7af0 (case 0=zero) and 0x004c7f41 (case 7=mode select). SetWindowLongA/SetWindowPos sites: 3 sites each found via reference_to. |
| U-1328 | RESOLVED | DAT_00773204 = startup fullscreen flag, set to 1 by FUN_00499400 at fullscreen path. Not runtime-toggled. |
| U-1329 | RESOLVED | FUN_00499400 = display mode selection orchestrator. Full decompile in 00499400.md. |
| U-1330 | RESOLVED | Site 1 in FUN_00499400: SetWindowLongA(GWL_STYLE=WS_POPUP) + SetWindowPos(SWP_FRAMECHANGED). Sites 2 and 3 in unanalyzed D3D functions (new U-3027, U-3028). |

## Functions analyzed

| RVA | Name | Confidence | New to project? |
|---|---|---|---|
| `0x00499400` | FUN_00499400 | C2 | Yes — first analysis |

FUN_004c7a70 decompiled for context only (writer of DAT_00911f98). Not newly classified — belongs to rw_engine_init domain.

## New uncertainties filed

| ID | Description |
|---|---|
| U-3027 | SetWindowLongA site 0x004c9667 / SetWindowPos 0x004c96c7 — unanalyzed D3D function 0x004c856a..0x004c9cc0 |
| U-3028 | SetWindowLongA site 0x004ca5ff / SetWindowPos 0x004ca65f — unanalyzed D3D function ~0x004ca400+ |
| U-3029 | DAT_00773414 = 3rd field of FUN_004c2ed0 output struct; meaning unknown |

## New stubs filed
0 (SetWindowLongA/SetWindowPos/ShowWindow are external Win32 imports, not stubs)

## IDs consumed
- U-3027..U-3029 (3 of 20 U-slots consumed)
- D-range: unused
- S-range: unused

## Deferred (cap_count)
0 — < 5 DEFERRED → no d3 session spawned. Halt.

## Key globals added to map (first confirmed in d2)

| Address | Semantics |
|---|---|
| `0x007d4130` | Display mode table base (stride 0x14 per entry: width/height/bpp/format/fullscreen_flag) |
| `0x007d4128` | Mode count |
| `0x007d4100` | Current mode index (D3D level) |
| `0x00773410` | Subsystem count |
| `0x007731fc` | Subsystem name table base |
| `0x0077338c` | Saved subsystem index |
| `0x00773390` | Saved mode index |
| `0x0077340c` | Current subsystem index |
| `0x00773200` | Current mode index (app level) |
| `0x00616028` | Display width |
| `0x0061602c` | Display height |

## MCP tool behavior on Mashed_pool15

- `function_at`: works for functions defined in the clone (FUN_00499400, FUN_004c7a70)
- `reference_to`: works correctly; returned all 3 WRITE refs to DAT_00911f98 and all READ refs to IAT entries
- `decomp_function`: works for defined functions
- `search_bytes`: returns 0 results (non-functional on unanalyzed pool clone — consistent with d1 observation)
- `memory_read`: works; used for manual byte inspection of unanalyzed regions

Pool clone limitation: functions in the 0x004c856a..0x004c9cc0 gap are not auto-defined. Cannot decode Sites 2 and 3 without master Ghidra project analysis.

## Scribe outcome
Queued for sweep (window_fullscreen_d2-20260508-1346). Files written:
- `re/analysis/window_fullscreen_d2/FINDINGS.md`
- `re/analysis/window_fullscreen_d2/00499400.md`
- `re/analysis/window_fullscreen_d2/SESSION_END.md`
