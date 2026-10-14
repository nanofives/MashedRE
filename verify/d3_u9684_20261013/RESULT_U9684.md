# RESULT: U-9684, the original's post-contact AI command (c1 = 255 x16, c0 ramp)

Pre-registration: `PREREG_U9684.md` (`adc86b49`), amendment 1 (`a8f364e7`, after G-LIVE failed), amendment 2
(`e6792a74`, before the counterfactual arms). Branch `work/u9684`, base `99056b2c`. Model claude-opus-5-5 (account2).
Resumed 2026-10-14 after the pause: every number below was re-run on a fresh build of `b5ed1601`
(`mashed_re.exe` sha256 `C9123FB212E3...`, no source change since the base). The WIP evidence of `b5ed1601` was
treated as unverified. Each re-run reproduced it except `U_TGT_RG`, which had been killed mid-run and is replaced.

## 0. Verdict first

> **Producer found, port exists, default stays OFF.** The 16-step `c1 = 255` hold and the `c0` ramp 15, 10, 5, 0 are
> `FUN_00414a70` returning 1 inside `FUN_00416250` (mode 3, look = cand), followed by the existing band rules on the
> new err (G-SRC 16/16 on three original captures). The port of `FUN_00414a70` is verbatim and exact offline
> (G-REPLAY 5,490/5,490) but default-OFF behind `MASHED_TGT3` + `MASHED_PATHPROG`. Live it **fails** (G-LIVE): the
> exe's car 2 returns 1 from its second call on the grid, steers off its line and never makes the contact.
> **Cause, narrowed:** the player's progress `0x008a96e8` on the grid. The original's parked player moves +0.0000921
> in z on its first step at speed 0 and so sits 0.00002 nodes behind the line (29.999977). The exe's player never
> moves (0.000000). With the original's own recorded inputs, replacing only that one value flips car 2's return
> 0 -> 1 on 790 / 790 of its calls (CF_PR). Position z alone and speed alone flip 0.
> (b) and collateral are worse with the producer on, so the pre-registered rule keeps it default-OFF. No code
> change, default G-CTRL unchanged: `991a5fff / be0d47ea / bc75b405`.

| gate | result |
|---|---|
| G-SRC | **PASS** 16/16 on `o_p2a`, `o_p2b`, `o_tg1` (M, E, B, Z each 16/16; row after the run = control, mode 0) |
| G-REPLAY (i) | **PASS** 5,490/5,490 exact at PC_24, ret and cand. Car 2's contact run (calls 791..806) is inside it |
| G-REPLAY (ii) | **PASS**: the band-rule model on the original's own err is G-SRC's E/B/Z columns, 16/16 x3 |
| G-LIVE `U_TGT` | **FAIL**: no `ret14a70 == 1` run at the exe's first car-2 `rec_9e4` drop (row 549), c1 0/16, c0 0/16 |
| G-LIVE `U_TGT_RG` (amendment 1) | **FAIL**: grid now bit-exact, still no run (first drop row 652), c1 0/16, c0 0/16 |
| G-CTRL | **PASS** `U_DEF` = `U_DEF2` = `991a5fff / be0d47ea / bc75b405` (the merged default). Unchanged: no flip |
| (b) per regime | DEF A 1/3 B 2/3 C 0/3; **TGT A 0/3 B 0/3 C 0/3** (loses 1 car in A, 2 in B) |
| collateral | **worse**: 41 / 97 paired fields past the floor (`U_TGT`) vs 39 / 94 (`U_DEF`) |
| CF arms (amendment 2, report) | `CF_PR` 790 flips, `CF_ALL` 790, `CF_Z` 0, `CF_SP` 0, `CF_NONE` 0 (control) |

Pre-registered default rule (section 2): ON only if G-REPLAY PASS, G-LIVE PASS, (b) loses no car and collateral is
not worse. Three of the four fail. **Default OFF, no knob change.** The brief's `MASHED_NO_<NAME>` opt-out is
not added because nothing is turned on by default.

## 1. The producer, from the bytes (G-SRC)

`FUN_00416250` (the aistep writer; decomp `verify/d3_gatefire_20261008/ctrlstep_decomp.txt`, bytes from
`MASHED.exe.unpatched`):

