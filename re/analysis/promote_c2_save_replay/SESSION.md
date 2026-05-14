# promote_c2_save_replay — C1→C2 Batch Session

**Session:** promote_c2_save_replay-20260513  
**Date:** 2026-05-13  
**Model:** claude-sonnet-4-6  
**Pool slot:** Mashed_pool7 (pre-assigned)  
**Promoted:** 21 rows (20 save + 1 render/save duplicate at 0x004963e0)

---

## Promoted rows

| RVA | Name | Subsystem | Notes |
|-----|------|-----------|-------|
| 0x00404e50 | sub_00404e50 (SAVE_LOAD_FN) | save | No uncertainties; opens gamesave.bin for read via FUN_004b3b70 |
| 0x00404f50 | sub_00404f50 (SAVE_WRITE_FN) | save | No uncertainties; opens gamesave.bin for write via FUN_004b3bb0 |
| 0x00404f80 | sub_00404f80 (gamesave.bin exists) | save | NEG/SBB/NEG bool idiom; S-0280 stub for FUN_00550b00 cleared (FUN_00550b00 promoted this session) |
| 0x00404e80 | Save::DeserializeFromBuffer | save | U-3558 (stride-scatter identity) U-3560 (profile ptr) catalogued; mechanics complete |
| 0x00404ee0 | Save::SerializeToBuffer | save | U-3558 (stride-gather identity) U-3559 (tail author) catalogued; DEADBEEF @ 0x00404F37 |
| 0x004099a0 | Save::AutosaveTrigger | save | U-1550 (state-4 semantics) U-1551 (adjacent globals distinct) catalogued; leaf, no callees |
| 0x004099e0 | sub_004099e0 (global setter) | save | Trivial leaf; writes param_1 to DAT_008a95a0 |
| 0x0040dd60 | Race::GuardConcludedAndP1Won | save | 23-byte leaf; bitwise idiom; resolves D-4540 S-1540 (noted in analysis) |
| 0x004963e0 | sub_004963e0 (CONFIG error-print) | save | fputs wrapper; no uncertainties |
| 0x004963e0 | sub_004963e0 (log writer) | render | Same function; C1 in render subsystem promoted simultaneously |
| 0x00496400 | sub_00496400 (CONFIG debug-log) | save | U-0827 (vsprintf variant) catalogued; mechanics complete |
| 0x00498910 | sub_00498910 (filename-init) | save | Leaf; copies "videocfg.bin" into 0x007731e8 |
| 0x00498950 | sub_00498950 (CONFIG_LOAD_FN) | save | No uncertainties; _fread(0x773208, 1, 0x200, file) |
| 0x004989b0 | sub_004989b0 (CONFIG_SAVE_FN) | save | No uncertainties; _fwrite(0x773208, 1, 0x200, file) |
| 0x004a4541 | sub_004a4541 (_fsopen wrapper) | save | 18-byte thunk; fixes shflag=0x40 (_SH_DENYNO) |
| 0x004b3b70 | sub_004b3b70 (file-read wrapper) | save | S-0281/S-0282/S-0283 are depth-2 stubs; mechanics complete |
| 0x004b3bb0 | sub_004b3bb0 (file-write wrapper) | save | U-0287 (ESI 4th-arg — partially resolved); U-0288 resolved in 004cbe80.md |
| 0x00550b00 | FUN_00550b00 (VFS file-exists router) | save | U-2330/U-2331/U-2332 catalogued; S-2320 cleared (FUN_00550980 promoted this session) |
| 0x00550910 | FUN_00550910 (VFS stream close) | save | U-3561 (arg identity) U-3562 (IAT entries) catalogued; mechanics complete |
| 0x00550980 | FUN_00550980 (VFS fread-style) | save | No open uncertainties; vtable[3] dispatch; fread element-count semantics |
| 0x00550bc0 | FUN_00550bc0 (stream ctx accessor) | save | 8-byte leaf; returns *(ctx+8); name uncertain (VFS_GetType vs VFS_GetMode) — [UNCERTAIN U-3562] covers this |

---

## Left at C1 (6 rows — reasons)

| RVA | Name | Reason |
|-----|------|--------|
| 0x00499400 | sub_00499400 (video-settings dispatcher) | Optional DialogBoxParamA(0x65) path not fully documented |
| 0x00430290 | Championship::Complete | U-1547 U-1548 U-1549 — undocumented profile fields (STOP-AND-ASK: profile serialization) |
| 0x0042a920 | Frontend::PostTrophyEvent | U-1552 open |
| 0x00410510 | Race::EvaluateResult | U-1867..U-1872 (6 open uncertainties); 820B complex function |
| 0x004cbe80 | FUN_004cbe80 (RW stream write) | U-2327/U-2328/U-2329 + D-6880; sufficient for C2 but skipped to stay at 20 save targets |
| 0x0040de00 | thunk_FUN_004117b0 | S-3654..S-3657 open; replay HOLD row (replay_record-20260503) still in SCRIBE_QUEUE Queued section (HOLD is superseded but not yet cleared) |

---

## SCRIBE_QUEUE HOLD observation

`replay_record-20260503` in `re/SCRIBE_QUEUE.md` (Queued section) still carries  
`HOLD=missing-per-rva-files` but is `superseded-by=replay_record-20260511-1200`.  
The superseding row was drained by `sweep-20260511-1822`; all per-RVA files exist.  
**This HOLD row is not blocking** any of the 20 save promotions (its RVAs are under  
subsystem "vehicle"). The thunk at 0x0040de00 was conservatively left at C1 as a  
precaution given its proximity to the replay HOLD. The HOLD row itself needs to be  
moved from Queued to Drained in a future ghidra-sweep or manual cleanup — but that  
requires user decision, not this session.

---

## Stub clears this session

- **S-0280** (FUN_00550b00 in 00404f80 notes): callee promoted to C2
- **S-2320** (FUN_00550980 in 00550b00 and 004cbe80 notes): callee promoted to C2

---

## Uncertainty notes

- **U-0288** (004b3bb0): resolved by 004cbe80 analysis — FUN_004cc160 receives stream-ctx ptr, not raw handle
- **U-0287** (004b3bb0): still open — ESI 4th-arg semantics partially resolved (not consumed by callee) but not fully closed
