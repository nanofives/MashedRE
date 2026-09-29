# D3 — U-D3-DRIVE-FORCE: the missing start boost. Criterion (e) PASSES, D3 still NOT CLOSED

Session 2026-09-29, branch `race/first-frame-parity`, commits `9573f3a3` (the port + the
criterion (e) evidence) and this note's commit (the guards).

Handed the §4.4 residue of `D3_DRIVE_2026-09-28.md`: after the spawn settle the port's AI
cars still gained 13-15 speed per frame at the launch against the original's 180-198, with
the gear, the input bytes, the wheel contacts, the wheel states and the gearbox constants all
matching, and the gap localised to the body-force accumulator `+0xb14`/`+0xb1c` but explicitly
**not established**, because the `[A8-B14CADENCE]` comment warned a render-tick snapshot of
`+0xb14` might be a post-substep residue rather than the force the integrator consumed.

**The §2.4 band is untouched.** Criterion (e) is scored by `re/tools/ai_speed_env.py --check`
against the reference and the ±2% band `D3_DRIVE_2026-09-28.md` §2.3/§2.4 fixed and committed
before any of this. No reference value, band, window or statistic was changed here.

---

## 1. The cadence question — SETTLED, both sides, and it does NOT differ

### 1.1 The port: render-tick `+0xb14` IS the consumption-time value

`MASHED_COUPLING_DIAG=1` prints `ctrl=(+0xb14, +0xb18, +0xb1c)` inside `Vehicle_Integrate2`
immediately before the tail consumes it (`Integrate2.cpp:461`, print; `:484-486`, consume).
`MASHED_MOTION_DIAG` prints `b14=[...]` after the whole substep loop
(`VehiclePhysicsRun.cpp`, the `[A8-B14CADENCE]` block). One 20 s run of the a8 recipe with
both on (`verify/d3_force_20260929/cad1/`):

```
motion b14 samples: 899   friction ctrl samples: 3596   ratio 4.000
stride=4: matched 898/899
```

4.000 because `friction_diag.log` is written for all four cars and `motion_diag.log` for slot 0
only. Taking every fourth `ctrl=` line and comparing it to the same frame's `b14=`, the two
agree on **898 of 899 frames**, to every printed digit (`0,-13000103,0` / `17799.7695,0,
846743.6875` / …). The single miss is the off-by-one at index 1, which is the spawn-settle
frame's extra inner `StepCar` call.

Mechanically that is forced: A4 runs **once per frame, outside** the substep loop
(`VehiclePhysicsRun.cpp:741-767` and the `MASHED_A8_A4_FIRST` block at `:680-684`), it zeroes
the accumulator at its start, A6a is its only accumulator, and nothing between A6a's tail and
the diag read touches those three floats (`py -3.12 re/tools/findoffset.py --writes 0xbf8
0xbf4` plus the source grep: the only writers in the exe target are
`VehicleControl.cpp:102` zeroing and `Integrate2.cpp:233-235`).

### 1.2 The original: also once, on two independent witnesses, and NO Frida probe was needed

§4.4's step 2 was conditional ("if they differ"). They do not, and the original's cadence then
fell out of evidence already available.

**Static.** `FUN_00470670` (A4), decomp read today:

```
*(undefined4 *)(in_EAX + 0xb20) = 0;      0x004706a9
*(undefined4 *)(in_EAX + 0xb1c) = 0;      0x004706af
*(undefined4 *)(in_EAX + 0xb18) = 0;      0x004706b5
*(undefined4 *)(in_EAX + 0xb14) = 0;      0x004706bb
...
FUN_0046ddb0(param_2,iVar1,param_4);      A5, once
FUN_00467650(param_1,param_2,iVar1,param_3);   A6a, ONCE, no loop   0x0047094c
FUN_00468980(param_2,param_3);            A6b, once
```

and `FUN_00470c70`'s chunk loop is `local_24 = 0x32; if (uVar11 < 0x32) local_24 = uVar11;`
with A4 called once per vehicle per chunk (`0x00471071`), so at the fixed budget of 50 there
is exactly one chunk and exactly one A6a per car per frame. Because A4 **zeroes** the
accumulator at entry, a render-tick snapshot can never be a multi-A4 accumulation either.

