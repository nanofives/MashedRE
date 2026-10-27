# RESULT D4-WSB-NARROW-FLIP: `MASHED_NARROW_DOT_ORDER` + `MASHED_NARROW_PAIR_INT` + `MASHED_LEAVES1_X87` default ON (L-37 = yes, 2026-10-27)

**Verdict: DECIDED-FLIP ready (GREEN), hash-neutral.** The three knobs are default ON together. On the flipped exe,
G-CTRL and B equal the current refs, D2 is 3/3 in band at the current default numbers, and the 13-track table is
identical to the run-13 reference. A liveness probe shows that the default exe runs the flipped narrow bodies and that
the opt-outs bring back the pre-flip body. `re/orchestrator/merge_train.json` is not edited and nothing is enqueued.

Knob evidence: `verify/wsbnarrow_20261027/RESULT_D4-WSB-NARROW-FIX2.md` (8/8 rows GREEN knob-ON).

## 1. The change

- `Collision/NarrowFixKnobs.h`: `NarrowDotOrder()` is ON unless `MASHED_NO_NARROW_DOT_ORDER` is set and not `0`.
  `NarrowPairInt()` is ON unless `MASHED_NO_NARROW_PAIR_INT` is set and not `0`.
- `Collision/RwpSolverLeaves1.cpp`: `Leaves1X87()` is ON unless `MASHED_NO_LEAVES1_X87` is set and not `0`.
- This is the same opt-out pattern as `Ai_RaceTimerRunEnabled` (`MASHED_NO_A5_RACE_TIMER_RUN`). The old enable
  variables `MASHED_NARROW_DOT_ORDER`, `MASHED_NARROW_PAIR_INT` and `MASHED_LEAVES1_X87` no longer do anything.
- Comment-only changes: the per-site `[D4-WSB-NARROW-FIX]` markers in `RwpSolverCore14/17/18/20.cpp` and
  `RwpSolverGlue5.cpp`, and the knob headers in `RwpSolverLeaves1.cpp` / `RwpSolverLeaves1X87.cpp`, now say
  default ON and name the opt-out.
- The SSE2 bodies of `0x00566200` / `0x005667c0` stay, because the opt-out needs them. `RwpSolverLeaves1X87.cpp`
  said the flip had to delete them. That deletion now waits on retiring the opt-out. `lint_rva_bodies.py` does not
  count the x87 TU, so the dual-copy count does not change.
- No tracker change. The 8 FIX2 rows can move only after a re-classify pass on this default. That is not part of this
  step.

Branch base: `orch/integration` `725474f9c`, then `orch/integration` `cb1ba12de` merged in
(`git -c merge.autoStash=false`). `git merge-base --is-ancestor 8ba60342e HEAD` holds, so the D4-FU-MENUCLOCK-FLIP
merge commit is in the tree that was built and gated. Build: `MASHED_NO_DEPLOY=1 mashedmod\build.bat`, Build OK, exe
`d1719409`, `.asi` `a2d51752`. `original\mashed_re_dev.asi` was not redeployed. The `.asi` was copied into the private
root only.

## 2. Gates and full set (driver `flip_driver.py`, merge_train.json runner and scorers)

| check | measured | ref | verdict |
|---|---|---|---|
| G-CTRL `H_DEF` step / gates / ptrace | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | PASS (unchanged) |
| B `B_DEF_K0 / K1 / K26` | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | PASS (unchanged) |
| (b) `vsum.py` | 3/3 arms 3/3 (A / B / C PASS) | 3/3 | PASS |
| (e) `ai_speed_env.py --check` | v1 / v2 / v3 PASS | PASS | PASS |
| D2 x3 solo, `score_d2_v2.py port` | 3/3 in band, see section 3 | band | PASS 3/3 |
| 13 tracks, `run13.py` + `cites_rres.json` vs `verify/colllut_20261026/tracks_score.txt` | 39 rows identical, every row CITED, 0 red, INSTRUMENT GATES: PASS | identical | PASS |

