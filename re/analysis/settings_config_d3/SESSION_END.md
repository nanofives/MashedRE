# SESSION_END — settings_config_d3
**Date:** 2026-05-08  
**Session ID:** settings_config_d3-20260508-0019  
**Pool slot used:** Mashed_pool7 (pool14 was locked; fallback)  
**Bucket:** re/analysis/settings_config_d3/

## Pre-flight

- SHA-256 MASHED.exe: `bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e` ✓
- Pool14 locked (`.lock~` 0-byte stale lock blocked MCP open) → used pool7
- No batch 23 S6 active (sweep-20260507-2002 and sweep-20260507-2353 both released)
- Threshold check: parent settings_config_d2 filed 6 DEFERRED items (D-3580..D-3585) ≥5 ✓

## DEFERRED discrepancy note

D-3580..D-3585 were listed as "filed" in `settings_config_d2/SESSION_END.md` but are **absent from `DEFERRED.md`** — the settings_config_d2 bucket was never queued for scribe, so the DEFERRED.md writes were never executed. This session directly analyzes the items from the d2 SESSION_END.md list. No entries to remove from DEFERRED.md.

**Status of each d2 deferred item:**
| D-ID (d2 list) | RVA | Disposition |
|---|---|---|
| D-3580 | 0x004a504f | Analyzed this session — CRT _woutput (C1) |
| D-3581 | 0x004c2e40 | Pre-empted by rw_engine_init_d3 (GetCurrentSubSystem C1) |
| D-3582 | 0x004c2f00 | Analyzed this session — RW_GetCurrentMode (C1) |
| D-3583 | 0x004c2de0 | Pre-empted by rw_engine_init_d3 (GetNumSubSystems C1) |
| D-3584 | 0x004c2e10 | Pre-empted by rw_engine_init_d3 (GetSubSystemInfo C1) |
| D-3585 | 0x004c2ea0 | Pre-empted by rw_engine_init_d3 (GetNumVideoModes C1) |

## Functions processed

| RVA | Proposed name | Classification | Notes file |
|---|---|---|---|
| 0x004a504f | CRT_woutput | C1 | 004a504f.md |
| 0x004c2f00 | RW_GetCurrentMode | C1 | 004c2f00.md |
| 0x004991f0 | VideoSettingsDlgProc | C1 | 004991f0.md |
| 0x00498d20 | ReadModeFromCombo | C1 | 00498d20.md |
| 0x00498f60 | VideoDialogInit | C1 | 00498f60.md |
| 0x00499170 | SubsystemSelectionChanged | C1 | 00499170.md |
| 0x00499740 | SetControlTextFromResource | C1 | 00499740.md |
| 0x00498d60 | PopulateModeCombo | C1 | 00498d60.md |

## Key findings

**U-0828 resolved:** `LAB_004991f0` confirmed as real function entry (body_start = 004991f0, body_end = 004993b5, 453 bytes). The video settings dialog procedure is fully mapped.

**Dialog control map (resource ID 101 / 0x65):**
| Control ID | Role |
|---|---|
| 1 (IDOK) | OK — apply and close |
| 2 (IDCANCEL) | Cancel — dismiss |
| 1000 | Subsystem combo box |
| 1001 | Mode combo box |
| 1047 | Checkbox A → DAT_007733a8 (inverted) |
| 1048 | Checkbox B → DAT_007733a4 (inverted) |
| 1049 | Checkbox C → DAT_007733ac (non-inverted) |
| 1050 | Static label |
| 1051 | Checkbox D → DAT_007733b0 (inverted) |
| 1003/1004/1005 | No-op controls [UNCERTAIN U-3007] |

**Mode filter** (from PopulateModeCombo): only modes with `flags[+0xC] != 0 AND width >= 640 AND height >= 480` are shown.

**RW GetCurrentMode** (0x004c2f00): sends command ID 0xa via the RW D3D vtable dispatcher. Completes the RW video command ID table.

**DAT_00773418** new discovery: mode-index-to-combo-position map written by PopulateModeCombo, read back by VideoDialogInit to set initial selection.

## New UNCERTAINTIES

- U-3007: dialog controls 1003/1004/1005 purpose unknown
- U-3008: checkbox flag semantic meanings (fullscreen/vsync/AA/etc)
- U-3009: DAT_00773418 array bounds

## New DEFERRED

None. All callees are Win32, CRT, or already analyzed.

## New STUBS

None.

## Cap usage

8 functions processed, 0 capped.

---

## W6 follow-up session — HALT

**Date:** 2026-05-08  
**Session ID:** settings_config_d3-W6 (prompt W6)  
**Pool slot assigned:** Mashed_pool14  
**Halt reason:** `Halt if <5 DEFERRED` threshold triggered — 0 unanalyzed parent deferred rows remain.

Pre-flight passed (SHA-256 ✓, pool14 unlocked). Threshold check failed:
- Parent bucket settings_config_d2 filed D-3580..D-3585 (6 items).
- All 6 resolved by this session's prior run (settings_config_d3-20260508-0019).
- D-3580..D-3585 never reached DEFERRED.md (scribe path: d2 bucket was never queued, d3 analyzed directly from SESSION_END notes).
- settings_config_d3 itself filed 0 new DEFERRED rows.
- DEFERRED.md currently contains 0 settings_config entries.

**Result:** No work performed. No SCRIBE_QUEUE entry. No per-RVA files written.
