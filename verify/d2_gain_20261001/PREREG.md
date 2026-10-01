# D2 attempt 10 — PRE-REGISTRATION: the per-frame speed budget BETWEEN contacts

Written and committed **before any run**. Not to be amended (attempt 7 amended its own gate,
§21.8; that is the failure this header exists to prevent). If a safety threshold fails I STOP
and report the failure.

Lane assigned: turn §22.4's `[UNCERTAIN U-9156]` ("the original GAINS speed between contacts,
the port LOSES") from a restatement of `driving-median` into a **term**: decompose each side's
per-frame speed change in the free-flight frames between contacts into its force
contributions, and name the first term that differs at matched speed and matched input.

Scoring arm, unchanged: `MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`, §16.7
(`MASHED_STEER_HOLD_AFTER=0`). §3 bounds unchanged and not renegotiable.

---

## 1. The decomposition, and why it is COMPLETE rather than an enumeration

In a **free-flight (non-contact) frame** the linear velocity `+0x9b0..0x9b8` has exactly ONE
writer: A6a `0x00467650`. Established, not assumed: §22.2 + §22.4 measured that both sides
write `+0x9b0` in exactly two places per frame — A6a `0x00467650` and `VehicleContactFixup`
`0x0046ef70` — and the latter does not run on a non-contact frame (ORIGINAL bitwise unchanged
across `0x0046f6c0` 2945/2945, `0x00469aa0` 2945/2945, `0x004709a0` 2932/2932; PORT's three
`WheelContactSolver` write sites fire 0 of 4000 substeps).

Inside A6a there are exactly **three** velocity writes, in program order:

| # | what | port | original RVA |
|---|---|---|---|
| **W1** | `v += linTerm * (ctrl + accum)` | `Integrate2.cpp:630-632` | inside `0x00467650`, immediately before the `+0x9e4` store at `0x004686cc` |
| **W2** | grip-clamp #6 HIGH arm, `v -= lat*k` | `Integrate2.cpp:727` | `0x004687f0 .. 0x0046897b` (§21.5, byte-faithful) |
| **W3** | grip-clamp #6 LOW arm, `v -= lat*k`, plus the full-stop block | `Integrate2.cpp:736`, `:742` | same range |

with

* `linTerm = dt * Rf(v,0x54) * kDt`; `kDt = 1/3000` exactly (`_DAT_005cc948` = `0x39aec33e`),
  `+0x54` = `0.0010000000474974513` on **2332 of 2332** frames of `orig_fp2.msd` (measured).
* `ctrl = (+0xb14, +0xb18, +0xb1c)`, the control/drive force. `+0xb18` is **exactly `0.0` on
  2332 of 2332** original frames (measured), so `ctrl` is horizontal and the `.msd` /
  `player_trace` pair already carries all of it.
* `accum = (l_b8, l_b4, lin_b0)`, A6a locals: the per-wheel force after the normal/tangential
  blend `frac = (l_d0 - m78)/l_d0` (`Integrate2.cpp:543-565`).

**The probe that closes the budget with no unknown.** Between W1 and W2/W3, A6a stores `|v|` to
`+0x9e4` (`Integrate2.cpp:636`; original `0x004686cc`, which §22.2 established is one of only two
`+0x9e4` stores in `0x00467650..0x00468990` and precedes the clamp at `0x004687f0`). The port's
other store is `Integrate2.cpp:128`, matching the original's `0x00467673` at A6a entry; the
render-tick snapshot therefore sees the **line-636 / `0x004686cc`** value. So, per frame `f`:

```
s_mid(f)  = +0x9e4(f)              speed AFTER W1, BEFORE the clamp
s_post(f) = |+0x9b0..0x9b8|(f)     speed AFTER the clamp (render-tick snapshot)

T_W1(f)    = s_mid(f)  - s_post(f-1)     the integration's speed contribution
T_clamp(f) = s_post(f) - s_mid(f)        grip-clamp #6's speed cost  (<= 0 by construction)
```

`T_W1 + T_clamp = s_post(f) - s_post(f-1)` is an algebraic identity, so the budget cannot be
missing a term. Gravity, slope, drag, rolling resistance and the grip path are each inside
`ctrl` or `accum` **by construction**, which is why this decomposition does not depend on my
enumerating the physics correctly.

