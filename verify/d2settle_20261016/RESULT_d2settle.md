# RESULT d2settle: parked-player first-step settle + U-9181 (+0x4a4)

Pre-registration `PREREG_d2settle.md` (`b3eb227b`), amendments 1-4 (`ab681fcb`, `e857a24f`, `919b3dc7`,
`23b05c32`). Branch `work/d2settle`, base `1caf1a10`. Model claude-opus-5-5 (account2). Exe runs used
`MASHED_ROOT` = a private copy of `original/` (CWD-log isolation only). Original captures used the shared install.

## 0. Verdict first

> **PARTIAL. U-9181 is closed and fixed by default. The settle mechanism is found and confirmed in the port, but not
> ported to the default build. That needs a user decision.**
>
> 1. **U-9181 was a mislabel plus a real 5e-5 defect.** The 692.302 figure was `g_suspScale`, not `+0x4a4`. The
>    real exe `+0x4a4` was 0.678003 against the original's 0.678037, because A3 `FUN_0046b540` used `std::sqrt`
>    where the original calls RW's LUT sqrt `0x004c3b30`.
>    **Fix A** (`VehicleInit.cpp`, opt-out `MASHED_NO_A3_LUT`) passes all its gates: G-A3 26/26 bit-exact,
>    G-4A4 18,663/18,663, D2 slips inside 3/3, (b) unchanged, collateral unchanged. It is **wired default ON**.
> 2. **The `+0x4a4` -> settle link is refuted.** Fix A leaves the settle step bit-identical.
> 3. **Settle mechanism, from the bytes, confirmed both sides.** `FUN_0046baa0` (the per-tick hold reset) sets the
>    four wheel states to 1 and fv to 0. The first `0x0046f6c0` call then latches all four wheels, drops the
>    most-loaded one, and translates the body matrix by -mean(fv)·n. In the original that is -0.0029486·n =
>    (0, -0.0029472, +0.0000921), against a measured move of (0, -0.0029474, +0.0000921).
>    The port's own solver, given the same seeds, real wheel points and the original's position (D0-e), lands at
>    x exact, z within 1 ulp, y 4.4e-6 off.
>    The exe misses it three ways: it never seeds the states, it uses stand-in wheel geometry, and it discards the
>    solver's translation (`car_pos_[1]` = ground + offset).
>    **Fix B is not ported.** It changes the exe's position model for every car on every frame.
>    G-SETTLE and G-PROG FAIL as registered.
> 4. **`+0x9e4` at rest.** The brief had it reversed: the original reads 0.0 at rest and the exe 0.638.
>    **Fix C** ports the every-frame refresh `0x004710a1..0x004710da` byte-faithfully and passes G-9E4. But it
>    moves D2 out of bounds (slip 2000-2600 0.2654, driving-median 1666.43), so it **stays default OFF**
>    (opt-in `MASHED_SPEED_REFRESH=1`).
> 5. **New default G-CTRL: `c2e4d60d / b50bd39b / ba90ed88`** (was `991a5fff / be0d47ea / bc75b405`).
>    `MASHED_NO_A3_LUT=1` reproduces the old hashes exactly.
>
> **U-9180:** driving-median moves **+8 toward the bound** (1849.16-1852.66 -> 1860.35-1860.98, n ~1365, 3 runs
> each). It is still OUT of `1904.70..1982.44`.

| gate | arm | result |
|---|---|---|
| G-A3 (26 A3 outputs vs `o_p0a.msd` frame 1) | fix A / opt-out | **PASS 26/26** / 0/26 (knob took) |
| G-4A4 (`rec_4a4` = 0.678036749) | `H_NOSR`, `B_NOSR_K0` | **PASS** 18,663/18,663, 2,321/2,321 (base 0.678002715) |
| G-9E4 (player `+0x9e4` = 0 at rest) | fix C on (`H_DEF`) / off (`H_FDEF`) | **PASS** 23/23 / FAIL 0/23 (0.6380304) |
| G-SETTLE (dz 0.0000921488, x unchanged) | default | **FAIL**, no move (fix B not ported). Diag D0-e: dz +0.0000922680 |
| G-PROG (player progress 29.999977) | `D_RG` / `D_OFFRG` | **FAIL**, 0.000000 on both (no settle) |
| No-op control (all opt-outs = base) | `H_OFF`, `H_FOFF` | **PASS**, `991a5fff / be0d47ea / bc75b405` on both builds |
| G-CTRL determinism | `H_NOSR` = `H_NOSR2` = `H_FDEF`; `H_DEF` = `H_DEF2` | **PASS** |
| D0-c (port solver, real wheels, states 0) | `D_RGX` | prediction **MISSED**: +0.00994·n vs original -0.002949·n |
| D0-e (as D0-c + `FUN_0046baa0` seeds) | `D_RGXS` | prediction **PASS**: (0, -0.00295186, +0.0000922680) |

