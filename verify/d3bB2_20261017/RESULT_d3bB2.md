# RESULT d3bB2: D3 (b) regime B car 2, the cause chain (2026-10-17)

Branch `work/d3bB2` from `race/first-frame-parity` `18ddb6ea2`. PREREG `PREREG_d3bB2.md` (section 2 committed with the first knob in
`f192435b1`) and amendments 1-3 (`f710b024e`, `9cb19d224`, `f129c71dd`), each committed before the runs it governs. Model
claude-opus-5-5, account2. Every exe built with `MASHED_NO_DEPLOY=1` and snapshotted on `D:\MashedWT\d3bB2_data\exe`: base `cfc75039`
(`18ddb6ea2`), `f0ee223b` (section 2), `06373470` / `04122573` (amendments 1 / 2), final `c7ec63d3` (amendment 3). Port runs used the
private roots `D:\MashedWT\d3bB2_root` / `_root13`. Original-side captures ran on the shared install with explicit PIDs, no
piz-collision AV: `o_pw1`, `o_pw2` (player record watchpoints), `o_c2r1` (car 2 record), `o_ov1` (car 2 override timer watchpoint).

## 0. Verdict

**No knob qualifies for default ON under the registered rule. Every knob is default OFF and the default build is unchanged**
(knob-off G-CTRL `4c81db89 / aa03f7ae / 7d3af534` twice on the final exe, K0 / K1 / K26 dumps byte-identical to dflip's).

Regime B car 2 fails (b) through a chain of four port differences, each found from the bytes and confirmed on the running original:

| # | first divergence (window call) | original code | exe fix | status |
|---|---|---|---|---|
| 1 | k = 1..2: c0 0, 0 against 11, 23 (all three cars) | AI clock `DAT_007f0ff4` advanced on pre-GO ticks by `FUN_0040fc00` `0x0040fe4f..0x0040fe5e`; `FUN_00416250` fresh-steer gate `0x00416608..0x00416621` | `MASHED_AI_COUNTDOWN_CLOCK` (new) | exact: k = 1, 2 equal on 9/9 car-regime pairs |
| 2 | k = 0..22: x drift 0.0022, contact one frame late | spawn yaw `FUN_00408b00` (0.1101 deg), grid, player first-step settle `FUN_004704c0` -> `FUN_0046f6c0(0)` `0x00470638` (watched `0x0046fc03`) | existing `MASHED_GRID_DIR`, `MASHED_ROUND_GRID(+_ORDER)`, `MASHED_POS_MODEL`, `MASHED_PATHPROG` | car 2 within 0.001 at k = 22, contact in the original's frame (SAT margin 3e-4) |
| 3 | k = 23: mode-3 brake missing (U-9684) | `FUN_00414a70` ret 1 -> mode 3 in `FUN_00416250` | existing `MASHED_TGT3` | car 2 enters mode 3 at call 23 |
| 4 | k = 23..31: heading -3.65 deg/frame against +0.147 | type-4 contact event: `FUN_00469df0` push -> `FUN_00422ba0` -> `FUN_00417370` AI override timer 1440, replayed by `FUN_00418560`'s tail (watched `0x0041739b`, `0x004187ee`) | `MASHED_AI_CONTACT_EVENT` (new) | G-COAST +0.146 against +0.1475 |
| 5 | k = 22: `FUN_00414a70` fires one tick early | `FUN_00425a40` runs the AI tick before every car's physics | `MASHED_AI_TICK_FIRST` (new) | G-ORDER FAILED (U-9747) |
| 6 | k = 24..: post-contact speed 1045.5 / 1038.3 against 1050.409; second coast at k = 61 missing | `FUN_00470c70` runs every car's A4 before any substep | not attempted (D2-touching restructure) | open, U-9746 |

With the whole stack (`S`) car 2's commands equal the original's on every call k = 0..38 except k = 1 (U-9747). Its regime-B items read
`c0#` 39 [13, 37], `c1#` 48 [17, 46], `steer#` 86 [29, 84], `abs_steer~` 17.5 [0, 16], each 1-2 over the bound. But the stack drops B
cars 1 and 3, so B reads 0/3. `MASHED_AI_CONTACT_EVENT` alone on the default passes B car 2 and fails B cars 1 and 3: the exe has
AI<->AI contacts in the window (1-2, 1-3, 2-3) and each now freezes both cars' commands for 29 ticks (U-9185 / U-9186 class).

## 1. Baseline (step 1)

`B_DEF_K1` on base `cfc75039`, two runs: dump `80bce410` both times (= dflip). `B_DEF_K4` / `B_DEF_K27` give the same verdict. Car 2
fails `c0#` 40 [13, 37], `c1#` 48 [17, 46], `steer#` 87 [29, 84]. Cars 1 and 3 pass. K0 `a89ff9ef`, K26 `54da2e83` (= dflip).

## 2. How each cause was found (step 2)

`sbs.py` (original row k against port row k of the (b) window), `hull.py` (contact inputs), `sat.py` (offline SAT on recorded inputs).

1. **AI clock.** All three cars' k = 1..3 values fit `c0 = err * speed * _DAT_005cd0e8` with speed 280 / 563 / 861. The port wrote 0, 0 because
   its clock read 50 / 100 / 150 (dump `clk_0ff4`) against 43100 on `o_reg_r13`; with the stamps at 0 the gate takes the decay arm
   (`0x00416623..0x00416643`, c1 = 0). On 31/31 labelled captures the clock is 0 on the first probed call and 38850..46400 at GO.
   Readers of `0x007f0ff4` (`datarefs_7f0ff4.txt`): stamp differences in `FUN_00416250`, `FUN_00416a30`, `FUN_00417da0`; the two
   absolute float reads are off the race path (`0x00470df2` gated by `[0x007f0fd0]`, `0x00429424` feeds a formatter).
2. **Spawn and settle.** The player's z moves +0.0000921 on the first physics step at speed 0. Writer by hardware watchpoint: `FUN_004704c0`
   writes both state matrices then `FUN_0046f6c0(0)` (`0x00470638`) moves slot 1 at `0x0046fc03`; the first `FUN_0046e9e0` (`0x00470ad3`)
   copies slot 1 into slot 0 (`0x0046ea76`). That is d2posmodel's fix B part 3. Car 2's spawn yaw is 180.0000 on the original and
   179.8899 on the port; `MASHED_GRID_DIR` applies `FUN_00408b00`'s law (gridyaw) and removes the 0.0022 drift.
3. **Contact frame.** `FUN_004709a0` tests pairs on the `[+0x9a8]` (start-of-substep) halves (`0x00470b44..0x00470ba9`). On the original's
   own records the SAT counts one vertex inside at frame 836 with test value -0.0003 (`sat.py`): a 3e-4 margin, so the 0.0022 drift
   alone moved the contact one frame.
4. **Post-contact coast.** The original's car 2 is grounded (`+0x9e0` 4.0) with c1 = 255, yet its heading moves a constant +0.147 deg per
   frame: the steer reaching `FUN_0046e9e0` is the frozen pre-contact c0 = 21 (0.23 x 21/23 x grip 0.7). Override timer watched on
   `0x0089a5e4`: `0x0046ac8e` (+0xaec 480), `0x0041739b` (1440, ret `0x00422ed2` = `FUN_00422ba0`), then `0x004187ee` -50 per tick to 0
   at `0x004187f6`. `FUN_00417370` = `0xb4 - 0xb4 * __ftol([0x0089a360] - 10.0)` (`[0x005cc55c]` = 10.0). The push is not dev-viz: both
   arms of `FUN_00469df0`'s tail push type 4 unless `FUN_0040e350()` is 1..5.
5. **Frame order.** `FUN_00425a40`: `FUN_00418860` (AI tick) -> `FUN_00424eb0` -> `FUN_00470c70` -> `FUN_00422ba0` -> `FUN_004252c0`. The
   exe steps the player before the AI tick, so the AI tick of the contact frame read the impulse (car 2 `FUN_00414a70` ret 1 at call 22).
6. **A4 order.** `FUN_00470c70` runs the A4 loop for all cars (`0x0047101b..0x00471099`) and the speed refresh (`0x004710a1..0x004710da`)
   before `FUN_004709a0` (`0x00471137`). The exe runs A4 inside each car's step, after the player's substeps applied the impulse.

## 3. Code (all default OFF)

- `MASHED_AI_COUNTDOWN_CLOCK`: `DAT_007f0ff4 += units` on each countdown freeze frame (`Ai_AdvanceCountdownClock`). `DAT_007f0ff8` untouched.
- `MASHED_AI_CONTACT_EVENT`: `g_ccLastContact` in `VehicleCarCarContact`; push at the `0x00470bcd` call site (mode gate 1..5); drain after all
  physics (`Ai_ContactEventDrain`: `FUN_0046cbb0` gate, `FUN_0040e470 != 1`, `FUN_00417370`). Under it the step dump records
  `FUN_00416250`'s output (`Ai_RecordStepOut`), the original probe's sample point. Trace `MASHED_AI_CONTACT_EVENT_TRACE`. The
  `CarCarContacts.cpp` comment that called the push dev-viz only is corrected.
- `MASHED_AI_TICK_FIRST`: the AI tick phase runs before the player's physics (one lambda, same body as before; knob off calls it at its
  old place). It also runs `PathProg_FrameTick` on countdown frames (`FUN_0040da50` on pre-GO ticks, `0x0040fc75`).