- `0x00416394` `call FUN_00414a70`, `0x0041639c` `cmp eax,1`, `0x004163ab` `call FUN_00416060` (LOS).
  `0x004163e6..0x004163f6`: look = cand (`[esp+0x2c]/[esp+0x30]` -> `[esp+0x34]/[esp+0x38]`), `mov ebx,3` (mode 3).
- `0x00416863..0x00416885`: `if (180.0 < err && err < 330.0)` (`0x005cd09c` = 180.0, `0x005cd0e0` = 330.0) then
  `ctrl[4] = ctrl[5] = ctrl[1] = 0xff`.
- `0x00416738..0x00416758` (err > 180 branch, band state `[0x0089a4ec + v*0x74] != 2` and
  `[0x007f0ff4] - [0x0089a4f0 + v*0x74] < 0xc8`):
  `ctrl[0] = ftol((0xc8 - elapsed) * 0.005 (0x005cd0ec) * [0x0089a4f4 + v*0x74])`. Stored value 21, 50 per call:
  15, 10, 5, 0.

`FUN_00414a70` reads no `+0xaec` timer and no contact flag. It reads per-car progress `0x008a96e8 + v*0x30c`
(via `FUN_00408a50`), the slot table `[0x005f2770] + 0x34 + 4*j`, the car positions (`+0x30 / +0x38` of
`FUN_0046d4a0`'s matrix), the speeds `0x00881f84 + i*0xd04` (= record `+0x9e4`, record base `0x008815a0`;
probe definition `re/frida/scenario_launch.py:1840-1845`), the own `+0x928` at row and `[0x0066d6d8]`.

`G_SRC.txt` (re-run 2026-10-14, identical to the WIP file apart from path separators): on each of the three
captures, the 16 rows after the contact carry mode 3, `ret14a70` 1, `ret16060` 1, err in (180, 330), c1 = c4 = c5 =
255, and c0 equal to the formula. The row after the run is mode 0, `ret14a70` 0.

**Why the exe does neither:** the port (`mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp`, naked body
`Ai/AiTargetClosest_naked.inc`) is default-OFF. `MASHED_TGT3` needs `MASHED_PATHPROG`, the only exe producer of
`0x008a96e8`. On the default build `FUN_00414a70` never runs, so mode 3 is never entered after a contact.

## 2. Why the port fails live

### 2.1 G-LIVE (`G_LIVE_U_TGT.txt`, `G_LIVE_U_TGT_RG.txt`)

Alignment and bounds as registered (`g_live.py`). With the producer on, the exe's car 2 returns 1 from call 1
(`U_TGT.tgt3.csv` row n 4: v 2, ret 1, cand (1.4709, -2.2462)), keeps mode 3 off its line, and never makes the
step-24 contact. Its first `rec_9e4` drop > 800 is at row 549 (`U_TGT`) / 652 (`U_TGT_RG`), with mode 0 and no
`ret == 1` run after it.

Regime-A car-1 / car-3 rows (report only, `g_live.py` tail): on `U_TGT`, cars 1 and 3 sit in mode 3 on rows
125..149 with `ret14a70` alternating 1 / 2 and `c1` never 255. On `U_TGT_RG`, car 3 has `c1` = 255 on row 125 only.
Whether the step-64 / 129 contacts still occur is not measured in these arms (no car-car log was armed).

### 2.2 First differing input (amendment 1 and 2)

`grid_inputs.py` and `U_*.ptrace.log`, against `o_tg1.msd.tgt.csv` (original, `--tgt-probe`):

| input | original | exe `U_TGT` | exe `U_TGT_RG` |
|---|---|---|---|
| player x / z, frame 0 | 0.50093675 / -1.99903715 | 0.50091845 / -2.00247097 | 0.50093675 / -1.99903715 |
| player z, frame 50 on | **-1.99894500** (moved +0.0000921 at speed 0, then rests) | unchanged | **unchanged** |
| player progress `0x008a96e8`, frame 50 on | `0x41effff4` = 29.999977 | 0.000862 | 0.000000 |
| player speed `+0x9e4` | 0.0 until the contact | 0.638046 at rest, vel (0,0,0) | 0.638046 |
| AI cars x / z, frame 0 | (-1.101, -0.902) (1.137, 0.202) (-0.465, 1.299) | off by up to 0.023 | equal to 3 decimals |
| car-2 `FUN_00414a70`, frame 50 | ret 0 | ret 1 | ret 1 |

`MASHED_ROUND_GRID` + `_ORDER` removes the grid offset bit-exactly. It does not change car 2's early return,
because the player's settle step is still missing.