## 1. U-9181 (+0x4a4): a mislabel on top of a real 5e-5 defect, fixed (fix A)

- **The 1021x gap was not a +0x4a4 gap.** U-9181's port figure 692.302 is motion_diag's `susp=` column, which prints
  `g_suspScale` = `_DAT_0088e5f0` (`VehiclePhysicsRun.cpp:1468`, `:1507`). The live original's `_DAT_0088e5f0` is
  692.302185 (`VehiclePhysicsRun.cpp:830-831`), so that column agrees across sides. The port's record `+0x4a4` is
  `rec_4a4` in `MASHED_AI_STEPDUMP`: 0.678002715 on 21,402 / 21,402 rows of `verify/d3_u9684_20261013/U_DEF.step.csv`.
  The original's is `0x3f2d93d1` = 0.67803675 on every frame of `o_p0a.msd` / `o_rd1.msd` / `o_p2a.msd`.
- **Real writer.** `dispsweep.py 0x4a4`: the only record write is `0x0046ba00 fstp [esi+0x4a4]` in A3 `FUN_0046b540`,
  value `[esp+0x1c] * [0x005cea54]` (1.05). `[esp+0x1c]` is the max over loop 1 (`0x0046b7f2..0x0046b852`) and loop 4
  (`0x0046b920..0x0046b97b`) of `call 0x004c3b30` on a float-rounded `(dz*dz + dy*dy) + dx*dx`. `fold_sweep.py`: one
  folded READ (`0x00470b44`), no other writer.
- **Cause.** `0x004c3b30` is RW's LUT sqrt; the port's A3 used `std::sqrt` (`VehicleInit.cpp:27`). Offline, from the
  original's own A3 inputs: the LUT model reproduces 24 / 24 A3 sqrt outputs bit-exactly, `std::sqrt` 0 / 24.
- **Fix A** (`VehicleInit.cpp`): `Math::LutSqrt` in loops 1-5 in the original's operand order, mass tail in byte order.
  Opt-out `MASHED_NO_A3_LUT=1`. **G-A3 PASS 26 / 26** bit-exact against `o_p0a.msd` frame 1 (`D_DEF2.a3.txt`, `g_a3.py`);
  0 / 26 with the opt-out (`D_OFF.a3.txt`). Side effect: `+0x58` / `+0x5c` (mass terms) now equal the original's
  `0x44b136a0` / `0x3a38e833`; the exe had `0x44b13443` / `0x3a38eaab`.
- **The +0x4a4 -> settle link, tested: refuted.** With fix A on and fix C off (`D_NOSR`) the settle step is
  bit-identical to the all-off arm (`D_OFF`): same `drive_delta`, velocity, loads. The first differing line is the
  steady-state A6a |v| (0.63803041 vs 0.638038516), which moves through the mass terms.

## 2. Why the parked player does not settle (from the bytes and the captures)

- **Original, first physics tick** (`o_p0a.msd` player; `o_rd1.msd`, `o_p2a.msd` cars 1, 2): frame 1 -> 2 moves every
  car by dy = -0.0029475, dz = +0.0000921, x bit-identical; loads 0 -> 1083.34 / 541.671; vel and `+0x9e4` 0.0 on
  every frame. `o_fx1.msd` (new, `--fixup-probe`): the fixup `0x0046ef70` fires **0** times in 20 s, and the
  velocity is 0 at every sampled point of the first ticks (substep entry, A6a entry, `0x0046f6c0`, `0x00469aa0`).
  So the move is not a velocity integral. dz / dy = -0.031262 matches the Training tri normal's n.z / n.y =
  -0.031250 (port probe n = (0, 0.999512, -0.031235)): the move is -0.002949 * n, a translation write along the
  contact normal. The one function in the chain that writes the translation is the wheel solver `0x0046f6c0`
  (`WheelContactSolver.cpp:255-260`, `pos -= mean(fv * n)` over the grounded wheels, at the active ring
  matrix `+0x30`). It has three callers: the substep `0x00470ae0`, the post-substep `+0x14` path `0x00471388` in
  `FUN_00470c70`, and the spawn function `FUN_004704c0` at `0x00470638`. The probe saw 8 solver calls before the
  player's first A6a (4 cars x 2).
