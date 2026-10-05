# PREREG — U-9188: which `a.yaw` writer carries the bridge/record disagreement

**Status when committed: UNRUN.** No code, no build, no game run has happened at the time
this file is committed. Results go in `RESULT_YAWW.md` in this directory.

Parent row: **U-9188** (`UNCERTAINTIES.md`). Parent gate: **D3 criterion (b)**, the body-heading
share of the matched-position residual. Prior session: `verify/d3_heading_20261004/RESULT_HEADING.md`
(commits `ed2eba34` .. `ef09fe40`).

---

## 1. What the registered scope was, and how this session changes it

`re/NEXT_SESSION.md:32-43` and U-9188's remedy column register **one** action: add a default-OFF
counter at each of `TrackRenderer.cpp:3283` / `:3393` / `:3423` / `:3717`, count per AI slot over
the (b) window, and watch car 2 separately.

That is still leg A below and it runs unchanged. **Two corrections and one addition are registered
here, before running, because they were found by reading the four sites rather than by measuring:**

### Correction 1 — `:3283` is NOT a non-resyncing writer when `phys` is on

U-9188 states that of the five `a.yaw` writers, "`:3351` resyncs the record ... and the other four
do not". For `:3283` that is **wrong**. The site reads:

```
3282:  if (a.spin > 0.f) {
3283:      a.spin -= in.dt; a.yaw += 12.0f * in.dt;
3284:      a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f;
3285:      if (phys) Vehicle::VehiclePhysics_ResetOrientation(v, a.yaw);
3286:      continue;
3287:  }
```

`:3285` is a `VehiclePhysics_ResetOrientation` one line after the write, guarded by exactly the
`phys` that is true on the (b) window. So `:3283` resyncs on the measured arm. It is still counted
in leg A — a non-zero count there is informative about the spin scaffold either way — but it is
**excluded as a candidate carrier of a persisting disagreement** on the same reasoning U-9188 used
to exclude `:3351`.

### Correction 2 — `:3393`, `:3423` and `:3717` are all unreachable on the (b) window

Reading the enclosing control flow rather than the sites alone:

- `:3228` opens `if (faithful_nav)`, where `faithful_nav = g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3)` (`:3227`).
- `:3381` is that `if`'s `else`. **`:3393` and `:3423` are both inside the `else`**, i.e. the legacy
  "AI v2" gate-ribbon branch.
- `:3717` is inside `AiOptionBStep`, whose **only** call site is `:3313`, `if (!phys) { ... }`.

The (b) window runs with `faithful_nav` true and `phys` true. So the registered four-site count is
**predicted to come back `:3283 >= 0`, `:3393 = 0`, `:3423 = 0`, `:3717 = 0`** — and U-9188's open
question "whether `:3423`'s gate-ribbon branch is taken on the ported path" is answered **no** by
control flow, not by a counter.

If that prediction holds, **the registered remedy list is exhausted without finding the carrier.**
Both of U-9188's conditional fixes ("if `:3423` fires…", "if `:3717` fires…") are then dead. That is
why leg B exists and why it is registered now rather than after leg A returns.

### Addition — candidate 5, the ORDER candidate

`AiBridgeSnapshot()` has **exactly one call site in the whole tree**, `:3244`, and it is **above the
AI physics loop**:

