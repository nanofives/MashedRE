# RESULT dflip: three user-decided default flips (2026-10-17)

Branch `work/dflip` from `race/first-frame-parity` `964c31f04`. PREREG `PREREG_dflip.md` committed with the code in
`58c4465c3`, before any run. Model claude-opus-5-5, account3. One exe for every arm: `58c4465c3`, `MASHED_NO_DEPLOY=1`,
snapshot `D:\MashedWT\dflip_data\exe\mashed_re_58c4465c.exe` (sha256 `62663D84D70AC977...`). Private game roots
`D:\MashedWT\dflip_root` (arms) and `D:\MashedWT\dflip_root13` (13-track replay). Every PID this lane spawned exited by
itself or was stopped by its own runner (the D2 arms' 90 s wall); no other process was touched. No piz-collision AV (the lane
runs the exe only). Provenance: `arms/RUNLOG.txt`, `arms/batch1.txt`, `arms/batch_d2.txt`.

## 0. Verdict

All three decisions are in, and every pre-registered expectation held except the 13-track row total: **870, not 867**,
+3 rows that are all `C27-CAR-PART-OFFSET` (cited, 0 RED) and that the assist opt-out removes (section 3).

| gate | expected | measured | verdict |
|---|---|---|---|
| G-CTRL new default (`H_DEF`, `H_DEF-2`) | `4c81db89 / aa03f7ae / 7d3af534`, repeat identical | `4c81db89 / aa03f7ae / 7d3af534` x2 | held |
| G-CTRL all opt-outs (`H_OPT`) | `d3b90ee0 / 80f01f35 / 56d62ae7` | `d3b90ee0 / 80f01f35 / 56d62ae7` | held |
| G-CTRL assist opt-out only (`H_NOHA`) | the same | `d3b90ee0 / 80f01f35 / 56d62ae7` | held |
| 13 tracks x 3 frames | 867 rows, 0 RED, Warzone 72 | **870 rows**, 0 RED, 39/39 CITED, Warzone 24 / 21 / 27 = 72, matched 326 / 326 / 296 | rows FAILED the prediction by +3, all cited |
| I-NAME | PASS everywhere; Neustein 1169 59/59, Highway 928 77/77 | 78/78 gate lines true; Neustein 1169 stand 59/59, orig 77/77; Highway 928 stand 77/77, orig 123/123; INSTRUMENT GATES PASS | held |
| D2 x3 | in bounds, H1 pass | see section 4, 3/3 in, H1 398/400 x3 | held |
| D3 (b) default | A 1/3, B 2/3, C 0/3, no passing car lost | A 1/3, B 2/3, C 0/3; A car 2, B cars 1 and 3 still pass | held |
| D3 (b) `MASHED_AI_LUT=5` stacked | A car 2 fails, so keep OFF | A 1/3 (car 3 passes, **car 2 fails** `c0#` 41), B 2/3, C 0/3 | held: keep OFF |
| collateral | `H_DEF` 42, `H_L5` 39 of 94 | 42, 39 | held |
| Warzone | deterministic, cycles, ret-1 all with the word, `W_NOHA` = pontoon's CARRY arm, `W_OPT` = pontoon's default, `WL_DEF` no crash | all held (section 6) | held |

## 1. Code

- `Race/HeadingAssist.{h,cpp}`: `HeadingAssist_Mask()` returns 0x1 by default; `MASHED_NO_HEADING_ASSIST=1` returns 0;
  `MASHED_HEADING_ASSIST=<mask>` still overrides the mask. Call site comment in `D3d9Render/TrackRenderer.cpp` updated.
- `Track/WarzonePontoon.{h,cpp}`: CARRY -> STATE -> ANIM default ON. `MASHED_NO_PONTOON_CARRY=1` (STATE + ANIM stay),
  `MASHED_NO_PONTOON_STATE=1` (ANIM stays), `MASHED_NO_PONTOON_ANIM=1` (all off = the 964c31f04 default); an opt-out also
  turns off every knob that implies it; the positive knobs still force on. Three TrackRenderer comments updated.
