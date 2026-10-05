# PRE-REGISTRATION — U-9191: which omega arm, and is car 1's 0.9866 deg GENERATED or ACCUMULATED?

**Committed UNRUN. 2026-10-05.** D3 criterion (b), continuing U-9191.

Version anchor verified before arming: `original/MASHED.exe.unpatched` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, `original/launch.exe`
`01506209E42C79A4E5BEDB43DCE9FB953F0CA628B26AF9FCD49EE2522DF78DA2`. Both equal `CLAUDE.md`'s anchor.

## 0. What is already established, and is NOT re-derived here

- Car 1's AI body heading diverges from the original's by **0.9866 deg** at matched position
  (`verify/d3_leg3_20261005/RESULT_LEG3.md` G-DIRECT, n = 105), confirmed by two derivations 0.7 %
  apart, with staleness, command and speed each excluded
  (`verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md`).
- **`+0x9bc`/`+0x9c0`/`+0x9c4` is the TORQUE triple; the angular velocity is `+0x144`/`+0x148`/
  `+0x14c`** (`verify/d3_omega_20261005/RESULT_RATEFIELD.md`, from
  `re/analysis/vehicle_promote_c2/0046e9e0.md:27-28`, committed 2026-05-12). **`+0x9c0` is not
  re-probed as a rate anywhere in this leg.**
- `FUN_0046e9e0` forks its omega source on **`ESI[4]` = record `+0x10`**: non-zero keeps the torque
  seed, zero rebuilds omega from the steer/throttle chain
  (`re/analysis/vehicle_dynamics/0046e9e0.md`, "Grounded-vehicle force path (`unaff_ESI[4] == 0`)";
  `BodyOrientationIntegrate.cpp:31`).

## 1. Field naming, grepped before being used

Per the standing rule, every offset below was checked against `re/analysis/**/0046e9e0.md` **before**
this file was written, and the plate beats any tracker row:

| offset | name | citation |
|---|---|---|
| `+0x144`/`+0x148`/`+0x14c` | angular velocity XYZ (`ESI[0x51..0x53]`) | `vehicle_promote_c2/0046e9e0.md:28`, `vehicle_dynamics/0046e9e0.md` "Angular velocity accumulation" |
| `+0x9bc`/`+0x9c0`/`+0x9c4` | **torque** XYZ (`ESI[0x26f..0x271]`) | `vehicle_promote_c2/0046e9e0.md:27` |
| `+0x10` | the omega-arm gate (`ESI[4]`); **no plate assigns it a semantic name** | `vehicle_dynamics/0046e9e0.md`; same gate ported at `Integrate2.cpp:527` and `PhysicsChainHooks.cpp:2139`, both commented only as "state +0x10" |
| `+0x9d4`/`+0x9dc` | body forward X / Z | `verify/d3_leg3_20261005/PREREG_LEG3.md` §2 |
| `+0x9e4` | linear speed magnitude (`ESI[0x279]`) | `BodyOrientationIntegrate.cpp:242` |

**`+0x10` is reported as a raw `u32` / `i32` / `f32` triple and is NOT given a semantic name in this
leg.** It is used only as "zero vs non-zero", which is exactly how the original branches on it.

## 2. STEP 1 — G-ARM. Which arm does each side take? Prerequisite to everything else.

Comparing the `+0x144..+0x14c` accumulator without knowing the arm would repeat the torque-vs-rate
error of 2026-10-05, so G-ARM runs **first** and gates step 3's accumulator leg.

**Port side — static, no measurement.** The claim to be checked by grep, not by a run:
`VehiclePhysicsRun.cpp:1001` calls `BodyOrient_OmegaFromSteer` **unconditionally**, and
`BodyOrient_OmegaFromAngVel` has **zero call sites** in the whole tree. If both hold, the port takes
the `+0x10 == 0` (steer) arm on **every** substep regardless of what `+0x10` contains. Recorded as a
static fact with line cites; if either grep comes back otherwise, the static claim is withdrawn and
G-ARM is re-scoped before any number is read.

