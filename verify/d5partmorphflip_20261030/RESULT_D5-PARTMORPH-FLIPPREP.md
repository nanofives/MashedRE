# RESULT D5-PARTMORPH-FLIPPREP: the D-11077 flip is prepared and measured ON, the branch merges OFF

Verdict: **DECISION**. With both defaults ON, the G-CTRL and B hashes equal the current references, and
`law_check.py` G4 PASSes on the G-CTRL recipe (H_LOG). The opt-outs work. The merge head is OFF and
gives the same references. One PREREG gate line misses: F2's G4 on B_LOG_K0 FAILs on the checker's
600-state-0-row floor only (404 rows, 0 law mismatches). Section 3 explains it. The flip itself is decision
row D-11077 (section 5).

Branch `orch/D5-PARTMORPH-FLIPPREP` from `orch/integration` `0cd4241d4`. PREREG `8fc13e336` (committed alone,
before any arm). No original-side run (no audio endpoint). No tracker touched.

## 1. What the branch carries

- `Core/FaithDefaults.cpp`: rows `{ "PARTMORPH", "1" }, { "CAR_STATE0", "1" }` in `kFaith`, inside
  `#if MASHED_FLIP_D11077`. Each row gives its opt-out through the existing seed loop: `MASHED_NO_PARTMORPH=1`
  / `MASHED_NO_CAR_STATE0=1` skip the row, `MASHED_NO_FAITH=1` drops every row, an explicit `MASHED_<name>`
  still wins. Nothing changes at the two sites (`TrackRenderer.cpp` `PartMorphEnabled` / `CarState0Enabled`
  still read `MASHED_PARTMORPH` / `MASHED_CAR_STATE0` == "1"). Each site gets one comment line naming the
  flip and its opt-out.
- Commit ON `c230b7d7e`: `#define MASHED_FLIP_D11077 1`. That build is "both defaults ON".
- Commit OFF `5bd46a48c` (branch head, what merges): `#define MASHED_FLIP_D11077 0`. Default unchanged.
  **The flip = that define 0 -> 1** (or delete the `#if` / `#endif` pair).
- Runner `run_arms.py` = the merge-train runner `verify/savelink_20261023/run_arms.py` with OUT / ROOT
  changed. It adds four stacks with no behaviour knob: LOG (`MASHED_DBG_CAR_STATE0` + `MASHED_DBG_PARTMORPH`
  logs), NOPM, NOS0, NOB (the opt-outs + LOG).

Exes (worktree builds, `MASHED_NO_DEPLOY=1`, rva-lint NEW=0 on both):

| Exe | Commit | SHA-256 |
|---|---|---|
| EXE_ON | c230b7d7e | `2e37dc2fb4a9f64c52cffdd051b1951e00e99fd827f1def5d509fa5f8029e802` |
| EXE_OFF | 5bd46a48c | `b36477091ecd5799259be246b7aaa67ee8ffb96c07fa30d4a8554a7f7850dfa8` |

## 2. Gates (outputs: `GATES.txt`)

References (`re/orchestrator/merge_train.json` at `0cd4241d4`, the same values the lead gave): G-CTRL
`8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`. merge_train.json and the lead's refs agree.

| Gate | Exe | Arms | Result |
|---|---|---|---|
| F1 ON default unchanged | ON | H_DEF, B_DEF_K0 / K1 / K26 | 8284be46 / ba407a16 / 31fe043e, 1d5bf4f9 x3. PASS |
| F2 ON default reaches the law | ON | H_LOG, B_LOG_K0 | Sim hashes = refs on both. s0_check PASS on both (0 L-model, 0 D-state, 0 decision-id mismatches). law_check G4: **H_LOG PASS** (55296 rows, 53204 state 0, slots 0..3, vtx lerps 39904 lines mismatch 0, DFF pair ADVANTAGE0.DFF 3.31e-10). **B_LOG_K0 FAIL** on the row floor only: 0 law mismatches, vtx 304 lines mismatch 0, DFF pair 3.31e-10, but 404 state-0 rows < 600. Partial |
| F3 opt-outs | ON | B_NOB_K0, B_NOPM_K0, B_NOS0_K0 | Step hash 1d5bf4f9 on all three. NOB writes neither log (both knobs OFF). NOPM: s0 log byte-identical to B_LOG_K0's, s0_check PASS, no pm log. NOS0: no s0 log, pm log G5 `--dormant` PASS (2496 rows, 0 state 0). PASS |
| F4 merge head unchanged | OFF | H_DEF, B_DEF_K0 / K1 / K26 | 8284be46 / ba407a16 / 31fe043e, 1d5bf4f9 x3. PASS |
| F5 merge head is OFF | OFF | B_LOG_K0 | Step 1d5bf4f9, neither log written (both knobs OFF). PASS |