- Instrument, not the shadow lane: `verify/d1r1_20261016/check_gates.py` `is_named(name, peer)`: a name `#WxH:<D3DFORMAT>`
  (the name `DrawRec.cpp` `TexDesc` gives a raster with no dictionary name; the original-side recorder writes the same) counts
  as named on a row when the same name is drawn on the other side of that row. A `#` name on one side only stays unnamed.
  `orig_gates(d, peer_dir)` / `stand_gates(d, trace, peer_dir)`; called without a peer they keep the old strict rule.
  `verify/rsky_20261016/run13.py score` passes the peer directory.
- Applied with `apply_code.py` (asserted exact replacements).

## 2. G-CTRL (`run_arms.py H_*`, TRAINING recipe, 14400 frames)

| arm | knobs | step | gates | ptrace | lines |
|---|---|---|---|---|---|
| `H_DEF` | none | **4c81db89** | **aa03f7ae** | **7d3af534** | 17497 / 54713 / 27356 |
| `H_DEF-2` | none | 4c81db89 | aa03f7ae | 7d3af534 | identical |
| `H_OPT` | `MASHED_NO_HEADING_ASSIST MASHED_NO_PONTOON_CARRY MASHED_NO_PONTOON_STATE MASHED_NO_PONTOON_ANIM` | d3b90ee0 | 80f01f35 | 56d62ae7 | 18671 / 55433 / 27716 |
| `H_NOHA` | `MASHED_NO_HEADING_ASSIST` | d3b90ee0 | 80f01f35 | 56d62ae7 | |
| `H_L5`, `H_L5-2` | `MASHED_AI_LUT=5` | b5728d30 | dab4aa0a | 8ed07549 | = d3b `H_HA5` |

The new default trio equals d3b's `H_HA`: the pontoon flips do not reach TRAINING. Opt-out chain from here: add
`MASHED_NO_HEADING_ASSIST=1` to any recipe that compares against `d3b90ee0` or an older trio.

## 3. 13 tracks x 3 frames (`run13.py`, cites `verify/rres_20261016/cites_rres.json`)