**Behavioural, and this is the decisive one.** In the original's own capture the linear
integration `Δv = dt · (+0x54) · kDt · (+0xb1c + accum)` — with `dt·(+0x54)·kDt = 1.66667e-5`,
`+0x54 = 0.001` measured — reproduces the original's OWN per-frame speed gain from the
captured `+0xb1c` on nine consecutive frames (`verify/d3_drive_20260928/e3.msd`, car 1):

| frame | `+0xb1c` | predicted Δv | measured Δ`+0x9e4` |
|---|---:|---:|---:|
| 860 | -1.08000e+07 | 180.00 | 180.00 |
| 861 | -1.14331e+07 | 190.55 | 190.51 |
| 862 | -1.13023e+07 | 188.37 | 188.00 |
| 863 | -1.19614e+07 | 199.36 | 198.24 |
| 864 | -1.23745e+07 | 206.24 | 203.92 |
| 865 | -1.24392e+07 | 207.32 | 203.71 |
| 866 | -7.50365e+06 | 125.06 | 119.62 |
| 867 | -2.54182e+06 | 42.36 | 35.79 |
| 868 | -2.55297e+06 | 42.55 | 35.41 |

A multi-pass accumulation cannot satisfy that: with *k* passes the frame's total gain is
`linTerm · Σᵢ b14ᵢ`, which is strictly greater than `linTerm · b14_final`, and the measured
gain is slightly **less** than the prediction (the difference is the velocity-opposing
`lin_b0`, which grows with speed exactly as the table's widening gap shows).

**CADENCE VERDICT: the two do not differ. The original's render-tick `+0xb1c` is the force
A6a consumed, once, and the force→velocity conversion in the port is already exact.** So
`[A8-B14CADENCE]`'s worry is refuted for this comparison and §4.4's 8-9x was a real gap. The
same table is also the proof that the conversion was never the defect.

---

## 2. MEASURED — the root cause: A6a's START BOOST block was never ported

### 2.1 What the port was missing

`Integrate2.cpp` carried, where the block belongs, exactly one line:

```
// [U-A6A-ST0] boost-state machine (+0xbf8 == 1 / == 2) gated on Vc_RoundST0 — shape only.
```

The block (`FUN_00467650` `0x00467d3a..0x00467e44`, decomp
`re/analysis/data/A6a_FUN_00467650_decomp_20260824.txt:276-317`) sits inside the same
per-wheel `p[-0xf] == 2 && p[-3] != 0` gate as the drive block and, while `+0xbf8 == 1` and
`+0xbf4 != 0`, adds `ff × wheelForwardAxis` into `+0xb14/18/1c` and decrements `+0xbf4` by
`dt`. `ff = 5e6` (literal at `0x00467d74`), or `_DAT_005cea28 = 8e6` when
`FUN_0040e340() == 4 && (DAT_0088e668 == param_1 || DAT_0088e66c == param_1)`.

### 2.2 The original's launch is that block, per car, to 0.02%

`verify/d3_force_20260929/orig_boost_launch.txt`, three separate original captures, same
recipe, `--statediff-car` 1 / 2 / 3, plus the player. Residual = captured `+0xb1c` minus the
drive-only prediction of `Integrate2.cpp:216-235` at the frame's entry speed:

| car | boost frames | residual while `+0xbf8 == 1` | run-out frame | after |
|---|---|---|---|---|
| 1 (`e3.msd` f860-865) | 6 | **-1.0000e+07 = 2 × 5e6** | -4.99772e+06 = 1 × 5e6 | +2391 / +2695 (0.09% of drive) |
| 2 (`o_c2.msd` f894-899) | 6 | **-1.6000e+07 = 2 × 8e6** | -7.94023e+06 ≈ 1 × 8e6 | +23328 / +26598 (0.9%) |
| 3 (`o_c3.msd` f883-888) | 6 | **-1.6000e+07 = 2 × 8e6** | -7.98675e+06 ≈ 1 × 8e6 | +7627 / +8481 (0.3%) |
| 0 PLAYER (`o_c0.msd`) | **0** | — | — | `+0xbf8` and `+0xbf4` are 0 on all 2725 frames |

Two state-2 wheels (`+0x168`/`+0x22c` = 2, `+0x2f0`/`+0x3b4` = 1) on every frame of every
capture, so "2 ×". The run-out frame gets one wheel's worth because the timer crosses the
`< 1` test between wheel 0 and wheel 1. From the frame after, the residual is the rounding of
my own drive-only arithmetic — i.e. **the drive-only law was already right**, which is why
frames 867-868 of `D3_DRIVE_2026-09-28.md` §4.4's own table were never the problem.

The timer confirms the cadence independently: `+0xbf4` is `1100, 900, 700, 500, 300, 100, 0`
on all four AI captures — exactly `-200` per frame = 2 qualifying wheels × 2 decrement sites
× `dt = 50`, the `FUN_0040e350() == 6` site at `0x00467cd0` and the boost block's own at
`0x00467dc6`.

### 2.3 The three bindings it needed, each decoded

- **A6a `param_1` is the CAR INDEX.** It was passed `0` with an `[UNCERTAIN]` at
  `VehicleControl.cpp:187`. The dispatcher calls A4 as `FUN_00470670(iVar6, fVar4, puVar18,
  param_2)` at `0x00471071`, where `iVar6` is its per-vehicle loop counter, and A4 forwards
  its own `param_1` unchanged at `0x0047094c`. A6a's only use of it is the
  `DAT_0088e668`/`66c` comparison (`0x00467d62`/`0x00467d6a`), which are car indices.
  Plumbed through as a new `car` argument on `VehicleControlIntegrate`.
- **`FUN_0040e340` (`0x0040e340`, 5 bytes) = `MOV EAX,[0x008a94d0] / RETN`**, the PARTICIPANT
  count — the name the port already uses for it in `TrackRenderer::ParticipantCount` and
  `exe_main.cpp:3400`. It is **already ported at C4** as `Util/UtilLeaves.cpp`
  `GetLiveCarCount` (hooks.csv row `0040e340`), which reads the global directly — right
  in-process, useless in the standalone, because `0x008a94d0` has **no writer** in
  `mashed_re.exe` (`Race/ScoringHooks.cpp:41/236/245/325` and `UtilLeaves.cpp:54` only read
  it, and `ScoringHooks.cpp` is asi-only). So the binding here is the car count
  `VehiclePhysics_Init` is given (4 on this recipe, which is the measured value). The
  pre-existing `void Fi_GameModeTick()` declaration is the same original function mis-typed
  as void; it is left alone (its one caller wants only the side effect) and a correctly typed
  `Fi_ParticipantCount()` added alongside, with all of that stated in the source.
- **`DAT_0088e668` / `DAT_0088e66c` are the two LEAST-progressed cars.** `FUN_00470c70` seeds
  `DAT_0088e660..66c` with `0,1,2,3` (`0x00470e2e`), reads `local_10[i] = FUN_00408a50(i)`
  for `i < FUN_0040e340()`, and bubble-sorts `local_10` **descending** while permuting the
  index array (`0x00470e66..0x00470f0a`). `FUN_00408a50` (`0x00408a50`) is
  `*(float*)(0x008a96e8 + car*0x30c)`, the per-car race-progress float. Ported verbatim as
  `Fi_UpdateBoostOrder()`.

  In the standalone that progress float has **no reachable writer** — checked, not assumed.
  The original's writer `FUN_00408a70` *is* ported (`Frontend/MenuMixed.cpp`
  `FrontendC2RoundI`, `RH_ScopedInstall` at `MenuMixed.cpp:634`, hooks.csv C3), but
  `MenuMixed.cpp` is in `asi_sources.rsp` only (`exe=0 asi=1`), so it is an .asi export with
  no call site on the `mashed_re.exe` race path; the only exe-side reference to the field is
  the read-only accessor `Frontend/Leaves.cpp` `PerCarRaceProgressGet` (`0x00408a50`). So
  every comparand is 0.0, no `<` is true, no
  swap happens and the pair stays `{2, 3}`. That is exactly the measured value at the lights —
  and it is *why* cars 2 and 3 get 8e6 and car 1 gets 5e6 in §2.2, since at the green the
  original's progress values are all equal too and its own sort is likewise a no-op on the
  seeded grid order.

