# RESULT d2engage: fix B's three D2 failure causes (2026-10-16)

Branch `work/d2engage`, rebased on `race/first-frame-parity` `34ea5b71`. PREREG `PREREG_d2engage.md` + amendments 1-3
(each committed before the runs it governs). Final measurements on the rebased build: exe sha8 `7da5de6b`, base exe
`81959000` (built from `34ea5b71`). Of the 30 arms run on both builds, 22 give bit-identical outputs
(`pre_rebase/RUNLOG.txt` vs `RUNLOG.txt`): every G-CTRL, D and D3 (b) arm, plus `D2_DEF_1`, `D2_K_1` and `D2_DX_1`.
The other 8 are 90 s wall-clock D2 arms. Their logs differ, but every verdict is the same (pass or fail per gate, d=15
vs d=14), and B's driving-median moved within 1864.87..1867.56. Prediction P3 of amendment 3 holds. `D2_BC_1` is new.
Model claude-opus-5-5. The first half of the lane ran on account2 and the resume on account3.

## 0. Verdict

| cause (brief) | found | ported behind | effect |
|---|---|---|---|
| 1. drive force engages at d=14 under the seed | yes, from the bytes, with a per-frame witness | `MASHED_POS_ORDER` (part 4) | engages at **d=15 on 3/3**, charge series identical to the original's (section 1) |
| 2. Training x=-2.04 wall pin | yes: it is cause 1's trajectory, not a separate defect of fix B | nothing new (part 4 removes it) | B escapes 3/3, H1 391/400. The port-wide post-impact lateral collapse that decides it stays open (section 3) |
| 3. post-A4 `ReassertContacts` reads another car's contact cache | yes, from the bytes | `MASHED_POS_CONTACTS` (part 5) | knob took. Inert on G-CTRL (`H_K` = base) and on every D2 metric (section 2) |

**Fix B (`MASHED_POS_MODEL`, parts 1-5) stays default OFF.** Section 5's rule fails on **driving-median** (1865.19 to
1867.56, bound 1904.70..1982.44) and on **G-SETTLE on the default grid**. Every other gate passes. The orchestrator
rule (amendment 3) holds every knob OFF anyway. **The default build is unchanged:** G-CTRL `716cb2fd / 2a2afda1 /
ba90ed88` on `BASE_H_DEF`, `H_DEF` and `H_DEF-2`.

**Part 5 alone meets section 5's independent rule** (`H_K` = the base hashes, so it only changes a path that has no
effect on G-CTRL). It is held OFF pending the user decision (section 6).

**driving-median (U-9180):** B moves it from 1860.27 (default, `D2_DEF_1`) to 1865.19..1867.56, a gain of +4.9..+7.3.
That leaves it 37.1..39.5 under the lower bound. Part 4 alone (`D2_O_1`) moves it the most, to 1879.09 (+18.8, still
25.6 under).

## 1. Gates (rebased build, scorers of PREREG section 3)

