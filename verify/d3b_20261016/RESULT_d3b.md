# RESULT d3b: D3 (b) re-score on the D2-closed default, and the regime-A cause found in the bytes (2026-10-16)

Branch `work/d3b`, base `race/first-frame-parity` `2cc15b2cf`. PREREG `PREREG_d3b.md` + amendments 1-5 (each committed
before the runs it governs; amendment 2 discloses that LR_M5 re-ran a gate after a further fix). Model claude-opus-5-5,
account3. Exes: base `68e037d7`, knob builds `1abe36ef` (LUT), `694f0dc3` (+ Catmull order), `eb5e1462` (+ heading assist).
All port runs on private `MASHED_ROOT` copies, `MASHED_NO_DEPLOY=1`, every PID killed was this lane's own (one: the
`H_L5-2` run stuck on the full disk). Six original-side launches on the shared install (`o_lb1`; `o_ww1` failed on the
full disk and was re-run to completion; `o_ww2`, `o_pk1`, `o_rt1`), each with its own PID, no piz-collision AV.

## 0. Verdict

1. **Re-score (job 1): A 1/3, B 2/3, C 0/3 on the D2-closed default**, confirmed. G-CTRL `d3b90ee0 / 80f01f35 / 56d62ae7`,
   K0 / K1 / K26 dumps byte-identical to d2close's. Failing items (`SCORE_DEF_PERCAR.txt`): A car 1 and car 3
   `abs_steer~` 0 (original 7 / 23); B car 2 `c1#` 60, `steer#` 93 (bounds 17..46, 29..84); C car 1 six items, car 2 four,
   car 3 five. The extra ring phases K2 (A), K4 / K27 (B), K54 / K57 (C) give the same verdicts.
2. **The regime-A cause is a whole unported function, `FUN_0046c7d0`** (C2 mapped, no port). Between the AI tick
   (0x00425a73) and the physics frame (0x00425a8d), FUN_00424eb0 calls it for every live car; when both steer bytes are 0 it
   turns the car's live body matrix about (0,1,0) toward the heading of the next three track pieces (FUN_004093b0). The port
   never runs it, so its AI cars keep turning on steer-0 frames where the original's turn back, the error falls under the
   1-degree deadband, and cars 1 / 3 drive with steer 0 for most of the window. Found by measurement, not inference (section 2).
3. **Ported behind `MASHED_HEADING_ASSIST=1` (AI slots 1..3), default OFF.** It reproduces the original's measured jump at
   car 1's first steer-0 frame to four digits (-0.23935 vs -0.2393 deg) and cuts car 1's heading residual from 1.033 to
   0.402 deg (0.290 with the LUT mask), but two registered gates fail as written: G-ASSIST (13 / 19 rows in band) and
   collateral (42 vs 39, +3 over +2). Section 4 therefore holds it OFF. (b) with it: per-regime counts unchanged (A 1/3,
   B 2/3, C 0/3), **no car that passes on the default turns to FAIL**; A car 1 `abs_steer~` 0 -> 6.0 (needs 7) and its
   steer-0 calls 131 -> 100 (original 102); C car 2 moves toward the original on all four failing items. User decision
   needed (section 6).
4. **The AI's RW-LUT arithmetic** (`MASHED_AI_LUT`, default OFF): the lookahead (FUN_00443dc0 + FUN_00443300 operand order,
   mask 5) is bit-exact against the original on **1384 / 1384** comparable (b)-window rows (exact sqrt: 559). It moves
   (b) items but not the counts, and its registered gate fails literally on regime B (probe-zero rows, section 3). OFF.
5. **Default build unchanged.** G-NOOP on every knob build: trio `d3b90ee0 / 80f01f35 / 56d62ae7`, K dumps
   `17d07c90 / 062a3c98 / b8124a63`.

## 1. Re-score

`run_arms.py B_DEF_K{0,1,26,2,4,27,54,57} H_DEF H_DEF-2`, scorer `percar.py` (verdict = `ai_ctrl_window.check_regime`
on `re/tools/b_envelopes.json`, plus each item next to the same car in one labelled original run of the regime).

| regime | n/3 | failing cars and items (port vs envelope, original car value) |
|---|---|---|
| A (K0, K2) | 1/3 | car 1 `abs_steer~` 0 [7,23] (7); car 3 `abs_steer~` 0 [7,23] (23) |
| B (K1, K4, K27) | 2/3 | car 2 `c1#` 60 [17,46] (46), `steer#` 93 [29,84] (84) |
| C (K26, K54, K57, 196/car) | 0/3 | car 1 `c0#` 9 (20), `c1#` 75 (18), `steer#` 83 (40), `abs_steer~` 11 (8), `accel#` 1 (3), `brake#` 1 (2); car 2 `c0#` 26 (32), `c1#` 66 (41), `steer#` 91 (73), `abs_steer~` 12 (9); car 3 `c0#` 21 (27), `c1#` 69 (79), `steer#` 89 (105), `c1~` 0 (23.5), `abs_steer~` 14 (28) |