## 4. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP (all builds, knobs unset) | K0 `a89ff9ef`, K1 `80bce410`, K26 `54da2e83` on `f0ee223b`, `06373470`, `04122573`; G-CTRL `4c81db89 / aa03f7ae / 7d3af534` x2 on `c7ec63d3` | PASS |
| G-TOOK (clock) | first-row clock 9100 = 50 x (181 + 1) | PASS |
| G-RAMP | k = 1, 2 equal to the original on 9/9 car-regime pairs (1/2, 11/23, 4/9) | PASS |
| G-EVT | car 2 timer 1440 at its first player contact; player slot state 1, no timer | PASS |
| G-COAST | car 2 heading +0.146 deg/frame on k = 23..31 under `S` (original +0.1475); -1.92 without the event knob | PASS |
| G-ORDER | car 2 first `FUN_00414a70` ret 1: call 1 under `S` (AI progress not converged at GO, U-9747), call 22 under `S-T` | FAIL |
| determinism | `B_CC_K1` x2 `5257c915`; `B_SL_K1` x2 `361fb408`; `H_CC` x2 | PASS |

## 5. D3 (b) per arm (`vsum.py`, `re/tools/b_envelopes.json`; `arms/final/SCORE_FINAL_B.txt`, `arms/knob/`)

