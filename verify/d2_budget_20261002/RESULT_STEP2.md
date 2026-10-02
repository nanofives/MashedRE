# RESULT — D2 attempt 18, STEP 2: the clamp RUNS and is CONSISTENT at its own phase, and the
# `+0x9e4`-vs-snapshot instrument understates the original by 51x

Executes `PREREG_STEP2.md` (committed `3c3e72f4`, unrun). One live capture on the running
anchored original, muted, one spawned PID tracked and killed, existing probe only — **no
source change, `NEW = 0` by construction**. Instrument `re/tools/statediff/a18_gate.py`.

Capture: `verify/d2_budget_20261002/orig_lb18.msd` (2334 frames, distinct payloads 1516) and
`orig_lb18.msd.latbracket.csv` (9332 rows). Release from **this** capture's own `+0xbf8`
marker: **frame 883** (not `orig_sl1.msd`'s 890 — per attempt 17 §2, the marker is
capture-specific and is re-derived, never inherited).

**One gate FAILED and is reported as a failure, not amended.** One section of
`RESULT_STEP1B.md` is WITHDRAWN.

---

## 1 The gates

| gate | asked | result | verdict |
|---|---|---|---|
| **CV** | `err = null`, `a6a >= 1200`, `a6b >= 1200`, `sub >= 2400`, pattern `0,2,1,1` on >= 99 % | `{"armed":true,"a6a":2333,"a6b":2333,"sub":4666,"skipped":0,"err":null}`, orphans **0**, pattern **2333/2333 = 100.00 %** | **PASS** |
| **KA-B** | probe `+0x9e4` and `&#124;v&#124;` at A6b entry == the same frame's `.msd` values, 4 ulps, >= 99 % | within budget on **37.76 %**, worst **1.42e7 ulps** | **FAIL** |
| **EV** | `d = 222..250`, `n >= 25`, median speed within 15 % of `orig_sl1.msd`'s 820.5 | **n = 29**, median speed at A6a entry **850.3** (3.6 % above) | **PASS** |

### 1.1 U-9160's ORIGINAL side, measured live

Gate CV's pattern check is simultaneously the substep count:

> **SUBSTEPS PER FRAME ON THE ORIGINAL = `4666 / 2333` = exactly `2.0000`**, with the
> per-frame site pattern `0,2,1,1` (A6a, A6b, substep, substep) on **2333 of 2333** frames,
> **0** malformed, **0** orphans, `skipped = 0`, `err = null`.

That confirms §14.6's `0x00469ad4 mov ebx,2` reading on the running binary and gives U-9160
its original-side half with a live witness. The port's 3-or-4
(`VehiclePhysicsRun.cpp:915`'s `while (remMs > 0.0f && guard++ < 64)` over
`frameMs = dt*3000.0f = 50.000004`) is unchanged and still a real fidelity defect; §26's
measurement that the residue pass is inert (contacts 0/1280, velocity writes 0/1280) is not
disturbed.

### 1.2 Why KA-B failed, and what it does and does not touch

KA-B paired the probe's **A6a-call ordinal** with the `.msd`'s **render-tick frame index**
as if they were the same counter. They are not the same counter, and the gate as written had
no provision for an offset — so it scored 37.76 % with a worst case of 1.42e7 ulps, which is
a pairing error, not a physical disagreement. **Reported as a FAILURE. Not re-thresholded,
not re-paired, not replaced.**

What it does **not** touch: the registered reading `G4` is read **directly off the probe** at
A6a entry and is **frame-pairing-independent**, because

> **`+0x9e0` at A6a entry has exactly ONE distinct value — `4` — across all 2333 frames of
> the capture.**

No offset of any size can change `G4`. The windowed figures below are therefore reported,
with the pairing failure stated.

---

## 2 The registered reading: BRANCH **OPEN**