**Original side — offline on a committed capture.** `verify/d3_elim_20261003/o_t1.msd` (car 1, the
same file and the same `read_msd` parse leg 3 scored), field `+0x10`, every parsed frame.

**G-ARM gate.** Let `N` be the number of frames `read_msd` yields from `o_t1.msd` and `Z` the number
with `+0x10 == 0`. RESULT_LEG3 reports `N = 3623` for this file and parse; the threshold is stated
against the **measured** `N`, and both `Z` and `N` are printed.

- **PASS — arms agree, step 3's accumulator leg is well-posed:** `Z >= 0.950 * N`, i.e. `Z >= 3442`
  of `N = 3623` frames if `N` is 3623 as expected.
- **FAIL — arms differ, and that is itself the finding:** `N - Z > 0.050 * N`, i.e. `N - Z > 181` of
  `N = 3623`. Then the port hardcodes an arm the original does not always take, **G-ACC is NOT run**,
  and the leg reports the arm divergence as the result.
- Reported either way: the distinct-value histogram of `+0x10` with counts, whether it is constant
  over the frames, and the raw `u32`/`i32`/`f32` of every distinct value.

## 3. STEP 2 — the instrument, and G-INERT

Four columns **appended** to the existing default-OFF `AiStepDump` (`TrackRenderer.cpp:3972`), so no
existing column moves and `ai_posmatch.py` / `ai_headattrib.py` / `ai_yawrate.py` are unaffected:

| column | offset | type | what |
|---|---|---|---|
| `rec_144`, `rec_148`, `rec_14c` | `+0x144`, `+0x148`, `+0x14c` | f32 | angular velocity x, y, z |
| `rec_10` | `+0x10` | i32 | the arm gate, so the port's own value is on record |

Raw reads only, through the `VehiclePhysics_RecordF32` the dump already uses for `+0x9e4`, plus an
i32 read for `rec_10`. No derived value, no new write, no new global.

**Original side needs no new run.** `o_t1.msd` carries `+0x144`/`+0x148`/`+0x14c` and `+0x10` at
those offsets inside its `0xd04`-byte record.

**G-INERT — a VOID condition, not a caveat.** The new capture's pre-existing columns must be
**identical on 0 mismatches out of the M rows keyed `(frame, seq, v)` present in both files**, across
the **42 pre-existing columns** of `verify/d3_leg3_20261005/L1.csv`, compared as strings. Threshold:
mismatching cells **= 0 of (42 x M)**; both `M` and the mismatch count are printed. Any non-zero
count voids the whole leg until explained.

**Port capture recipe** (`re/NEXT_SESSION.md` "Recipes", unchanged):

```
py -3.12 re/tools/sa_capture.py verify/d3_arm_20261005/A1 8,30,60 MASHED_MUTE=1 \
    MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="d3 arm A1" \
    MASHED_AI_STEPDUMP=verify/d3_arm_20261005/A1.csv
```

## 3b. AMENDMENT — G-PORTARM. Registered UNRUN, after G-ARM FAILED and before any port run.

**G-ARM FAILED**: the original's `+0x10` is `1` on **2558 of 3623** frames (**70.60 %**) and `0` on
**1065 of 3623** (29.40 %), against the registered PASS clause `Z >= 3442 of 3623`. Per §2 that
means **G-ACC is NOT run** and the arm divergence is the finding.

That changes which number step 2's columns are read for first, so the new gate is registered here
**before the capture runs**. The columns themselves are **unchanged** from §3 — nothing is added,
nothing is renamed, and `rec_10` was already on the registered list.

**G-PORTARM.** The port's `rec_10` over car 1's `ai_posmatch.window` rows in the new capture
(`n_w` = that row count, printed). Distinct-value histogram with counts, raw `u32`/`i32`/`f32`.

- **MISMATCH-DATA** if `rec_10 != 0` on **>= 0.50 * n_w** rows: the port's record **carries** the
  gate's input and the orientation caller ignores it. The remedy is then local to
  `VehiclePhysicsRun.cpp:1001` plus the already-written `BodyOrient_OmegaFromAngVel`.