## 2. How the regime-A cause was found

- **Look point (amendment 1).** On car 1's window the port and the original agree on speed and position (0.2, 0.005) but not
  on the look point: the original's jumps, the port's moves smoothly. Cause: three RW-LUT sites the port computed with
  `std::sqrt` (FUN_00443dc0 phase 6 `FUN_004c3c60`, phase 8 `FUN_004c3bf0`, FUN_00415e20 `FUN_004c39b0`) and the Catmull-Rom
  operand order (FUN_00443300). With mask 5 the look replay (`LR_M5`, original positions fed into the port's lookahead) is
  bit-exact on every comparable window row. The A cars still fail: same look point, smaller steer error.
- **Heading (amendment 3, `heading.py`, `o_rd3.msd`).** Through step 24 the port's car 1 heads within 0.01 deg of the
  original's. On the original's steer-0 frames (+0x1a8 = 0, driving arm +0x10 = 0, +0x9e0 = 4.0) its body turns -0.16 to -0.20
  deg per frame, while FUN_0046e9e0's own terms predict +0.077 (the port does exactly that, +0.0785). The accumulator
  +0x148 decays by exactly damp^2 there, so FUN_0046e9e0 saw no steer.
- **Where (amendment 4, `o_lb1` lat-bracket + new solver site).** The turn sits between the previous frame's last substep and
  the next A4 call: -0.191 .. -0.264 deg on 9 steer-0 frames, never on a steer frame (`lbjump.py`).
- **Who (amendment 5, `o_ww2`, new `--watch-write` hardware watchpoints).** The only runtime writers of car 1's at.x are
  FUN_0046e9e0 (0x0046ef4e) + its ortho (0x004c4a11) and, on 31 frames, 0x004c4cf1 (FUN_004c4a50 under FUN_004c4d20) whose
  accurate backtrace ends at the call 0x0046cb10 in **FUN_0046c7d0**. G-REPRO 220/220 on all three cars for every capture.
- **Inputs.** `_DAT_00613134` = 1.0 at runtime (`o_pk1`, nine samples, control `_DAT_00613108` = 105). The range table
  FUN_00409330 walks is {0, 29, -1} on Training (`o_rt1`).

## 3. Gates (PREREG section 3 and amendments)

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_NOOP*` = default K hashes on `1abe36ef`, `694f0dc3`, `eb5e1462`; `H_NOOP` (1abe36ef) and `H_NOOP3` (eb5e1462) = default trio | PASS |
| G-REPLAY-LOOK (mask 1) | bit-equal 232 / 3037 (A), 330 / 3184 (B), 245 / 524 (C) window rows | FAIL as registered |
| G-REPLAY-LOOK (mask 5, amendment 2) | literal: A 99.2 %, **B 93.8 %**, C 99.2 %; without the probe-zero rows >= 99.98 %; (b) windows 1384 / 1384 | FAIL literally on B |
| G-TOOK | every knob changes the step dumps; assist trace 435 non-zero angles; `mashed_re.log` "range table populated, verbatim walk" | PASS |
| G-REPRO (original captures) | 220/220 x3 on `o_lb1`, `o_ww1`, `o_ww2` | PASS |
| G-SPLIT | the turn is outside FUN_0046e9e0's interval (amendment 3 prediction held) | PASS |
| G-WRITER | FUN_0046c7d0 via 0x0046cb10 | found |
| G-ASSIST | 13 / 19 car-1 steer-0 rows at 1900..2500 in [-0.30, -0.15] (min -0.3234, max +0.2232) | FAIL as registered |
| determinism | `H_L3` = `H_L3-2`, `H_L7` = `H_L7-2`; HA / HA5: see section 4 | PASS |
| collateral (`coll.py`, 94 fields) | base 39, L3 38, L7 40; HA / HA5: section 4 | within +2 |

Post-hoc witness for the assist (not a registered gate): with the port's FUN_004093b0 curvature for piece 0 (1.320496), the
formula gives -(1952.5/6000) x 50/60 x (45 - 4 x 1.320496)/45 = -0.23935 deg; the original's measured jump at that frame
(`o_lb1` seq 7399, speed 1952.5) is -0.2393.

## 4. (b) per arm (K0 / K1 / K26; `SCORE_*_PERCAR.txt`)

| arm | knobs | A | B | C | per-car movement vs default |
|---|---|---|---|---|---|
| DEF | none | 1/3 | 2/3 | 0/3 | |
| L1 / L3 | LUT 1 / 3 | 0/3 | 2/3 | 0/3 | A car 2 now fails `c1#` 74-77 |
| L2 | LUT 2 | 0/3 | 2/3 | 0/3 | A car 2 fails `steer#` 97 |
| L5 | LUT 5 | 1/3 | 2/3 | 0/3 | A car 1 adds `c1#` 12; C car 1 `abs_steer~` 11 -> 9 (8), `c1#` 75 -> 71 |
| L7 | LUT 7 | 0/3 | 2/3 | 0/3 | |
| G, L1G, L3G, L5G, L7G | + `MASHED_GRID_DIR` | 0/3 | 2/3 or 1/3 | 0/3 | GRID_DIR costs A in every stack |
| **HA** | assist | **1/3** | **2/3** | 0/3 | **A car 1 `abs_steer~` 0 -> 6.0**; B car 2 `c1#` 60 -> 48, `steer#` 93 -> 87, `c0#` -> 40 (> 37); C car 2 `c0#` 26 -> 34 (32), `c1#` 66 -> 47 (41), `steer#` 91 -> 80 (73), `abs_steer~` 12 -> 10 (9) |
| HA5 | assist + LUT 5 | 1/3 | 2/3 | 0/3 | **A car 3 PASSES**, car 1 5.5, but car 2 (a default pass) fails `c0#` 41 |
| HAG, HA5G | + GRID_DIR | 0/3, 1/3 | 1/3 | 0/3 | worse |

HA meets section 4's (b) rule (no default-passing car turns to FAIL); HA5 does not (A car 2).

Steer-0 calls in the regime-A window (220 calls, first 60 in brackets), the quantity behind `abs_steer~`:

| | car 1 | car 2 | car 3 |
|---|---|---|---|
| original (`o_reg_r1`) | 102 (12) | 86 (9) | 77 (19) |
| DEF | 131 (36) | 67 (9) | 138 (25) |
| HA | **100 (13)** | 58 (6) | 116 (15) |
| HA5 | 106 (13) | 70 (6) | 108 (15) |

Car-1 body heading against the original's (`heading.py`, `o_rd3.msd` vs the K0 dump, median |delta| over window steps,
the +-360 wrap rows excluded by the median; the port's dump row is one physics step behind the `.msd` row on both arms):
DEF 1.033 deg (steps 1..119) / 4.228 (steps 60..119); **HA 0.402 / 2.318; HA5 0.290 / 0.579.** U-9191 recorded 0.9866 deg
on the default. With the assist the port's car turns back on its own steer-0 frames as the original's does (k = 25 -> 26:
-0.164 deg against the original's -0.161 at its first steer-0 frame).
G-CTRL / determinism / collateral (exe `eb5e1462`):

| arm | step | gates | ptrace | repeat | collateral (base 39 of 94) |
|---|---|---|---|---|---|
| `H_NOOP3` | d3b90ee0 | 80f01f35 | 56d62ae7 | = default | 39 (`H_DEF`) |
| `H_HA` | 4c81db89 | aa03f7ae | 7d3af534 | x2 identical | **42 (+3, over the registered +2)** |
| `H_HA5` | b5728d30 | dab4aa0a | 8ed07549 | x2 identical | PENDING_HA5_COLL |
| `H_L3` / `H_L7` | 24a22f13 / 10027849 | d52b2d87 / 6f21011e | fbc731c9 / cc1cdbf5 | x2 identical | 38 / 40 |

## 5. Code (default OFF everywhere, G-NOOP measured)

- `Race/HeadingAssist.cpp/.h` (new, exe only): FUN_0046c7d0 + FUN_004093b0 + FUN_00409330's walk (verbatim on the populated
  table, `(p+1) mod N` fallback logged), every constant cited; `MASHED_HEADING_ASSIST=1`, trace
  `MASHED_HEADING_ASSIST_TRACE`. Called from `TrackRenderer.cpp` right after the AI tick.
- `Ai/AiStandalone.cpp`: `MASHED_AI_LUT` mask (0x1 lookahead LUT, 0x2 steer-error LUT, 0x4 Catmull-Rom order), look replay
  `MASHED_AI_LOOK_REPLAY` (measurement only).
- `Race/PathProgStep.cpp`: `PathProg_NodeProgress(v)` (read-only getter of the shadow row's +0xc8).
- `Vehicle/VehiclePhysicsRun.cpp`: `VehiclePhysics_BodyBasis(slot)` accessor.
- `re/frida/scenario_launch.py`: `--lat-bracket-solver` (site 3 at 0x0046f6c0), appended lat-bracket columns, `--watch-write`
  (hardware write watchpoints with accurate backtraces).

## 6. User decisions needed

1. **`MASHED_HEADING_ASSIST=1` default ON?** A faithful port of an unported default-path function, reproducing the original's
   measured turn to four digits, no default-passing car lost in any regime, A car 1 closer (steer-0 calls 100 vs the
   original's 102), C car 2 closer on all items, heading residual 1.03 -> 0.40 deg; but it failed its registered sanity
   band and collateral by one field (+3 vs +2) and moves no regime count. Recommendation: ON (it is the original's
   behaviour), with the player form (slot 0, a D2 input) done in its own lane.
2. **`MASHED_AI_LUT=5` default ON?** Bit-exact lookahead, literal gate failed on probe-zero rows only. Alone it changes no count;
   with the assist it passes A car 3 and fails A car 2. Recommendation: keep OFF until the assist decision, then re-score
   HA5 against HA.

## 7. Follow-ups

- The player form of the assist (FUN_00424eb0 also runs it for slot 0): touches D2's player, needs the D2 three metrics.
- With HA, regime-A car 3 still holds steer 0 for most of its window and car 1 reads 6.0 vs 7: next instrument is the same
  heading split on car 3 (`--statediff-car 3`), and on car 1 after step 40.
- `MASHED_GRID_DIR` costs regime A in every stack (gridyaw's finding) even with the assist: unexplained.
- [UNCERTAIN] The assist switches to the neutral block 0x007f19b8 when `+0x9f0` or `+0x10` is non-zero (transcribed). The
  port's `+0x10` reads 0 on every AI row of the HA dumps; its `+0x9f0` on AI cars is not logged, so whether that switch
  matches the original's is unmeasured.
- The CRT `acos` at PC_24 is not reproduced bit for bit ([UNCERTAIN] in HeadingAssist.cpp); FUN_004c4d20's `fsin/fcos`
  likewise.
- Infrastructure: drive C: went to 0 bytes at about 19:05 (a SimGolf scratchpad held 74 GB) and the private roots lost their
  `.piz` files; two G-CTRL arms booted without assets and were re-run (memory `temp-scratchpad-files-can-vanish`).

## 8. Proposed re-classify transaction (not applied; default unchanged, decisions pending)

- **U-9191** (car-1 heading residual, "generated vs accumulated undecided"): cause found. GENERATED, on steer-0 frames, by
  `FUN_0046c7d0` (the steer-0 heading assist), which the port did not run; -0.19 .. -0.26 deg per frame on car 1, writer
  proven by hardware watchpoint (`o_ww2`). Ported behind `MASHED_HEADING_ASSIST`, default OFF pending decision 6.1.
- **New U-9729** (structural): the player form of the same assist (FUN_00424eb0 runs FUN_0046c7d0 for slot 0 too) is not
  ported; it is a D2 input (player heading on steer-0 frames), unmeasured.
- **hooks.csv notes, no C-level change** (no Frida diff of the bodies): `0046c7d0` += "exe port Race/HeadingAssist.cpp
  (MASHED_HEADING_ASSIST, AI slots), reproduces the original's measured turn to 4 digits, verify/d3b_20261016";
  `004093b0` += "exe port inside Race/HeadingAssist.cpp (TrackHeading)"; `00443dc0` / `00443300` += "exe LUT + operand order
  behind MASHED_AI_LUT=5, look replay 1384/1384 bit-exact".
- CHANGELOG: `2026-10-16 d3b: D3 (b) re-scored on the D2-closed default (A 1/3, B 2/3, C 0/3). Regime-A cause found: the
  steer-0 heading assist FUN_0046c7d0 (between the AI tick and the physics, all live cars) was never ported; ported for AI
  slots behind MASHED_HEADING_ASSIST (default OFF, user decision). AI lookahead RW-LUT + Catmull order behind MASHED_AI_LUT
  (bit-exact 1384/1384). New scenario_launch probes --watch-write / --lat-bracket-solver. verify/d3b_20261016/RESULT_d3b.md`

## 9. Provenance

`RUNLOG.txt` (port arms: exe sha8, knobs, pid, exit, output sha8). Original captures: `o_lb1`, `o_ww2` (`.log`,
`.provenance.json`, `.watch.csv`, `.latbracket.csv` local), `o_pk1.log`, `o_rt1.range66ce58.bin`. Scorers: `percar.py`,
`score_replay.py`, `heading.py`, `lbsplit.py`, `lbjump.py`, `coll.py`; decompilations `decomp_*.txt`, listings via `disasm.py`.
Step / gates / ptrace dumps over 1 MB and `*.msd` are local only (sha8 in RUNLOG); the default K0 / K1 / K26 dumps are committed.