### 2.3 Counterfactual replay (amendment 2, report only)

Input: the 2,373 `o_tg1` rows with frame < 45150 (791 calls per car, before the player first moves). The player's
recorded columns are replaced by the exe's values (`make_cf.py`) and replayed through the verified body
(`MASHED_TGT3_REPLAY`, PC_24).

| arm | replaced | ret mismatches | which car |
|---|---|---|---|
| `CF_NONE` | nothing (control) | 0 / 2,373 | none |
| `CF_PR` | `pr0` := 0.0 | **790** / 2,373 | car 2. Post hoc split `CF_PR_V13` (cars 1, 3 only): 0 / 1,582 |
| `CF_Z` | `z0` := -1.99903715 | 0 | none |
| `CF_SP` | `sp0` := 0.638046 | 0 | none |
| `CF_ALL` | all three | 790 | the first 30 printed rows are all car 2 |

790 = car 2's 791 calls minus its frame-0 call, where the original's own `pr0` is already 0.0. The prediction
(`CF_PR` and `CF_ALL` flip, `CF_Z` and `CF_SP` alone do not) held. **The player's progress alone carries car 2's
early return of 1.** The player's speed difference (0.638 vs 0.0) is a second player-at-rest difference, but it
does not carry this return.

[UNCERTAIN] Why the original's parked player moves +0.0000921 in z between frame 0 and frame 50 at speed 0 is not
read. The exe's player physics leaves it static. Evidence missing: an original-side player-state trace across
the first race steps (position, velocity, contact / suspension terms), diffed against the exe's `PLAYERTRACE`.

## 3. Before / after

### 3.1 G-CTRL (14,400 frames, `run_h4.ps1` recipe; step / gates / ptrace sha8)

| arm | knobs | hashes |
|---|---|---|
| `U_DEF`, `U_DEF2` | none | `991a5fff / be0d47ea / bc75b405` (= merged default) |
| `U_PP` | PATHPROG | `940e9163 / 3fcb23bf / 913300c8` |
| `U_TGT`, `U_TGT2` | PATHPROG TGT3 | `e0ec898a / 00ee597a / c6275844` (deterministic) |
| `U_RG` | PATHPROG ROUND_GRID ROUND_GRID_ORDER | `6f13a30e / 74962937 / e238b630` |
| `U_TGT_RG` | + TGT3 | `7913a61b / eb738a57 / b06be06d` |

### 3.2 (b) per regime (`SCORE_B.txt`; canonical scorer, A/B @220 cars pooled, C @196 per car)

| stack | A (K=0) | B (K=1) | C (K=26) |
|---|---|---|---|
| DEF (`B_DEF_K*` = car2pickup `B_K00/K01/K26` bit-exact: `3cb5c20e`, `b47ae9cd`, `4e0f01eb`) | 1/3 (car 2) | 2/3 (cars 1, 3) | 0/3 |
| PP | 0/3 | 2/3 | 0/3 |
| TGT | **0/3** | **0/3** | 0/3 |
| TGT_RG (report) | 0/3 | 1/3 (car 1) | 0/3 |

PATHPROG alone costs car 2 in A (`steer_distinct` 99 vs [33, 96]). TGT3 on top costs cars 1 and 3 in B (car 1
`brake_median` 255, car 3 `c0_distinct` 58).

### 3.3 Collateral (`COLLATERAL_CROSS_U_{DEF,TGT}*.txt`)

`collateral.py --mode banded --speed s.v2.rec_9e4 --anchor s.v2.substate:ge:6 --bands 1000-1500,1500-2000,2000-2600`,
600 frames from the anchor (`cut600.py`), A = `o_p2b`, floor `o_p0a`.

| B arm | floor | paired past the floor |
|---|---|---|
| `U_DEF` (baseline) | `U_DEF2` | 39 / 94 (= car2pickup `C_DEF`) |
| `U_TGT` | `U_TGT2` | 41 / 97 |

Moved outside with TGT: `s.v1.ai_mode`, `s.v1.c1`. Moved inside: `s.v2.c0`. New paired fields `s.v{1,2,3}.ret14a70`,
of which `s.v1.ret14a70` is past the floor. Every band is off-regime (the tool's own warning, median frame indices
differ > 50%), as in RESULT_CAR2PICKUP §5.