| arm | knobs | A | B | C | notes |
|---|---|---|---|---|---|
| DEF | none | 1/3 | 2/3 | 0/3 | B car 2 `c0#` 40, `c1#` 48, `steer#` 87 |
| CC | clock | 1/3 | 2/3 | 0/3 | A car 3 PASSES, **A car 2 turns to FAIL** `c0#` 41; B car 2 39 / 49 / 87; same on K2, K4, K27, K54, K57 |
| E | event | 0/3 | 1/3 | 0/3 | **B car 2 PASSES**; B car 1 `c1#` 16 [17, 46], car 3 three items |
| T | tick first | 1/3 | 2/3 | 0/3 | as DEF on every verdict |
| CCE / CCET | clock + event (+ tick first) | 0/3 | 1/3 | 0/3 | identical dumps; B car 2 only `c0#` 39 |
| S | all + U-9684 chain + GRID_DIR | 0/3 | 0/3 | 0/3 | B car 2 39 / 48 / 86 / `abs_steer~` 17.5 |
| S-E | S without event | 0/3 | 0/3 | 0/3 | car 2 spins after the contact |
| S-T | S without tick first | 0/3 | 0/3 | 0/3 | car 2 stepped 135 of 220 calls (NOT SCORED) |

## 6. G-CTRL, 13 tracks, D2

| arm | G-CTRL step / gates / ptrace | 13 tracks (rows, RED, CITED frames, instrument gates) | D2 x3 |
|---|---|---|---|
| knobs off | `4c81db89 / aa03f7ae / 7d3af534` x2 | 870, 0, 39/39, PASS | (dflip) |
| CC | `490c4b2d / 1fdd0ea6 / 46088ac4` x2 | 867, 0, 39/39, PASS | see below |
| CCET | `33b5eab2 / 2b13c757 / f3e8e2f9` | 867, 0, 39/39, PASS | see below |

The 3 rows that go under CC are dflip's `C27-CAR-PART-OFFSET` rows (Roundabout 1091, Training 924 / 959), with more draws matched
(316 / 211 / 209 against 314 / 209 / 205). CCET moves one C27 row between Training 884 and 924.

