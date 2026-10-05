# RESULT — U-9188: which `a.yaw` writer carries the bridge/record disagreement

**RAN 2026-10-05.** Pre-registration `PREREG_YAWW.md`, committed unrun at `49fc1ca0` and amended
unrun at `5f6f49c4`. Nothing below was decided after the fact; every gate's PASS/FAIL clause is
quoted from the file as committed.

**Headline: U-9188's central conclusion is RETRACTED.** The "0.5214 / 1.1430 / 0.4300 deg
bridge-vs-record disagreement" is the AI car's **per-frame heading change**, captured by a
non-atomic `ReadProcessMemory` poll. Measured in-process at known program points, the port's
published heading and the record's forward row agree to **0.000000 deg, max 2e-6**, on every slot
on every frame the car is stepped. **H2 (poll artifact) — as registered.**

A real port-only bridge defect **does** exist, but it is a different one, it was not what U-9188
measured, and it **cannot** be the carrier of D3 criterion (b).

---

## 1. Arm

```
py -3.12 re/tools/sa_capture.py verify/d3_yaww_20261005/y1 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl "MASHED_TITLE=U-9188 yaww r1" \
    MASHED_AI_STEPDUMP=verify/d3_yaww_20261005/y1.csv \
    MASHED_AI_YAWW=verify/d3_yaww_20261005/y1.yaww.csv
```

The standing (b) recipe (`re/NEXT_SESSION.md:449-452`) plus the one new default-OFF knob.
`faithful_nav=1` and `phys=1` are **witnessed in the dump**, not assumed. 2520 frames at the last
summary flush, 2585 in the CSV. `MASHED_REAL_PHYSICS` is not set and defaults ON
(`VehiclePhysicsRun.cpp:225-236`).

**Instrumentation is inert.** Knob-OFF control `ctl.csv` against knob-ON `y1.csv`: **6480 of 6480
common rows byte-identical, 0 differing.** The row-count difference (6480 vs 7660) is wall-clock
only — the knob-ON run booted slower (`y1_t08.png` is 4,386 bytes, near-black, against `ctl_t08.png`
at 283,783) so it banked fewer AI steps inside the same 60 s window. Knob-OFF wrote no `yaww` file
of any kind.

**One malformed row of 10,338** (`frame 2584, slot 1`) — the last line, truncated mid-`fprintf` when
the harness killed the process by PID. Excluded from every statistic. Stated rather than silently
dropped.

---

## 2. Leg A — all nine `a.yaw` writers, per slot

| site | slot0 | slot1 | slot2 | slot3 |
|---|---|---|---|---|
| `w2690_grid` | 0 | **1** | **1** | **1** |
| `w3283_spin` | 0 | 0 | **57** | 0 |
| `r3285_reset` | 0 | 0 | **57** | 0 |
| `w3304_respawn` | 0 | 0 | 0 | 0 |
| `r3307_reset` | 0 | 0 | 0 | 0 |
| `w3334_sync` | 0 | **2520** | **1232** | **2520** |
| `w3351_offmesh` | 0 | 0 | 0 | 0 |
| `r3355_reset` | 0 | 0 | 0 | 0 |
| `w3393_v2spin` | 0 | **0** | **0** | **0** |
| `w3423_v2ribbon` | 0 | **0** | **0** | **0** |
| `w3683_oblimit` | 0 | **0** | **0** | **0** |
| `w3717_oboffmesh` | 0 | **0** | **0** | **0** |

- **A-COV — PASS.** `w3334_sync > 0` on all three AI slots. The counter block executed; leg A is not
  VOID.
- **A-REACH — PASS.** `w3393 = w3423 = w3683 = w3717 = 0`, with `faithful_nav=1` and `phys=1` in the
  same dump. **PREREG Correction 2 is confirmed by measurement.** Both of U-9188's conditional
  remedies ("if `:3423` fires…", "if `:3717` fires…") are **dead**, as predicted, and the registered
  remedy list is exhausted without finding a carrier.