`G4 = (+0x9e0 == 0x40800000)` as an exact dword compare, read at A6a entry — bit-identical
to what the gate at `0x00468761` reads, because A6a reads `+0x9e0` twice and **writes it zero
times** over all 1243 instructions of `0x00467650..0x0046897b`.

| window | n | med speed @A6a entry | `+0x9e0` distinct | **`G4` TRUE** | `d&#124;v&#124;` across [A6a entry -> A6b entry] (A6a ALONE) |
|---|---:|---:|---|---:|---:|
| `d` 200..222 | 23 | 341.1 | `[4]` | **23/23 = 100.00 %** | **+19.36351** |
| **`d` 222..250** | **29** | **850.3** | `[4]` | **29/29 = 100.00 %** | **+32.01650** |
| `d` 250..260 | 11 | 1356.5 | `[4]` | **11/11 = 100.00 %** | **+29.74997** |

**BRANCH OPEN fires** (bar: `G4` true on >= 80 %). Because KA-B failed, the tool prints
*"the reading does NOT execute"* as registered; the reading is reported above with §1.2's
independence argument attached, and the branch's *consequence* is then tested by the §3
diagnostic rather than asserted.

> **`RESULT_STEP1B.md` §2.2's reading — "grip-clamp #6's velocity stores appear NOT to
> execute at matched `d`" — is WITHDRAWN.** The gate is open on 100 % of frames in every
> window and on all 2333 frames of the capture. A5's zero at `0x0046ddd1` is real, but its
> per-wheel rebuild (`ForceIntegrator.cpp:59` on the port side) restores `4.0` before A6a on
> every frame of this arm.

---

## 3 The clamp at its OWN phase — CONSISTENT with its transcription, and the standing
## instrument is 51x off

**Post-hoc diagnostic, chosen after the branch fired, and labelled as such — not a
pre-registered gate.**

A6b entry is the first sample after A6a returns, and `+0x9e4` is written **only before** the
clamp (`0x004686cc`; the only other writers are A6a's entry `fstp` `0x00467673` and the
spawn-init `0x0046bc36`, both earlier — and the folded-base sweep over 622 511 instructions
returns 4 reads and **0** writes, with its `+0xbf8` known answer PASSING). A6a writes no
forward axis; A5 wrote it before A6a. Therefore

```
R_c  = |v| @A6b_entry  /  (+0x9e4) @A6b_entry
```

is **exactly** grip-clamp #6's magnitude ratio, with the clamp's own forward axis.

| window | n | med speed @A6b | `1-R_c` | **`1-R_c^2`** | `R_c>1` | slip @A6a | slip @A6b (post-clamp) | demanded at `k >= 0.1249` | **measured / demanded** |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `d` 200..222 | 23 | 362.3 | 1.7377e-02 | **3.4453e-02** | 0/23 | 0.288343 | 0.253707 | 1.5075e-02 | **2.29** |
| **`d` 222..250** | **29** | **879.9** | **3.1220e-04** | **6.2431e-04** | **2/29** | **0.044704** | **0.040491** | **3.8398e-04** | **1.63** |
| `d` 250..260 | 11 | 1380.5 | 1.2364e-03 | **2.4712e-03** | 0/11 | 0.111984 | 0.076744 | 1.3794e-03 | **1.79** |

> **The original's grip-clamp #6 costs MORE than the structural minimum its own arithmetic
> demands, in every window** (ratio 1.63 to 2.29, measured against the floor
> `s^2(2k-k^2)` at `k >= 0.1249`, with `s` taken as the measured POST-clamp slip, which is a
> lower bound on the pre-clamp slip). **Clamp #6 on the original behaves exactly as
> transcribed. There is no contradiction to explain.**

### 3.1 The consequence: the `+0x9e4`-vs-snapshot split is phase-biased, 51x on the original

The same capture, reduced at the render tick by `a18_sink.py`, gives
`1 - R^2 = 1.2194e-05` at `d = 222..250`. At the clamp's own phase it is **6.2431e-04**.

