# SESSION_END — promote_c2_settings_dialog
**Date:** 2026-05-16
**Session ID:** ma1-ghidra-s8
**Pool slot used:** Mashed_pool7 (read-only; bookmark_add refused; analysis-only session)
**Bucket:** re/analysis/promote_c2_settings_dialog/
**Worktree:** .worktrees/ma1-ghidra-s8/

## Pre-flight

- SHA-256 MASHED.exe.unpatched: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓ matches anchor
- Worktree created on branch discovery/ma1-ghidra-s8 from main b63e920
- Pool 7 opened read-only via mcp__ghidra__program_open (script auto-assigned pool1 to the env which I released-skipped per stale-lock; opened pool7 directly for the pre-assigned slot)
- No s5/s7 overlap claims affect 0x00498*..0x00499* RVAs I selected

## Candidate discovery

Initial sweep for registry-IO candidates (RegOpenKeyA / RegQueryValueExA) found:
- `FUN_00499890` — already C2 in promote_c2_d3d9_window bucket (LogOSVersion).
- `FUN_004fbd78` — registry-based D3DX PSGP probe (HKLM\Software\Microsoft\Direct3D), called by 44 D3DX-library functions; classified as **render/D3DX library territory, not boot/settings**. Skipped to stay within bucket scope.

Pivot: settings persistence cluster around videocfg.bin (the dialog + table-init machinery feeding ConfigLoad/ConfigSave C3 hooks). Eight C1 candidates from settings_config* notes with no overlap with s1..s7 claimed RVAs.

## Functions promoted

| RVA | Proposed name | C1 plate source | C1→C2 |
|---|---|---|---|
| 0x00498d20 | ReadModeFromCombo | settings_config_d3/00498d20.md | ✓ added to hooks.csv |
| 0x00498d60 | PopulateModeCombo | settings_config_d3/00498d60.md | ✓ added to hooks.csv |
| 0x00498f60 | VideoDialogInit | settings_config_d3/00498f60.md | ✓ added to hooks.csv |
| 0x00499170 | SubsystemSelectionChanged | settings_config_d3/00499170.md | ✓ added to hooks.csv |
| 0x004991f0 | VideoSettingsDlgProc | settings_config_d3/004991f0.md | ✓ added to hooks.csv |
| 0x00499740 | SetControlTextFromResource | settings_config_d3/00499740.md | ✓ added to hooks.csv |
| 0x00499400 | VideoSettingsDispatcher | settings_config/00499400.md | ✓ row 364 C1→C2 (row 135 render-dup tracker artifact) |
| 0x00498c00 | VideoModeTableInit | settings_config_d2/00498c00.md | ✓ row 853 render→ now also save tracking via this addendum |

All eight have a `## Why C2` addendum in this bucket citing constants/offsets/DAT at exact RVA, listing callees with confidence, and addressing pre-existing UNCERTAINTYs and STUBs.

## Key findings

**Resolved at promotion time (no new tracker rows needed):**
- U-0828 confirmed RESOLVED (LAB_004991f0 is a real function entry per settings_config_d3 SESSION_END).
- U-0829 RESOLVED (DAT_00773410, DAT_007731f8 written by FUN_00498c00 this batch C2).
- U-0830 RESOLVED (DAT_0077338c, DAT_00773390 written by FUN_00498ea0 SnapshotCurrentVideoSettings C1; field meaning = snapshot_subsystem_index, snapshot_mode_index).
- STUBs S-0821..S-0827 all resolved against mapped callees (see 00499400.md addendum table).

**Open UNCERTAINTYs (already filed, do NOT block C2):**
- U-3007 — controls 1003/1004/1005 no-op handler; dialog resource inspection required.
- U-3008 — checkbox inversion semantic (1047/1048/1051 inverted vs 1049 direct); depth-2 question about FUN_00499400 LAB_00499590.
- U-3009 — DAT_00773418 array bounds; BSS layout inspection required.

These three were filed during settings_config_d3-20260508; carried forward unchanged.

## Tracker mutations

- `hooks.csv`: 6 new rows added (00498d20, 00498d60, 00498f60, 00499170, 004991f0, 00499740) at C2 save; 1 row promoted line 364 (00499400) C1→C2; 1 row note updated line 853 (00498c00) C1→C2 with save-cluster cross-reference.
- `UNCERTAINTIES.md`: no new rows (all 3 open UNCERTAINTYs U-3007/U-3008/U-3009 pre-existed; U-0828/U-0829/U-0830/U-1227/U-1228 noted as resolved-or-carried in addenda).
- `STUBS.md`: no new rows (S-0821..S-0827 already filed; resolutions noted in 00499400.md addendum).
- `DEFERRED.md`: no new rows.
- `re/analysis/CHANGELOG.md`: 8 new lines (one per promotion).

## Refused
None — all 8 candidates passed the C2 gate. No row in re/DEFERRED.md.

## Cap usage
8 functions promoted of the 8-RVA target. Cap satisfied.