## 3. The B_LOG_K0 row floor, stated plainly

The 600-row floor in `law_check.py` comes from PREREG_D5-PARTMORPH.md:91, where G4 ran on B_PMF_K0, an arm
that forced slot 0 to state 0 on every frame. This PREREG reused the checker unchanged and listed B_LOG_K0
under F2, so by the letter F2 is not met on that arm. No threshold was changed after the fact.

Why the count is lower than the producer's 3192 rows on B_S0_K0: D3-SUBSTATE-FLIP (MASHED_SUBSTATE default
ON) moved the round-1 end on both recipes from tick 399 to tick **1096** (s0_check: `all0 windows=1
[(1096, 1197, 7)]` on B, `[(1096, 14397, 7)]` on H). The L rows now pass through sub-states 3 / 4 / 5 /
6 before 7. That matches the SUBSTATE flip. On the 1200-frame B recipe this leaves 101 state-0 ticks x 4
slots = 404 rows. Every one of them replays exactly. The G-CTRL recipe (14400 frames) gives 53204 rows and
PASSes.

## 4. What is not measured here (PREREG section 4)

- The 13-track run13 table (`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1) on the ON exe. The
  flip draws parts 0x12 / 0x14 and LOD 0 on every car after a round end, so a draw table that runs past a
  round end can move. Not run here.
- D2 arms (D2_DEFH_1..3) on the ON exe. Both knobs moved no sim hash on any arm here or in
  D5-PARTMORPH-PRODUCER, so D2 is not expected to move, but it was not measured.
- No visual parity check (original-side capture) of the flipped draws. The producer's W1 captures (mode 10 /
  rule 0, racing ticks) hold 0 state-0 rows, so they witness nothing about the flipped draws.
- Named gaps carried from D5-PARTMORPH-PRODUCER section 4 (car-load state 0 at `0x00420482`, the result
  camera's own motion, the match-end sub-state) are unchanged.

## 5. Decision D-11077 (filed as L-62 with `roadmap.ps1 decision`)

Flip `MASHED_PARTMORPH` + `MASHED_CAR_STATE0` default ON (`MASHED_FLIP_D11077` 0 -> 1 in FaithDefaults.cpp)?
- (a) flip now: G-CTRL / B unchanged (measured, F1), so the merge train's hash gates pass. The flip pass
  runs the run13 table on the flipped exe and re-bases its reference if rows move.
- (b) flip after a run13 + D2 measurement on EXE_ON in its own step.
- (c) keep OFF.
Recommendation: (b). The sim gates are clean, but run13 is the one default gate this step did not run, and
the visible change (parts 0x12 / 0x14, LOD 0 after a round end) is exactly what a draw table can see.

## 6. Raw data (not committed), `D:/MashedWT/D5-PARTMORPH-FLIPPREP_data`

| File | SHA-256 |
|---|---|
| arms_on / arms_off: H_DEF.step.csv, H_LOG.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms_on / arms_off: H_DEF.gates.csv, H_LOG.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms_on / arms_off: H_DEF.ptrace.log, H_LOG.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms_on: B_DEF_K0/K1/K26, B_LOG_K0, B_NOB_K0, B_NOPM_K0, B_NOS0_K0 .step.csv; arms_off: B_DEF_K0/K1/K26, B_LOG_K0 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms_on/H_LOG.s0.csv | 980cb6b03de24162446c7ceab0c4677bd52d44830ddfeaed3d751d2270db2ccb |
| arms_on/H_LOG.pm.csv / .vtx | c2dd2806457eb2e79241bc9a00d6a1266c6b60fad7d0e5ae980c963688966535 / 7e9b4682de5293be99cc5b495db6fe0fb966326ff6354ce77cbf0bd36b40e385 |
| arms_on/B_LOG_K0.s0.csv = B_NOPM_K0.s0.csv | fc0fad7e73a42e37f5f3a667bd52eff0ba7d6c0d17758ac5f6a4c4128e0a108a |
| arms_on/B_LOG_K0.pm.csv / .vtx | b8bf8baf5dbb4021f026e0b40eb53210604e8adbf434083cda929d086175104f / 6722f7cb9a0f95ef3a652fd6a84de742dad843a17942b34d4553ddadd770187e |
| arms_on/B_NOS0_K0.pm.csv | 4ff0e58e94d5a1250728f9322e22f8702f1a14bd625a10f88cd07af724d7d8ec |
| arms_on/RUNLOG.txt | b9d98cd5b46b611a4d7a8ba1e38d0e513dcffce82b11c27b327b6704397d614d |
| arms_off/RUNLOG.txt | 9c9c6326f533d837c1467a0357d5169d77f443687dae5dc6e81c7b8cd27f0925 |
| exe_on/mashed_re.exe, exe_off/mashed_re.exe | see section 1 |