- **A-SPIN — PASS on its literal clause, interpretation superseded.** `w3283 = 57` on slot 2 only,
  with `r3285 = 57`, so the reset was reached on **every** count. But the clause was written on the
  assumption that reaching `:3285` resyncs the record. **PREREG Correction 4 shows it does not**, and
  §4 below shows this exact site is the real defect. The gate passes; what it was taken to prove
  does not hold.
- **`w2690_grid` fires once per AI slot.** That is the writer **no prior enumeration contains**
  (PREREG Correction 5), now witnessed. `w3304_respawn` and `w3351_offmesh` never fire on this arm.

---

## 3. Leg B — in-process angle, and the retraction

Degrees. `pub_rec` is `sa_headwatch.py`'s quantity with no poll. Slot 0 is **construction-zero**, not
a floor.

| slot | n | `pub_rec` med | `pub_rec` max | `post_rec` med | `post_rec` max | `pub_post` med | `pub_post` max |
|---|---|---|---|---|---|---|---|
| 0 | 2585 | 0.000000 | 0.000000 | — | — | — | — |
| 1 | 2583 | **0.000000** | 0.000002 | 0.000000 | 0.000002 | **0.491309** | **2.556407** |
| 2 | 2583 | **66.828354** | 178.580169 | 0.000000 | 0.000002 | **0.491309** | **2.556403** |
| 3 | 2583 | **0.000000** | 0.000002 | 0.000000 | 0.000002 | **0.421121** | **2.556403** |

Split by whether the AI loop stepped the car that frame:

| slot | frames | `pub_rec` med | `pub_rec` max | `> 1 deg` |
|---|---|---|---|---|
| 2, **stepped** | 1231 | **0.000000** | 0.000002 | 0 |
| 2, **not stepped** | 1352 | **66.828354** | 178.580169 | **1351** |
| 1, stepped | 2583 | 0.000000 | 0.000002 | 0 |
| 3, stepped | 2583 | 0.000000 | 0.000002 | 0 |

**B-INPROC — INCONCLUSIVE as a single verdict, by the registered rule.** Slots 1 and 3 are
`0.000000`, below the 0.05 H2 clause. Slot 2 is 66.83, neither inside `[0.25x, 4x]` of its U-9188
value of 1.1430 (i.e. `0.286..4.572`) nor below 0.05. PREREG §3b pre-declared exactly this split
outcome as INCONCLUSIVE "so a split result cannot be read as whichever answer is convenient", and it
is reported as such. The per-slot decomposition below is unambiguous on its own terms and is not a
re-reading of that gate.

### U-9188 was measuring the per-frame turn

| | car 1 | car 2 | car 3 |
|---|---|---|---|
| U-9188's claimed bridge-vs-record median | 0.5214 | 1.1430 | 0.4300 |
| this run's `pub_post` — the **per-frame heading change** | **0.491309** | 0.491309 | **0.421121** |
| this run's `pub_rec` — the actual bridge-vs-record angle, stepped frames | **0.000000** | 0.000000 | **0.000000** |

Cars 1 and 3 match the per-frame turn to **5.7 %** and **2.1 %**. The decisive corroboration is the
cap: U-9188 reports that cars 1 and 3 "never exceed **2.5564**" and reads that as "the signature of a
quantised rate limit". This run's `pub_post` maxima are **2.556407** and **2.556403**. It is a
quantised rate limit — the port's **steering rate limit on the per-frame turn**. U-9188 found the
right cap on the wrong quantity.

`sa_headwatch.py` reads `g_aib.fwd[v]` and the record non-atomically at `--hz 30` against a faster
game. A poll landing anywhere across the `:3331` (record written) → `:3334` (`a.yaw` synced)
boundary banks exactly one frame of turning. That is the artifact, with its magnitude and its cap
both matched.

**Why U-9188's player floor did not catch it, as pre-registered:** `g_aib.fwd[0]` is
`cos/sin(car_yaw_)` and the player's record forward is `cos/sin(io.yaw)` with `car_yaw_ = io.yaw` at
`:3003`, which is **above** the snapshot. The player agrees by construction and has no
`:3331..:3334` window to straddle, so its hard `0.0000` was guaranteed and was never evidence that
the instrument could resolve a small angle.

