# RESULT — D2 attempt 19, STEP 1: `T_post` splits, and the carrier is **`0x0046f6c0`**, not the substep budget

Executes `PREREG_STEP1.md` (committed `3837cfe5`, **unrun**). One armed port run, one unarmed
control, both muted, both `MASHED_WIN_POS=primary-bl` (recipe), both with `MASHED_TITLE`, both
PIDs spawned and reaped by `a8_run_port.py` and nothing else killed. Original side **reused**
from attempt 18 (`verify/d2_budget_20261002/orig_lb18.msd`, release **883**); no new
original-side capture, no original-side probe.

Captures:

```
verify/d2_sink_20261002/p_armed/  sink.log 28 233 lines, motion_diag.log 1630 lines, PID 38648
verify/d2_sink_20261002/p_ctrl/   motion_diag.log 1628 lines (no sink file), PID 31028
verify/d2_sink_20261002/split.csv  per-frame, d = -1..1627
```

**One gate FAILED and is reported as a failure, not amended. One decision rule is REPLACED,
and §5 says exactly which and why.**

---

## 1 The gates

| gate | asked | result | verdict |
|---|---|---|---|
| **CH** | unarmed run writes no sink file; its `motion_diag.log` byte-identical to the armed run's | no sink file in `p_ctrl`; the two logs are **byte-identical on all 1628 shared lines**, armed captured 2 extra tail frames at the kill boundary | **PASS**, with the caveat in §1.1 |
| **CV** | the five once-per-frame tags present exactly once; no orphan `sub_top`/`sub_end`; substep histogram | 1629 frames; `miss` 0 and `dup` 0 for all five; orphans **0**/**0**; `nsub` histogram **{3: 1629}**; 0 unparsed lines; 0 rejected frames | **PASS** |
| **KA1** | `m(w1) == r9e4(w1)` exact float compare | **1629/1629 = 100.0000 %** | **PASS** |
| **KA2** | the telescoping sum of every producer delta == `m(snap) - m(w1)`, `<= 1e-5` rel | **100.0000 %**, worst **2.030e-15** | **PASS** |
| **KA3** | `m(snap)` == the same frame's `motion_diag` `\|vel=[..]\|`, `<= 1e-6` rel | **70.3499 %**, worst **4.665e-06** | **FAIL** |
| **EV** | `d = 222..250`, `n >= 25`, `gnd == 4.0 >= 90 %`, `\|ctrl_xz\| > 0 >= 90 %`, median speed within 25 % of 633.2 | **n = 29**, gnd4 **100.00 %**, ctrl>0 **100.00 %**, median speed **633.2**, median frame index **237** | **PASS** |

### 1.1 CH's caveat, stated rather than waived

The armed log is 1630 lines and the control's 1628. Every one of the 1628 shared lines is
byte-identical. The difference is **capture length at the 90 s kill boundary**, which attempt
18 observed between two runs that carried **no probe at all** (`RESULT.md`: *"s1 differs only
in tail capture length 1630 vs 1628"*). So the probe is inert on every frame both runs hold,
and the two extra frames are outside the `d = 222..250` window by 1377 frames.

### 1.2 Why KA3 failed, and what it does and does not touch

**Reported as a FAILURE. Not re-thresholded, not re-paired, not replaced.** The post-hoc cause
check below is labelled as such.

`motion_diag.log` prints `vel=[%g,%g,%g]` — **6 significant decimal digits**. The probe prints
`%.9g`. Re-rounding the probe's **own** vector through `%g` before recomputing the magnitude:

| comparison | within 1e-6 | worst |
|---|---:|---:|
| probe `mag` vs `motion_diag` `\|vel\|`, as-is | 1146/1629 = **70.3499 %** | 4.665e-06 |
| probe vector **re-rounded through `%g`**, then vs `motion_diag` | 1628/1629 = **99.9386 %** | 1.565e-06 |

So the miss is the **decimal print width of the channel KA3 compares against**, not a phase
disagreement: a 6-significant-digit print carries a ~5e-7 half-ulp and a 3-vector magnitude
compounds three of them, which is the 1e-6 bar itself.

What it does **not** touch:

- **KA2 passes at 100.0000 % with a worst case of 2.030e-15.** The entire split is computed
  inside the probe's own full-precision channel; `motion_diag` is never used to form a
  producer delta.
- The two independently-sourced `T_post` medians agree: **-27.50393** (motion_diag, the
  budget's own definition) against **-27.50415** (probe, `m(snap) - m(w1)`) — **8.0e-06**
  relative, n = 29.
- `-27.50393` is **bit-for-bit attempt 18's** figure for the same window
  (`verify/d2_budget_20261002/RESULT.md`), from a different build and a different run.

---

## 2 The split, `d = 222..250`, **n = 29**, median speed **633.2**, median frame **237**

ORIG `T_post` over the same window and the same `d`: **-0.00464** (n = 29, median speed 790.9,
release 883) — reused from attempt 18, quoted as a **total**, not decomposed.

### 2.1 By producer, as the decision list enumerated it

| term | site | n | median | share of `T_post` |
|---|---|---:|---:|---:|
| `D_clamp6` | `0x004687f0..0x0046897b` | 29 | **-2.668457** | **9.70 %** |
| `D_a6b` | `0x00468980` | 29 | +0.000000 | 0.00 % |
| `D_damp` | `0x00470948`, gate `+0x9f0 == 2` | 29 | +0.000000 | 0.00 % |
| `D_gap` | A4 exit -> loop top | 29 | +0.000000 | 0.00 % |
| `D_tail` | last `sub_end` -> render tick | 29 | +0.000000 | 0.00 % |
| `D_sub[0]` | substep 0 | 29 | -8.298279 | 30.17 % |
| `D_sub[1]` | substep 1 | 29 | -7.737854 | 28.13 % |
| `D_sub[2]` | substep 2 | 29 | -7.157898 | 26.03 % |

> **`D_clamp6` = 9.70 %.** Attempt 18 reached **"one tenth"** by a completely different route —
> feeding the port's own measured `l_60` = 676.82 through clamp #6's transcribed arms
> (`7.2038e-03` of `7.4031e-02`, `RESULT_STEP1B.md` §4). Two independent instruments, the same
> answer. **§23.2's refutation is confirmed a second time, and `k`/`l_60` is not the carrier.**

> **`D_a6b`, `D_damp`, `D_gap` and `D_tail` are EXACTLY ZERO on all 29 frames.** A6b writes no
> velocity (§26.4, now measured on the port too), the parked damp never fires
> (`+0x9f0 == 0`, which the probe line carries directly — the gap `RESULT_STEP1B.md` §4 named),
> and nothing at all runs between A4's exit and the loop top or between the last `sub_end` and
> the render tick. **Three of U-9178's four candidate producers are eliminated by measurement.**

### 2.2 Inside each substep — and this is where it is

| sub | n | chunk (ms) | fixups | `D_orient` `0x0046e9e0` | `D_wheel` `0x0046f6c0` | `D_fixup` `0x0046ef70` |
|---:|---:|---:|---:|---:|---:|---:|
| 0 | 29 | 25.000000 | 1 | +0.000000 | **-8.298279** | +0.000000 |
| 1 | 29 | 25.000000 | 1 | +0.000000 | **-7.737854** | +0.000000 |
| 2 | 29 | **0.000004** | 0 | +0.000000 | **-7.157898** | +0.000000 |

### 2.3 By CODE SITE — summed over the frame's substeps, then median

| site | RVA | n | median | share |
|---|---|---:|---:|---:|
| grip-clamp #6 | `0x004687f0..0x0046897b` | 29 | -2.668457 | 9.70 % |
| A6b | `0x00468980` | 29 | +0.000000 | 0.00 % |
| A4 parked damp | `0x00470948` | 29 | +0.000000 | 0.00 % |
| A9 position+orientation | `0x0046e9e0` | 29 | +0.000000 | 0.00 % |
| **`SolveWheelContacts`** | **`0x0046f6c0`** | 29 | **-23.194031** | **84.33 %** |
| `VehicleContactFixup` | `0x0046ef70` | 29 | +0.000000 | 0.00 % |
| gap + tail | — | 29 | +0.000000 | 0.00 % |

(84.33 % + 9.70 % = 94.03 %; the residue is median non-additivity — the per-frame telescoping
identity is exact, KA2 at 100 % / 2.030e-15.)

---

## 3 Three things this measures that were previously assumed

1. **The port runs exactly 3 substeps per frame, on 1629 of 1629 frames**, chunks
   **25.000000 / 25.000000 / 0.0000038147**. The "3-or-4" of U-9160 is **3**, flat, on this
   arm. `frameMs = dt * 3000.0f = 50.0000038`, so `while (remMs > 0.0f)` takes
   25, 25 and then a **3.81e-06 ms** residue step.
2. **The residue substep is NOT cheap.** At `chunk = 3.81e-06` ms — one 6 500 000th of the
   frame — substep 2 still costs **-7.157898**, i.e. **26.03 %** of `T_post` and **86 %** of
   what the full 25 ms substep 0 costs. So the velocity cut inside `0x0046f6c0` is **not
   dt-proportional**; it is a per-CALL cut. Deleting the residue substep would remove
   ~26 % of the sink, not ~0 %.
3. **The contact fixup is not involved here.** `VehicleContactFixup` runs on a median of
   **0** frames in the window (2 fixups across all 29 frames) and its net `D_fixup` is
   **+0.000000** on every substep. The three anchored writes `0x0046f52c` / `0x0046f5ba` /
   `0x0046f5f3` that U-9178 lists as candidates **do not fire in the defect window**. (Their
   per-leg medians in §2.2's extended table — bounce +85.59, damp -69.98, slide -0.04 on
   substep 0 — are medians over the **2** frames that did contact, and are reported only to
   show the legs are wired, not as a window statistic.)

---

## 4 The original's own number for the same site

`WheelContactSolver.cpp:33-46` already records it, from `scenario_launch.py --fixup-probe` on
the anchored original (`verify/d2_bounce_20260930/orig_fp3.msd.fixupprobe.csv`, the §16.7 arm):

> the player's velocity is **bitwise unchanged across `0x0046f6c0` on 2945 of 2945 samples**,
> across `0x00469aa0` on 2945 of 2945, and across the substep entry `0x004709a0` on 2932 of
> 2932. The original writes the velocity in exactly **two** places per frame, A6a
> `0x00467650` and `VehicleContactFixup` `0x0046ef70`.

So at this `d` the original's `0x0046f6c0` contributes **0** and the port's contributes
**-23.194031 per frame**. That is the whole of U-9178's missing ~90 %, and it is **not** the
substep budget: the budget multiplies the defect by 1.5x (3 calls instead of 2), it does not
create it.

---

## 5 The decision, and the ONE rule that is REPLACED

**As registered, PREREG §3 rules 1 and 2 BOTH FAIL TO FIRE:**

```
largest single producer: D_sub[0] median -8.298279  share 30.17%   -> under 50%, rule 1 no
S2plus (substeps s >= 2): -7.157898               share 26.03%   -> under 50%, rule 2 no
=> PREREG 3 rule 4: NO CARRIER DOMINATES -> report the split and STOP
```

That verdict is **printed by the tool and reported here as the registered outcome.**

**Rule 1 is REPLACED by rule 1', and here is why.** PREREG §3's candidate list enumerated the
substep legs as `D_sub[0]`, `D_sub[1]`, `D_sub[2]` — i.e. it indexed **one code site by
substep ordinal**. A rule that ranks *code sites* cannot be evaluated on a list that splits a
single code site into three rows: `0x0046f6c0` is **one** site called three times, and no
amount of calls can push any one row above 50 % once the list divides it. That is a
**granularity defect in the rule, not a reading of the data** — and the per-site legs
`D_orient[s]`, `D_wheel[s]`, `D_fixup[s]` were **already defined in the same pre-registration**
(§2.1), so the per-site aggregate is a registered quantity, not one invented after the fact.

> **Rule 1' — sum each code site over the frame's substeps BEFORE taking the median.** It
> changes the grouping of registered quantities and nothing else: no new threshold, no new
> channel, no new window, the same 50 % bar.

Under rule 1': **`SolveWheelContacts` `0x0046f6c0` carries 84.33 %** of the port's `T_post`.

> ## CARRIER = **`0x0046f6c0`**, the wheel contact solver, writing velocity that the
> original's `0x0046f6c0` does not write (0 of 2945 on the original's own §16.7 arm).
>
> The **substep budget (U-9160) is a MULTIPLIER, not the carrier**: 3 calls where the
> original makes 2 (`0x00471106..0x00471141`, §6), so it inflates the defect by **1.5x** and
> accounts for substep 2's **26.03 %**. Removing it alone leaves **~58 %** of the sink
> standing. **Rule 2 is correctly NOT fired and U-9160 is correctly NOT the answer.**

Because the carrier is a specific producer, **PREREG §3 rule 3 governs STEP 2**: confirm it,
then fix that body at its RVA. The confirmation is owed first — the port's `0x0046f6c0` has
**three** velocity write sites (`WheelContactSolver.cpp:295`, `:303`, `:337`) and this step
measures their **sum**, not which one fires. That is `PREREG_STEP1B.md`.

**No source change beyond the probe. No knob, no clamp, no fitted constant. No tracker
mutation. No C-level moved.** AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched.

---

## 6 The original's substep loop, transcribed (and an RVA citation corrected)

Registered in `PREREG_STEP1.md` §1 before the run, repeated here because it is the standing
fact attempt 18's handoff got wrong. From `original/MASHED.exe.unpatched` via
`py -3.12 re/tools/disasm_fn.py`:

- **`0x00469ad4 mov ebx,2` is NOT the substep count.** It is `0x34` bytes inside
  `FUN_00469aa0`, the contact-history scan. `D2_REOPEN_2026-09-29.md` §19.2 reads it
  correctly ("`0x00469aa0`'s history-shift loop runs 2 iterations"); §20.9 then re-used the
  same RVA for "the frame contains two substeps". Both cannot be true of one `mov`.
- **`FUN_004709a0` contains no time loop.** Its `0x00470ab0 cmp ebp,2` is the **retry** cap
  (`inc ebp` at `0x00470b0a` on a reported fixup and at `0x00470bde` on a car-car hit), which
  the port already models at `VehiclePhysicsRun.cpp:932`. `FUN_004709a0` is a **16-vehicle
  loop** (`0x004709f0` .. `0x00470c53 inc eax / cmp eax,0x10 / jl`).
- **The substep loop is `0x00471106..0x00471141`, inside the dispatcher `FUN_00470c70`:**

```
00471106  mov  edi,[esp+0x10]     ; rem = the frame chunk, an INTEGER ms count
0047110a  test edi,edi
0047110c  jbe  0x471143           ; rem <= 0 -> no substep at all
00471110  cmp  edi,0x19           ; LOOP TOP, 25
00471113  mov  esi,edi
00471115  jbe  0x47111c
00471117  mov  esi,0x19           ; sub = min(rem, 25)
00471126  fild [esp+0x20]         ; (float)sub   <- INTEGER-sourced, so no residue step
00471137  call 0x4709a0           ; THE SUBSTEP
0047113f  sub  edi,esi            ; rem -= sub
00471141  jne  0x471110           ; while (rem != 0)
```

with `chunk = min(remaining, 0x32)` at `0x00470f50..0x00470f61`, also integer, and the
per-frame budget pinned to `0x32` = 50 (`VehiclePhysicsRun.cpp:739-746`'s existing decode of
`DAT_007f1000`). **`50 -> 25, 25` is exactly 2, by integer arithmetic.** The port's
`while (remMs > 0.0f)` over a float `50.0000038` is what produces the 3.81e-06 ms third step.

`VehiclePhysicsRun.cpp:119`'s `kMaxSubstep = 25; // 0x19 (FUN_004709a0 inner chunk)`
misattributes the constant's home. The constant is right; its RVAs are `0x00471110` and
`0x00471117`.

This correction does not disturb attempt 18's live `4666/2333 = 2.0000` — that counts hits on
`0x004709a0` and is independent of which `mov` sets the bound.

---

## 7 Artefacts

```
verify/d2_sink_20261002/
  PREREG_STEP1.md                   committed 3837cfe5, UNRUN
  p_armed/{sink.log, motion_diag.log, PROVENANCE.txt}
  p_ctrl/{motion_diag.log, PROVENANCE.txt}          CH control, no probe armed
  split.csv                         per-frame split, d = -1..1627
mashedmod/src/mashed_re/Vehicle/D2SinkProbe.{h,cpp}  the probe, default-OFF, exe-only
re/tools/statediff/a19_split.py                      the reducer
```
