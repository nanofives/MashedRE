# SESSION_END — boot_app_init_d5-20260508-1722

## Summary
- Session: boot_app_init_d5-20260508-1722
- Pool slot: Mashed_pool4 (pool0 had stale lock from prior GG6 session; pool4 used)
- Input: D-9520 (36 RVAs from boot_app_init_d4-cont1)
- Drift-skips: 0 (all 36 RVAs were untracked)

## Work done
- 8 thunks identified: 00471eb0, 00494ef0, 00494f20, 00421590, 0047ba10, 004b6550, 004955c0, 004963d0
- 28 non-thunks decompiled and analyzed
- 36 × first-pass .md files written to re/analysis/boot_app_init_d5/
- 0 functions deferred (cap_count=0; no function exceeded 1500 bytes)
- D-9520 fully consumed

## ID ranges used
- S-3320..S-3334 (15 S-IDs assigned to thunk targets + new callee stubs)
- U-3327..U-3333 (7 U-IDs assigned to new unmapped callees)
- U-3334, U-3335 used as uncertainty markers in analysis notes (not new functions)
- D range: none used (cap_count=0)

## New callees discovered (S-IDs)
| ID | RVA | Seen in |
|---|---|---|
| S-3320 | 0x00471df0 | 00471eb0.md |
| S-3321 | 0x00493f70 | 00494ef0.md |
| S-3322 | 0x00494460 | 00494f20.md |
| S-3323 | 0x004210f0 | 00421590.md |
| S-3324 | 0x00496970 | 0047ba10.md |
| S-3325 | 0x004b6700 | 004b6550.md |
| S-3326 | 0x00495580 | 004955c0.md |
| S-3327 | 0x00496370 | 004963d0.md |
| S-3328 | 0x0045e040 | 00467010.md |
| S-3329 | 0x00466a50 | 00467010.md |
| S-3330 | 0x004e5700 | 0042c2a0.md |
| S-3331 | 0x0041ec00 | 0041ffb0.md |
| S-3332 | 0x004768c0 | 0041ffb0.md |
| S-3333 | 0x00558180 | 00558240.md |
| S-3334 | 0x00558400 | 00558240.md |

## New callees needing analysis (U-IDs)
| ID | RVA | Seen in | Notes |
|---|---|---|---|
| U-3327 | 0x004d41e0 | 00496ce0.md | takes single ptr, called twice with 007730xx globals |
| U-3328 | 0x00412010 | 0045b930.md | subsystem shutdown #1 |
| U-3329 | 0x0045b350 | 0045b930.md | subsystem shutdown #2 |
| U-3330 | 0x00458880 | 0045b930.md | subsystem shutdown #3 |
| U-3331 | 0x004593b0 | 0045b930.md | subsystem shutdown #4 |
| U-3332 | 0x00404820 | 0045b930.md | subsystem shutdown #5 |
| U-3333 | 0x004840f0 | 004841d0.md | bulk factory/alloc, called 94× no-args |

## Uncertainties filed
- [UNCERTAIN] 00558240: texture dimension growth formula; U-3334 marker in notes
- [UNCERTAIN] 00484170: 6-dword record field semantics; U-3335 marker in notes

## DEFERRED
- D-9520 fully consumed — all 36 RVAs analyzed; remove from DEFERRED.md
- No new D-rows (cap_count=0)

## Scribe instruction
Bucket: boot_app_init_d5
36 per-RVA .md files at re/analysis/boot_app_init_d5/
Append 36 rows to hooks.csv (C1, new, subsystem=boot)
Write 36 Ghidra bookmarks
Remove D-9520 from DEFERRED.md
