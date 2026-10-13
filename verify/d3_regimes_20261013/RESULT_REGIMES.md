# RESULT: criterion (b) scored in all three regimes (default build)

Date 2026-10-13. Branch `work/d3regimes` (base `0bc7ccd0`). Measurement and harness only, no port code
changed. Scorer: `re/tools/ai_ctrl_window.py --check-regime` rule (the one `RESULT_B2FLIP.md` uses),
wrapped per regime by `score_regimes.py`. Envelopes: `re/tools/b_envelopes.py` over
`ORIG_LABELS.txt` -> `b_envelopes_regimes.json`.

## 0. Verdict first

> **Default build, (b) per regime: A 2/3, B 1/3, C 0/3.**
>
> | regime | car 1 | car 2 | car 3 | score |
> |---|---|---|---|---|
> | A (K = 0, 2) | PASS | FAIL `abs_steer~` 0 (env 7..23) | PASS | **2/3** |
> | B (K = 1, 4, 27) | FAIL `c1#` 57 (17..46), `c1~` 49 (0), `abs_steer~` 49 (0..16) | FAIL `c0#` 43 (13..37), `c1#` 47 (17..46), `steer#` 89 (29..84) | PASS | **1/3** |
> | C (K = 26, 54, 57) | FAIL 5 items | FAIL 5 items | FAIL 5 items | **0/3** |
>
> C's 5 failing items per car: `c0#`, `c1#`, `steer#`, `c1~`, `abs_steer~` (envelope 27 / 95 / 121 /
> 27.5 / 36.5, width 0). Port values: car 1 15 / 69 / 83 / 0 / 7, car 2 39 / 75 / 113 / 5.5 / 44.5,
> car 3 30 / 48 / 77 / 0 / 11.5.
>
> - **The reference gap was smaller than the brief said.** Three exact-argv labelled original B runs
>   already existed (`d3_ai_20260926/o_inputs`, `d3_ai_20260927/o4`, `d3_drive_20260928/e1`), plus a
>   second C run (`d3_drive_20260928/e2`). `RESULT_BREGIME.md` built its envelopes from its own 8 runs
>   only. The port was already run at C phases (K = 26/54/57, `RESULT_BREGIME_C.md`). This session
>   added 18 fresh labelled original runs (13 A, 3 B, 2 C) under today's harness.
> - **Original run-to-run noise inside a regime is zero on every scored metric.** A: 21 runs, one
>   window per car (hash-identical). B: 6 runs, hash-identical. C: 4 runs, car 3 differs in 2 of 220
>   calls in one run (r20, calls 216 and 219, `c1` +1), with all 10 metrics unchanged. The only
>   variation the original shows is which regime the roll lands in.
> - **Regime C cannot score cars 1/2 with the 220-call window, in any original run.** Cars 1/2 are
>   stepped exactly 196 calls after the window start in **4 of 4** C runs (220 in 21 of 21 A runs, at
>   least 631 in 6 of 6 B runs). So the U-9204 "confound" (run 2's car 1/2 drop-out) belongs to
>   regime C, not to one run. A C run with three full windows does not occur on this recipe.
> - **U-9204's "flag 2 is inert in the default port" is stale since the B2 flip.** The port's C
>   windows now differ from its A windows (car 1 hash `d280df5a` vs `44ea9d72`, the same holds for every car), which
>   matches `RESULT_B2FLIP.md` §0. They still miss the original's C envelope on 5 items per car.

## 1. How a run is labelled A / B / C

Per car, the **first non-zero value of `DAT_0089a368` inside the car's 220-call window** (window = from
the car's first AI-step call with accel `c4 != 0`): none -> A, 1 -> B, 2 -> C
(`re/tools/ai_ctrl_window.py:80-96` `window_regime`, ROADMAP §D3 (b) amendment 2026-10-09). The value
is read per AI-step call from the capture's own `flag_a368` column (original: `scenario_launch.py
--statediff-aistep`, hook on `FUN_00416250` `0x00416250`; port: `MASHED_AI_STEPDUMP`). The original
runs also carry a 60 Hz poll of the same address (`orig_rampwatch.py`, column `mode368`) as a second
witness. In all 18 fresh runs the two agree (`o_reg_r*.label.txt`).

Writers of `DAT_0089a368`, as annotated in the port source (`Ai/AiPreTick.cpp`,
`Ai/AiStandalone.cpp:1730-1788`; addresses not re-disassembled this session):

| address | write | condition |
|---|---|---|
| `0x00414185` | 0 | `FUN_00414180` race-state reset |
| `0x004177b6` / `0x004177c4` | 2 | `FUN_00443080` (target enable) != 0 |
| `0x00417ac3` / `0x00417acc` | 2 | game mode `fd0` = 4 or 9 |
| `0x00417ad5` / `0x00417adc` | 0 | game mode 8 |
| `0x00417b14` | 1 | flag == 2 and timer `0x0089a36c` > 180000 |
| `0x00417c43` | (return) | flag != 0: no roll |
| `0x00417c8a` | 1 | roll 1: `AiRand(0,100) <= 0x005f30a0[row*5+band]` |
| `0x00417cde` | 2 | roll 1 lost, roll 2: `AiRand(0,100) <= 0x005f3180[row*5+band]` |

On this recipe the flag goes non-zero **61 game frames after the window start** in every B and C run,
original and port (original: 864 -> 925, 930 -> 991, 812 -> 873, ...; port: 0 -> 60, the port's
frame 0 is its window start). In the port, the regime per ring phase K is the band-0 roll reading draw
K of the verbatim ring (`verify/d3_bscore_20261009/predict_regime_k.py`, n0 = 0, 64/64).

## 2. Run inventory

All original runs share the regime-A reference argv
`scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-out <x>
--statediff-car 1 --statediff-aistep --hold 60` (each run's `*.msd.provenance.json`).

| regime | runs | source (date, git head) |
|---|---|---|
| A | 21 | `d3_bscore_20261009/o_breg_r{1,3,4,5,7,8}` (10-09, `db548ead`), `d3_drive_20260928/e{3,4}` (09-29, `c3121fbc`), `o_reg_r{1,2,4,6..12,14,16,17}` (10-13, `0bc7ccd0`) |
| B | 6 | `d3_ai_20260926/o_inputs` (09-26, `eebaf511`), `d3_ai_20260927/o4` (09-27, `4ff428ad`), `d3_drive_20260928/e1` (09-29, `c3121fbc`), `o_reg_r{13,15,18}` (10-13) |
| C | 4 | `d3_bscore_20261009/o_breg_r2` (10-09), `d3_drive_20260928/e2` (09-29), `o_reg_r{19,20}` (10-13) |
| void | 2 | `o_reg_r3`: game exited during track load while another session's MASHED (pid 9392) ran (piz collision, re-run rule). `o_reg_r5`: `could not attach` (rc 3), pid 29940 had exited by the next check |

Not used: `o_wobj2` (B, extra `--wobj-probe`), `o_c30a/b`, `o_tg1`, `o_wobj1`, `s1/s2` (A, extra
probes), `o_spread1..3` and `orig_step_slots` (no flag column). Every one of them gives the same
per-car window as the labelled runs of its regime. Fresh-run label rate: 13 A / 3 B / 2 C of 18.

Port (default build, `run_fk.ps1` recipe: Training, car 1, round 1, rule 4, `MASHED_DETERMINISTIC`,
1200 frames, `MASHED_RING_SKIP` = K), `PORT_RUN.txt`:

| K | predicted | labelled | window hashes (car 1/2/3) |
|---|---|---|---|
| 0, 2 | A | A | `44ea9d72` / `5c59e7ee` / `c7ae564f` |
| 1, 4, 27 | B | B | `0704659c` / `0e09da43` / `43f031d8` |
| 26, 54, 57 | C | C | `d280df5a` / `f01aa0fb` / `4a49720a` |

The knob took: 8/8 labels equal the `predict_regime_k.py` map, and K = 1 / 26 reproduce
`d3_b2flip_20261009/FK01` / `FK26` byte-for-byte (`fe0c45d2`, `6ae8c99d`). Each regime gives one window
set in the port too.

## 3. Envelopes (`b_envelopes_regimes.json`, cars pooled, min..max)

| metric | A (21 runs) | B (6 runs) | C (4 runs, car 3 only) |
|---|---|---|---|
| `c0#` | 13..35 | 13..37 | 27 |
| `c1#` | 21..70 | 17..46 | 95 |
| `steer#` | 33..96 | 29..84 | 121 |
| `c1~` | 0 | 0 | 27.5 |
| `abs_steer~` | 7..23 | 0..16 | 36.5 |
| `accel#` / `accel~` | 2..3 / 255 | 3..4 / 25..102 | 2 / 255 |
| `c0~`, `brake#`, `brake~` | 0, 2, 0 | 0, 2, 0 | 0, 2, 0 |

A is unchanged from `re/tools/b_envelopes.json`. B is new. C is unchanged (the 3 extra runs equal run 2).
Because the original has no run-to-run noise inside a regime, each envelope is the spread across the
three cars, and C (one car) has width 0.

Diagnostic only, not the committed window (`SCORE.txt` tail): at n = 196 all three original C cars are
scorable. Pooled C envelope @196: `c0#` 20..32, `c1#` 18..79, `steer#` 40..105, `c1~` 0..23.5,
`abs_steer~` 8..28, `accel#` 2..3. The port's K = 26 under that rule: car 1 FAIL (`c0#` 15, `abs~` 6),
car 2 FAIL (`c0#` 39, `abs~` 29.5), car 3 PASS, so 1/3.

## 4. Decisions needed (user)

1. **Adopt B's envelope** into `re/tools/b_envelopes.json` (the decided rule, now with a labelled
   reference): `py -3.12 re/tools/b_envelopes.py re/tools/b_envelopes.json $(Get-Content
   verify/d3_regimes_20261013/ORIG_LABELS.txt | ? { $_ -notlike '#*' })`. Not done here. It changes the
   canonical scoring input.