### 2.4 `Fi_GameMode()` corrected from 0 to 6

`FUN_0040e350` (`0x0040e350`, 6 bytes) is `return DAT_0063ba8c;`. The standalone has no
`DAT_0063ba8c` state machine (`TrackRenderer.cpp:3568` already records that), so the stub is a
constant — and `0` was the wrong constant. The original's in-race value is **6**, on two
independent measurements:

1. `FUN_00470c70` replaces a car's ctrl block with the neutral `DAT_007f19b8` unless the mode
   is 6, `0xb` or `0xa` (three separate CALL+CMP at `0x00470ff2`/`0x00470ffe`/`0x0047100a`).
   The original's AI cars demonstrably drive from their real bytes (`+0xb20 == 1`, and a
   `+0xb1c` consistent with accel byte 255 clamped to 160), so the value is in {6, 10, 11}.
2. The `-200`/frame `+0xbf4` cadence of §2.2 needs the `== 6` site to run on both qualifying
   wheels. Intersected with (1), the value is 6.

Blast radius audited call site by call site; every other reader compares against 7 only, and
`RubberBandGate`'s extra `kRubberThr < g_rubberBand[car]` term cannot fire because
`g_rubberBand[16]` is all zero with no writer anywhere in the standalone. So the only live
consequence is the intended timer site. `MASHED_GAMEMODE_STUB=0` reverts.
`forceint_selftest.cpp` (in neither source list, so it does not build) still expects
`RubberBandGate(0) == 0`, which still holds, but its stated *reason* is now stale.

