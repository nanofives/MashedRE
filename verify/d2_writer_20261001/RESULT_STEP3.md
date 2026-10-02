# RESULT — D2 attempt 15, STEP 3: the LAUNCH closes in the DEFAULT build. The three metrics still FAIL.

Measured against [`PREREG_STEP3.md`](PREREG_STEP3.md), committed at `33a477cf` before the
first measurement run. **No gate was amended.** Build HEAD `33a477cf`, `Build OK`,
`rva-lint allowlisted=122 NEW=0`, **no launch knob of any kind in the scored runs** —
`MASHED_D2_BOOSTHOLD` no longer exists and `MASHED_NO_LAUNCH_REV` appears only in D-0.

Runs `r1` `r2` `r3` (scored) and `d0` (control). `participants=1` confirmed in
`mashed_re.log` (`MATCH-SEED rule=0 participants=1 teams=0 seed=6 engine=1`).

---

## 0 One VOID run, disclosed

The **first** attempt at `r1` was **VOID under gate W**: the exe exited `0xC0000005` with no
`motion_diag.log`. Cause, found by running the registered control arm
(`MASHED_NO_LAUNCH_REV=1`, which booted and produced 1624 lines): the standalone's lazy
physics init sat **inside** the `VehiclePhysics_Enabled()` block that the countdown branch
`return`s before reaching, so for the whole pre-race `g_vehicleArrayBase` was `nullptr` and
the first charge tick dereferenced `null + 0xbf4`.

Fixed by **hoisting the init above the countdown**, which is also the original's ordering:
its race state machine's case 1 ("start of race", `FUN_004111c0`) runs the per-car record
init (`FUN_0046b1c0` / `FUN_0046b540`) and only then advances toward the countdown, which is
state 5. The guard is in the standalone WIRING, not in either ported body — both RVA bodies
are untouched by it.

STEP 3 then restarted from run 1, as registered.

---

## 1 The gates

| gate | required | measured | |
|---|---|---|---|
| **W** wiring | the witness line with `bf8` ending at 2 | `launchrev release car=0 bf4 3000 -> 3000  bf8 0 -> 2` on **r1, r2, r3**; absent in `d0` | **PASS** |
| **D-0** control | `d0` identical to attempt 14's `s1` on every shared line | **0 differing of 1625 shared lines** | **PASS** |
| **a1** lag | best-fit integer lag `L = 0` over `d`=16..95, n=80 | **L = 0** at 0.19 % (L=14: 49.03, L=15: 51.27, L=16: 53.43) | **PASS** |
| **a2** engagement | port `+0xb14` engages at `d` = 15 | **`d` = 15** | **PASS** |
| **b** recovery | H1 iff `>=50 %` AND median `>=900`; H2 iff `<10 %` | **243/400 = 60.8 %**, median **132.8** | **INCONCLUSIVE** |
| **c** metrics | 3 metrics in bounds, 3 runs | **0 of 3**, see §3 | **FAIL** |

All three scored runs are identical to every printed digit.

> ### GATE a PASSES IN THE DEFAULT BUILD. The charge the port computes for itself is
> **3000** — the original's own saturated value — so the launch is now reproduced by the
> ported law rather than by a fitted constant.

---

## 2 Launch and recovery

| arm | peak | at `d` | trough | at `d` | post-trough `>=100` | % | median | max | `b14` at |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| **ORIGINAL** `orig_solo3` | 1832.40 | **95** | 85.45 | 101 | **398 / 400** | **99.5 %** | **1333.9** | 2478.3 | `d`=15 |
| **PORT default, r1/r2/r3** | 1835.50 | **95** | 83.43 | 106 | **243 / 400** | **60.8 %** | **132.8** | 737.9 | **`d`=15** |
| port D-0 control (`d0`) | 1856.57 | 80 | 91.81 | 88 | 32 / 400 | 8.0 % | 32.3 | 260.9 | `d`=0 |

The peak lands on the **original's own frame**, 1835.50 against 1832.40 — **0.17 %** apart.

**These are attempt 14's knob-ON numbers to every digit** (peak 1835.50 at `d`=95, trough
83.43 at `d`=106, 243/400, median 132.8, max 737.9, L=0). That is the strongest available
evidence that attempt 14's fitted trigger was a faithful stand-in for the real one: the
ported law, driven only by the port's own accel input through the original's own constants,
reproduces it exactly. It also means **the residual recovery gap is not an artefact of the
knob** — it survives the real fix.

Gate b's two clauses split exactly as they did in attempt 14: the fraction clause passes,
the median clause fails by a wide margin (132.8 against 900). **INCONCLUSIVE, and no third
branch was added.**

---

## 3 The three scored metrics — 3 of 3 runs identical, 0 of 3 in bounds

Bounds unchanged from `d81a8df6`. Every row carries n, median speed and median frame index.

| metric | bound | PORT | n | median speed | median frame (`d`) | verdict |
|---|---|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1983** | 19 | 1665.05 | **86** (`d`=85) | **FAIL**, over by **0.00195** (+3.0 % vs the 0.19245 mean) |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — | **FAIL** |
| driving-median | 1904.70 .. 1982.44 | **1019.77** | 76 | 1019.77 | **80** (`d`=79) | **FAIL**, **−47.5 %** vs the 1943.57 mean |

Reference, same reducers, same run: ORIG slip 1500-2000 **0.1937** (n=314, median speed
1790.00, median frame 1606, `d`=720); ORIG slip 2000-2600 **0.2498** (n=540, 2231.30, frame
1795, `d`=909); ORIG driving-median **1937.89** (n=1154, frame 1710, `d`=824).

> **§26.10's median-frame guard fires on every row.** The original populates these metrics
> at median `d` 720 / 909 / 824; the port at 85 / — / 79. The two sides are being scored at
> entirely different moments of the run, so the *magnitudes* of the gaps are not readable as
> physics errors. The bounds are still scored, because the bounds are what D2 closes
> against — and they are not met.

Against attempt 14's default (knob-OFF) arm — slip 1500-2000 **0.2033** (n=20, `d`=70),
driving-median **1355.66** (n=54, `d`=53) — the default build has moved to attempt 14's
diagnostic numbers: slip 1500-2000's error halves (+5.6 % → +3.0 %) and driving-median moves
the **wrong** way (1355.66 → 1019.77) while its n rises 54 → 76. That is §28.5's property of
the statistic, now in the default build: a partially-recovering car adds frames just above
the 500 floor at low speed, which pulls the median down. **[UNCERTAIN]** whether it recovers
on an arm where the recovery completes.

---

## 4 Verdict

**D2's LAUNCH is closed.** Gate a passes in the default build with no knob, the charge is
derived rather than fitted, and the peak is on the original's frame to 0.17 %.

**D2 does NOT close.** Gate c fails 3 of 3 and gate b is INCONCLUSIVE. A second defect
survives downstream of the launch, and it is now measurable for the first time: with the
launch matched, the two arms share the whole approach and much of `d`[95,250], which is the
overlapping state every instrument in attempts 12-14 lacked.

Next, per `PREREG_STEP3.md` §3 and pre-committed before this number was seen: take
**`+0xb0c`** first, name the first diverging term after the trough **frame-aligned, not
speed-banded**, and test it on the running original before any fix.
