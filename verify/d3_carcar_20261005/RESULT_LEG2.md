# RESULT — LEG 2, ring slot `[+0x9a8]` is published: 6 of 6 gates PASS, and my pre-registered expectation FAILED

Pre-registration: [`PREREG_CARCAR.md`](PREREG_CARCAR.md) §3, committed **UNRUN** at `3404612d`.
Leg 1: [`RESULT_LEG1.md`](RESULT_LEG1.md) at `050647bc`.

**No C-level moved. No band moved. `original/` untouched.** Source footprint: **49 insertions /
0 deletions in one file**, `Vehicle/VehiclePhysicsRun.cpp` — the pre-existing `[+0x9ac]`
write is left byte-for-byte as it was, so the knob-off path is identical to the pre-leg
build. Built exe SHA-256 `ABE7BA36035B94C8294158B2071FE2E7FD6D3160D99EC6EB10AECD8F8D31E3A2`.
**Leg 3 was not started.**

## 1. The change

`SyncContactRingMatrix` now publishes the basis + position into the `[+0x9a8]`-selected
ring half as well as the `[+0x9ac]` one. Default-ON with the A/B revert
`MASHED_RING_SLOT0=0`, per the v3 flag rule. **Exactly one `getenv` read site in the whole
port** (`VehiclePhysicsRun.cpp:412`, grep-verified).

**DEVIATION, registered in the source, not discovered after the fact.** The original's two
halves differ by one substep, because A9 (`0x0046e9e0`) writes `dst = src + delta` into the
half the selectors do not currently point at. The port keeps one pose in caller-owned
storage (U-9152), so it publishes the **same** pose into both halves. Measured:
`slot0 == slot1` on **7855 / 7987 / 7993 of 7855 / 7987 / 7993** rows on the three ON runs.
Closing that needs A9's double-buffer write and is a different, much larger change.

## 2. The gates

Arms, all on the same recipe (`re/tools/sa_capture.py`, `MASHED_TRACK_VIEW=Training`,
`MASHED_CAR=1`, `MASHED_ROUND=1`, 60 s), scored by `re/tools/ai_speed_env.py --check` and
`re/tools/ai_ctrl_window.py --check`:

| arm | build | files |
|---|---|---|
| `BASE` | committed `32d763a2` (slot 0 NOT published) | `P1.csv`, and `B1.csv` from the same build with the leg-1 instrument stashed out |
| `ON` | this leg, default | `ON1.csv`, `ON2.csv`, `ON3.csv` |
| `OFF` | this leg + `MASHED_RING_SLOT0=0` | `OFF1.csv` |

| gate | threshold | measured | denominators | verdict |
|---|---|---|---|---|
| `G-NOREG-E` | every printed digit equal to `launch` **1426.4 / 2053.0 / 2055.2** and `ft_median_m0` **2550.6 / 2053.0 / 2278.2** | **identical on every digit**, `n` = 100 / 23 / 39, `flag=[0]`, `regime0=1` | **3 of 3** cars x **3 of 3** ON runs; `(e)` **PASS 3/3** on each | **PASS** |
| `G-NOREG-B` | **<= 13** failing bands | **exactly 13** (car 1 **5**, car 2 **4**, car 3 **4**), and the band NAMES are the same five/four/four — `c0_distinct`, `c1_distinct`, `steer_distinct`, `c1_median`, `abs_steer_median` | 30 bands over 3 cars, on **3 of 3** ON runs | **PASS** |
| `G-BANDS-UNEDITED` | both empty | both empty, checked **before** the first run and **after** the last | `git diff --stat` and `git status --porcelain` on `re/tools/ai_ctrl_window.py` and `re/tools/ai_speed_env.py` | **PASS** |
| `G-KNOBOFF` | every printed digit equal to the committed build's | identical; and slot 0 back to all-zero on **7985 of 7985** rows | 3 of 3 cars, 1 OFF run | **PASS** |
| `G-DET` | identical to every printed digit | identical | **3 of 3** ON repeats, both scorers | **PASS** |
| `G-SLOT0-LIVE` (control) | slot `[+0x9a8]` non-zero and within **1e-3** of `own_x`/`own_z` on **>= 95 %** | **0.999618 / 0.999624 / 0.999625** | **7852 of 7855**, **7984 of 7987**, **7990 of 7993** rows; the 3 excluded per run are the all-zero pre-race rows. BASE's same figure is **0 of 7775 = 0.000000** | **PASS** |

**The control is what makes the five "no change" gates readable at all.** Without it,
`G-NOREG-E` and `G-NOREG-B` printing the BASE digits would be indistinguishable from a knob
that never took (memory `verify-the-harness-knob-actually-took`, `absent-log-proves-nothing-run-a-control`).
It took: slot 0 goes from **0 of 7775** agreeing with `own_x`/`own_z` to **0.9996** of rows,
and `G-KNOBOFF` puts it back to **7985 of 7985** all-zero.