| line | event |
|---|---|
| `:3003` | **player**: `car_yaw_ = io.yaw` (player's own `StepCar` already returned) |
| `:3244` | `AiBridgeSnapshot()` — writes `g_aib.fwd[0]` from `car_yaw_` (`:3728`) and `g_aib.fwd[v]` from `a.yaw` (`:3736`) |
| `:3245` | `Ai_Standalone_Tick()` — the consumer; reads `g_aib.fwd[v]` through the accessor at `:106` |
| `:3331` | **AI**: `VehiclePhysics_StepCar(v, …)` → writes the record's `+0x9d4`/`+0x9dc` |
| `:3334` | **AI**: `a.yaw = io.yaw` |

So on every frame:

- the **player's** bridge heading is built from a `car_yaw_` written **earlier in the same frame** →
  it agrees with the record exactly;
- each **AI car's** bridge heading is built from an `a.yaw` written **in the previous frame**, while
  the record's `+0x9d4` it is being compared against was written by **this** frame's `:3331`.

**No scaffold writer is required to explain the disagreement.** A one-game-frame offset does.

This is consistent with every number in U-9188, including two it does not otherwise explain:

- the player floor of **exactly 0.0000 deg at every lag** — the player is the one slot whose
  snapshot is in-frame, so the mechanism predicts a hard zero there and nowhere else;
- **car 2 being qualitatively different** (327/933 above 10 deg, 16 above 90, max 176.65) — a
  per-frame offset scales with how fast that car is turning, and the spin scaffold at `:3283`
  advances `a.yaw` at 12.0 rad/s, which at a 60 Hz frame is **11.46 deg/frame**.

**Why U-9188's lag refutation does not refute this.** That fit swept integer lags 0..4 over
`sa_headwatch.py`'s **poll samples** at `--hz 30` against a game running faster than 30 fps. A
one-**frame** offset is not an integer number of **samples**, so the sweep could not express it. The
refutation is sound for sample-staleness and silent on frame-order. Stated so the earlier result is
not overturned on a misreading of it: L=0 being the minimum remains true and remains evidence
against *sampler* lag.

---

## 2. Legs

### Leg A — the registered four-site count (runs as registered)

Four default-OFF counters at `:3283` / `:3393` / `:3423` / `:3717`, incremented per AI slot. No
behaviour change: counter increments only, under one env knob, nothing read by game code.

**Coverage control (mandatory, memories `arm-coverage-counters-before-first-run` and
`absent-log-proves-nothing-run-a-control`).** A fifth counter at **`:3334`**, the baseline writer
that must fire every frame for every live AI slot on this arm. Leg A is **VOID** if `:3334` reads 0
— that proves the counter block never executed, not that the sites never fired. A dump of
`faithful_nav` and `phys` is emitted alongside, so "unreachable by control flow" is witnessed rather
than inferred.

### Leg B — the ORDER discriminator

Per frame, per AI slot, under the same knob, record the angle between:

- `fwd_snap` = the heading **`AiBridgeSnapshot` actually published** this frame (`cos/sin` of `a.yaw`
  as of `:3244`), and
- `fwd_rec` = `cos/sin` of `a.yaw` **immediately after `:3334`**, the value that agrees with the
  record by construction.

Both are port-internal, same frame, same thread, read at known program points — so unlike
`sa_headwatch.py` this is **not** a poll and carries no H-JITTER term.

Also recorded, as the separator between the two candidate families: whether `a.yaw` changed between
`:3334` and the **next** frame's `:3244` by any route **other than** the next frame's `:3334`. A
non-zero count there is a scaffold writer; zero means the only producer is frame order.

---

## 3. Pass / fail, registered before running

| gate | claim | PASS | FAIL |
|---|---|---|---|
| **A-COV** | the leg A counter block executed | `:3334` count > 0 on every live AI slot | 0 anywhere → leg A **VOID**, fix the harness, re-run |
| **A-REACH** | the three unreachable sites are unreachable | `:3393` = `:3423` = `:3717` = **0**, with `faithful_nav=1` and `phys=1` in the same dump | any non-zero → Correction 2 is **wrong**; take U-9188's conditional fix for whichever fired and leg B waits |
| **A-SPIN** | `:3283` is not a persisting carrier | `:3283` = 0, **or** `:3283` > 0 with `:3285` reached on the same count | `:3283` > 0 with `:3285` not reached → `phys` is not what this file assumes; stop and re-read |
| **B-ORDER** | frame order carries the residual | median of leg B's `fwd_snap` vs `fwd_rec` angle is **within the same band** as U-9188's measured 0.5214 / 1.1430 / 0.4300 deg on cars 1/2/3 — taken as the same band if each car's median is inside **[0.25x, 4x]** of its U-9188 value — **and** car 2 is the largest of the three | all three medians below **0.05** deg → frame order is **refuted** as the carrier; publish that and leg B closes negative |
| **B-SCAFFOLD** | no scaffold writer is needed | the non-`:3334` change count is **0** | non-zero → a writer outside the five is live; name it before claiming the order mechanism is sufficient |

**B-ORDER is a reproduction gate, not a fit.** The band and the car-2-largest clause are both taken
from U-9188's already-published numbers, which are fixed before this runs. Nothing in leg B has a
tunable parameter.

**What a PASS does and does not license.** B-ORDER passing identifies the carrier of the heading
residual. It does **not** move any function's C-level — nothing here reads or changes a function at
an RVA, and `TrackRenderer.cpp`'s AI loop is port-only scaffolding with no original counterpart, so
there is no `diff-original` leg available or owed. It does **not** close criterion (b): the
2026-10-02 counterfactual matrix had no arm passing (b) on any car, so the fix that follows must be
re-scored against (b) on its own and is expected to be necessary rather than sufficient.

**No fix is registered here.** Moving `AiBridgeSnapshot()` below the AI loop would change what
`Ai_Standalone_Tick` reads on the same frame, which is a behaviour change on the measured arm and
needs its own pre-registration. This session measures.

---

## 3b. AMENDMENT, committed before anything ran: candidate 5 is REFUTED, and so is U-9188's mechanism

Everything in §1–§3 above was committed at `49fc1ca0` and is left standing as written. This section
was added before any code was written, any build was run, or any game was started. It is an
amendment by **static reading only** — no measurement has been taken. Per memory
`pre-register-the-decision-not-the-diagnosis`, the registered **decision** (count the writers, then
measure the published-vs-record angle in-process at two program points) is honoured unchanged; what
is corrected is the **rationale**.

### Correction 3 — candidate 5 (frame order) is REFUTED

The record's forward row has **exactly three writers in the whole tree**, and all three are the same
expression inside the physics run:

- `VehiclePhysicsRun.cpp:621-623` — `F(r, kForward+0) = cos(io.yaw)`, `+4 = 0.f`, `+8 = sin(io.yaw)`
- `VehiclePhysicsRun.cpp:820-821` — same, x and z
- `VehiclePhysicsRun.cpp:1004-1005` — same, x and z

`off::kForward == 0x9d4` (`VehicleStruct.h:105`). Nothing outside `VehiclePhysics_StepCar`'s call
tree writes it. So the record's forward **is** `cos/sin` of the same `io.yaw` that `:3334` assigns to
`a.yaw`, it is **frozen between one `StepCar` and the next**, and `a.yaw` is frozen over the same
interval unless a writer moves it. A one-frame read offset over an interval on which **both**
operands are constant produces **zero** disagreement.

**Candidate 5 as committed in §1 is wrong.** I registered it as the leading hypothesis and it does
not survive its own first check. Recorded rather than quietly dropped.

### Correction 4 — `VehiclePhysics_ResetOrientation` does not touch the record

U-9188 states that `:3351` "resyncs the record (`VehiclePhysics_ResetOrientation` at `:3355`)", and
Correction 1 above reused that same reading for `:3285`. **Both are wrong.** The function is five
lines (`VehiclePhysicsRun.cpp:518-523`):

```
518:  void VehiclePhysics_ResetOrientation(int slot, float yaw) {
519:      if (slot < 0 || slot >= 16) return;
520:      BodyOrient_Init(g_bodyBasis[slot], yaw);
521:      g_bodyBasisOk[slot] = true;
522:      g_bodyBasisReseed[slot] = true;   // heading discontinuity — see the decl comment
523:  }
```

It writes `g_bodyBasis[slot]`, `g_bodyBasisOk[slot]` and `g_bodyBasisReseed[slot]`. **It never writes
the record, and it never writes `+0x9d4`.** What it resyncs is the **body basis**, not the record's
forward row. Every use of the word "resync" in U-9188 and in Correction 1 is therefore about a
different piece of state than the one being compared.

This does not restore the disagreement, because of the comment at `VehiclePhysicsRun.cpp:516-517`:
"the integrated basis keeps the pre-teleport heading — **the basis is now the authority for
`io.yaw`**". `StepCar` overwrites `io.yaw` from the basis, writes the record's forward from that, and
`:3334` assigns it back into `a.yaw`. So a scaffold rewrite of `a.yaw` that does not re-seed the
basis is **discarded** on the next step rather than carried into the record.

### Correction 5 — the writer enumeration is incomplete: nine, not five

`a.yaw` writers in `TrackRenderer.cpp`, by grep for `\.yaw\s*(=|\+=|-=|\*=)`, excluding `io.yaw`,
`car_yaw_`, `pu_player_.yaw` and `pu_ai_[i].yaw`:

| line | site | re-seeds the basis? | reachable on the (b) arm? |
|---|---|---|---|
| `:2690` | `a.yaw = baseYaw` (grid placement) | **no** | yes, one-shot |
| `:3283` | `a.yaw += 12.0f * in.dt` (spin scaffold) | yes, `:3285` when `phys` | yes |
| `:3304` | `a.yaw = atan2(...)` (respawn relocation) | yes, `:3307` when `phys` | yes |
| `:3334` | `a.yaw = io.yaw` | n/a, it is the sync | yes, every frame |
| `:3351` | `a.yaw = ry` (off-mesh re-aim) | yes, `:3355` | yes |
| `:3393` | `a.yaw += 12.0f * in.dt` (v2 spin) | no | no, legacy branch |
| `:3423` | `a.yaw += yerr * …` (v2 ribbon limiter) | no | no, legacy branch |
| `:3683` | `a.yaw += yerr * …` (`AiOptionBStep` limiter) | no | no, `!phys` only |
| `:3717` | `a.yaw = ry` (`AiOptionBStep` off-mesh) | no | no, `!phys` only |

**`:2690` and `:3304` appear in no prior enumeration.** U-9188's list of five omits both. Leg A
counts all nine.

### What this does to the session, stated plainly

Taking Corrections 2–5 together, **static reading predicts the port-side published-vs-record heading
angle is ZERO on the (b) arm at every program point**: the five reachable writers are `:2690`
(one-shot, self-healing on the next step), `:3283`/`:3304`/`:3351` (basis re-seeded, so carried
rather than discarded) and `:3334` (the sync itself), and the four non-re-seeding writers are all
unreachable.

That **contradicts U-9188's measured medians of 0.5214 / 1.1430 / 0.4300 deg.** Both cannot stand.
So exactly one of the following is true, and leg B as reformulated separates them:

- **H1 — a writer outside the static picture.** Something not found by the grep above moves `a.yaw`,
  `g_aib.fwd[v]`, or the record's forward. Leg A's counters plus leg B's in-process angle at two
  program points localise it.
- **H2 — U-9188's measurement is a POLL ARTIFACT.** `sa_headwatch.py` reads `g_aib.fwd[v]` and the
  record non-atomically via `ReadProcessMemory` at `--hz 30` against a faster game. If the
  in-process angle is zero on the same build while `sa_headwatch.py` reports 0.5+ deg, the poll is
  the source. **This would retract U-9188's central conclusion** — "the body-heading residual is a
  PORT-ONLY BRIDGE defect" — and send D3 (b) back to the physics basis that the 2026-10-04 session
  recorded as NOT REACHED.

**Registered prediction, before running: H2.** The reason is the player floor. U-9188 reads the
player's floor as exactly `0.0000` deg at every lag and treats that as the instrument's noise floor,
licensing the AI cars' non-zero values as real. But `g_aib.fwd[0]` is `cos/sin(car_yaw_)` (`:3728`)
and the player's record forward is `cos/sin(io.yaw)` with `car_yaw_ = io.yaw` at `:3003` — the player
agrees **by construction**, so a hard zero there is guaranteed and is **not** evidence that the
instrument can resolve a small angle. H-JITTER was checked as a position bound, which §3 of
`PREREG_HEADING.md` registered, but a position bound does not bound an angle
(memory `a-bound-on-a-product-is-not-a-bound-on-a-factor`).

I may be wrong about this. The gate below is written so that it is decided by the run and not by me.

### Added gate

| gate | claim | PASS | FAIL |
|---|---|---|---|
| **B-INPROC** | the in-process angle reproduces U-9188 | every car's median inside `[0.25x, 4x]` of its U-9188 value → **H1**, the residual is real and leg A localises the writer | all three medians below **0.05** deg → **H2**, U-9188's result is a poll artifact; file the retraction and do not fix a defect that is not there |

Between those two outcomes (some car in band, others at zero) is neither — it is reported as
INCONCLUSIVE with the per-car numbers, and no conclusion is drawn. Registered now so a split result
cannot be read as whichever answer is convenient.

**Leg B therefore measures three angles per frame per slot, in-process, at two program points:**
`ang_pub_rec` (published `g_aib.fwd[v]` vs the record's forward, at `:3244`), `ang_post_rec`
(`cos/sin(a.yaw)` vs the record's forward, immediately after `:3334`), and `ang_pub_post` (the two
headings against each other). Slot 0 is recorded for completeness and is **labelled
construction-zero**, not treated as a floor.

---

## 4. Honest statement of what could make this whole file wrong

- If `phys` is **false** on the (b) window — not read, assumed from `MASHED_REAL_PHYSICS`'s default
  — then `:3313` takes `AiOptionBStep` and `:3717` is live, Correction 2 collapses, and leg A's
  registered interpretation is void. A-REACH's dump of `phys` is what catches this, which is why it
  is in the dump and not in the prose.
- If `g_aib.fwd[v]` has a second writer outside `AiBridgeSnapshot`, candidate 5 is incomplete. The
  four reads/writes of `.fwd[` found in the tree are `:106` (read), `:3728`, `:3736` (both inside
  `AiBridgeSnapshot`), plus `pu_player_`/`pu_ai_` at `:3847`/`:3854` which are a different struct.
  Recorded so the claim is auditable, not re-derived.
- The 11.46 deg/frame arithmetic for car 2 assumes 60 Hz. The frame rate on the arm is not measured
  in this file and the claim is offered as consistency, not as a fit.