**Opt-out control** (`run_arms.py` copy, stack `NONF` = the three `MASHED_NO_*`): `H_NONF` `5943c5f1 / 364350ae /
52052890`, `B_NONF_K0 / K1 / K26` `3a03670c / e85ea6fb / 53778664`. These are the same as the flipped default, so
G-CTRL and B cannot tell the two bodies apart. Section 4 covers that.

## 3. D2 (solo window, `mashed_lock.py run --solo`)

| arm | motion_diag lines / sha8 | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| `D2_DEF_1` | 1628 / `ae9532cc` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |
| `D2_DEF_2` | 1627 / `4ae5c331` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |
| `D2_DEF_3` | 1627 / `4ae5c331` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |

All three arms are over 1600 lines on the first pass, so there was no re-run. The numbers equal the current D2 default
(0.1942 / 0.2492 / 1896.74). `ae9532cc` is byte-equal to D4-FX-FLIP-DEADCOAST's `D2_DEF_3`. No build ran during the D2
window, and the solo lock held the machine lock for the three arms. The merge train that landed MENUCLOCK had released its
test slot before the driver started (the slot holders at 10:39 local were two other projects, `driver.txt` head).

## 4. Liveness: the default exe runs the flipped bodies

G-CTRL, B, D2 and the 13 tracks do not move, so they do not show that the flip is live. `run_c4n_flip.py` (a copy of
`run_c4n_fix2.py` with its own root, data and label, which also pops the `MASHED_NO_*` variables) ran the canonical
census scenario with the in-process full-output A/B probe (`MASHED_HOOK_ONLY=L2_C4N_<rva>`). It used the two rows whose
OFF control showed diffs in FIX2. One boot per row, each with RACE_OK on attempt 1, `A/B-IS-REAL`, `installed=1`, and
`ret_diff 0`.

| arm | env | row | calls | ok | diff | verdict |
|---|---|---|---|---|---|---|
| DEF | none | `00575c60` | 7553 | 7553 | **0** | flipped body = original |
| DEF | none | `00575fe0` | 2689 | 2689 | **0** | flipped body = original |
| NOF | the three `MASHED_NO_*=1` | `00575c60` | 5825 | 4164 | **1661** | pre-flip defect back |
| NOF | the three `MASHED_NO_*=1` | `00575fe0` | 2113 | 871 | **1242** | pre-flip defect back |

These rows run through `NarrowDotOrder()` / `NarrowPairInt()`. `[UNCERTAIN]`: `Leaves1X87()` (`0x00566200` /
`0x005667c0`) was not probed separately on the default exe. Its default comes from the same opt-out expression,
compiled in the same build. A probe of those two RVAs on this exe would close it.

## 5. Files

Committed here: this RESULT, `flip_driver.py` (copy of `verify/fxflip_20261027/flip_driver.py`, own paths, adds the
`ctrl` stage), `run_arms.py` (copy of `verify/savelink_20261023/run_arms.py`, adds stack `NONF`), `run_c4n_flip.py`,
`driver.txt` (driver stdout, the runner's SyntaxWarning lines removed), `c4n_driver.txt`, `score_d2.txt`,
`tracks_score.txt`, `vsum_b.txt`, `e_check.txt`, `raw_sha256.txt`.

Raw data is not committed. It lives in `D:\MashedWT\D4-WSB-NARROW-FLIP_data` (`arms\` holds the gate step / gates /
ptrace files, the control arms and the D2 motion_diag logs. `tracks\` holds the replay draws. `c4n\` holds the probe
logs, launches and manifests). The SHA-256 of every cited raw file, plus the exe and `.asi`, is in `raw_sha256.txt`.
Roots: `D:\MashedWT\D4-WSB-NARROW-FLIP_root`, `D:\MashedWT\D4-WSB-NARROW-FLIP_root13`.

## 6. For L0

merge_train.json refs stay the same: G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c / e85ea6fb / 53778664`. The
flip is hash-neutral on every gate, so it can merge with a normal re-gate. The opt-outs are `MASHED_NO_NARROW_DOT_ORDER=1`,
`MASHED_NO_NARROW_PAIR_INT=1` and `MASHED_NO_LEAVES1_X87=1`. Follow-up: run re-classify for the 8 FIX2 rows on this
default, and probe the Leaves1 x87 pair on the default exe.