- **Exe** (`D_DEF.settle.txt`, `MASHED_SETTLE_DIAG`): (1) no physics during the countdown (`TrackRenderer.cpp:3357-3373`);
  (2) the one-shot spawn-settle step leaves vel `(0, -216.668, 0)` after A4, 2 substeps, `c9ec = 0`, and a
  `drive_delta` of `(0, -0.0100310, 0)` with no z part, which it discards (`VehiclePhysicsRun.cpp:671-672`);
  (3) the port runs the same solver on a stand-in wheel geometry (every wheel at groundY - 0.05,
  `VehiclePhysicsRun.cpp:517-521`): fv = -0.0499756 on all four wheels and a correction of +0.0499512 * n (up) on
  EVERY call, overwritten by `SyncContactRingMatrix` (`:1196`); (4) the exe has no vertical position authority:
  `car_pos_[1] = GroundHeight + car_ground_off_` every frame (`TrackRenderer.cpp:3475`), spawn y 0.47481969 against
  the original's 0.43742681.
- **Mechanism test D0-c** (amendment 2, report only): the port's own solver, fed the real suspension points
  (`MASHED_RW_WHEELPOS=1`) at the original's frame-1 position (round grid, y = `0x3edff668`), call 1 changes states
  only (`st` 0,0,2,2; fv front +0.0094, rear -0.0099: a level body on a 1.79 deg slope), call 2 corrects by
  **+0.00994 * n** (up) where the original moves **-0.002949 * n** (down). PREDICTION MISSED. So the solver body
  alone does not carry the settle. The open terms were the per-wheel states and fv that the original enters its
  first solver call with.
- **D0-d** (`o_ws1.msd.wheelstate.csv`, original, `--wheelstate-probe`, rate gate run first: 6,016 solver calls /
  751 frames). The player's FIRST `0x0046f6c0` call of every frame enters with states (1,1,1,1), fv (0,0,0,0)
  (seq 0, 4, 8, 16, 24, 32). Frame 1's second call (seq 12) enters with fv (0.009396, 0.009396, -0.009946,
  -0.009946), which is call 1's classifier output. From frame 2 on, the second call enters with
  (0.006447, 0.006447, -0.012895, -0.012895).
  The seeds are `FUN_0046baa0` (`SubStateCallees.cpp:337-348`, 0x0046bb23 / 0x0046bb37). It writes int 1 to
  `0x00881738/0x008817fc/0x008818c0/0x00881984` = `+0x198/+0x25c/+0x320/+0x3e4` (the four wheel states) and 0 to
  `+0x194/+0x258/+0x31c/+0x3e0` (the four fv). The original runs it on every phase-3 hold tick
  (`0x0041033e`, case 3 `FUN_004102f0`), and also at `0x0040ed6e` / `0x0041043a`.
- **Arithmetic** (`WheelContactSolver.cpp:176-260`).
  - State 1 with a found key latches all four wheels, so bVar16 = 4.
  - `0x004701e8`'s drop removes the largest |fv| (w2, -0.009946).
  - The translation moves by -(1/3)·Σ fv·n = -0.0029486·n = (0, -0.0029472, +0.0000921). The original's
    measured frame 1 -> 2 move is (0, -0.0029474, +0.0000921).
  - Call 2: 0.006447 + 0.006447 - 0.012895 ≈ 0, which is equilibrium. The body then rests, as the captures show.