| gate | arm(s) | measured | verdict |
|---|---|---|---|
| no-op G-CTRL | `H_DEF`, `H_DEF-2` vs `BASE_H_DEF` | `716cb2fd / 2a2afda1 / ba90ed88` on all three | PASS (P1, P2 met) |
| determinism | `H_B` = `H_B-2` | `27b385de / ca0ba0a1 / 72ecde47` twice | PASS |
| knob-took part 4 | `D2_BO_1`, `D2_B_1..3` | marker at line 0, settle line `bf4=3000 bf8=0`, `launchrev release` at line 2, d0 line `bf4=2800 bf8=2` | PASS |
| knob-took part 5 | `D_B` | settle call 1 enters st (1,1,1,1) fv 0 gc 4. `D_B.settle.txt` sha8 `8dfe7239` is the same file before and after the rebase | PASS |
| G-LAUNCH | `D2_B_1..3` | `+0xb14` engages d=15 3/3, best lag L=0 at 0.13 % | PASS |
| G-SETTLE, original grid | `D_BRG` | x bit-identical, dz +1.226e-7 (<= 2.4e-7), dy -4.593e-6 (<= 1e-5) | PASS |
| G-SETTLE, default grid | `D_B` | x bit-identical, dz +3.575e-7, dy -1.330e-5 | **FAIL** (spawn-pose margin, as d2posmodel) |
| G-PROG | `D_BRG` (control `D_RG`) | 29.999977 on all 23 parked rows (control 0.0 x27) | PASS |
| D2 slip 1500-2000 | `D2_B_1..3` | 0.1953 x3 in 0.18855..0.19635 | PASS |
| D2 slip 2000-2600 | `D2_B_1..3` | 0.2499 x3 in 0.24488..0.25487 | PASS |
| D2 driving-median | `D2_B_1..3` | 1867.56 / 1867.56 / 1865.19 (n 1292 / 1292 / 1291) | **FAIL** 0/3 |
| recovery H1 | `D2_B_1..3` | 391/400, median 1293.1, 3/3 | PASS |
| collateral | `H_B` (floor `H_B-2`) vs `H_DEF` (floor `H_DEF-2`) | 40 vs 39 fields above floor | PASS (+1) |