- **MISMATCH-UPSTREAM** if `rec_10 == 0` on **>= 0.95 * n_w** rows: the port's record never carries
  the gate value, so wiring the gate alone would still take the steer arm on every substep, and the
  **writer** of `+0x10` is the missing port. Supporting static fact, already grepped: **no file under
  `mashedmod/src/` writes vehicle record `+0x10`** — the only two sites are reads, `Integrate2.cpp:527`
  and `PhysicsChainHooks.cpp:2139`.
- Between 0.05 and 0.50 non-zero: **INCONCLUSIVE**, histogram reported.

G-PORTARM does **not** revive G-ACC. Comparing an accumulator across sides that provably take
different arms stays out of scope for this leg, in either branch of G-PORTARM.

## 4. STEP 3 — generated vs accumulated. Three gates, and what each can and cannot say.

Population throughout: **car 1**, the `ai_posmatch.window` window (first 220 calls from the first
`c4 != 0`), pairing radius **R = 0.12** (`ai_posmatch.py:24`).

**Pairing key, unchanged from `PREREG_LEG3.md` §5c:** port `own_x`/`own_z` against the original's
`+0x958`/`+0x960`. The port writes `+0x958`/`+0x960` as **identically 0.0** (it keeps position in
`a.pos[]`), so the record-position key does not exist on that side. Not revisited here.

**The admissibility term, and it is checked before any median is reported.** The `.msd` is one
snapshot per render frame while the dump is several AI calls per frame. Leg 3 measured the **median**
induced heading error of this join at **0.0000 deg** and its **p90** at 0.0902 deg against the
registered 10x line of 0.0892. Accordingly:

- **Medians only. No tail statistic is reported from this leg, and none may be quoted from it.**
- **G-ONSET VOID condition:** the measured **median** induced heading error of the join must be
  **<= 0.025 deg**, which is 10x below **0.25 deg**, the smallest number any G-ONSET clause compares
  against. If it exceeds 0.025 deg, G-ONSET is VOID and reports no verdict.

### G-ACC — does the port's rate STATE agree at the matched instant? (needs step 2; gated on G-ARM PASS)

`+0x148` cross-side at matched pairs, read directly and never differenced. `+0x144`/`+0x14c` printed
alongside and not gated on.

- **G-ACC-FLOOR, in the units of the claim.** Floor = the **original's own median
  |`+0x148`(f+1) - `+0x148`(f)|** over the `n_step` consecutive matched-frame pairs. The cross-side
  figure = the **median |port `+0x148` - original `+0x148`|** over the `n` matched pairs.
  **Admissible only if cross-side median >= 10.0 x floor median**; `n`, `n_step`, both medians and
  the ratio are printed. Below 10.0x the leg reports **VOID** and **explicitly not "the accumulators
  agree"** — an inadmissible difference is not a finding of agreement.
- **If admissible:** `frac = cross-side median |delta +0x148| / original median |+0x148|` over the
  **same `n` matched pairs** (same denominator on both terms, stated so the 2026-10-05
  mismatched-denominator failure cannot recur). **frac >= 0.20 → a live rate-state difference exists
  at the matched instant** (favours GENERATED). **frac <= 0.05 → the port's rate state agrees there**
  (favours ACCUMULATED). In between → INCONCLUSIVE.
- **Registered limit, so the result is not over-read:** `+0x148` is itself a damped *integral* of
  past omega (`BodyOrientationIntegrate.cpp:315-322`), so a difference in it is **already** partly
  accumulated. G-ACC therefore bounds the *instantaneous rate-state* disagreement and is **not** on
  its own the generated-vs-accumulated discriminator. G-ONSET and G-SPAN are.

### G-ONSET — the discriminator, in heading units, offline

Car 1's matched pairs ordered by the port's window ordinal (`seq`), split into terciles. `T1` = the
first `floor(n/3)` pairs, `T3` = the last `floor(n/3)`. At `n = 105` that is **35 of 105** in each,
both above the 30 floor legs 1-2b used.

