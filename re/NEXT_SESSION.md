# Next session kickoff

Updated 2026-09-29 at the close of the **D2 re-close attempt 2** session (U-9147 / U-9151).
Branch `race/first-frame-parity`. Nothing is pushed.
Superseded kickoff: the 2026-09-29 "re-close attempt 1" one (kept below).

> ## START HERE — D2 is STILL REOPENED, but the map changed
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) **§9-§11**
> and ROADMAP §D2's "Re-close attempt 2" block. **Do not re-derive any of it.**
>
> **The §3 bounds are unchanged and are not renegotiable.** PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**.
>
> | metric | attempt 1 | **now** | verdict |
> |---|---:|---:|---|
> | slip 1500-2000 | 0.1332 | **0.1445** | FAIL, -24.9% |
> | slip 2000-2600 | 0.2179 | **0.2557** | FAIL, **+2.3% — 0.00083 over the upper bound** |
> | driving-median | 1818.42 | **1740.54** | FAIL, -10.4% — **moved the wrong way**, kept per §3d |
>
> 4 of 5 runs identical; run 3 was the known U-9148 second attractor and is reported.
>
> **DONE, do not redo:**
> - **U-9147 IS LOCALIZED.** The first divergent quantity is `d(bodyH)/frame`, a
>   speed-independent constant: **-0.04249 (port) vs -0.04462 (original)** = **100/105**.
>   `_DAT_00613108` is **105.0** in the running original (9 live `--peek` samples); the port
>   hardcoded 100.0. **FIXED** — `g_handlingTorque`, the handling table completed with the
>   measured tags 6/12/18, selector 6. `d(bodyH)/frame` and the axis-minus-forward offset
>   now match the original exactly. `MASHED_HANDLING_TYPE=0` reverts.
> - **U-9151 is CLOSED.** `0x004c4600` is a dispatcher; the multiply is the measured
>   `0x005cb2a0`, ported naked-x87 (`Math/RwMatrixMultiplyCpu.cpp`, hooks.csv row at C3).
>   Two GREEN diffs (12/12 and 10/10). The exe's A6b orient is **bound**: 7 runs of 7 exit 0,
>   25 natural samples, `xfok=1` on all.
> - **A6a is CLEARED by measurement**, with a tool that self-checks first
>   (`re/tools/statediff/a6a_replay.py`, self-check 1 worst rel. error 7.8e-07). Block #4
>   per-wheel law 0.7-1.9% per wheel; block-#5 yaw torque 1.1-1.7%; clamp #6 `k_vel` ±3%
>   with opposite signs in the two bands; wheel geometry 0.002 rad. `g_suspScale` was
>   MEASURED on the original at **692.3021850585938** — the port's to the last digit.
> - **§6.2's handling-globals elimination is WITHDRAWN** (the A3 walk chains through `e[3]`
>   and matches tag 6, giving 105).
> - `a8_wheelfit.py`'s cross-side fit stays withdrawn. Do not quote a lateral-coefficient
>   ratio from it.
>
> **PICK UP HERE — the residual is the VELOCITY heading, and the scored window is a
> TRANSIENT.** With `d(bodyH)/frame` now exact on both sides:
>
> | | ORIG 1500-2000 | PORT | ORIG 2000-2600 | PORT |
> |---|---:|---:|---:|---:|
> | `d(bodyH)/frame` | -0.04462 | **-0.04462** | -0.04462 | **-0.04462** |
> | `d(velH)/frame` | -0.04238 | -0.03965 | -0.04307 | -0.04135 |
> | `beta` | 0.1929 | 0.1345 | 0.2498 | 0.2290 |
>
> `|d(velH)| < |d(bodyH)|` on **both** sides, so neither car is in steady state inside the
> scored window — both are still building slip. And the band populations differ (port
> n=227 vs original n=312 at 1500-2000). So the next question is **how far into the
> spin-up each side is when the band is scored**, not the tire law:
> `a8_momentum`'s effective-dt already matches to 0.3-5.7%, and every local law in A6a is
> cleared above. Suggested first move: plot slip against *time since the steer-hold onset*
> rather than against speed, on both sides, and see whether the port's curve is the same
> curve sampled earlier — if it is, the defect is in how fast the car reaches the band
> (acceleration / `RecoverOffMesh`), not in the cornering law.
>
> Then, in order: **`RecoverOffMesh`** (`TrackRenderer.cpp:2142-2164`, halves `car_speed_`
> 11-59x per 1080 frames, no original counterpart — it bears directly on driving-median,
> which is now the *worst* of the three at -10.4%), and **U-9152** (the `+0x928` vs
> `g_bodyBasis` storage split).
>
> **New tooling this session, reuse it rather than rebuilding it:**
> - `MASHED_A6ADUMP=<path>` — A6a's own per-wheel `lac/la8/la4 / f5 / le / lbc / dF` plus
>   `l60 / grip / k_vel / arm`, at `%.17g`, in **two phases** (`act.*` = what A6a computed,
>   `snap.*` = the render-tick record the `.msd` sees). Default-OFF.
> - `re/tools/statediff/a6a_replay.py` — the replay, four self-checks, and the
>   CROSS / GEOMETRY / HEADINGS / CLAMP #6 / ANGULAR tables. **Run the self-checks before
>   believing any table.**
> - `re/frida/scenario_launch.py --peek "<rva>:<f|d|i|u>,..."` — plain `Memory` reads of
>   image globals, no `Interceptor`, no hook, no write. Forms: bare RVA, `@<abs>`, and
>   `i<rvaA>+<rvaB>+<off>` (the RW device-table pattern). This is how `_DAT_00613108`,
>   `_DAT_0088e5f0` and the device multiply were all pinned.
> - `MASHED_A6BTEST=<path>` — the exe-side A6b witness.
> - arg_type `matrix_multiply`; hooks `rw_matrix_multiply_cpu`, `rw_matrix_rotate_inner_cpu`.
>
> **Guards as of this session** (re-run them, don't assume): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3** with `c1_median` 52.5 / 38.0 / 49.0; power-ups **11/11 decision
> CLEAN** with `g3` contact diverging; oracle rule 3 **GREEN**; build with `rva-lint NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.

## SUPERSEDED kickoff — D2 re-close attempt 1 (kept as history)

Updated 2026-09-29 at the close of the **D2 re-close attempt 1** session (U-9149 / U-9147).
Branch `race/first-frame-parity`. Nothing is pushed.
Superseded kickoff: the earlier 2026-09-29 one (player-regression U-9141 / U-9145).

> ## START HERE — D2 re-close attempt 1 is DONE and D2 is STILL REOPENED
>
> Read [`re/analysis/D2_REOPEN_2026-09-29.md`](analysis/D2_REOPEN_2026-09-29.md) and
> ROADMAP §D2's "Re-close attempt 1" block. **Do not re-derive any of it.**
>
> **The bounds are pre-registered and must not be renegotiated** (that note §3, committed in
> `d81a8df6` *before* any fix, from four original solo captures). PASS intervals:
> `slip 1500-2000` **0.18855 .. 0.19635**, `slip 2000-2600` **0.24488 .. 0.25487**,
> `driving-median` **1904.70 .. 1982.44**, plus ≥3 port runs agreeing within the original's
> own half-range. Current port, 3/3 identical: **0.1332 / 0.2179 / 1818.42** — FAIL, FAIL,
> FAIL. If you think a bound is wrong, change it **in writing with the reason, before the
> next measurement**, never after seeing a result.
>
> **DONE, do not redo:**
> - **U-9149 decoded.** `[esp+0x3c]` at `0x0047093b` is `E+0x0c` = A4's `param_3` slot,
>   reused at `0x004706a2` to hold `record + [record+0x9a8]*0x40 + 0x928`, an `RwMatrix`.
>   Same pointer A5 gets as arg 2. The `.asi` `Call_A6b` is **fixed and verified** (144
>   self-test samples, 64 airborne, `ndiff=0` and `xfok=1` on every one).
> - **A6b is NOT U-9147.** Disjoint gates: A6b needs `+0x9e0 == 0`, the metric scores
>   `+0x9e0 >= 3.5` (`a8_slip_axis.py:35`). All five dual-copy leads are now eliminated.
> - **Eliminated with reasons** (don't re-try): A6a's matrix argument (never read, 0 reads
>   at `E+0xc`); the four handling globals `0x00613108/14/30/3c` (A3 seeds them to exactly
>   the port's hardcoded values and the override key `[0x00613140]` is 0 = the default
>   entry; max variant ±5%); "the port chases the velocity heading"
>   (`BodyOrient_IntegrateStep` **is** wired at `VehiclePhysicsRun.cpp:812`).
>
> **PICK UP HERE — U-9147 is NOT localized, and three attempts at localizing it today were
> all withdrawn. Read `D2_REOPEN_2026-09-29.md` §6.0's third correction before touching it.**
>
> `a8_wheelfit.py`'s cross-side lateral-coefficient comparison is **unsound in both modes**:
> `port_frames` builds the port's `lat` from a 2-D velocity heading (`u = (cos velH, 0,
> sin velH)`) and ignores the `wld4` the port actually logs, so the two sides' fit bases
> differ. Arithmetic tell: with `p[-1] == 0` (measured, all four wheels, both sides) A6a's
> lateral scale obeys `f5 <= lbc` always (`Integrate2.cpp:441-450`), so the `a/lbc = 1.840`
> that was reported is impossible from that code path. **Do not quote a lateral-coefficient
> ratio from this tool until the instrumentation below lands.**
>
> **What IS established and can be relied on:** every input to the coefficient matches the
> original (`p[0x15]` 0.15, `p[0x16]` 0.0125, `p[0x1b]` 1091.8/1083.8/1084.6/536.3, `p[-1]`
> 0 on all four, `g_suspScale` 692.3 vs ~710, `le4` capped 1024 both sides); and A6a's own
> `|lat|` agrees **1.001 / 1.003 / 0.948 / 0.960** (port `wld4` vs the corrected original
> side — both are A6a's quantity, so this one IS apples-to-apples). The
> `min(le4,1024)`-clamp hypothesis is **refuted** (original is also 1024 everywhere).
>
> **Next step is INSTRUMENTATION, not a fix.** Add A6a's real lateral basis to the port's
> `[A8-ORIENT]` diag line — the vector `lac/la8/la4` (`Integrate2.cpp:448`) and the applied
> lateral scale `f5` (`:442`/`:449`) — and make `a8_wheelfit.py`'s `port_frames` read them
> instead of rebuilding `lat` from `velH`. Only then is `a/lbc` measurable on both sides.
> The `--lat-mode wheelpoint` fix to the ORIGINAL side is correct and stays.
>
> Then, in order: **U-9152** (the `+0x928` vs `g_bodyBasis` storage split),
> **`RecoverOffMesh`** (`TrackRenderer.cpp:2142-2164`, halves `car_speed_` 11-59x per 1080
> frames — bears on driving-median, the metric closest to its bound at -6.4%), and
> **U-9151** (the exe A6b binding, blocked on a CPU port of `RwMatrixMultiply 0x004c4600`;
> binding it as-is crashes the exe with `0xC0000005`).
>
> **Guards as of this session** (re-run them, don't assume): criterion (e) PASS 3/3; AI (b)
> FAIL 3/3 with `c1_median` 49/45/52.5; power-ups 11/11 decision CLEAN with `g3` contact
> diverging; oracle rule 3 GREEN; build with `rva-lint NEW=0`.
>
> **The D3 modes 3/7 hold stands.** D2 must close before it starts.

> ## ORDER OF WORK CHANGED 2026-09-29 (user decision): **D2 IS REOPENED**
>
> **Do the D2 re-close first. The D3 modes 3/7 port does not start until D2 closes again.**
>
> **Why.** The evidence that closed D2 compared a **three-opponent port run** against a
> **ONE-car original capture** (`orig_steerR.msd.provenance.json` has no `--cars`;
> `re/frida/scenario_launch.py:1739` defaults to 1). On the matched solo arm the port has
> **never** reproduced the original: slip 1500-2000 `0.1332` vs **`0.1913`** (**-30%**),
> slip 2000-2600 `0.2179` vs **`0.2498`** (**-13%**), driving-median `1818` vs **`1941`**
> (**-6%**). Evidence: `re/analysis/PLAYER_REGRESSION_2026-09-29.md` §5-§6.
>
> **This does not overturn the headline below.** There is still no player physics *regression
> between commits*. The port is short against the **original** at both commits — that is the
> D2 question the three-vs-one asymmetry hid.
>
> **The gate is now:** D2 metrics on the **SOLO arm** (`MASHED_MEASURE_SOLO=1`) against the
> original's **solo** capture, within bounds **pre-registered before the fix**. Pre-register
> first — `a-band-scored-off-regime-is-not-a-measurement` is a live precedent on this lane.
>
> **Blocking:** **U-9149** (A6b `0x00468980`'s context pointer from the stack slot at
> `0x0047093b` — dead in the exe, contradicted in the `.asi` forwarder; both `0x00468980` and
> `0x00470670` are now C2) and **U-9147** (the standing slip gap).
>
> ROADMAP §D2 carries the REOPENED block; the CLOSED block is kept below it as history.

**THE HEADLINE: there is NO player-car physics regression since D2 closed.** The
`-16% / -14% / -64%` the previous kickoff item 3 described was the controlled arm's own
asymmetry, and the D2 reference turns out to be a **one-car race**. On the reference's own
scenario HEAD reproduces `56ad3806` to `-3.1% / -0.1% / +3.3%`. Read
`re/analysis/PLAYER_REGRESSION_2026-09-29.md` and ROADMAP §D2's **second** amendment; do not
re-derive either. **U-9141 and U-9145 are RESOLVED.** (Still true — but see the REOPENED
block above: "no regression between commits" is not "matches the original".)

**Three user decisions are in force from 2026-09-29 and are already actioned** — do not re-ask them:
1. **U-9142: KEEP the spawn settle, default-ON**, `MASHED_NO_SPAWN_SETTLE=1` stays as the A/B revert.
   **AI criterion (b) is re-baselined with the settle ON** (ROADMAP §D3), and the (b) bands are NOT moved.
2. **U-9141: the D2 gate recipe has a CONTROLLED arm.** Corrected 2026-09-29b: the arm to use
   is **`MASHED_MEASURE_SOLO=1`** (no opponents — the reference's own scenario), not
   `MASHED_MEASURE_NOOPP=1` (opponents parked, a scenario neither side ran). `--max-lines 1080`
   stays. ROADMAP §D2 carries both amendments; the second supersedes the first's conclusions.
3. **D3 CLOSES BY PORTING BEHAVIOUR MODES 3 AND 7** (`FUN_00414c30` + the world-object query
   `FUN_00484c70`). **D3-R1 is no longer a carried residue** — AI (b) now fails on all three
   cars at correct speed, so there is one open criterion on three cars, not a car-1 residue.
   **ON HOLD from 2026-09-29: this port does not start until D2 re-closes** (see the ORDER
   block at the top). The decision about *how* D3 closes stands; only its start is deferred.

## READ FIRST — the tracker changed shape on 2026-09-29 (dual-copy session)

A separate 2026-09-29 session actioned the user's decision on the dual-copy audit. Three
things are different from every kickoff before it. **Do not re-derive any of them.**

1. **`hooks.csv` has a tenth column, `exe_file`** (last, so positional readers still work).
   `file` names the copy the evidence measured — by convention the `.asi`. `exe_file` names
   the TU compiled into `mashed_re.exe`. Empty = the exe has no port. Regenerate with
   `py -3.12 scripts/backfill_exe_file.py`. Four repair scripts that asserted a literal 9
   columns were fixed; everything else was already header-keyed.
   **The number to keep in mind: after the demotions, of 1184 C3/C4 rows only 203 have
   `exe_file == file`** — 183 name a different exe TU, and 798 are empty (the exe has no port
   at all, so the evidence does not cover the default build). Before the demotions the same
   split was 203 / 212 / 798 of 1213.

2. **`re/CONFIDENCE.md` has a new clause, "Which copy the evidence covers".** A row is C3/C4
   **for the shipping exe** only if `exe_file` is empty or `== file`, or the exe copy has its
   own evidence. When `exe_file != file` the level describes the `.asi` copy and **may not be
   cited in a parity, D3-criterion or DoD argument.** Fixing an exe copy by reading is
   C2-grade; a fixed copy does not restore the row.

3. **29 rows were demoted to C2** (9 × C4→C2, 20 × C3→C2) — `C4 184 → 175`, `C3 1029 → 1009`.
   **Eight of them are AI rows that D3 criterion (b) runs on**: `0x004177b0`, `0x00415e20`,
   `0x00416250`, `0x00416a30`, `0x00417da0`, `0x00418560`, `0x00418860`, `0x00443080`. Five
   more are the physics A-chain. This does **not** change the (b) measurement or the D3 gate
   table below — it changes what the trackers are allowed to claim about the bodies (b) runs
   on. Full record: `re/analysis/DUAL_COPY_FIX_2026-09-29.md`.

**And this is the part that bears on D3 (b) directly:** the AI copies the exe runs are
`Ai/AiStandalone.cpp`, not the `.asi` TUs the C3s were earned on, and they differ in ways that
plausibly *cause* (b) — `rate1` pinned `0.0f` (`AiStandalone.cpp:983`, `:1102`) makes the brake
gate permanently false and fires the curvature multiplier unconditionally; `int mode = 0`
(`:844`) kills eight targeting modes; `SteerAngleError` takes heading from velocity (`:175`)
while its own sibling at `:215` uses body-forward. Those are named in the audit's §8.2 as the
most direct levers on (b). Nobody has tried them yet — **this session changed no game code.**

A build guard now stops new pairs appearing: `scripts/lint_rva_bodies.py`, called from
`mashedmod/build.bat` before the compile step. It WARNs on the 110 known pairs in
`re/tools/dual_copy_allowlist.txt` and **FAILS the build on anything new**. If a build stops
with `[rva-lint] FAILED`, you have added a second body for an RVA — share one TU, do not
silence it. Burning that list to zero is a named ROADMAP D4 item.

## Where D3 stands

**D3 is NOT closed, and there is now exactly ONE gate failure left: AI criterion (b).**
Full record: `re/analysis/D3_DRIVE_FORCE_2026-09-29.md` and, for the session before it,
`D3_DRIVE_2026-09-28.md`. ROADMAP §D3 "D3 closure state 2026-09-29" is the authoritative
summary; do not re-derive either.

| third | state |
|---|---|
| Powerups (c) | **MET** 2026-09-28d. Sweep re-run 2026-09-29: 11 of 11 decision CLEAN, contact CLEAN on 10 of 11, `g3` DIVERGES unchanged (the known 2-query-of-546 R_FLAME residue). |
| Modes (a)-(d) | MET. Rule 3 oracle GREEN 2026-09-29, and that run *did* produce 2 segment-ends, so the rule-3 tail arm is covered. |
| AI (a), (c), (d) | MET. |
| AI (e) | **MET 2026-09-29** — all six gated values inside 0.05% of the reference against a ±2% band. U-9140 resolved: the cause was A6a's unported START BOOST block. |
| AI (b) | **NOT MET on all three cars, and now measured under matched speed for the first time.** This is the only thing between here and D3 CLOSED. |

### What is CLOSED and must not be re-opened or re-measured

- **U-9140 RESOLVED.** The 8-9x drive-force gap was A6a's `+0xbf8` start-boost block, never
  ported. Ported verbatim (`Integrate2.cpp`, cites `0x00467d3a..0x00467e44`), 5e6/wheel or
  8e6/wheel for the two least-progressed cars, measured on three separate original captures.
- **The `[A8-B14CADENCE]` question is SETTLED and the answer is "they do not differ".** The
  port's render-tick `+0xb14` equals its consumption-time value on 898/899 frames, and the
  original's snapshot is provably a single-pass value (A4 zeroes at entry and calls A6a once;
  plus `linTerm × captured +0xb1c` reproduces the original's own per-frame Δspeed on 9
  consecutive frames). Do **not** re-run a cadence probe.
- **U-9142 ANSWERED by measurement: KEEP the spawn settle.** `MASHED_NO_SPAWN_SETTLE=1` fails
  (e) on all three cars by -2.1% to -6.9%. It is no longer a user decision.
- The force→velocity conversion, the gear law, the gearbox constants, the wheel states and
  the drive-only accumulator law are all confirmed faithful. The launch is not a physics
  question any more.

## What is owed, in the order it should be taken

### 1. AI criterion (b) — the ONLY D3 blocker. Start here.

`re/tools/ai_ctrl_window.py --check <csv>` on a fresh standalone capture:

```
py -3.12 re/tools/sa_capture.py verify/<tag> 8,30,60 MASHED_MUTE=1     MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=left-bl     MASHED_TITLE="D3 AI (b) <what>" MASHED_AI_STEPDUMP=verify/<tag>.csv
py -3.12 re/tools/ai_ctrl_window.py --check verify/<tag>.csv
py -3.12 re/tools/ai_speed_env.py   --check verify/<tag>.csv   # (e) must stay PASS
```

Where it stands on the default build (2026-09-29, `verify/d3_force_20260929/sa_b2.csv`):

| car | failing bands |
|---|---|
| 1 | `c0_distinct` 7 (floor 13), `c1_distinct` 106 (ceil 70), `steer_distinct` 112 (ceil 96), `c1_median` 48.0 (band `[0,0]`), `abs_steer_median` 48.0 (ceil 23) |
| 2 | `c1_distinct` 93, `steer_distinct` 121, `c1_median` 42.0, `abs_steer_median` 58.0 |
| 3 | `c1_distinct` 98, `steer_distinct` 116, `c1_median` 46.5, `abs_steer_median` 46.5, `accel_distinct` 1 (floor 2), `brake_distinct` 1 (needs 2) |

**Read the `c1_median` / `abs_steer_median` rows first — they are the NEW information and the
biggest ones.** The band is `[0,0]` for `c1_median`, i.e. the original's AI issues its steer
on the `c0` byte with `c1` at zero for at least half the window, and the port issues 42-48 on
`c1`. That is a sign/channel asymmetry, not a magnitude tuning problem: `c0` and `c1` are the
mutually exclusive steer pair (`+steer -> input[0]`, `-steer -> input[1]`,
`VehiclePhysicsRun.cpp` WS-A8-STEER block), so the port is steering one way far more than the
original. `c0_distinct` 7 on car 1 has been the standing symptom since 2026-09-26 and is
diagnosed in `D3_AI_RESIDUE_2026-09-27.md` (the `DAT_0089a368` spline-bank roll → curvature →
the `curv>20` multiplier at `0x0041665c`). Check whether the same chain explains the
`c1`-side pile-up before opening a new hypothesis.

Two things that must NOT be used as an excuse:

- These numbers are **worse** than the 2026-09-28 ones, and that is not a regression from the
  boost port: `MASHED_NO_START_BOOST=1` reproduces the 2026-09-28 (b) table exactly. What
  changed is that the cars now go the right speed, so the steer bands are for the first time
  scored on a car whose lookahead/curvature inputs are in the right regime. The older (b)
  numbers were not a measurement of the AI law.
- `accel_distinct` / `brake_distinct` = 1 is the D3-R1 story (unported behaviour modes 3 and 7,
  `FUN_00414c30` / `FUN_00484c70`), i.e. the port genuinely never lifts or brakes. It is a
  real band failure and it is *not* fixable inside the steer chain.

### 2. U-D3-BOOST-ARM — find the writer that arms the boost

The A6a boost FORCE law is transcribed and RVA-cited. The **arming** is a measured seed:
`+0xbf8 = 1`, `+0xbf4 = 1300`, once per AI slot, at its first real post-settle step
(`VehiclePhysicsRun.cpp`, the START BOOST ARM block). `MASHED_NO_START_BOOST=1` reverts.

`py -3.12 re/tools/findoffset.py --writes 0xbf8 0xbf4 0xbf0` puts **every** `.text` access to
those fields inside `FUN_00467650`, so the real arming store uses a base the displacement
sweep cannot see. Next commands, in order:

```
# 1. Frida WRITE watchpoint on &rec[car]+0xbf8, armed during the COUNTDOWN. A6a's two
#    +0xbf8 stores are both inside `bf8 == 1` / `== 2` arms, so while bf8 == 0 nothing in
#    A6a writes it and the first fault IS the arming instruction. Report the faulting EIP,
#    then decomp its containing function.
# 2. If the watchpoint API is unavailable: a Ghidra script walking stores whose base is
#    DAT_008815a0 + k and whose displacement is 0xbf8 - k.
```

Two things the writer would settle: whether 1300 is a constant or a value the countdown
computes from the throttle timing (`+0xbf4` rises +150/frame net through the last five
countdown frames and stops at the green), and therefore whether a human player who does not
jump the lights is boosted — the port currently does not boost slot 0, because no original
capture shows an armed player. Also open, and cheap once the writer is known:
`[UNCERTAIN] U-D3-BOOST-ORDER`, the per-car race-progress float at `0x008a96e8 + car*0x30c`
has no writer in the standalone (`FUN_00408a70` unported), so the 8e6 pair is pinned to the
grid order `{2,3}` instead of tracking race order. No effect on (e).

### 3. U-9141 / U-9145 — **RESOLVED 2026-09-29b.** Everything below this line is HISTORY.

Do **not** run the bisect the old item 3 asks for; it was run, and its premise was wrong.
Authoritative record: `re/analysis/PLAYER_REGRESSION_2026-09-29.md` (plan pre-registered at
`235e964a`, verdict at `2348614e`) and ROADMAP §D2's second amendment. In one paragraph:

- The arm was **asymmetric** — the knob was applied at HEAD and not at `56ad3806`. Applied at
  both ends, `647a5e24` (the *same commit*, both ways) moves `1931.36 → 692.07`, so the
  `-64%` is 0% a commit.
- The D2 reference is a **SOLO race**: `orig_steerR.msd.provenance.json` carries no `--cars`
  and `scenario_launch.py:1739` defaults it to 1; re-run live, `cars=1`, `SegmentCheck`
  1448/1448 with **segment-end 0** and `m1Max = -1`.
- With **`MASHED_MEASURE_SOLO=1`** at both ends: original `0.1913 / 0.2498 / 1940.59`,
  `56ad3806` `0.1374 / 0.2181 / 1760.49` (3/3), HEAD `0.1332 / 0.2179 / 1818.47` (4/5) —
  **HEAD vs `56ad3806` = `-3.1% / -0.1% / +3.3%`. No regression.**
- U-9145's coupling was two channels, **neither a shared physics global**: (A) the harness's
  steer-hold onset was on the real clock while the sim runs on a real-time accumulator —
  **fixed**, it now counts sim steps; (B) the opponents get the PLAYER eliminated at
  `race_time_` 2.1-2.6 s, which freezes `race_[0].gate` into the player's own off-mesh
  re-aim. `g_torqueRingPhase`, `g_suspScratch` and the pickup field are all REFUTED.

**What is newly owed, and where it sits in the order.** Item 1 (AI (b)) is still first, and
the closure path is now decision 3 above — port behaviour modes 3 and 7. Then, in this order:

- **U-9147** — on the matched (solo) arm the port is **~28% short on `slip 1500-2000` at BOTH
  commits** (0.1374 / 0.1332 against 0.1913). Not a regression; a D2-era gap the recipe's
  scenario mismatch concealed. Next command: `MASHED_MEASURE_SOLO=1` + `MASHED_COUPLING_DIAG=1`
  and a per-frame per-wheel lateral-force diff against `orig_steerR.msd`, the way
  `A8_velocity_vector_motion_20260825.md` follow-up 27 does. Re-baselining the ROADMAP §D2 row
  on this arm is a **user decision** — do not do it unasked.
- **U-9146** — does the ORIGINAL also eliminate a stationary player at ~2.5 s with three
  opponents? The reference is solo, so it cannot say. Next command:
  `py -3.12 re/frida/scenario_launch.py --oracle --rule 0 --cars 4 --poke-ctrl-slots --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38`,
  then read `segment-end` / `deadMax` from `log/rules_oracle_rule0.json`. Regardless of the
  answer, `race_[0].alive → race_[0].gate → the off-mesh re-aim` (`TrackRenderer.cpp:2808`)
  has no original counterpart.
- **U-9148** — one HEAD solo run in five lands in a second attractor, so a real-time-keyed
  input survives the sim-clock fix on the HEAD side. Next command: two
  `MASHED_PLAYERTRACE=1 MASHED_MEASURE_SOLO=1` runs until both attractors are sampled, then
  diff the two `player_trace.log` files for the first differing FIELD.
- **U-9149 — take this one WITH U-9147; it is the live lead for it.** A4 loads A6b's ESI from
  `[esp+0x3c]` (`0x0047093b`) two instructions after loading A6a's from EDI (`0x00470934`),
  so A6b's context is never null and the exe's `nullptr` at `VehicleControl.cpp:195` makes
  the whole rotation-apply dead — airborne auto-level and the velocity-align rotation never
  run. The `.asi` C4 forwarder's `ESI = record` assumption is **also** unsupported by that
  instruction. Next command: decompile `FUN_00470670` and read the third argument of its
  `FUN_00468980` call at `0x00470943`. **Do not invent a matrix to pass.**

### 3b. The dual-copy leads (`aa4795af`) — four are DONE, one is open

Judged against the original 2026-09-29b, `PLAYER_REGRESSION_2026-09-29.md` §7.3, commit
`3e4fba77`. **Do not re-do these four**; the fifth is U-9149 above.

- **A5 `0x0046ddb0` constants — FIXED, and there were EIGHT, not the four the audit named.**
  All were 6-significant-digit truncations of exact round numbers (1/3000, 1/300, -1/30000,
  5e-6, 1/3, 0.99, 1e-4, 2^-31), now `asFb(bits)`. Re-audit with
  `audit_consts.py`-style bit comparison against `original/MASHED.exe`: **30 exact, 0
  mismatch**. Worth running the same check on any other header that annotates `_DAT_`
  addresses — this class of slip is invisible to review and trivial to detect.
- **A3 `0x0046b540` output stride — FIXED to `0x40`**, settled from `add ebx, 0x40` in all
  three loops, not from symmetry. Measured inert on this recipe.
- **A6a `0x00467650` gear clamp — FIXED to bound 5** (the original declares `local_54[5]` and
  reads it unclamped). Measured currently inert: `gear` took only 0..4 over 79,356 frames.
- **`CarCarContacts.cpp:192-195` — REFUTED as the U-9145 coupling.** `0x00469df0` has **zero
  call sites** in the whole tree. The empty `Rw_MatrixDerive` is latent dead-code damage and
  belongs to the audit's own §8.2 item 6 pass.
- **None of the four moves U-9147's ~28% slip gap**, though the constants are demonstrably
  live ((e) moved 0.1 on two cars). That is a useful negative — do not re-search there.

---

**HISTORY (2026-09-29, first pass). Superseded — kept for the audit trail.**

### 3-old. U-9141 — the controlled arm EXISTS and it found something. Redo the bisect on it.

**Read `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` §3 before touching this.** The arm and
its pass rule were pre-registered at `1d0ca916` before any run; the results are at `4938adba`.
Do not re-derive either, and do not move the bounds.

**The arm works.** `MASHED_MEASURE_NOOPP=1` (opponents not updated) plus `--max-lines 1080`
gives a run-to-run spread on `slip 1500-2000` of **exactly 0** on 3/3 runs at both HEAD and
`56ad3806`, where the uncontrolled arm still spreads 0.0088. The knob is proved inert when
unset (a default run on the build containing it reproduces the pre-knob build to every
printed digit). So the instrument question is settled.

**What it found, and it inverts the 2026-09-28 conclusion.** With the opponents absent at
BOTH ends, HEAD is `-16.0% / -14.0% / -64.2%` off `56ad3806`
(slip 1500-2000 0.1609 vs 0.1916, slip 2000-2600 0.2296 vs 0.2669, driving-median 691.0 vs
1932.1). The 2026-09-28 bisect concluded *"no commit in `56ad3806..HEAD` edits the player's
solver, therefore the drift is the instrument"* — **on a controlled instrument that does not
hold.** There is a real player-side difference, and the uncontrolled recipe could not have
seen it: its 0.128..0.177 spread brackets both 0.1609 and 0.1916.

`56ad3806`'s controlled arm reproduces the ROADMAP §D2 row on 2 of 3 gated statistics plus
`av.y` to four decimals (slip 0.00%, slip +0.04%, av.y exact) and misses the driving-median
by +2.39% against a ±2% bound. **D2 is not reopened** — that overshoot is already in the
record at §3.1 of the 2026-09-28 note, so the row's `1887` is itself ~2% low at its own
commit. Re-baselining that figure is a **user decision**; do not do it unasked.

**What is owed, in order.**

1. **Resolve U-9145 first — it may be the whole of U-9141.** The opponents move the PLAYER's
   driving-median 691 → 2538 (3.7x) on the same build and recipe, and `VehicleCarCarContact`
   (`0x00469df0`) has **zero callers** in the port, so no car-car path exists to do it with.
   The coupling is shared mutable state. Per-global A/B on the controlled arm, one temporary
   env-gated diag at a time, removed afterwards; candidates and citations in §3.5 of the note
   (`g_torqueRingPhase` `DAT_007f101c` and A4's steer ring `+0x1ac`/`+0x270` at `0x00470670`
   is the first one to try). **One run per configuration decides**, at a spread of 0.
2. **Then re-bisect `56ad3806..HEAD` on the controlled arm**, one run per commit, over the 17
   `mashedmod/`-touching commits of `D3_DRIVE_2026-09-28.md` §1.2:
   ```
   py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
       MASHED_MEASURE_NOOPP=1 MASHED_TITLE="U-9141 controlled bisect <sha>"
   py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
       --port verify/<tag>/motion_diag.log --max-lines 1080
   ```
   Classification fixed in §3.5: `slip 1500-2000` `>= 0.185` GOOD, `<= 0.170` BAD, between =
   INDETERMINATE and gets a second run. Commits before `09a73dc6` have no opponent loop, so
   the knob is a no-op there.

**Two premises to carry, both already paid for.** The §1.3 one-run-decides rule was refuted on
the UNCONTROLLED arm (0.1840 and 0.1283 from one build) and is sound on this one — do not
re-litigate it in either direction without citing which arm you mean. And one asymmetry
remains in the arm: at `56ad3806` the opponents are moved by the pre-D3 kinematic Option B
model, whereas the knob leaves them PARKED; if U-9145 is real, the HEAD end should use the
Option B treatment instead (§3.4's `NOAIPHYS`, measured 0.1736 / 0.1561 / 0.1774).

## Added 2026-09-29b (the player-regression session)

- **`MASHED_MEASURE_SOLO=1`** (`D3d9Render/TrackRenderer.cpp`, **both** spawn sites — the
  car-load spawn AND `StartRound`'s `ai_cars_.assign`) — MEASUREMENT HARNESS ONLY, default-OFF:
  spawns **no** opponents, so `ai_cars_` stays empty and `UpdateRace`, the `RaceCamera`
  framing, `ParticipantCount()` and the rule engine all see a one-car race. **This is the arm
  the D2 gate should use**, because the original-side reference was captured that way. Verify
  it took: `mashed_re.log` must log `MATCH-SEED … participants=1`. Gating only one of the two
  sites leaves it silently inert — that mistake cost a whole bisect's worth of mislabelled
  runs this session.
- **The steer-hold onset is counted in SIM STEPS, not real seconds** (`exe_main.cpp`,
  `steerHoldApply()`). It used to be decided once per RENDER frame from a real clock while the
  car sim runs on a real-time fixed-timestep accumulator, so the sim step at which the held
  lock began tracked CPU load. Same 4 s threshold. This is what made the default `a8` arm
  deterministic — four consecutive commits now reduce bit-equal.
- **`MASHED_PLAYERTRACE=1`** (`D3d9Render/TrackRenderer.cpp`) — default-OFF per-sim-step
  `%.17g` dump to `./player_trace.log`: world position, `in.dt`,
  `race_[0].gate/laps/progress/alive`, and record floats `+0xb14` / `+0xb1c` / `+0x9e4`, one
  line before and one after `UpdateRace`. **Diff two of these and read the first differing
  FIELD** — that single recipe found both U-9145 channels and refuted four named suspects. It
  is far cheaper than a per-global A/B and it cannot be fooled by a knob that is inert.
- **A `git checkout HEAD -- mashedmod/` restore inside a bisect script will silently delete
  your uncommitted edits.** It ate two of them this session. Commit before probing.
- `a8_run_port.py` at 50 s discarded two boots that stalled in the frontend (`NAV_DEMO
  phase=0 00_challengeselect` in `mashed_re.log`); at **90 s** the exe exits on its own with a
  complete race. Use 90 and re-run once on an empty log.

## Added 2026-09-29 (U-D3-DRIVE-FORCE + the D2 controlled arm)

- `MASHED_MEASURE_NOOPP=1` (`D3d9Render/TrackRenderer.cpp`) — MEASUREMENT HARNESS ONLY, on the
  `MASHED_STEER_HOLD` precedent: sets the per-opponent update loop's bound to 0 and does nothing else.
  Default-OFF and proved inert when unset. **Only legitimate on the D2 controlled arm, where BOTH ends
  of the comparison run it.** Do not use it to make any other number look better.
- `--max-lines N` on `a8_slip_axis.py` and `a8_momentum.py` — truncates the PORT side to the first N
  logged frames, before the regime filter and before the spike median. The ORIGINAL side is
  deliberately never truncated. N = 1080 is the D2 controlled arm's fixed 18.0 s window.
- Frame count == simulated time in the standalone: the chain dt is pinned at `frameMs = 50`, measured as
  a single distinct `linTerm=1.66667e-05` over all 3596 samples of
  `verify/d3_force_20260929/cad1/friction_diag.log`. Frame COUNT varies with machine load (25 vs 30 fps
  gave 1268 vs 1497 frames in the same 50 s wall clock), which is why pinning it is the right control.

- `MASHED_NO_START_BOOST=1` — revert arm for the A6a start boost. Reproduces the 2026-09-28
  criterion (e) and (b) numbers exactly, which is what makes any before/after here legitimate.
- `MASHED_GAMEMODE_STUB=0` — revert arm for `Fi_GameMode()` 6 → 0. Its only live consequence
  is A6a's `+0xbf4` timer site; audited call-site by call-site in `ForceIntegratorStubs.cpp`.
- `Fi_UpdateBoostOrder()` (`ForceIntegratorStubs.cpp`) — ported `FUN_00470c70`
  `0x00470e2e..0x00470f0a`: seeds `DAT_0088e660..66c` with 0,1,2,3 and sorts descending by
  the per-car progress float. Pinned to `{2,3}` in the standalone because that float has no
  writer (see U-D3-BOOST-ORDER above).
- `VehicleControlIntegrate` gained a `car` argument, so A6a's `param_1` is the real car index
  instead of a hardcoded 0. That resolves the `[UNCERTAIN]` that was on `VehicleControl.cpp:187`.
- `re/tools/findoffset.py` is the right tool for "who writes struct field +0xNN" and it was
  what proved the boost arming writer is NOT reachable by a displacement sweep. Read its two
  CAVEATS before citing a hit.
- **`MASHED_AI_STEPDUMP` needs `MASHED_TRACK_VIEW=Training`.** Without it the standalone sits
  in the frontend, never calls `AiStepDump()`, and you get three screenshots and no CSV with
  no error. Cost this session one capture. The full recipe is in
  `verify/d3_force_20260929/PROVENANCE.txt`.
- `a8_run_port.py` moves `motion_diag.log` but **not** `friction_diag.log`. If you run with
  `MASHED_COUPLING_DIAG=1`, delete `friction_diag.log` first and move it yourself afterwards.

## Tools added in the 2026-09-28 session

- `re/tools/ai_speed_env.py` — the criterion (e) scorer. Holds the band as `REFERENCE` /
  `BAND_PCT` / `GATED`. `--check <csv>`, `--envelope`, `--json`.
- `re/tools/statediff/msd_fields.py` — print arbitrary vehicle-record fields per frame out
  of an MSD1 capture. `<msd> 0x490:i 0x494:i 0x498:f ... [--every N] [--first N]
  [--distinct]`. This is what read the original's gearbox and suspension state.
- `MASHED_MOTION_DIAG` now also prints `b0c` / `gb498` / `gb49c`.
- `MASHED_MOTION_DIAG_AI=1` logs the gearbox and launch fields for the **opponent** slots to
  `motion_diag_ai.log` — the widening `D3_SPEED_GAP` §6.3 asked for. Separate file on
  purpose: the `a8` reducers key on `reseed=` … `wax=[…]` and assume slot 0.
- **`MASHED_TITLE` (from `a9da810a`, another session) — use it on every run.** The standalone
  window title is now `Mashed RE | <label> | <state>`, where the label is `MASHED_TITLE` if
  set and otherwise the run's `MASHED_*` env vars. Both harnesses forward bare `KEY=VAL`
  arguments into the child env, so it needs no code change:
  ```
  py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
      MASHED_D3_NOOPP=1 MASHED_TITLE="U-9141 bisect <sha>"
  py -3.12 re/tools/sa_capture.py verify/<tag> 8,65 MASHED_MUTE=1 ... \
      MASHED_TITLE="U-9140 cadence check"
  ```
  This session ran without it and had several near-identical windows open at once while
  bisecting; label them.

## Standing gotchas this session paid for

- The criterion (e) window and the criterion (b) window are the same span
  (`ai_ctrl_window.py`, 220 calls from the first `c4 != 0`), so the two cannot disagree
  about which calls are the race. Keep it that way.
- `flag_a368` (`DAT_0089a368`) is **not binary** on the speed-gap recipe — it takes 0, 1 and
  2. The reference regime is 0 on every window call. A capture with `regime0=0` on a car is
  re-taken, not scored. Two of four originals taken this session landed off-regime.
- The original is **deterministic** on this recipe: 10 regime-0 captures agree to the printed
  0.1 on `launch`, `ft_median_m0` and `ft_median`; only `start_frame` varies (802..891).
  A zero-width envelope is why criterion (e)'s band had to be inherited (2%) rather than
  measured, and that is stated in the note rather than hidden.
- The standalone is deterministic too on the AI capture: `sa_ctl` reproduced the 2026-09-27
  `sa_d1` exactly, which is what makes a one-capture before/after legitimate here.
- Four of this session's `a8` runs produced 38-, 79-, 302- and 361-line logs, i.e. races
  that ended in under a second. Those are truncated boots, not samples; the shortest complete
  race observed is 1023 lines. Discard below ~900 and say so.
