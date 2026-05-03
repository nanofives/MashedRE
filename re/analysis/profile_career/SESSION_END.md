# Session End — profile_career

**Session:** profile_career-20260503  
**Slot:** Mashed_pool9  
**Date:** 2026-05-03

---

## Summary

Session BBBB. Goal: identify CAREER_FN. Three strategies attempted; string search (strategy 1) found nothing in PE — user-visible strings live in `.piz` archives. Call-graph from SAVE_LOAD_FN (strategy 3) succeeded.

Primary CAREER_FN identified as **FUN_00430290** (CHAMPIONSHIP_COMPLETE_FN, 986 bytes), the per-track completion handler called by the end-of-race flow. Two supporting functions fully reversed: AUTOSAVE_TRIGGER (FUN_004099a0) and TROPHY_EVENT_POST (FUN_0042a920).

Key discovery: gamesave.bin layout corrected — session P had a decimal arithmetic error (0x24fa0 = 151,456, not 150,432). Track progression table confirmed at offset 0x24A40 (1,312 bytes); replay buffer (150,076 bytes) at offset 0x4 via DAT_008a94a8 pointer. Feature unlock table at DAT_007f105c already labeled by hud_frontend_d2 session; all 12 entries start unlocked. Four alphabetic unlock codes at 0x005cd92c found in data section; role deferred.

---

## Functions catalogued

| RVA | Name | Confidence | File |
|-----|------|-----------|------|
| 0x00430290 | Championship::Complete | C1 | 00430290.md |
| 0x004099a0 | Save::AutosaveTrigger | C1 | 004099a0.md |
| 0x0042a920 | Frontend::PostTrophyEvent | C1 | 0042a920.md |

## Trackers updated

- hooks.csv: +3 rows (00430290, 004099a0, 0042a920)
- STUBS.md: +S-1540 (FUN_0040dd60 guard)
- DEFERRED.md: +D-4540 D-4541 D-4542
- UNCERTAINTIES.md: +U-1547 through U-1553 (U-1548 and U-1551 resolved in-session)

## Deferred (profile_career-cont1)

- D-4540 FUN_0040dd60 — guard predicate in FUN_00430290
- D-4541 FUN_00448220 — 17KB frontend/career-menu; reads unlock codes; consumes trophy event IDs
- D-4542 FUN_00410510 — race-end evaluator; determines whether championship handler fires