- **ACCUMULATED:** median |heading residual| over T1 **<= 0.25 deg** (T1 = 35 of 105 pairs) **AND**
  median over T3 **>= 0.50 deg** (T3 = 35 of 105 pairs). 0.50 deg is legs 1-2b's own FAIL threshold;
  0.25 deg is half of it.
- **GENERATED:** median over T1 **>= 0.50 deg** (35 of 105) **AND** `T3 / T1 <= 2.0`.
- Anything else: **INCONCLUSIVE**, with both tercile medians printed.
- **NO-VERDICT** if either tercile carries **< 30 pairs** of the `n` matched.

Heading is measured **directly** as `atan2(+0x9dc, +0x9d4)` on both sides — no `err` inversion, the
identity U-9192 records.

### G-SPAN — the pairing-free check

Total heading swept across the window on each side **independently**, as the sum of per-step
`wrap180` increments. No cross-side instantaneous pairing is used at all; only the window definition
is shared. `dH_p` over the port's windowed car-1 rows (count printed), `dH_o` over the `o_t1.msd`
frames inside the frame range the original's own windowed `o_t1.msd.aistep.csv` rows span (count
printed). The two counts differ by construction; a total sweep does not depend on sample count
provided no step aliases.

- **VOID condition:** `max |per-step wrap180 increment|` must be **< 90.0 deg** on **both** sides, or
  the sum aliases and G-SPAN reports nothing. Both maxima printed.
- **ACCUMULATED-consistent:** `|dH_p - dH_o| >= 0.50 deg` — the window alone manufactures at least
  the FAIL threshold's worth of divergence.
- **INHERITED-BEFORE-WINDOW / GENERATED-consistent:** `|dH_p - dH_o| <= 0.25 deg` while the matched
  residual is 0.9866 deg — both sides sweep the same total, so the offset was already present at the
  window start.
- In between: INCONCLUSIVE.

### Combination rule, fixed now

- G-ONSET ACCUMULATED **and** G-SPAN ACCUMULATED-consistent → **ACCUMULATED**.
- G-ONSET GENERATED **and** G-SPAN `<= 0.25` → **GENERATED-OR-INHERITED-BEFORE-WINDOW**, and those
  two are **not separable by this instrument** — stated plainly rather than collapsed to "generated".
- Any disagreement between the two → **INCONCLUSIVE**, with both reported.
- G-ACC is reported alongside as corroboration and **cannot overturn** G-ONSET + G-SPAN, because its
  quantity is itself an integral.

## 5. What a result here does and does not license

- **It does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b)
  on any car**, and nothing in this leg changes a band.
- **No C-level moves** unless a gate produces behavioural evidence at an RVA, which none of these is
  designed to do. `0x0046e9e0` stays C2.
- **Cars 2 and 3 are out of scope.** Their numbers are already seen and NO-VERDICT on n; the match
  radius is **not** loosened and the window is **not** lengthened. Either change needs its own
  pre-registration.
- A G-ARM FAIL is a **finding about the port**, not about U-9191's magnitude, which stands either way.

## 6. Also owed in this session, comment-only, no behaviour change

- `BodyOrientationIntegrate.cpp:63` — `kAngVel`'s comment says "angular velocity triple" for
  `+0x9bc`. It is **torque**. Comment-only.
- `BodyOrientationIntegrate.cpp:29-38` — the header says "DELIBERATELY NOT WIRED YET. Nothing calls
  these functions". That is **stale**: `VehiclePhysicsRun.cpp:1001-1003` calls
  `BodyOrient_OmegaFromSteer`, `BodyOrient_IntegrateStep` and `BodyOrient_Heading`, and `:520`/`:816`
  call `BodyOrient_Init`. Comment-only.
- U-9192 item 2: re-read `re/analysis/D3_B_OFFLINE_2026-10-02.md` §3.5 for the `d_body_heading`
  identity inference. Read-only.

## 7. Process

Frida is **not** used: step 1 and step 3's offline legs read committed captures, and step 2's capture
is a standalone run with no injection. The one spawned process is `mashed_re.exe` via `sa_capture.py`;
its PID is the only one this session may kill, and no blanket kill by name is permitted.