- **D0-e** (amendment 4, `MASHED_D2S_SEED1`): the D0-c arm plus those 8 seeds before call 1.
  - Call 1: states (1,1,1,1), correction **(0, -0.00295186, +0.0000922680)**. Predicted
    (0, -0.0029519, +0.0000922) within 2e-6, so **PASS**.
  - Call 2: correction 0 (fv 0.006452 / 0.006432 / -0.012884 / -0.012884).
  - Resulting position (0.500936747, 0.434474945, -1.99894488) against the original's frame 2
    (0.50093675, 0.43447936, -1.99894500): x exact, z within 1 ulp, y 4.4e-6.
- **Why fix B is not ported** (a user decision, section 5). Reproducing the settle by default needs all three:
  1. the `FUN_0046baa0` wheel seeds before the first solver call. The exe has the function ported
     (`VehiclePhysicsStateReset`), but calls it only under `MASHED_SUBSTATE`.
  2. real suspension points instead of the stand-in "every wheel at groundY - 0.05"
     (`VehiclePhysicsRun.cpp:517-521`, documented there as a calibrated substitute).
  3. the solver's translation as the car's position. Today the ring `+0x30` is overwritten by
     `SyncContactRingMatrix` and the exe sets `car_pos_[1] = GroundHeight + car_ground_off_` every frame
     (`TrackRenderer.cpp:3475`). The spawn y convention would also change (exe 0.4748 vs original 0.4374).

  Items 2 and 3 change every car's position on every frame. That is the D2 wheel / position model, not a
  first-step patch, and the CLAUDE.md "architecture-level" stop rule applies. G-SETTLE and G-PROG therefore
  FAIL as registered. Car 2's early `FUN_00414a70` return (U-9684) stays blocked.

## 3. `+0x9e4` at rest (fix C)

- The exe's 0.6380385 is A6a's `|v|` written at `Integrate2.cpp:643-646`, BEFORE its parked full stop
  (`:755-758`) zeroes the velocity. The original refreshes `+0x9e4 = |+0x9b0|` and `+0x9e8 = |+0x9bc|` after the
  A4 loop on every frame (`0x004710a1..0x004710da`, gate `+0x4 != 0 || +0x10 == 1`). A6a's only call is inside A4
  (`0x00470936`), so the refresh comes after A6a. The port did it on A4-skipped frames only
  (`VehiclePhysicsRun.cpp:991-994`); car2pickup had already proposed it (`RESULT_CAR2PICKUP.md:149-150`).
- Fix C ports it (default OFF, opt-in `MASHED_SPEED_REFRESH=1`). It passes G-9E4: `H_DEF` reads 0 on 23 / 23
  at-rest frames before car 2's contact at f=24. Bisect on the D2 arm (`D2_NOA3_1`, fix C only): the first real
  step's `+0x9e4` reads 0.64 against 0.65, the run then lands in a different regime, and the scores are
  slip 1500-2000 0.1956, slip 2000-2600 **0.2654 OUT**, driving-median **1666.43** (n 132 / 184 / 527 against
  353 / 562 / 1367). A + C together: 0.2008 / 0.2627 / 1798.12, all OUT, 3/3 runs identical.
  Collateral A + C: 42 / 94 (`s.v2.ai_spline_idx`, `s.v2.c4`, `s.v2.c5` move outside the floor).
  The pre-registered rule keeps it OFF. Which D2 consumer reacts to the post-clamp `+0x9e4` is not measured.

## 4. Before / after

### 4.1 G-CTRL (14,400 frames, step / gates / ptrace sha8)

| arm | build | knobs | hashes |
|---|---|---|---|
| `H_OFF` | A+C default | `NO_A3_LUT`, `NO_SPEED_REFRESH` | `991a5fff / be0d47ea / bc75b405` (= base) |
| `H_DEF`, `H_DEF2` | A+C default | none | `a297e784 / 41ca6bf6 / c73bbdd4` |
| `H_NOSR`, `H_NOSR2` | A+C default | `NO_SPEED_REFRESH` (= fix A only) | `c2e4d60d / b50bd39b / ba90ed88` |
| **`H_FDEF`** | **final** | **none (shipped default: A on, C off)** | **`c2e4d60d / b50bd39b / ba90ed88`** |
| `H_FOFF` | final | `NO_A3_LUT` | `991a5fff / be0d47ea / bc75b405` (= base) |