2. **Regime C window length.** Under the 220-call window, C scores against car 3 alone (n = 1 car,
   width 0) on every original run. Options: (i) keep, C stays a single-car reference. (ii) score C at
   n = 196, the shortest original C span (port 1/3 under pooled cars). (iii) score C per car at 196.
3. **U-9204 rewrite** (via `re-classify`): the "inert" statement and the "second C run with three full
   windows" resolution path are both overtaken by this note.

## 5. Proposed text for the orchestrator

ROADMAP §D3 (b), append to the amendment:

> **Update 2026-10-13 (`verify/d3_regimes_20261013/RESULT_REGIMES.md`):** labelled original references
> now exist for all three regimes (A 21 runs, B 6, C 4, all with the reference argv). Inside a regime
> the original is noise-free (hash-identical windows, C: 2 of 220 calls differ by 1 in one run). In C
> the original steps cars 1/2 for 196 calls (4/4 runs), so C is scored on car 3 alone. **Default build:
> A 2/3, B 1/3, C 0/3.**

NEXT_SESSION, replacing item 2 of the 2026-10-13 D3 survey block:

> 2. DONE 2026-10-13 (`verify/d3_regimes_20261013/RESULT_REGIMES.md`): B and C references exist, (b) on
>    the default build = A 2/3, B 1/3, C 0/3. Owed: user decisions on adopting the B envelope and on C's
>    window (220 = car 3 only, or 196). Port B fails car 1 (`c1~`/`abs_steer~` 49 vs 0) and car 2
>    (`c0#`/`c1#`/`steer#` 1..6 above the band), C fails all three cars on 5 items.

## 6. Raw

`capture_regimes.sh` (extends `d3_bscore_20261009/capture_breg.sh`: cap 30, stops at 2 fresh B + 2
fresh C), `label_run.py`, `score_regimes.py`, `CAPTURE.txt`, `o_reg_r{1..20}.{log,poll.csv,
msd.aistep.csv,msd.provenance.json,label.txt}` (`.msd` raw is gitignored, `.gitignore:140`, left on
disk), `ORIG_LABELS.txt`, `ENVELOPES.txt`, `b_envelopes_regimes.json`, `PORT_RUN.txt`,
`P_K{00,01,02,04,26,27,54,57}.step.csv`, `SCORE.txt`.