Same-build controls: `D2_DEF_1` 0.1927 / 0.2489 / 1860.27, d=15, 398/400. `D2_B0_1` (d2posmodel's B) reproduces
NA / NA / 1338.62 n=54, d=14, 11/400.

## 2. Causes 1 and 3: what the bytes say and what the knobs do

**Cause 1 (PREREG 0.1-0.3).** The release `FUN_0046d780` sets `+0xbf8 = 2` at `0x0046d7a2`. The `+0xbf8 == 2` arm of
A6a `FUN_00467650` (`0x00467dee..0x00467e44`) decrements `+0xbf4` by dt at two sites per qualifying front wheel: the
`FUN_0040e350() == 6` site at `0x00467cf1..0x00467cfe` and the arm's own site. That is 200 per frame from 3000, so the
original engages at d=15. Under d2posmodel's seed, the exe's spawn-settle step ran after `LaunchRev_Release()` with
seeded states 1. That opened the gate and spent one extra 200, so the settle line read 2800 and engagement came at
d=14. The original runs `FUN_0046baa0` (`0x0041043a`) and the charge (`0x00410441`) on every countdown tick and the
release (`0x0041049b`) after them. `DAT_0063ba8c` is 5 on those ticks and becomes 6 only at `0x004104e3`. Part 4 does
three things: it moves slot 0's settle before the release, it runs the settle with `Fi_GameMode()` = 5, and it runs
the release tick's own `FUN_0046baa0` after it for every slot.

Per-frame witness (`DRIVEGATE.txt`): the `D2_B_1` series index k carries the same `bf4` / `bf8` as `orig_solo3.msd`
capture index 885+k for every k = 0..17, with all four port wheel states 1 (the original's `+0x198` / `+0x25c` read 1
on those indices) (3000/0, 2800/2, 2600/2 ... 200/2, then 0/0 at k=15). `+0xb14`
first goes nonzero at k=16 (x -269841), the original at idx 901 (-265866.2), both d=15. `D2_B0_1` runs one index early
at every k (settle 2800, engagement at series 15 = d=14). `D2_DEF_1` reaches the original's d0..d15 values, but its
settle line reads `bf4=3000 bf8=2`: the settle runs after the release with the gate closed.

**Cause 3 (PREREG 0.4).** In `FUN_00470c70`, between the end of the A4 loop (`0x00471099`) and the substep loop
(`0x00471106`), the original runs only two things: the speed refresh (`0x004710bc..0x004710da`) and `call 0x0046dc20`
at `0x004710f4`. The callee reads `+0x9f0` and writes the queue `0x007e9de0..` only. It writes no wheel state, no
normal and no `+0x9e0`. The port's post-A4 `ReassertContacts(r)` read the `g_c*` globals of the last car that ran the
solver, which zeroed the seeded states on slot 0's first step. Part 5 skips that call.

Measured effect:
- G-CTRL: `H_K` = `716cb2fd / 2a2afda1 / ba90ed88`, bit-identical to the base.
- D2: `D2_K_1` = `D2_DEF_1` on every metric, and `D2_BK_1` = `D2_B0_1` on every metric (pre-rebase).
- Under the seed it changes the settle's first solver call, but the settle's end position is bit-identical to B0's
  (prediction 4.3, amendment 1).

## 3. Cause 2: the wall pin (report, `CAUSE2.txt`, `cause2.py`)

**Correction to PREREG 0.5.** 0.5 cites U-9173 as open. **U-9173 was RESOLVED 2026-10-02 as a banding artefact**
(UNCERTAINTIES.md), so this lane withdraws the attribution "low-speed lateral damping gap (U-9173)". The finding below
is the post-impact lateral hold that U-9156 already records (D2_REOPEN section 21.2: lateral held at -81.7..-81.9 on
the original against the port's -3.76).

Measured on the original (`orig_solo3.msd`, first impact at idx 981, sp 1815.4 -> 219.9) and on the port's three
branches (A6a dump, first impact = row 0):

| frames after impact | original lat retention | port lat retention (B0X / DX / BX alike) | port clamp #6 |
|---|---|---|---|
| +1..+6 | 0.819 0.857 0.862 0.867 0.874 0.880 | 0.819 0.824 0.860 0.865 0.871 0.878 | HIGH arm, k 0.195..0.199 |
| +7 | **0.998** (lat 81.75) | 0.885 (lat 75.1) | HIGH arm, k 0.199 |
| +8..+11 | **0.999 1.001 1.003** 0.894 (lat 81.7..81.9, then 73.3) | **0.900 0.757 0.495 0.265** (lat 67.6 -> 6.7) | LOW arm, k 0.194 -> 0.799, grip product 26416 -> 6603 |
| second contact | +19 (idx 1000) | **B0X +18** (sp 174.3 -> 136.6), DX / BX **+19** (sp 192.5 -> 149.8) | |

(Port columns are B0X rows. DX and BX agree with them to under 1 %, one frame later in absolute frame number.) On the
original's idx 988-991, `+0x9e0` = 4.0 and the four states are 1. Those are the record inputs of clamp #6's gates
(`0x0046874c` `|vel| != 0`, `0x00468761` `+0x9e0 == 4.0`).

What decides the pin:
1. The lateral collapse at +8..+11 happens on every port branch, so it does not set the pinned branch apart.
2. What sets B0X apart is the second contact one frame earlier and about 18 units slower. That shift comes from the
   d=14 engagement (cause 1).
3. After it, B0X spends 491 frames in the 0-50 band, all 491 on the LOW arm, with median k 0.9285 and median lateral
   retention 0.1114 (prediction 4.6 holds).
4. With part 4 on, B's second contact is at +19, as on the original, and B escapes on 3/3.

[UNCERTAIN] (open part of U-9156 / U-9180): the original's clamp-#6 arm, `k` and grip product on idx 988-991 are not
in `orig_solo3.msd`. It is not measured whether the original's clamp runs there with k >= 0.1 while other forces
restore the lateral, or removes nothing. Resolving it needs a live A6a-entry / `--lat-bracket` capture on the
original at first impact +7..+10. No port is attempted here. The input that differs feeds clamp #6's grip product,
and the sibling d2bsp lane (`FUN_00538c80` and the chunk loop) changes the contact producer upstream of it.

## 4. Report arms

| arm | knobs | result |
|---|---|---|
| `H_O` | part 4 alone, G-CTRL | `d0bb3784 / 93240bfd / fa35321e`: part 4 alone changes the default. Its 4(b) `VehiclePhysicsStateReset` runs for every slot after the settle, so the first real step enters the reset state instead of the settle's output |
| `D2_O_1` | part 4 alone | 0.1931 in / 0.2520 in / **1879.09** OUT, d=15, 391/400 |
| `D2_BC_1` | B + fix C (`MASHED_SPEED_REFRESH`, amendment 2) | 0.2020 OUT / 0.2629 OUT / 1806.09 OUT (n 122 / 180 / 448), d=15, 395/400. Fix C inside B fails all three metrics |
| D3 (b) `B_DEF_K{0,1,26}` | default | A 1/3, B 2/3, C 0/3 |
| D3 (b) `B_B_K{0,1,26}` | B | A **0/3** (car 2), B **1/3** (car 3, `c0_distinct` 38 vs 13..37), C 0/3 |

D3 (b) scorer note: `d3b_score.py` now reads `re/tools/b_envelopes.json`, which has per-car C envelopes (user decision
2026-10-13). The `verify/d3_regimes_20261013` copy pools C, and `check_regime` reported "no envelope" for every C car.
A and B verdicts are the same under both files. The step dumps under B are shorter (1909 / 2405 / 2956 rows against
2322 / 3055 / 3055 on the default). That is recorded and not explained.

## 5. D2 watch list (DEFERRED D-11071 re-pickup items touched)

- **`ReassertContacts`** (part 5): no measured D2 or G-CTRL effect (`H_K` = base, `D2_K_1` = `D2_DEF_1`).
- **Player-car speed on Training** (parts 4 and B): driving-median +4.9..+7.3 (B) and +18.8 (O alone), both still
  outside the bound. Slips stay inside under B. `+0x4a4`, `FUN_00538c80`, grip-clamp #6 code and the chunk loop are
  not touched.

## 6. User decisions needed

1. **Part 5 (`MASHED_POS_CONTACTS`) default ON?** It meets section 5's independent rule (inert on G-CTRL, a fidelity
   fix of a path with no effect there). It is held OFF by the orchestrator rule until the pathprog_a section 6
   decision is confirmed.
2. **d2posmodel 5.2 (which grid witnesses G-SETTLE)** is still open. B passes on the original's grid and fails on the
   default grid.
3. Fix B as a whole: section 5 says OFF (driving-median, default-grid G-SETTLE). B matches the default on D2 (slips
   in, H1 PASS, d=15) but drops one D3 (b) car in each of regimes A and B. No flip is proposed.

## 7. Follow-ups

- Live A6a-entry capture on the original at first impact +7..+10 (the `[UNCERTAIN]` in section 3, U-9156 / U-9180).
- Re-score B after d2bsp merges (the contact producer feeds clamp #6's grip product).
- Why B loses D3 (b) cars and shortens the AI step dumps (part 4(b) resets every slot, first suspect, unmeasured).
- Harness: `verify/d3_regimes_20261013/score_regimes.py` crashes printing the per-car C envelope, and its envelope
  copy pools C. Use `re/tools/b_envelopes.json`.

## 8. Provenance

`RUNLOG.txt` (one line per arm: exe sha8, knobs, pid, exit, output sha8). Scorers: `score_arms.py` (D2 + launch + H1
+ knob-took), `g_settle.py` (`G_SETTLE.txt`), `collateral_run.py` (`COLLATERAL_CROSS_H_{B,DEF}.txt`), `d3b_score.py`
(`SCORE_D3B.txt`), `cause2.py` (`CAUSE2.txt`), `msd_win.py` / `md_win.py` (`DRIVEGATE.txt`). Score files: `SCORE_D2.txt`.
Pre-rebase evidence: `pre_rebase/`. Step / gates / ptrace CSVs over 100 KB and `*.log` files are not committed.
Their sha8 are in `RUNLOG.txt`. Runs used private `MASHED_ROOT` copies containing `original\` (no write to the shared
install, `MASHED_NO_DEPLOY=1` on both builds). Every PID killed was one this lane spawned (`run_arms.py`).