`evidence/tracks/scores_diff.txt`, `gates_diff.txt`: 39 frames, 870 rows, 0 RED, every frame CITED. Against the
pre-dflip default (pontoon's `SCORE13_N4.txt`, 867):

| frame | rows N4 -> dflip | matched | row that differs |
|---|---|---|---|
| Training 884 / 924 / 959 | 20 -> 21 each | 215 -> 213, 211 -> 209, 209 -> 205 | + `MOVED Advantage` (C27-CAR-PART-OFFSET), AABB moved 0.0159 / 0.0190 / 0.0186 |
| Roundabout 1091 | 33 -> 34 | 316 -> 314 | + `MOVED Advantage` (C27), 0.0112 |
| Dump 735 | 20 -> 19 | 338 -> 340 | - `MOVED Advantage` (C27): 0.0122 with the assist off, under tolerance with it on |
| Warzone 584 / 624 / 659 | 24 / 21 / 27 | 326 / 326 / 296 | none (= pontoon's N4 CARRY arm; the boat as three per-atomic draws) |

Attribution: the same exe with `MASHED_NO_HEADING_ASSIST=1` on tracks 7, 10, 13 (`evidence/tracks_noha/`) gives N4's
counts exactly (33 / 20 / 20 x3, matched 316 / 338 / 215, 211, 209). The other row changes between the two arms are the same
rows with their draw indices in a different order. C27's note: "the part poses come from the standalone's own wheel /
suspension state" (U-9718); the matched-pose replay transplants the car frame, not the wheel state, and the assist moves the
AI cars' wheel state. The changed frames' diff text is in `evidence/tracks*/<track>_diff_<row>.txt`.

I-NAME: `"unnamed": []` on all 78 gate lines. Neustein 1169 original 77/77 (failed before this rule), standalone 59/59;
Highway 928 standalone 77/77 (82 draws against the original's 131), original 123/123.

## 4. D2 (`D2_DEF_1..3` alone, `score_all.py d2`, bounds `d81a8df6`; `arms/SCORE_D2_dflip.txt`)

| run | lines | slip 1500-2000 [0.18855, 0.19635] | slip 2000-2600 [0.24488, 0.25487] | driving-median [1904.70, 1982.44] (n) | H1 | launch |
|---|---|---|---|---|---|---|
| `D2_DEF_1` | 1632 | 0.1890 in | 0.2480 in | 1913.06 in (1422) | 398/400, 1362.7 | L = 0 at 0.19 % |
| `D2_DEF_2` | 1630 | 0.1890 in | 0.2476 in | 1912.04 in (1420) | 398/400, 1362.7 | L = 0 at 0.19 % |
| `D2_DEF_3` | 1633 | 0.1890 in | 0.2482 in | 1913.06 in (1423) | 398/400, 1362.7 | L = 0 at 0.19 % |

All above the 1600-line floor, none re-run. The values sit inside the spread of the earlier D2_DEF runs on this base
(pontoon rebased: 0.2476..0.2483, 1912.01..1914.88). The D2 recipe is the player solo, which the AI-slot assist does not
reach (d3b measured `D2_HA_1` byte-identical to d2close's `D2_DEF_2`).

## 5. D3 (b) (`percar.py`, `re/tools/b_envelopes.json`, per-car envelope; `SCORE_DEF_PERCAR.txt`, `SCORE_L5_PERCAR.txt`)

New default, ring phases K0 / K2 (A), K1 / K4 / K27 (B), K26 / K54 / K57 (C): **A 1/3, B 2/3, C 0/3** on every phase.
The K0 / K1 / K26 step dumps are byte-identical to d3b's `B_HA_K*` (`a89ff9ef`, `80bce410`, `54da2e83`).

| regime | passing | failing cars and items (port, envelope) |
|---|---|---|
| A | car 2 | car 1 `abs_steer~` 6.0 [7, 23]; car 3 `abs_steer~` 0.0 [7, 23] |
| B | cars 1, 3 | car 2 `c0#` 40 [13, 37], `c1#` 48 [17, 46], `steer#` 87 [29, 84] |
| C | none | car 1 six items, car 2 four, car 3 five |

Against the pre-dflip default (d3b section 1): the same cars pass (A car 2, B cars 1 and 3). A car 1 `abs_steer~` 0 -> 6.0;
B car 2 `c1#` 60 -> 48 and `steer#` 93 -> 87 but `c0#` now out (40 > 37); C car 2 `c0#` 26 -> 34, `c1#` 66 -> 47, `steer#`
91 -> 80, `abs_steer~` 12 -> 10 (envelope 32 / 41 / 73 / 9).

**`MASHED_AI_LUT=5` on the new default** (dumps byte-identical to d3b `B_HA5_*`): A 1/3 (**car 3 passes**, car 1 5.5,
**car 2 fails `c0#` 41 [13, 35]**), B 2/3 (car 2 `c0#` 43, `c1#` 52, `steer#` 94), C 0/3 (car 1 seven items, one more
than the default: `c1~` 16 vs 0). Collateral 39 vs the default's 42. A car that passes on the new default turns to FAIL, so
under the (b) rule it does **not** qualify for default ON. Not flipped.

Collateral (`coll.py`, 94 fields): `H_DEF` 42 (pre-dflip 39), `H_L5` 39.

## 6. Warzone (`run_arms.py W_* / WL_*`, rule 8; `analyze_w.py`)

| arm | knobs | object | AI reader `0x00414f00` | hashes step / pontoon / seek |
|---|---|---|---|---|
| `W_DEF`, `W_DEF-2` | none | init; enters call 2047 (word `ff0100`), state 1 61 calls, state 2 974 (c8 max 32.43), state 3 517 | 4201 calls, gate1 1194, ret1 885, all 885 with the word set | `00d02fe4 / c4d244c0 / 0a8bf220`, both runs |
| `W_NOHA` | assist off | enters 1839 (`ff0010`), 970 state-2 calls, c8 32.30 | gate1 1380, ret1 885 | `f84119ef / a88dbf57 / 49972586` = pontoon's CARRY arm byte for byte |
| `W_OPT` | all opt-outs | no pontoon trace | gate1 0, ret1 0 | `ea4b8425 / - / e0fc7578` = pontoon's default |
| `WL_DEF` | none, 18000 frames | three full cycles 0 -> 1 -> 2 -> 3 -> 0 (enter 2047 / 6120 / 10137, back to 0 at 4057 / 8142 / 12117), c8 max 32.67 | 18735 calls, gate1 7124, ret1 3584, all with the word | `a8f1bb38 / 84dd095e / f9e87d9d` |

`WL_DEF` exit 0 by itself after 605 s; its log ends `R6 MATCH OVER t=11.0s winner=car0 (score>11 rule, 0x00410510)`, then
the results screen. With the assist on, a different AI car boards (word `ff0100` against `ff0010`) 208 calls later: the
assist moves the AI trajectories, the object's per-call behaviour is pontoon's replay (1073/1073), unchanged here.
Draw rows on the replay frames: 72 (section 3).

## 7. Trackers (re-classify, no C-level change; `apply_trackers.py`)

- **U-9191 RESOLVED** (cause: unported `FUN_0046c7d0`, found by d3b; ported, default ON).
- **U-9743 FILED** (structural): the player (slot 0) form of the assist, a D2 input. d3b's proposed "U-9729" was taken;
  U-9743 is the next free id (U-9742 was the highest).
- **U-9736 RESOLVED** (the I-NAME rule of section 1).
- **U-9738** note: exit approximation accepted by the user 2026-10-17, CARRY default ON; row stays open, Blocks now nothing.
- **U-9730 -> D-11078**, **U-9741 -> D-11079** (DEFERRED by the user, with re-pickup conditions); both U rows marked DEFERRED.
- hooks.csv notes: `0046c7d0`, `004093b0` (dflip, default ON), `00443dc0`, `00443300` (d3b LUT / operand order behind
  `MASHED_AI_LUT=5`, default OFF). No confidence change.
- CHANGELOG: one line under `<!-- ENTRIES -->`.
- Duplicate check after the transaction: no duplicate U-id in UNCERTAINTIES.md; D-11078 / D-11079 once each (the 17
  duplicate D-ids in DEFERRED.md predate this lane, the same 17 on `964c31f04`).

## 8. Follow-ups

- U-9743: the player form of the assist (mask bit for slot 0), measured on the D2 recipe x3 and G-CTRL; needs the original's
  slot-0 assist angles first.
- C27 now holds 5 more frames' worth of variation from AI wheel state: U-9718 (wheel / suspension state not replayed) is
  the path that would remove them.
- `MASHED_AI_LUT=5`: re-score after the next AI change; today it trades A car 2 for A car 3.
- B car 2 `c0#` moved out of its envelope with the assist (40 vs 37) while `c1#` / `steer#` moved toward it; the next (b)
  lane on regime B starts there.

## 9. Evidence kept in git / moved

In git: PREREG, this RESULT, scorers (`run_arms.py`, `percar.py`, `coll.py`, `analyze_w.py`, `apply_code.py`,
`apply_trackers.py`), `arms/` logs, scores, seek / pontoon traces, `B_DEF_K{0,1,26}.step.csv`, the D2 `motion_diag` logs,
`evidence/tracks*/` (scores, gates, status, the five changed frames' diffs). Moved to `D:\MashedWT\dflip_data\verify\`
(sha8 in `arms/batch1.txt`): the H_ step / gates / ptrace dumps, the W / WL step dumps, the extra-phase and L5 B dumps, the
collateral cuts and the full `tracks/` / `tracks_noha/` replay directories.