## 3. MY PRE-REGISTERED EXPECTATION FAILED, and this is the leg's real output

`PREREG_CARCAR.md` §3 states, in writing and before the run: *"Registered expectation,
stated before the run: **this is NOT inert.** Three sites read the `+0x9a8`-selected slot
and currently receive a zero matrix … So A4's and A6b's `wheelBlock` input changes.
Predicting 'no change' here would be a control that agrees with the default by
construction."*

**It is inert on this recipe, and the prediction was wrong for two distinct reasons — one
of which is that the pre-registration UNDER-ENUMERATED the readers.** There are **four**,
not three:

| # | reader | in `mashed_re.exe`? | does slot 0 reach anything? |
|---|---|---|---|
| 1 | `Vehicle/PhysicsChainHooks.cpp:536` | **NO** — `PhysicsChainHooks.cpp` is in `asi_sources.rsp` only (`exe=0 asi=1`) | never executes in the shipping binary |
| 2 | `Vehicle/PhysicsChainHooks.cpp:2749` | **NO** — same TU | never executes in the shipping binary |
| 3 | `Vehicle/VehicleControl.cpp:103` | yes, and it is called (A4, once per frame) | it only **forwards** the pointer to `Vehicle_Integrate2`, whose exe body declares the parameter **unused**: `Integrate2.cpp:123` reads `void* /*wheelBlock*/` |
| 4 | **`Vehicle/ForceIntegrator.cpp:112-118`** — A5 Phase 2, the drafting / proximity grip term. **NOT named in the pre-registration.** | yes (`exe=1 asi=0`), and it is called at `VehicleControl.cpp:208` | its `delta` **genuinely changed**: `delta[k] = *(gb + 0x958 + c*0xd04 + [rec_c+0x9a8]*0x40 + 4k) - vF(self, self[0x26a]*0x10 + …)` reads **both** cars' slot 0, so before this leg it was identically `(0,0,0)` for every pair |

So reader 4 is a live consumer whose input really did change — and the statistics still did
not move. **The reason is mechanical and it is the original's own law, not port
scaffolding:** Phase 2's only output is `local_70`, and Phase 3 **unconditionally
overwrites it** —

```
ForceIntegrator.cpp:137   if (g_playerCount == 4)      local_70 = 1.0f;
```

— and `g_playerCount == 4` on this recipe: `TrackRenderer.cpp:3086` calls
`VehiclePhysics_Init(1 + ai_cars_.size(), …)`, the dump carries `v = 1,2,3` so
`ai_cars_.size() == 3`, and the live `mashed_re.log` prints `MATCH-SEED rule=0
participants=4 teams=0 seed=6 engine=1`. So the whole drafting computation is discarded
before it can affect grip.

**Therefore the five no-change gates are PREDICTED, not lucky** — and that is a
qualitatively better result than an unexplained pass.

### The caveat this creates, stated because the gates were run on ONE recipe

`ForceIntegrator.cpp:137-139` only discards `local_70` for `g_playerCount` **4** or **9**
(and forces `0.75f` for 8 when `self[0] != 0`). **On a participant count outside
{4, 8, 9}, reader 4 is live and leg 2 is NOT inert.** Nothing here measures that, and it is
`[UNCERTAIN]`. Two related port-side facts that would have to be settled first, neither
established here: `self[0]` is **0 for every car** in the port (the self-index store at
`0x0046bab8` belongs to the original's absolute `DAT_008815a0` array, not to `g_records` —
`re/analysis/CHANGELOG.md` 2026-10-03), so Phase 2's `c != self[0]` guard makes each AI car
skip the player and include itself; and `Vec3Norm3` on the former all-zero `delta` was
being asked to normalise a zero vector on every iteration.

## 4. What leg 2 bought, and what it did not

**Bought.** Ring slot `[+0x9a8]` now carries a real basis and position for every car on
every substep, proven live. That is exactly the input leg 1 identified as the single
blocker, so **leg 3 is now unblocked**: the proximity gate at `0x00470b44..0x00470ba9` will
compare two real centroids instead of two `(0,0,0)`s, and `CarCarContacts.cpp:93`/`:97`'s
angular lever arms will be ~0.5-unit arms instead of absolute world positions. And because
slot 0 has no *effective* consumer on this recipe, any (e)/(b) movement leg 3 produces is
attributable to leg 3 alone.

**Did not buy.** Any movement on criterion (b) — 13 of 30 bands, unchanged, same names. Nor
any claim about the car<->car contact itself: `0x00469df0` is still **C2** and still has no
call site. Nor fidelity of the ring double-buffer: the port's two halves are the same pose
where the original's are one substep apart (§1).

**Guards not run, and not registered for this leg:** the power-up sweep and the modes
oracle. The change touches neither path, but that is an argument, not a measurement.