### 2.5 [UNCERTAIN] U-D3-BOOST-ARM — what is NOT decoded

The writer of `+0xbf8 = 1` / `+0xbf4 = 1300` is **not located**, and that is the one part of
this fix that is a measured seed rather than a transcription.

`py -3.12 re/tools/findoffset.py --writes 0xbf8 0xbf4 0xbf0` finds four `+0xbf8` accesses and
seven `+0xbf4` accesses in the whole of `.text` and **every one is inside `FUN_00467650`**
(`0x00467d08..0x00467e44`); `+0xbf0` has no writer at all. So the arming store does not use a
`[reg + 0xbf8]` displacement and the displacement sweep is structurally blind to it — the
dword-index-off-a-computed-base blind spot. The countdown behaviour says the writer exists and
is active only before the lights: `+0xbf4` rises `+50` per frame through the last five
countdown frames while A6a's `== 6` site is subtracting `-100`, so something external is
adding `+150`/frame, and it stops at the green (the in-race step is exactly `-200`, i.e. A6a
alone).

Next commands, in order:

```
# 1. Frida WRITE watchpoint on &rec[car]+0xbf8, armed during the countdown. A6a's two
#    +0xbf8 stores are both inside `bf8 == 1` / `== 2` arms, so while bf8 == 0 NOTHING in
#    A6a writes it and the first fault IS the arming instruction. Report the faulting EIP,
#    then decomp its containing function.
# 2. If the watchpoint API is unavailable: a Ghidra script walking stores whose base is
#    DAT_008815a0 + k and whose displacement is 0xbf8 - k.
```