W1 is linear in its two addends, so projecting on the incoming unit velocity
`u = v_post(f-1) / s_post(f-1)`:

```
T_drive(f) = linTerm * ( ctrl(f) . u(f) )          drive/control force, longitudinal
T_rest(f)  = T_W1(f) - T_drive(f)                  everything else W1 did
```

`accum` is not a record field, so on the ORIGINAL `T_rest` cannot be split further from a
snapshot; it is reported as one residual. On the PORT it **is** split, because
`MASHED_COUPLING_DIAG` (`friction_diag.log`, `Integrate2.cpp:606-629`) prints `ctrl`, `accum`
and `linTerm` verbatim at the A6a phase: `T_accum = linTerm*(accum.u)` and
`T_2nd = T_rest - T_accum` (the second-order direction-change part).

**The three registered terms are `T_drive`, `T_rest`, `T_clamp`**, and
`T_drive + T_rest + T_clamp == s_post(f) - s_post(f-1)` exactly.

## 2. Channels (both sides already instrumented; NO source change, NO new instrument)

* ORIGINAL: `verify/d2_bounce_20260930/orig_fp2.msd` (MSD1, 2332 frames, rec `0xd04`, base
  `0x8815a0`) — fields `+0x9b0..b8`, `+0x9e4`, `+0xb14/+0xb18/+0xb1c`, `+0x9d4..dc`, `+0x54`,
  `+0x9e0`. Paired contact ground truth: `orig_fp2.msd.fixupprobe.csv` (site 0 = the
  `0x0046ef70` entry). **No Frida run is needed on the original side.**
* PORT: a fresh run at HEAD with `MASHED_PLAYERTRACE=1` (already carries `vel`, `v9e4`, `b14`,
  `b1c`, `bodyfwd`, `gnd`), plus `MASHED_COUPLING_DIAG=1` and `MASHED_A6ADUMP` for the S2
  ground truth, `MASHED_WORLD_CONTACT_LOG` + `MASHED_FIXUP_LOG` for the contact count. All are
  pre-existing and default-OFF.
* Instrument-insensitivity control, already measured: `p1/player_trace.log` (with the 7 MB
  `MASHED_A6ADUMP` armed) and `p2/player_trace.log` (without) are **identical on 1496 of 1496
  common frames**, so arming these logs does not move the trajectory.

## 3. Alignment

**Contact-frame detector (registered, instrument-free, symmetric):** frame `f` is a contact
frame iff `s_post(f) > 1e-3` and `s_mid(f)/s_post(f) > 1.02`.
**Already validated on the original: it returns exactly 23 frames and they are exactly
`probe_frame + 1` for all 23 of `orig_fp2`'s site-0 fixups — 23 of 23, no extras, no misses**
(`979, 998, 1011, 1023, 1035, 1047, 1060, 1072, 1084, 1097, 1255, 1352, 1363, 1510, ...`).

* **Gap `k`** = the frames strictly after contact frame `k` up to and including the frame before
  contact frame `k+1`. Scored steps for gap `k` = `f` in `[c_k + 1, c_{k+1} - 1]`, each step
  using `s_post(f-1)` as its "from" endpoint, so the step out of the contact frame IS scored
  and no step straddles a contact.
* ORIGINAL gap 0: `c_0 = 979`, `c_1 = 998` → steps `f = 980..997`, n = 18.
* PORT gap 0: the same detector on the fresh run's `player_trace.log`.
* **Matched speed:** the headline comparison is restricted to steps with
  `s_post(f-1)` in **[150, 260]** (the original's gap-0 range is 181..231; §22.4's gap-0
  endpoints are 219.89/251.48 original/port). The full per-frame gap-0 table is reported
  unrestricted as well, and a second cut takes per-gap medians over all of gap 0.
* **Matched input:** `|ctrl| > 0` required on every scored step, both sides (§22.4 established
  full throttle on both over this train).
* All nine gaps are also reported, per gap, so the gap-0 reading is not a single-window result.

## 4. Tolerance per term

Side noise is **zero**, measured, not estimated: ORIGINAL `orig_solo3` vs `orig_solo4` are
bit-identical on all eleven channels (§22.1); PORT `p1` vs `p2` identical on 1496/1496. So the
tolerance is a registered floor.

A term **diverges** iff BOTH hold over the scored steps:

