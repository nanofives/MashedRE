# Session End — profile_career_d2

**Session:** profile_career_d2-20260505  
**Slot:** Mashed_pool1  
**Date:** 2026-05-05  
**Parent:** BBBB (profile_career-20260503)

---

## Summary

Depth-2 session. Cleared all three deferred items from parent session BBBB:

- **D-4540 (FUN_0040dd60)** — fully reversed. 23-byte guard predicate. Returns non-zero iff `DAT_0063b90c==1` (race concluded) AND `DAT_007f0fcc!=0` (player 0 won). Championship logic fires only when player 1 wins a concluded race.

- **D-4542 (FUN_00410510)** — fully reversed. 820-byte race-end evaluator. Determines winner across all race types (switch on `DAT_007f0fd0` for modes 4,5,7,8,9,10). Sets `DAT_0063b90c=1` and `DAT_0063ba8c=0xb` on race conclusion. Sets `DAT_007f0fcc=1` iff player 0 (first player) wins. Returns winner index (1-based) or -1 (draw) or 0 (no winner yet).

- **D-4541 (FUN_00448220)** — partially reversed. Post-race camera tracking + unlock code generator. Camera targets the sole finished player's vehicle. For mode-2 quick-race: generates a 10-char code at DAT_00899100 using the four seed keys at 0x005cd92c as XOR rotation constants. Deeper callees (camera render, result display) deferred to D-5500..D-5504.

Key discovery: the four alphabetic codes at 0x005cd92c ("PSOLPEBCYB MDXKHDIGNX LFDGUYVDEU TKGFWNCIKR") are **XOR rotation seed keys** for the unlock code generator — not player-facing passwords. U-1553 resolved.

Additional: FUN_0040e4b0 (72B, "sole finished player" getter) documented as C1 in the same session.

---

## Functions catalogued

| RVA | Name | Confidence | File |
|-----|------|-----------|------|
| 0x0040dd60 | Race::GuardConcludedAndP1Won | C1 | FUN_0040dd60.md |
| 0x00410510 | Race::EvaluateResult | C1 | FUN_00410510.md |
| 0x00448220 | Frontend::PostRaceResultCamera | C1 | FUN_00448220.md |
| 0x0040e4b0 | Race::GetSoleFinishedPlayer | C1 | FUN_00448220.md (additional) |

## Trackers updated

- hooks.csv: +4 rows (0040dd60, 00410510, 00448220, 0040e4b0)
- STUBS.md: +S-1860..S-1869 (10 new stubs)
- UNCERTAINTIES.md: +U-1867..U-1873 (7 new); U-1552 partially resolved; U-1553 resolved
- DEFERRED.md: D-4540/4541/4542 marked resolved; +D-5500..D-5504 (5 new)

## Deferred (profile_career_d2-cont1)

- D-5500: FUN_00441990 — alternative result handler (DAT_007f1a50==1 path)
- D-5501: FUN_00446520 — main result display state machine
- D-5502: FUN_00429a70 — race data getter C (unlock code generation)
- D-5503: DAT_0063b900..0x0063bb00 race-state block layout
- D-5504: Camera-follow callees (FUN_0046d4a0, FUN_0046b4f0, FUN_0046d510, FUN_00442e00)