Until then the port seeds the measured state: `+0xbf8 = 1`, `+0xbf4 = 1300` (the pre-A6a
value; A6a's own two sites leave the measured 1100), once per AI slot, on its first real
post-settle step — the standalone's green light. Reverted by `MASHED_NO_START_BOOST=1`.

**The one stated DEVIATION.** The arm is restricted to `slot != 0` because that is what the
captures show: all three AI cars armed on 12/12 captures, the player armed on **neither**
player recipe (`orig_steerR.msd` held-lock: `+0xbf4` counts up to 3000 and saturates,
`+0xbf8 == 0` on 2210 frames; `o_c0.msd`: both 0 on all 2725 frames). There is no original
observation of an armed player, so the player's arming law is unported, not decided — and
restricting the arm leaves the D2 player force path bit-untouched by construction. A human who
does not jump the lights presumably *is* boosted in the real game; that needs the writer,
i.e. U-D3-BOOST-ARM.

---

## 3. MEASURED — criterion (e) PASSES on all three AI cars

`re/tools/ai_speed_env.py --check`, band from `D3_DRIVE_2026-09-28.md` §2.4 unchanged. Four
arms, all `regime0 = 1` on all three cars, full output in
`verify/d3_force_20260929/e_b_check.txt`.

| car | stat | reference (band) | **default (`sa_b2`/`sa_b3`)** | `NO_SPAWN_SETTLE=1` | `NO_START_BOOST=1` | pre-session (§4.2) |
|---|---|---|---:|---:|---:|---:|
| 1 | `launch` | 1425.7 (1397.2..1454.2) | **1426.4 (+0.05%)** | 1326.8 (-6.9%) | 200.5 (-85.9%) | 200.5 |
| 1 | `ft_median_m0` | 2551.6 (2500.6..2602.6) | **2550.7 (-0.04%)** | 2497.2 (-2.1%) | 1364.3 (-46.5%) | 1364.3 |
| 2 | `launch` | 2052.5 (2011.5..2093.6) | **2053.0 (+0.02%)** | 1964.8 (-4.3%) | 200.5 (-90.2%) | 200.5 |
| 2 | `ft_median_m0` | 2052.5 (2011.5..2093.6) | **2053.0 (+0.02%)** | 1979.7 (-3.5%) | 200.5 (-90.2%) | 200.5 |
| 3 | `launch` | 2055.0 (2013.9..2096.1) | **2055.2 (+0.01%)** | 1966.4 (-4.3%) | 200.5 (-90.2%) | 200.5 |
| 3 | `ft_median_m0` | 2278.1 (2232.5..2323.7) | **2278.3 (+0.01%)** | 2211.3 (-2.9%) | 353.2 (-84.5%) | 353.2 |

**Criterion (e): PASS on all three cars, on both gated statistics, every one of the six inside
0.05% of the reference** — against a ±2% band and a pre-session gap of 46-90%.

Three things make that table an instrument rather than a coincidence:

- **Determinism.** `sa_b3` repeats `sa_b2` to every printed digit on all three cars and on
  all four statistics including the ungated ones.
- **The revert knob is validated against a prior session's numbers, not its own.**
  `MASHED_NO_START_BOOST=1` reproduces `D3_DRIVE_2026-09-28.md` §4.2's post-settle column
  exactly — 200.5 / 1364.3 / 200.5 / 200.5 / 200.5 / 353.2 — so the boost is the entire
  remaining gap and nothing else in this session's diff moved (e).
- **U-9142 is answered by measurement.** With `MASHED_NO_SPAWN_SETTLE=1` and the boost on,
  (e) FAILS on all three cars by -2.1% to -6.9%. **The spawn settle is REQUIRED for (e) to
  pass** and should be kept. The 2026-09-28 session left keep-or-revert to the user with the
  (b) cost as the only argument against; the (b) cost is now moot in the sense that (b) fails
  in both arms (§4), and (e) discriminates cleanly.

An ungated statistic for the record, not a gate: the full-window `ft_median` is 3422.9 /
3425.6 / 3525.1 against the original's 2254.8 / 2648.1 / 2691.4, i.e. +52 / +29 / +31%. It was
+25 / +7 / +5% pre-settle. That statistic is ungated precisely because residue D3-R1 (unported
behaviour modes 3 and 7, `FUN_00414c30` / `FUN_00484c70`) means the original brakes and lifts
inside that span and the port cannot — see §2.2 of the 2026-09-28 note. It is now a larger
number because the port reaches the original's real speeds and then keeps the throttle pinned
where the original lifts.

---

## 4. AI criterion (b) — still FAILS, and now fails WIDER. This is why D3 does not close

`re/tools/ai_ctrl_window.py --check`, same captures:

| car | default (`sa_b2`) | `NO_START_BOOST=1` (`sa_nb`) | §4.3 post-settle (2026-09-28) |
|---|---|---|---|
| 1 | FAIL — `c0_distinct` 7 (floor 13), `c1_distinct` 106 (ceil 70), `steer_distinct` 112 (ceil 96), **`c1_median` 48.0 (band [0,0])**, **`abs_steer_median` 48.0 (ceil 23)** | FAIL — `c0_distinct` 7, `c1_distinct` 87, `accel_distinct` 1, `brake_distinct` 1 | FAIL — `c0_distinct` 7, `c1_distinct` 87, `accel_distinct` 1, `brake_distinct` 1 |
| 2 | FAIL — `c1_distinct` 93, `steer_distinct` 121, **`c1_median` 42.0**, **`abs_steer_median` 58.0** | FAIL — `c1_distinct` 72, `steer_distinct` 104 | FAIL — `c1_distinct` 72, `steer_distinct` 104 |
| 3 | FAIL — `c1_distinct` 98, `steer_distinct` 116, **`c1_median` 46.5**, **`abs_steer_median` 46.5**, `accel_distinct` 1, `brake_distinct` 1 | FAIL — `c1_distinct` 75, `accel_distinct` 1, `brake_distinct` 1 | FAIL — `c1_distinct` 75, `accel_distinct` 1, `brake_distinct` 1 |

Read it honestly, in both directions:

- **The `NO_START_BOOST` column reproduces §4.3 exactly**, so this session introduced no new
  (b) defect. (b) was already failing on all three cars before it.
- **The default column is worse**, and the new failures are the steering magnitudes:
  `c1_median` 42-48 against a band of `[0,0]` and `abs_steer_median` 46.5-58 against a ceiling
  of 23. The port's AI now steers hard and to one side.
- **And the honest framing: the old (b) numbers were not a measurement of the AI law.** They
  were taken on cars running 46-90% slower than the original at the same point of the same
  window. Steering demand is a function of speed through the whole lookahead/curvature chain,
  so scoring the steer bands at the wrong speed scored the wrong thing. §2.4 of the
  2026-09-28 note made that argument for the *speed* statistic; it applies to (b) as well,
  and it cuts the other way: **(b) is now measured under matched speed for the first time, and
  under matched speed it is further out than anyone had measured.**

So the AI third of the D3 gate has (a) met, (c) met, (d) met, **(b) NOT met** — unchanged in
verdict, changed in size and, for the first time, measured on a car that goes the right speed.

---

## 5. Regression guards

Full output `verify/d3_force_20260929/guards.txt`.

| guard | result |
|---|---|
| **D2 clean-env (a8 held-lock)**, 3 runs default + 3 runs `MASHED_NO_START_BOOST=1` control | **MOVES, attributed.** Default: slip 1500-2000 `0.1411 / 0.1323 / 0.1411`, slip 2000-2600 `0.2993 / 0.2992 / 0.2993`, driving-median `2539.40 / 2538.21 / 2539.39`, 1497-1499 lines. Control: `0.1281 / 0.1266 / 0.1281`, `0.2928 / 0.2947 / 0.2928`, `2488.60 / 2507.94 / 2488.60`, 1623-1624 lines — which **reproduces §4.5's pre-fix row to four decimals** (`0.1281 / 0.1281 / 0.1266`, median `2489 / 2489 / 2508`). See below. |
| power-up 9-type decision + contact replay sweep (`re/tools/pu_replay/sweep.ps1`) | **11 of 11 decision CLEAN**; contact CLEAN on 10 of 11, `g3` **DIVERGES — unchanged**, exactly the known 2-query-of-546 R_FLAME residue |
| modes oracle, rule 3 (`scenario_launch.py --oracle --rule 3`) | **GREEN.** `SegmentCheck 0x00410d10` 2468/2468 MISMATCH 0 with **2 segment-ends**, `EvaluateResult 0x00410510` 2/2 MISMATCH 0, `FinishOrder 0x004177b0` 3382/3382 MISMATCH 0, `err` null. Unlike the 2026-09-28 spot-check this run *did* produce segment-ends, so the rule-3 tail arm is covered here. |
| `mashedmod\build.bat` | both targets clean; `mashed_re_dev.asi` "all 418 objects up to date" on every rebuild, i.e. the **.asi is untouched** — every file changed is exe-only (`exe_sources.rsp`), and `PhysicsChainHooks.cpp` (asi-only) reads the live globals |
| AI criterion (b) | **FAILS, wider — §4** |

**On the D2 movement, stated plainly rather than waved through.** The player's force path is
unchanged *by construction*: the boost arm is gated on `slot != 0`, and the other four edits
are the `param_1` plumb-through (used only inside the boost arm), the participant/order
bindings (read only inside the boost arm) and `Fi_GameMode()` 0→6 (whose only live effect is
the `+0xbf4` timer, and the player's `+0xbf4` is never armed). The same-session control run
confirms it empirically. So the whole +2..10% move is the **three opponents** now launching
correctly into the world the player is solved in — which is precisely the variance mechanism
`D3_DRIVE_2026-09-28.md` §3.4 CONFIRMED and §3.6/U-9141 filed: on this recipe the aggregate is
no longer a measurement of the drive laws, and it needs a controlled arm (fixed frame count,
opponents absent or seeded) before it gates anything again. For direction: slip 1500-2000 moves
**toward** the original (0.128 → 0.141 against 0.1913) and the driving-median moves away
(2489 → 2539 against 1940.6 on this reducer). Neither is offered as a pass.

---

## 6. Verdict on the gate

**Criterion (e) is MET. D3 is NOT CLOSED, because AI criterion (b) is not met.**

The gate's AI third requires (a)-(e); (e) was the last one opened and it is now green to 0.05%
on all three cars, but (b) is red on all three and red wider than before. The powerups third
and the modes third are unchanged by this session (both measured above).

What this session delivers: the §4.4 cadence question answered both ways from evidence rather
than a new probe; the 8-9x drive-force gap root-caused to an unported block and closed; the
per-car 5e6/8e6 split decoded and measured on three separate original captures; criterion (e)
from -46/-90% to within 0.05%; U-9142 answered by measurement (keep the settle); and a
correction to the (b) framing, which had been scored on cars that did not go the right speed.

What it leaves: **(b)**, which is now the sole D3 blocker and is a steering-magnitude defect
in the ported AI at correct speed, plus `[UNCERTAIN] U-D3-BOOST-ARM` (§2.5) and
`[UNCERTAIN] U-D3-BOOST-ORDER` (the progress float has no writer, so the 8e6 pair cannot
evolve with race order; it does not affect (e), whose boost window is the six frames at the
lights where the original's own progress values are equal too).

## 7. Reproduce

`verify/d3_force_20260929/PROVENANCE.txt` has every command verbatim. Large `.msd` / `.csv` /
`.png` payloads are gitignored (`.gitignore:140-141`); the distilled tables
(`orig_boost_launch.txt`, `e_b_check.txt`, `guards.txt`, `cad1/cadence.txt`) are committed.
Builds were made in place at this branch's HEAD; no worktree was created, no old-commit probe
was run, `original/` was never modified, and every `mashed_re.exe` / `MASHED.exe` PID was
spawned and killed by the harness that spawned it, by PID.

---

## 8. Disclosure — three concurrent commits landed on this branch, and none can confound

The three investigation sessions running alongside this one committed to
`race/first-frame-parity` while it was measuring. Disclosed here rather than left for
someone to find in the log:

| commit | subject | files under `mashedmod/` |
|---|---|---|
| `883e268e` | U-SEA-ARCTIC: the sea sits at road height … | **0** |
| `869513ba` | Pickups: the orb is a scaffold billboard … | **0** |
| `ebbc4c68` | Gray car chassis: 27 non-render atomics … | **0** |

Checked, not assumed: `git show --name-only --format= <sha> -- mashedmod/` is empty for all
three. Every one touches only `re/analysis/` and `verify/`, so none of them can change
`mashed_re.exe` or `mashed_re_dev.asi`, and every measurement in this note was taken on a
build whose source differs from `1bedbf2c` only by this session's own four files. No probe of
an old commit was made, so nothing needed a `git checkout HEAD -- mashedmod/` restore.