* **(a) material against the budget:** `|median(T_port) - median(T_orig)| >= 0.20 * |median(T_W1_orig)|`
* **(b) material against the residual being explained:** `|median(T_port) - median(T_orig)| >= 0.30 * |median(dS_orig) - median(dS_port)|`, where `dS(f) = s_post(f) - s_post(f-1)`.

## 5. Decision rule

1. Rank `T_drive`, `T_rest`, `T_clamp` by `|median(T_port) - median(T_orig)|` on the matched-band
   scored steps.
2. **The diverging term** = the highest-ranked term satisfying both (a) and (b). It is named with
   the W1/W2/W3 row it belongs to, i.e. with its original RVA and its port `file:line`.
3. If it is **`T_drive`** → the term is the control force `+0xb14/+0xb1c` and its producer; next
   step is the original-side witness on that producer before any code change.
4. If it is **`T_rest`** → the term is the per-wheel force accumulator and its blend
   (`Integrate2.cpp:543-565`); the PORT-side `T_accum` / `T_2nd` split from `friction_diag.log`
   says which half, and the original-side witness is then required before any code change.
5. If it is **`T_clamp`** → the lane re-enters grip-clamp #6, which §21.5 proved byte-faithful.
   That is a contradiction, so I report it as such and author **no** fix.
6. If **no** term satisfies (a) and (b) → **the lane closes without a term.** I say so plainly,
   list what it ruled out, recommend the next lane, and author no fix.

**Step 4/5 of the task (fix + promotion leg) is entered only via branch 3 or 4, and only after
the named term has been tested against the RUNNING original.** No fix is authored from the port
side alone.

## 6. Safety thresholds — if ANY fails I STOP and report it

* **S1 (no third writer).** `T_clamp(f) <= +1e-3 * s_post(f-1)` on >= 99% of scored steps, both
  sides. A systematically POSITIVE `T_clamp` means something between `0x004686cc` and the
  snapshot also writes velocity, which would void attributing `T_clamp` to the clamp.
* **S2 (the estimator has ground truth, PORT).** Joining `friction_diag.log` to
  `player_trace.log` by `spd`, the snapshot-phase `T_drive` must match the A6a-phase
  `linTerm*(ctrl.u)` within **10%** on >= 90% of joined scored steps, and the join must be
  unique. If it fails, the render-tick-phase estimator is unsound and the cross-side table is
  void. (This is the registered answer to memory `verify-the-harness-knob-actually-took` and to
  §22.2's "snapshot vs act phase" caveat.)
* **S3 (the channel CAN see a difference — a control that does differ).** On the ORIGINAL alone,
  `median(T_drive)` must rise by >= **1.5x** between the speed bands `[150,260]` and
  `[1500,2000]`. `Integrate2.cpp:576` records the original's own `|+0xb14..1c|` rising 2.7x over
  1-500 → 2000-2600, so a flat `T_drive` would mean the channel is blind and every null in this
  lane is uninterpretable.
* **S4 (grounded).** `+0x9e0 == 4.0` on >= 95% of scored steps, both sides, so the clamp branch
  under analysis is the one that ran.
* **S5 (the arm took).** The fresh PORT run must reproduce §22.4's `driving-median` **1355.66**
  and its gap-0 opening speed **251.48** to the printed digits. If not, it is not the §16.7 arm
  and nothing in the table is comparable.
* **S6 (detector on the port).** The detector's contact-frame count on the port must equal the
  `world_contact.log` fixup count within 1.

## 7. Re-scoring, if and only if a fix is authored

`MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`, 3 runs, against the
UNCHANGED `d81a8df6` bounds: slip 1500-2000 `0.18855..0.19635`, slip 2000-2600
`0.24488..0.25487`, driving-median `1904.70..1982.44`. Bounds are not changed.
Dual-copy guard must stay at `NEW=0`.

## 8. Routes that are CLOSED and that this lane does not reopen

The fixup impulse; the substep velocity chain (contact and non-contact); `+0x9e4`'s write order;
grip-clamp #6's transcription; the `l_60`/`ld4` lane; the contact cadence (U-9159, refuted).
U-9160 (substep budget 3-4 vs a fixed 2) is banked as measured-inert and is not relevant to this
lane, because the residue pass contacts 0 of 1280 and writes velocity 0 of 1280 — this lane
scores per-FRAME record snapshots, so the residue pass cannot enter the budget at all.
