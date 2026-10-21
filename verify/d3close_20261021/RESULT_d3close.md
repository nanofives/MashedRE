# RESULT d3close: every D3 gate re-scored on the soflip default (2026-10-21)

**Verdict: every D3 pass criterion is met on today's default. Every gating prediction in the PREREG held. There is one
deviation, inside the band (D2 run 3, section 6). No code, knob or band changed.** Closing D3 is Mariano's decision. The draft
block for ROADMAP.md §D3 is in `ROADMAP_D3_BLOCK.md`.

Branch `work/d3close` from `665951c20`. PREREG `PREREG_d3close.md` (`e20f389ae`), amendment 1 (`4b308248e`), both committed
before the arms they govern. Build `MASHED_NO_DEPLOY=1 mashedmod\build.bat`, exe `053df088` (same source as soflip's
`50558a73`, different worktree path; every behavioural hash below equals soflip's). Private root `D:\MashedWT\d3close_root`
(265 files, 506,063,661 bytes = `original/`). Evidence `D:\MashedWT\d3close_data` (`MOVED_OFF_GIT.txt`), small summaries
copied here. Every arm self-exited on its first attempt: no crash, no piz collision, no re-run, no orphan process
(`RUNLOG.txt`). The original-side runs were serialized: 4 + 1 re-captures, then 12 oracle runs.

## 1. Gate table

| third | criterion | scorer | measured | band | verdict | evidence |
|---|---|---|---|---|---|---|
| AI | (a) `FUN_00443300`, `FUN_00443dc0` ported | code | `Ai/AiStandalone.cpp:299-305` `CatmullRom` (0x00443300), `:405-414` `SplineLookahead` (0x00443dc0 phases 1-7) + `:506` phase 8 wall-march; TU in `mashedmod/exe_sources.rsp:11`. The `.asi` forms `AiSplineHooks.cpp:351` / `AiNavHooks.cpp:431` are the dev hooks. `AiStandalone.cpp:411` still calls phase 8 "a follow-up", which is stale, since `:506` ports it | ported | **met** | source |
| AI | (b) ctrl bytes per regime | `vsum.py` (`ai_ctrl_window.check_regime`, `re/tools/b_envelopes.json`) | K0 A 3/3, K1 B 3/3, K26 C 3/3; step `9bd86ffe / 433abcd2 / 1a1ff727`; flag_a368 {0} / {0,1} / {0,2} | per-regime envelopes | **met 9/9** | `vsum_b.txt` |
| AI | (c) opponents driven by ported ctrl bytes | code | `D3d9Render/TrackRenderer.cpp:5182-5198`: `Ai::Ai_Standalone_Tick()` (FUN_00418860) every race frame, ctrl block fed to `StepCar`; the Option B motion model is reachable only under `MASHED_REAL_PHYSICS=0` | no scaffold on default | **met** | source |
| AI | (d) `MASHED_AI_TICK` deleted | code | no `getenv("MASHED_AI_TICK")` in `mashedmod/src`; the only hit is the deletion comment `TrackRenderer.cpp:5185` | deleted | **met** | grep |
| AI | (e) speed response, regime 0 | `re/tools/ai_speed_env.py --check` | `B_DEF_K0` (`regime0=1` all cars): launch **1425.7 / 2052.5 / 2055.0**, ft_median_m0 **2551.6 / 2052.5 / 2278.1**, `k_own` 100 / 23 / 39 | ±2% of ref 1425.7 / 2052.5 / 2055.0 and 2551.6 / 2052.5 / 2278.1 | **met 3/3, every gated value equal to the reference at the printed 0.1** | `e_check_B.txt`; witness `H_DEF` identical (`e_check_H_SR.txt`) |
| Powerups | (a) dispatcher every frame | code | `TrackRenderer.cpp:5865` `TickPowerupDispatch(in.dt)` per race frame; `:6871-6878` fills all 4 slots | per frame | **met** | source |
| Powerups | (b) slots 1..3 own and fire | code + recorded run | `TickPowerupDispatch` passes `pu_ai_` to slots 1..3; AI fire decision `FUN_00415220` ported (`Ai/AiStandalone.cpp:827`); live fire recorded 2026-09-27 (180 s race, slots 1-3 armed and fired) | | **met** (no new live run) | source; ROADMAP 2026-09-27 |
| Powerups | (c) 9 types, decision + contact | `run_pu.py sweep` (= `re/tools/pu_replay/sweep.ps1` logic) over pu_replay built from this tree | decision **CLEAN 11/11**; contact CLEAN on c2 c3 g2 g4 m1r m2r s1r s2r, **g3 DIVERGES only on R_FLAME query `0x45afcc` 546 vs 548** (byte-identical replay to 2026-10-02), o3 / o4 no contact channel. Box gate: `b3r` state 3 for 483 rows, CLEAN; `MASHED_PU_FORCE=nobox` **592** decision mismatches + contact DIVERGES | as recorded 2026-09-28d | **met**, with the R_FLAME 2-of-546 residue recorded, not passed | `pu_sweep.txt`, `pu_b3.txt` |
| Modes | (a) live oracle, 11 rules | `scenario_launch.py --oracle` (stock original vs the ported law) | **GREEN, 0 mismatches on all 12 runs** (rules 0..10, rule 10 twice) | 0 mismatches | **met** | `modes_oracle.txt` |
| Modes | (b) APPEND fires | same | appends 1 / 1 / 1 / 1 on rules 4 / 7 / 8 / 9 | > 0 | **met** | `modes_oracle.txt` |
| Modes | (c) round target from the rule | code + unit test | `TrackRenderer.cpp:7241-7281` match start = `FUN_0040b180` seed + `FUN_00410510` target (`Race/RuleEngine.cpp:144`); `ruleengine_test` ALL PASS (67 checks) | | **met** | `unit_tests.txt` |
| Modes | (d) rule engine default-on | code + unit test | `TrackRenderer.cpp:7235-7237` (`MASHED_RULE_ENGINE=0` is the only off switch), `:7255`; `racemodes_test` GREEN (96 checks) | | **met** | `unit_tests.txt` |
| D2 | D2 band, 3 runs alone | `verify/d2rebase_20261019/score_d2_v2.py port` | slip1500 0.1900 x3; slip2000 0.2550 / 0.2550 / 0.2549; drvmed 1965.67 / 1965.67 / 1970.58; H1 398/400 x3; 1632..1636 lines | slip1500 0.18396..0.20586, slip2000 0.24555..0.25557, drvmed 1893.63..1997.46 | **PASS 3/3** | `score_d2_def.txt` |
| default identity | G-CTRL | sha8 | `H_DEF` step / gates / ptrace `f78de789 / 43bbe0fb / 161b1a6a` | = soflip | **equal** | `RUNLOG.txt` |
| 13 tracks | run13 | | not re-run: no step changed code | | cited: soflip 877 rows 0 RED | `verify/soflip_20261021/tracks_def_score.txt` |

## 2. AI (e)

On 2026-10-09 (old rule-4 recipe) (e) passed with every car's `k_own` = 220: no car left mode 0 inside the window. Today each
car leaves mode 0 at exactly the original's own `k` (100 / 23 / 39, `MODE_CLEAN_K` in `ai_speed_env.py`). All six gated
values equal the reference (the original's value from 10 regime-0 captures) at the printed 0.1. K1 and K26 are `regime0=0`
and are not scored by the rule. They are listed in `e_check_B.txt` only for completeness: K1 car 1 reads ft_median_m0
2043.0, off-regime.

**The PREREG's scorer caveat does not apply on this default.** `B_SR_K0` (`MASHED_SPEED_REFRESH=1`) hashed byte-identical to
`B_DEF_K0` (`9bd86ffe`). Reason (`Vehicle/VehiclePhysicsRun.cpp:1749` / `:1752`): `MASHED_D2LAT_REFRESH`, default ON since
d2ref 2026-10-17, rewrites `+0x9e4` every frame with the original's LUT arithmetic (`0x004710a1..0x004710da`), and the older
`SPEED_REFRESH` branch is the `else if` after it, unreachable on the default. So `rec_9e4` is the same quantity on both sides
today. (On pre-10-17 dumps or under `MASHED_NO_D2LAT_REFRESH` it is not, and the memory note was updated to say so.)

No bisect was needed: (e) did not fail.

## 3. AI (b)

`B_DEF_K0 / K1 / K26` reproduce soflip's step hashes, so the (b) table is soflip's to the byte: A 3/3, B 3/3, C 3/3. The
`H_DEF` dump (regime A on all three cars) also scores 3/3.

## 4. Powerups (c)

**Four captures had to be re-taken.** `m1`, `m2`, `s1`, `s2` (and the box witness `b3`) have no `.msd.puhook.csv` in git or on
disk. They were never committed, and the last sweep that read them was 2026-10-02 (`re/tools/pu_replay/sweep_out/`). The
committed `sweep.ps1` now prints `MISSING` for those four. They were re-captured on the original with their recorded argv (from
each `.msd.provenance.json`) as `m1r m2r s1r s2r b3r`.

| capture | recorded (2026-09-28d) | today | note |
|---|---|---|---|
| o3, o4 | decision CLEAN | CLEAN | replay byte-identical to 2026-10-02 except a path string |
| c2, c3, g2, g4 | CLEAN / CLEAN | CLEAN / CLEAN | replay byte-identical to 2026-10-02 |
| g3 | CLEAN / DIVERGES (R_FLAME `0x45afcc` 546 vs 548) | same, same row only | byte-identical to 2026-10-02 |
| m1 -> m1r | CLEAN / CLEAN | CLEAN / CLEAN | same activations (MORTAR 154, GUN 164, MISSILE 33 frames); AIM fallback 202/202 and 90/90, sweep 351/351 |
| m2 -> m2r | CLEAN / CLEAN | CLEAN / CLEAN | MORTAR 154 + MISSILE 29 (recorded 14 + two 1-frame `-1`); MORTAR query 240/240, gate 3/3; **reached box state 2** |
| s1 -> s1r | CLEAN / CLEAN | CLEAN / CLEAN | 4 x MISSILE 33 frames as recorded; sphere 77/77, gate 1/1, ground 154/154 |
| s2 -> s2r | CLEAN / CLEAN (box state 3 for 71 rows) | CLEAN / CLEAN | **stayed at box state 1**: the third activation fired over 33 frames, where s2's sat at state 3 for 72 |
| b3 -> b3r (amendment 1) | gate CLEAN, nobox 374 | gate **CLEAN**, nobox **592** + contact DIVERGES | `--pu-box 1` forced state 3 for 483 rows |

So every decision and contact verdict is as recorded. The state-3 short-circuit is covered by `b3r` (forced), and state 2 by
`m2r` (natural). A re-capture is a new capture, so frame and event counts differ from the recorded ones where noted.

## 5. Modes

| run | rule | argv beyond the base | SegmentCheck calls / mis / ends | Eval | FinishOrder calls / mis / appends |
|---|---|---|---|---|---|
| r0 | 0 | | 1243 / 0 / 4 | 4/4 | 2819 / 0 / 0 |
| r1 | 1 | | 1889 / 0 / 1 | 1/1 | 2468 / 0 / 0 |
| r2 | 2 | | 1421 / 0 / 3 | 3/3 | 2642 / 0 / 0 |
| r3 | 3 | | 1867 / 0 / 1 | 1/1 | 2459 / 0 / 0 |
| r4 | 4 | `--poke-lap 0:3` | 1127 / 0 / 2 | 2/2 | 2088 / 0 / **1** |
| r5done | 5 | `--track 3 --poke-collect 1:1` | 437 / 0 / 1 | 1/1 | 823 / 0 / 0 |
| r6 | 6 | | 1689 / 0 / 2 | 2/2 | 2564 / 0 / 0 |
| r7 | 7 | `--poke-lap 1:3` | 444 / 0 / 1 | 1/1 | 830 / 0 / **1** |
| r8 | 8 | `--poke-lap 0:3` | 1124 / 0 / 2 | 2/2 | 2090 / 0 / **1** |
| r9c2 | 9 | `--cars 2 --poke-lap 1:3` | 1129 / 0 / 2 | 2/2 | 2102 / 0 / **1** |
| r10exp | 10 | `--mode 3` | 1801 / 0 / 1 | 1/1 | 2201 / 0 / 0 |
| r10win | 10 | `--mode 3 --poke-lap 0:2` | 449 / 0 / 1 | 1/1 | 838 / 0 / 0 |

Base `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --oracle --hold 40`, `MASHED_MUTE=1`. r10exp reproduces the
recorded 1801 SegmentCheck calls exactly. Scope, as in 2026-09-26: the oracle checks the ported LAW against the stock original.
It does not execute `mashed_re.exe`. The port's side is the two unit tests built from this tree (`Race/RuleEngine.cpp`,
changed once since, `19e95b2f2`, same predicate) and the default runs above, where `MODE_LIVE` / `SLOT_TYPE_LIVE` feed
the engine its game mode word and slot types. Still not reached by any run (`D3_MODES_2026-09-26.md` §1.2): the `motion0 != 0`
exits of rules 8 / 9 / 10, the rule-5 `alive[0] == 0` exit, the rule-7 `participants-1 <= dead` exit, the rule-9 `gap >= 0.9`
arm.

## 6. Deviation from the PREREG

One, inside the band: D2 run 3 drvmed **1970.58** (slip2000 0.2549, 1636 lines) against the predicted 1964..1967. Runs 1 and 2
read 1965.67. D2 is wall-clock bound (90 s; memory `d2-arms-are-wall-clock-bound`). slip1500 and H1 are identical on all three.
The band is 1893.63..1997.46, so the verdict is unchanged.

## 7. Found this session (no tracker change)

- **Lost evidence:** `verify/d3_contact_20260928b/{m1,m2,s1,s2}.msd.puhook.csv` and `verify/d3_box_20260928/b3.msd.puhook.csv`
  are gone. These are the files `sweep.ps1` and `b3.nobox.diff.txt` cite. They are not gitignored, just never committed. The
  re-captures `m1r m2r s1r s2r b3r` sit in `D:\MashedWT\d3close_data\pu\`. Committing their `.puhook.csv` (5.36 MB for the five:
  m2r 2.05, s2r 1.54, b3r 1.45, m1r 0.20, s1r 0.11) or pointing `sweep.ps1` at them is a decision for Mariano.
- `AiStandalone.cpp:411` still describes phase 8 as "a follow-up". It is ported at `:506`. Comment only.
- No tracker row is resolved or new, so no apply script ran.