### 4.2 D2 (`SCORE_D2.txt`; bounds `d81a8df6`, recipe pinned in `score_d2.py`, reproduces attempt 20's t1)

| arm | slip 1500-2000 | slip 2000-2600 | driving-median | n (s1 / s2 / dm) |
|---|---|---|---|---|
| base (`D2_OFF_1..3`) | 0.1943 in x3 | 0.2524 in x3 | 1852.66 / 1849.16 / 1852.66 OUT | 353 / 562 / 1367-1373 |
| **fix A (`D2_NOSR_1..3`) = new default** | **0.1927 in x3** | **0.2489 in x3** | **1860.98 / 1860.35 / 1860.35 OUT** | 376 / 562 / 1364-1366 |
| fix C only (`D2_NOA3_1`) | 0.1956 in | 0.2654 OUT | 1666.43 OUT | 132 / 184 / 527 |
| A + C (`D2_DEF_1..3`) | 0.2008 OUT x3 | 0.2627 OUT x3 | 1798.12 OUT x3 | 121 / 183 / 453 |

Original 0.1937 / 0.2498 / 1937.89. Fix A moves slip 1500-2000 from +0.0006 to -0.0010 against the original,
slip 2000-2600 from +0.0026 to -0.0009, and driving-median from -85.2 to -76.9 (2.3% under the bound's floor).

### 4.3 D3 (b) per regime (`SCORE_B.txt`, `SCORE_B_NOSR.txt`)

| arm | A (K=0) | B (K=1) | C (K=26 @196) |
|---|---|---|---|
| base (`B_OFF_K*`, hashes = car2pickup `3cb5c20e / b47ae9cd / 4e0f01eb`) | 1/3 (car 2) | 2/3 (cars 1, 3) | 0/3 |
| fix A (`B_NOSR_K*`) = new default | 1/3 (car 2) | 2/3 (cars 1, 3) | 0/3 |
| A + C (`B_DEF_K*`) | 1/3 (car 2) | 2/3 (cars 1, 3) | 0/3 |

### 4.4 Collateral (`collateral.py --mode banded`, A = `o_p2b`, floor `o_p0a`, 600 frames from car-2 sub-state >= 6)

Base `H_OFF` (floor `U_DEF2`): 39 / 94 (reproduces U-9684's 39 / 94). Fix A `H_NOSR` (floor `H_NOSR2`): **39 / 94,
same field set** (`cross_diff.py`: nothing moved). A + C `H_DEF` (floor `H_DEF2`): 42 / 94.

## 5. Decisions needed (user)

1. **Fix B (the settle) as a D2 position-model change.** Seed the wheel states via the existing
   `VehiclePhysicsStateReset` before the first solver call, switch to real suspension points
   (`MASHED_RW_WHEELPOS` path), and make the solver's ring translation the exe car's position (x, y, z).
   This lane shows the mechanism works in the port (D0-e). It does not show what that does to D2, D3 or the
   off-mesh recovery. It is a multi-session D2 slice. Recommendation: open it as its own D2 slice, gated on the
   three D2 metrics plus G-SETTLE / G-PROG. Until then U-9684 / `MASHED_TGT3` stay blocked.
2. **Fix C default.** It is faithful by bytes and wrong by D2 metrics. Recommendation: keep it OFF and re-test it
   inside the fix-B slice. It changes the same `+0x9e4` consumers that the position model feeds.
3. U-9180: fix A moves driving-median +8 toward the bound. The carrier of the remaining 2.3% (to the bound's
   floor 1904.70) is still not identified. No decision needed beyond accepting fix A's default.

## 6. Proposed tracker transaction (re-classify, NOT applied here)

- **U-9181 RESOLVED.** Premise mislabelled (`susp=` = `g_suspScale`). The real `+0x4a4` divergence was A3's
  `std::sqrt` against `0x004c3b30`'s LUT. Fixed default-ON (fix A, G-A3 26/26, G-4A4 PASS). Writer
  `0x0046ba00` / `FUN_0046b540`, reader `0x00470b44` (car-car) and the broadphase radius.
- **U-9180** stays open. Driving-median 1860.35-1860.98 under fix A (was 1849.16-1852.66).
- **New U row (semantic, player physics at rest), as U-9684 proposed, now with the mechanism.** `FUN_0046baa0`
  seeds + `0x0046f6c0` first-call 4 -> 3 wheel drop + translation write. Exe gaps (a) seeds, (b) stand-in wheels,
  (c) discarded translation. Gates U-9684 and any `MASHED_TGT3` default. Path: section 5.1.
- **New structural row:** fix C (`0x004710a1..0x004710da` every-frame refresh) ported default-OFF, D2-regressing.
- **DEFERRED D-11071**: this lane is a D2 REOPEN CANDIDATE. It touched `+0x4a4` (fix A changes the broadphase
  radius by +3.4e-5 and the mass terms `+0x58/+0x5c`), plus the contact collector inputs and the player at rest.
  Hits are allowed this round (user 2026-10-16) and reported here.
- **hooks.csv:** no status change. `0x0046b540` is ported in the exe (`VehicleInit.cpp`); fix A is exe-side.
  This lane adds no C3 evidence (no run_diff).
- **CHANGELOG line (proposed):** `2026-10-16 d2settle (verify/d2settle_20261016/RESULT_d2settle.md): U-9181
  resolved, A3 FUN_0046b540 now uses the LUT sqrt 0x004c3b30 (G-A3 26/26, +0x4a4 0.678037), default ON, new
  G-CTRL c2e4d60d/b50bd39b/ba90ed88; D2 slips in, driving-median 1860.35-1860.98 (+8); parked-player settle =
  FUN_0046baa0 seeds + 0x0046f6c0 4->3 drop translation (D0-e PASS), not ported (user decision); every-frame
  +0x9e4 refresh ported default-OFF (MASHED_SPEED_REFRESH, D2 out of bounds).`

## 7. Raw

Committed: `PREREG_d2settle.md`, this file, scripts `run_arms.py`, `g_a3.py`, `g_9e4.py`, `score_d2.py`.
Scores: `SCORE_D2.txt`, `SCORE_B.txt`, `SCORE_B_NOSR.txt`, `COLLATERAL_CROSS_H_{OFF,DEF,NOSR}{,_stdout}.txt`.
Data: `D_*.settle.txt`, `D_*.a3.txt`, `D_*.pathprog.csv`, `H_{FDEF,FOFF,DEF,NOSR}.{step,gates}.csv`,
`B_*.step.csv`, original `o_fx1.msd.fixupprobe.csv`, `o_ws1.msd.wheelstate.csv` + provenance.
Force-added (gitignored, cited): `D_*.ptrace.log`, `H_FDEF.ptrace.log`, `H_DEF.ptrace.log`,
`D2_{OFF,NOSR}_{1,2,3}.motion_diag.log`, `D2_DEF_1.motion_diag.log`, `D2_NOA3_1.motion_diag.log`.
Not committed: `.msd` binaries (gitignored), the remaining duplicate-hash `H_*` arms (hashes in 4.1).

## 8. Evidence hygiene (orchestrator notices 1 and 2, 2026-10-16)

- **ptrace contamination: not affected.** Every exe run in this lane used `MASHED_ROOT` = a private copy of
  `original/` in this session's scratchpad (`run_arms.py` `ROOT`), so `player_trace.log` was written there, never to
  the shared main-root file. Check: each `H_*.ptrace.log` holds exactly one `f=1` line and a strictly +1 step
  sequence (13,678 steps for `H_OFF/H_FOFF/H_DEF/H_DEF2`, 14,038 for `H_NOSR/H_NOSR2/H_FDEF`), and three
  independent runs agree (`ba90ed88` x3, `bc75b405` x2 = the recorded base). The ptrace hashes in 4.1 stand.
- **Shared `.asi`: never deployed.** The first build (`5dbc656e` era, before `MASHED_NO_DEPLOY=1`) copied
  `mashed_re_dev.asi` into the worktree's own `original\` (a plain directory, not a junction). Every later build
  used `MASHED_NO_DEPLOY=1`. The shared `original\mashed_re_dev.asi` still reads 13/10/2026 20:39:06, 1,153,536
  bytes, the timestamp it had before this lane started. No original-side run here loaded an `.asi` from this branch.
- **Renumbering:** the U rows proposed in section 6 have no IDs. They take U-9687+ at merge (U-9686 taken, base moved
  to `4b751ff3`).