## 4. Proposed tracker transaction (re-classify, not applied here)

- **U-9684**: stays open, narrowed. New text: the producer is `FUN_00414a70` returning 1 in `FUN_00416250`
  (`0x00416394` / `0x0041639c` / `0x004163e6..0x004163f6`, mode 3), then the band rules `0x00416863..0x00416885`
  (c1/c4/c5 = 0xff) and `0x00416738..0x00416758` (c0 ramp), G-SRC 16/16 x3. Its port is exact offline
  (5,490/5,490) and default-OFF (`MASHED_TGT3` + `MASHED_PATHPROG`). Live blocker: the player's grid progress
  (exe 0.0, original 29.999977), which carries car 2's early return of 1 (CF_PR 790/790). Resolution path: the new
  row below, then re-run G-LIVE and (b) with `MASHED_TGT3`. Priority low-medium (unchanged).
- **New U row** (semantic, player physics at rest): the original's parked player moves +0.0000921 in z between
  frame 0 and frame 50 at speed 0 (`o_tg1` rows n 1 / n 4: z `-1.99903715` -> `-1.99894500`). The exe's player never
  moves (`U_TGT_RG.ptrace.log`). The exe's player `+0x9e4` also reads 0.638046 at rest against the original's 0.0.
  Path: an original-side player-state trace over the first race steps, diffed against `PLAYERTRACE`. It gates
  U-9684 and any `MASHED_TGT3` default. Priority medium.
- **hooks.csv**: no status change. `0x00414a70`, `0x00414300` stay C2 `mapped` (naked bodies in the exe, no
  installed hook). This lane adds no C3 evidence.
- **CHANGELOG line (proposed):**
  `2026-10-14 U-9684 narrowed (verify/d3_u9684_20261013/RESULT_U9684.md): the post-contact c1=255 x16 / c0 ramp is FUN_00414a70 ret 1 -> mode 3 in FUN_00416250 (G-SRC 16/16 x3). Port exact offline (5,490/5,490) but G-LIVE FAIL: the exe's car 2 targets the player from call 1 because the player's grid progress is 0.0, not 29.999977 (the original's parked player settles +0.0000921 z on step 1, CF_PR 790/790). (b) TGT A 0/3 B 0/3 C 0/3 vs DEF 1/3 2/3 0/3, collateral 41/97 vs 39/94. Default OFF, G-CTRL unchanged 991a5fff/be0d47ea/bc75b405.`

## 5. Decisions needed (user)

1. Open the player-at-rest row (the first-step settle and the 0.638 `+0x9e4` at rest) as the next D2/D3 slice. It is
   a player-physics change, so it touches the D2 watch list and needs the user's go-ahead.
2. Whether to run any further U-9684 arm before that row is fixed. Recommendation: no. Every live arm inherits the
   wrong player progress, so G-LIVE cannot pass until it is fixed.
3. `MASHED_ROUND_GRID` default (pending since 2026-10-11): this lane shows that it fixes the round-1 grid
   bit-exactly, but it does not change car 2's early return on its own. Not a reason to flip it here.

## 6. Raw

Committed: `PREREG_U9684.md`, `G_SRC.txt`, `G_REPLAY_pc24.txt`, `G_LIVE_U_TGT.txt`, `G_LIVE_U_TGT_RG.txt`,
`SCORE_B.txt`, `COLLATERAL_CROSS_U_{DEF,TGT}{,_stdout}.txt`, `U_*.{step,gates}.csv`, `U_TGT*.tgt3.csv`,
`U_TGT_RG.pathprog.csv`, `S_TGT600.*`, `B_*_K*.step.csv`, `CF_*.tgt.csv`, `CF_*.replay.txt`, scripts `g_src.py`,
`g_live.py`, `grid_inputs.py`, `cut600.py`, `make_cf.py`, `run_arms.py` (the runner, explicit PIDs only).
Force-added (gitignored, cited): `U_DEF.ptrace.log`, `U_DEF2.ptrace.log`, `U_TGT.ptrace.log`, `U_RG.ptrace.log`,
`U_TGT_RG.ptrace.log`. Left out deliberately (gitignored, not cited): the per-run `mashed_re.log` copies `*.log`
and `U_PP.ptrace.log`, `U_TGT2.ptrace.log` (not cited beyond their sha8 in §3.1; `U_TGT2`'s equals the
committed `U_TGT.ptrace.log`).