D2 (`D2_<arm>_1..3`, each alone, 90 s, `verify/d2close_20261016/score_all.py d2`, bounds `d81a8df6`; `arms/d2/SCORE_D2_d3bB2.txt`):

| run | lines | slip 1500-2000 [0.18855, 0.19635] | slip 2000-2600 [0.24488, 0.25487] | driving-median [1904.70, 1982.44] (n) | H1 | launch |
|---|---|---|---|---|---|---|
| `D2_CC_1` | 1626 | 0.1890 | 0.2480 | 1909.37 (1416) | 398/400, 1362.7 | L = 0 at 0.19 %, d = 15 |
| `D2_CC_2` | 1634 | 0.1890 | 0.2482 | 1914.84 (1424) | 398/400 | same |
| `D2_CC_3` | 1630 | 0.1890 | 0.2476 | 1912.04 (1420) | 398/400 | same |
| `D2_CCET_1` | 1631 | 0.1890 | 0.2480 | 1912.04 (1421) | 398/400 | same |
| `D2_CCET_2` | 1628 | 0.1890 | 0.2476 | 1912.01 (1418) | 398/400 | same |
| `D2_CCET_3` | 1630 | 0.1890 | 0.2476 | 1912.04 (1420) | 398/400 | same |

6/6 in bounds, all above the 1600-line floor, none re-run. `D2_CC_2` is byte-identical to d2close's `D2_DEF_2` (`0c02cb88`) and
`D2_CC_3` = `D2_CCET_3` (`59461ead`): the D2 recipe runs the player solo, so the three AI-side knobs do not reach it. `D2_CC_1`'s
driving-median 1909.37 is 2.7 under the dflip spread (1912.04..1913.06) and 4.7 above the bound, with the identical slips, H1 and launch.

## 7. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

- **New U-9744** (AI clock at GO), **U-9745** (type-4 event / AI override), **U-9746** (A4 order), **U-9747** (AI progress at GO under
  tick-first). U-9743 was the highest id on `6cd980e46`.
- **U-9684** note: the chain above reproduces car 2's mode-3 run on calls 23..38.
- hooks.csv notes on `00469df0`, `00417370`, `00422ba0`, `0040fc00`, `00470c70`. CHANGELOG one line.

## 8. User decisions needed

1. `MASHED_AI_COUNTDOWN_CLOCK` default: faithful and exact, but it trades A car 2 for A car 3 (as `MASHED_AI_LUT=5` does). The rule says OFF.
2. Whether to open U-9746 (every car's A4 before any substep). It changes the player's physics order, so D2 must be re-scored.
3. `MASHED_AI_CONTACT_EVENT` is the original's behaviour and passes B car 2 alone, but its default waits on the exe's extra AI<->AI
   contacts (U-9185 / U-9186).

## 9. Follow-ups

- U-9746 first: with it the post-contact speed and the second coast at k = 61 are the next comparisons.
- U-9747: find the exe writer of the AI cars' alive word and make it hold during the countdown.
- U-9185 / U-9186 with `MASHED_AI_CONTACT_EVENT` on: the extra contacts now cost 29 frozen ticks each.
- Collateral (`coll.py`) was not run in this lane.

## 10. Provenance

`arms/RUNLOG.txt`, `arms/{knob,k2,k3,final,d2}/RUNLOG.txt` (exe sha8, knobs, pid, exit, output sha8). Scripts: `run_arms.py`, `vsum.py`,
`percar.py`, `sbs.py`, `hull.py`, `sat.py`, `sum13.py`, `apply_trackers.py`. Static evidence: `decomp_*.txt`, `disasm_*.txt`,
`datarefs_*.txt`. Original captures: `o_pw1`, `o_pw2`, `o_c2r1`, `o_ov1` (`.watch.csv`, `.aistep.csv`, `.provenance.json`; `.msd` local).
13-track replays: `D:\MashedWT\d3bB2_data\tracks\{def,cc,ccet}` (scores copied to `tracks/`). Moved off git by `move_evidence.py`
(299 files, sha8 in the RUNLOGs that stay): every `.msd`, the latbracket CSVs and the arm outputs this file does not cite, under
`D:\MashedWT\d3bB2_data\verify\`. In git: PREREG, this RESULT, scripts, RUNLOGs, scores, the cited dumps and traces, the D2
`motion_diag` logs (force-added, `*.log` is ignored), the original-side `.watch.csv` / `.aistep.csv` / `.provenance.json`.
