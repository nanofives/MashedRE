# DRAFT for ROADMAP.md §D3 (not applied; closing D3 is Mariano's decision)

Insert directly under the §D3 pass criteria, above "D3 closure state 2026-10-09":

---

#### D3 closure state 2026-10-21 (d3close): **every D3 pass criterion is met on the default build. D3 is READY TO CLOSE, pending Mariano's decision.**

Record: `verify/d3close_20261021/RESULT_d3close.md` (PREREG `PREREG_d3close.md` + amendment 1, committed before the arms they
govern). Default re-scored as is: FAITH stack + u9774 five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` +
`MODE_LIVE` + `SLOT_TYPE_LIVE` + `SUBSTEP_OUTER` (`Core/FaithDefaults.cpp`), recipe F (`MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`).
No code, knob or band changed. G-CTRL `f78de789 / 43bbe0fb / 161b1a6a` (= soflip).

| third | criteria | state 2026-10-21 |
|---|---|---|
| AI (WS-C) | (a) met (`Ai/AiStandalone.cpp:299-305` 0x00443300, `:405-414` + `:506` 0x00443dc0 incl. phase 8). **(b) met 9/9**: A 3/3, B 3/3, C 3/3 on `B_DEF_K0/K1/K26` (step `9bd86ffe / 433abcd2 / 1a1ff727`, `re/tools/b_envelopes.json`). (c) met (`D3d9Render/TrackRenderer.cpp:5182-5198`). (d) met (no `MASHED_AI_TICK` reader). **(e) met 3/3 on regime 0**: launch 1425.7 / 2052.5 / 2055.0, ft_median_m0 2551.6 / 2052.5 / 2278.1, every value equal to the original's reference at the printed 0.1, and each car leaves mode 0 at the original's own `k` (100 / 23 / 39) | **met** |
| Powerups (WS-D) | (a) (b) met. (c): decision CLEAN on 11/11 captures; contact as recorded, g3 = the R_FLAME `0x45afcc` 2-of-546 residue (recorded, not passed). m1 / m2 / s1 / s2 / b3 re-captured from their argv (the `.puhook.csv` files were lost); box state 3 forced by `b3r` is CLEAN, and its `nobox` control gives 592 mismatches | **met** (R_FLAME residue named) |
| Modes (WS-G) | live oracle GREEN, 0 mismatches on all 11 rules (12 runs); APPEND 1 each on rules 4 / 7 / 8 / 9; `ruleengine_test` 67 checks and `racemodes_test` 96 checks pass on this tree | **met** |

Guards on the same default: D2 PASS 3/3 alone (slip1500 0.1900 x3, drvmed 1965.67 / 1965.67 / 1970.58, H1 398/400). 13 tracks
cited from soflip (877 rows, 0 RED; no code changed since).

**What closing D3 would carry forward (non-gating, named):**

- **U-9779 / U-9781**: the round end at k=220 on the B recipe. The original goes to sub-state 7 and car 3's position freezes
  while its velocity decays (writer `0x0046ea62`). The port stays in sub-state 6. This is outside the 220-call (b) window.
- **U-9782**: the retry counter of `FUN_004709a0` (`inc ebp` at `0x00470b0a` / `0x00470bde`, test `0x00470ab0`) against
  `StepSubstep`.
- **U-9780**: car 2's vx at k=218 on B K0, 1 ulp (filed under the deleted fiber form; re-check under `SUBSTEP_OUTER`).
- **U-9775**: the D2 driving-median of the fully faithful player path (`COLL_LUT` + the u9774 five) is 1890.05, below the
  1893.63 floor. `COLL_LUT` stays off by default.
- **Snapshot / restore after relocation**: the port-only snapshot / restore that `SUBSTEP_OUTER` keeps (substep RESULT §5.1).
- **Box-state producers**: `FUN_0045ba00`'s callers `FUN_004111c0` / `FUN_0040e590` / `FUN_00424eb0` / `FUN_00422fd0` /
  `FUN_0040be50` are not ported. The standalone writes only 1 / 4 at race start (`TrackRenderer.cpp:6880-6885`), so states 2 / 3
  arise only in replay, never live.
- **R_FLAME** 2-query residue on g3 (the per-group sort `FUN_0045ac40` is unported).
- **Modes exits never reached** by any oracle run: the `motion0 != 0` exits of rules 8 / 9 / 10, the rule-5 `alive[0] == 0`
  exit, the rule-7 `participants-1 <= dead` exit, the rule-9 `gap >= 0.9` arm.
- **D-11078** (two-car original capture for the one-vehicle-TXD-per-race question, U-9730) and **D-11079** (unify the duplicate
  exe bodies of `0x004c4680` / `0x00532b80`, U-9741).
- **D1-residue R1-R2**, owed before D5: R1 an original-side capture at a matched, frame-synced pose (`drawlist_diff.py` GREEN or
  every row cited); R2 the verbatim race-camera pose from `Race/RaceCamera.cpp` drives the renderer.
- **Evidence hygiene**: the re-captured `.puhook.csv` (`D:\MashedWT\d3close_data\pu\`, 5.36 MB) are not committed, and
  `re/tools/pu_replay/sweep.ps1` still lists the lost paths.

**Decision needed (Mariano):** close D3 on this record and carry the list above forward, or name a criterion to tighten
first. If closed: open D4, and update CLAUDE.md "Current phase" and `re/NEXT_SESSION.md` in the same commit.