**And car 2's 1.1430 is two regimes averaged.** U-9188 reports 327 of 933 samples above 10 deg and 16
above 90, max **176.65**; this run's stale regime has max **178.58**. The 933 samples mix a
poll-artifact population near 0.5 with a frozen-record population near 66.8, so the median lands in
the low group while the tail comes from the high one. Same family as memories
`rate-stats-per-sample-not-totals` and `band-on-speed-compares-different-moments`: one statistic
computed across a regime change.

---

## 4. The real port-only bridge defect, which is a different one

Slot 2 is stepped on frames 0..1231 with `pub_rec` exactly `0.000000` throughout. At frame **1232**
it stops being stepped — eliminated under `MASHED_ROUND=1`, the `round_mode_ && !race_[ci+1].alive`
branch that `continue`s **before** the sync. Over frames 1232..~1289 the spin scaffold at `:3283`
advances `a.yaw` across **58 distinct values**, from `-1.526086` to `+9.873910` rad, which is the 57
`w3283_spin` hits. It then freezes, and `pub_rec` is pinned at **66.828354 deg for the remaining
~1300 frames**.

The mechanism is **PREREG Correction 4**. `VehiclePhysics_ResetOrientation`
(`VehiclePhysicsRun.cpp:518-523`) writes `g_bodyBasis`, `g_bodyBasisOk` and `g_bodyBasisReseed` and
**never touches the record**, so `:3285` does not resync `+0x9d4`/`+0x9dc` — U-9188's and PREREG
Correction 1's word "resync" is about the basis. Normally the next `StepCar` papers the gap over by
rewriting the forward row from `io.yaw`. For a car that is **never stepped again**, nothing ever
does, and the bridge publishes a heading 66.8 deg off the record for the rest of the race.

Two secondary observations, both recorded and neither acted on:

- **`a.yaw` is never wrapped.** `+9.873910` rad is outside `[-pi, pi]`. `cos/sin` absorb it so the
  published vector is unaffected, which is why this is benign today and why it is worth knowing
  before anyone compares `a.yaw` to a wrapped heading.
- **`post_rec` is `0.000000` with max `2e-6` on all three AI slots.** The sync at `:3334` is exact.
  There is no drift term anywhere in the bridge.

**This defect cannot be the carrier of D3 (b).** `ai_ctrl_window.py:25-29` scores the first **220**
calls where `c4 != 0`; the divergence begins at frame **1232**, on a car that has been
**eliminated**. It is outside the scored window and off a non-racing car.

---

## 5. What this does to the gate, and what is owed

- **U-9188's answer to U-9185 item (b) is RETRACTED.** "The body-heading residual is a PORT-ONLY
  BRIDGE defect, not a physics one" does not survive an in-process measurement of its own quantity.
- **D3 criterion (b) is reopened at the physics basis**, which the 2026-10-04 session recorded as
  **NOT REACHED**. The heading share of U-9185's matched-position residual (0.9796 / 0.9085 / 0.6506)
  is **not** explained by the bridge and has to be re-attributed.
- **No C-level moves.** Nothing here reads or changes a function at an RVA. `TrackRenderer.cpp`'s AI
  loop is port-only scaffolding with no original counterpart, so no `diff-original` leg is available
  or owed.
- **No D2 WATCH row.** No D2 code was read or changed.
- **Two of my own calls were wrong and are recorded as such.** PREREG §1 registered frame order
  (candidate 5) as the leading hypothesis; it was refuted by static reading before running and is
  corrected in §3b. PREREG Correction 1 reused U-9188's misreading of `ResetOrientation` and is
  corrected by Correction 4. The amendment was committed unrun at `5f6f49c4`, before any
  measurement, so neither correction is hindsight.
- **Owed:** a fix for the eliminated-while-spinning stale heading is **not** registered here and
  should not be taken on this session's evidence alone — it is cosmetic within the scored window and
  the cheap repair (have `:3285` write the record, or skip publishing for `alive == 0` slots) changes
  what `Ai_Standalone_Tick` reads and needs its own pre-registration.
- **Owed:** `sa_headwatch.py` measures a quantity its sampling cannot resolve. It should either read
  both operands under a frame marker or be retired for this question. Any other result taken from it
  is suspect by the same mechanism.