> **The render-tick split UNDERSTATES the original's clamp cost by 51x** (6.2431e-04 /
> 1.2194e-05), n = 29, median speed 850.3/879.9, `d = 222..250`. Its `R > 1` count is
> **14 of 29** at the snapshot against **2 of 29** at the clamp's phase — the snapshot
> measurement is dominated by whatever happens between A6b entry and the render tick, where
> `|v|` recovers most of the clamp's cut. **[UNCERTAIN] what restores it**: A6b writes no
> velocity (§26.4), A4's tail parked damp is gated off (`+0x9f0 == 0` on 29/29), the contact
> fixup is gated off (`+0x9ec == 0` on 29/29), and §22.2 measured the original's velocity
> bitwise unchanged across all three substep members (2945/2945, 2945/2945, 2932/2932). One
> of those four statements does not hold at this `d`, and which one is not measured here.

**This bears directly on §23.2.** Its headline — *"the port loses a median 14.7 % of its
linear speed per frame between `0x004686cc` and the snapshot; the original loses 0.0002 %"* —
is built on this split. On the original the real per-frame clamp cost at matched `d` is
`1 - R_c = 3.12e-04` = **0.031 %**, not 0.0002 %. And `RESULT_STEP1B.md` §4 showed the same
split **OVERSTATES** the port by **10.3x** against the port's own measured `l_60`. So the
split is biased in **opposite directions on the two sides**, which is exactly the shape that
manufactures a large false cross-side factor.

> **The 6 072x factor of `RESULT_STEP1B.md` §2 is therefore an INSTRUMENT ARTEFACT in
> unknown part.** `T_post` remains the term the budget named (STEP 1, and that naming uses
> only the snapshot on both sides symmetrically), but its decomposition and its magnitude
> **cannot** be read off the render-tick split. The correct-phase instrument now exists on
> the original side (A6b entry, this capture). **The port needs the same phase before any
> cross-side clamp number is quoted again.**

### 3.2 Original-side determinism control, unregistered but reported

`orig_lb18.msd` (release 883, this run, with `--lat-bracket` armed) reproduces
`orig_sl1.msd` (release 890, attempt 17, with `--slide-probe` armed) **to every printed digit
at matched `d`**: `d = 222..250` gives `T_drive +33.20455`, `T_rest -3.95911`,
`T_post -0.00464`, `T_W1 +27.78004`, `dS +27.74798`, `s'(snapshot) 0.044704`,
`1-R^2 1.2194e-05`, median speed 790.9 — identical on both captures. Two different boots,
two different probes, two different release frames, the same numbers. The original arm is
deterministic at matched `d` and the probe is behaviourally inert.

---

## 4 Fix: NONE AUTHORED

`PREREG_STEP2.md` §5 registered, before the run, that a fix would not be authored even on
branch CLOSED without a port-side gate measurement. Branch OPEN fired instead, and §3 then
removed the premise a fix would have rested on: **clamp #6 is consistent on the original at
its own phase, so there is nothing in it to fix**, and the cross-side magnitude that would
have justified touching its inputs is shown to be instrument-biased.

No knob, no clamp, no fitted constant, no `mashedmod/src` change. **`NEW = 0` by
construction for the whole attempt** (`git diff 6e512717..HEAD -- mashedmod/src` is empty).
No C-level moved. AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched.

## 5 Artefacts

```
verify/d2_budget_20261002/
  orig_lb18.msd                      2334 frames, live original + --lat-bracket
  orig_lb18.msd.latbracket.csv       9332 rows (A6a 2333, A6b 2333, substep 4666)
  orig_lb18.msd.provenance.json
  gate.csv                           per-frame d 180..280, G4 and both phases
re/tools/statediff/a18_gate.py       the reducer (CV / KA-B / EV / G4 / the clamp-phase diag)
```
