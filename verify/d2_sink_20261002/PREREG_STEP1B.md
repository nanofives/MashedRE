# PRE-REGISTRATION — D2 attempt 19, STEP 1B: WHICH write inside `0x0046f6c0` carries it

Committed **before the run**. Base `11d4ccc2`.

STEP 1 named the carrier: **`SolveWheelContacts` `0x0046f6c0` = 84.33 %** of the port's
`T_post` at `d = 222..250` (n = 29, median speed 633.2, median frame 237, `d` from release
R = 1, `L = 0`), against **0 of 2945** velocity writes across the same RVA on the original's
own §16.7 arm. PREREG_STEP1 §3 rule 3 therefore governs: **confirm the producer, then fix that
body at its RVA.** The port's `0x0046f6c0` has **three** velocity write sites and STEP 1
measured their **sum**. This step names which.

## 1 The three sites, and the original's own structure

`py -3.12 re/tools/disasm_fn.py 0x0047001f 0x00470150` over `MASHED.exe.unpatched`, read this
session. The friction block `LAB_0047001f` is a 4-wheel loop with `esi = edi+0x208 + k*0xc4`:

```
00470030  mov eax,[esi-0x70] / cmp eax,1 / jne 0x4701b2   ; pf[-0x1c] == 1   (wheel active)
00470040  cmp dword [esi-0x18],0xff020202 / jne 0x4700a2  ; pf[-6] == the surface sentinel
0047004c  mov [esi-0x70],ebp                              ; pf[-0x1c] = 0
0047004f  fmul [0x5cc9fc]  x3                             ; * 1000.0  (kFricVel)
00470076  fadd [edi+0x9b0] / fstp [edi+0x9b0]   (+9b4,+9b8) ; ---- SITE 0
004700a2  fld [esi-4] / fcomp [0x5cc340] / jp 0x4701b2     ; pf[-1] < 0.7  (kLowSpeed)
004700b9  mov [esi-0x70],ebp                              ; pf[-0x1c] = 0
004700bc  fmul [0x5cc55c]  x3                             ; * 10.0   (kFricImp)
004700ef  fadd / fstp [edi+0x9b0] (+9b4,+9b8)             ; ---- SITE 1
```

Both arms fall into `0x004700a2`, so the second test is **not** re-gated by `pf[-0x1c]` — the
port's two sequential `if`s at `WheelContactSolver.cpp:293` and `:300` reproduce that control
flow. The third site is the **airborne lateral drift** at `WheelContactSolver.cpp:337`, gated
by `self[0x27c] == 0 && gc != 0 && (gc < 2.0) != (gc == 2.0)`; at `gnd == 4.0` (100 % of the
window, STEP 1 gate EV) that gate is **false**, so site 2 is predicted inert. **That prediction
is registered here and scored below.**

## 2 The instrument

Extends the same default-OFF `MASHED_D2SINK` channel — no new env var, no second log.
`D2Sink::MarkWheel(tag, rec, wheel, a, b, c, d)` adds `w=` and four named floats to the line.
Three new call sites, all inside `WheelContactSolver.cpp`, each **immediately after** its
site's three velocity stores:

| tag | file:line | RVA | extra fields |
|---|---|---|---|
| `wcs_fric` | `WheelContactSolver.cpp:295-299` | `0x00470072..0x0047009c` | `a=pf[-2] b=pf[-1] c=pf[0] d=pf[-6]` |
| `wcs_imp` | `WheelContactSolver.cpp:303-305` | `0x004700eb..0x0047010d` | same |
| `wcs_drift` | `WheelContactSolver.cpp:337-339` | the drift store | `a=d[0] b=gc c=d[2] d=0` |

Plus `wcs_in`, emitted once per `WheelContactSolver` call at its **entry**, so the solver's own
interval closes: `D_wheel = m(sub_wheel) - m(sub_orient)` already brackets the call, and
`m(wcs_in)` must equal `m(sub_orient)` exactly (nothing runs between them).

## 3 Gates — BLOCKING

| id | asks | bar |
|---|---|---|
| **KB1** | `m(wcs_in) == m(sub_orient)` exact float compare, per substep | **100.00 %** |
| **KB2** | within each solver call the per-site deltas telescope: `m(sub_wheel) - m(wcs_in)` equals the sum of the site deltas | `<= 1e-5` rel on **>= 99 %** of substeps |
| **KB3** | **coverage.** Every substep in the window emits `wcs_in`; the per-site hit counts are reported with their ZEROS visible (a site that never fires is reported as `0`, not omitted) | 0 missing `wcs_in` |
| **CH2** | the armed run's `motion_diag.log` is byte-identical to STEP 1's `p_ctrl` control over the shared prefix | must hold |
| **EV2** | STEP 1's EV, re-scored on this run: `n >= 25`, `gnd4 >= 90 %`, `ctrl > 0 >= 90 %`, median speed within 25 % of 633.2 | must hold |

## 4 The registered reading

1. **SITE SHARE.** For each of the three sites, the median per-frame sum of its delta over the
   window, and its share of `D_wheel` (median -23.194031) and of `T_post` (median -27.50393).
2. **THE NAMED SITE** is the one carrying **>= 80 %** of `D_wheel`. If no site reaches 80 %,
   report the split and **do not fix** — that is the same stop rule PREREG_STEP1 §3 rule 4
   applies, and it is restated here deliberately.
3. **THE PREDICTION, registered before the run:** `wcs_drift` fires **0** times in the window,
   because `gnd == 4.0` on 29/29 and its gate needs `gc < 2.0` or `gc == 2.0`. If it fires,
   the gate transcription at `WheelContactSolver.cpp:323` is wrong and that is the finding.
4. **THE INPUTS.** For the named site, the median of its four logged inputs, so STEP 2 can ask
   whether the defect is the site's *arithmetic* or the *fields that reach it*. The original's
   corresponding fields are **not** captured here; if STEP 2 needs them it registers an
   original-side probe first, **entry hooks only**.

## 5 What this step does NOT do

No fix. No knob, no clamp, no fitted constant. No tracker mutation. No C-level change. The
only `mashedmod/src` edit is the probe, and every edit is inside `if (armed)` or is the probe
TU. AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched.

## 6 Run plan

One armed run, muted, `MASHED_WIN_POS=primary-bl` (recipe), `MASHED_TITLE` set, PID tracked and
reaped by `a8_run_port.py`:

```
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_sink_20261002/p_site 90 \
    MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 \
    MASHED_TITLE=d2-a19-site MASHED_D2SINK=<abs>/verify/d2_sink_20261002/p_site/sink.log
```

STEP 1's `p_ctrl` is reused as the CH2 control (the probe is default-OFF, so an unarmed build
and an unarmed run are the same thing; the binary differs only inside `if (armed)`).
Reducer: `re/tools/statediff/a19_split.py --sites`, written before the run.
