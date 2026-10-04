# PRE-REGISTRATION — U-9185 item (b): which port-side heading candidate carries the residual. **UNRUN.**

Written and committed **before** any capture in this directory exists. Branch
`race/first-frame-parity`, HEAD at write time `9659611e`. **No game code, no `.rsp`, no
build, no band change.**

## The question

U-9185 measured, at **matched position**, a median `|d signed err|` of
**0.945 / 1.296 / 0.466** deg on cars 1/2/3, decomposing into target direction
**0.2815 / 0.4814 / 0.4477** and **body heading 0.9796 / 0.9085 / 0.6506**. The heading is
the larger median share on 2 of 3 cars, and U-9185 records it as *"a vehicle-physics
output, not an AI one"*. This step separates the port-side candidates for that heading
share. **It fixes nothing; it selects.**

## How the heading reaches the AI in the port (path, cited)

`AiStandalone.cpp:858` `SteerAngleErrorFwd` takes it from `s_host.own_fwd_xz`, which is
`aib_own_fwd_xz` (`TrackRenderer.cpp:97-100`) returning `g_aib.fwd[v]`. That is filled at
`TrackRenderer.cpp:3729` as **`(cos(a.yaw), sin(a.yaw))`**, with `a.yaw` taken out of
`Vehicle::VehiclePhysics_StepCar` at `TrackRenderer.cpp:3327` (passed in at `:3315`).
Inside the step, `VehiclePhysicsRun.cpp:819` sets `io.yaw = BodyOrient_Heading(basis)` and
`:1002-1003` writes `F(r, kForward+0) = cos(io.yaw)` / `F(r, kForward+8) = sin(io.yaw)`,
with `off::kForward = 0x9d4` (`VehicleStruct.h:105`) — **the same record field the
original's `FUN_0046d510` returns**.

## Three candidates, and the third is new

- **C1 — the integrated basis itself differs** from the original's matrix row. A
  **physics output**, i.e. a D2 surface reached through the AI.
- **C2 — the scalar reconstruction is lossy.** `(cos(yaw), sin(yaw))` stands in for the
  record's forward basis row.
- **C3 — phase / staleness.** `a.yaw` is captured at `TrackRenderer.cpp:3327` and
  reconstructed at `:3729` inside `AiBridgeSnapshot`, which runs at a different point in
  the frame from where `+0x9d4` is written (`VehiclePhysicsRun.cpp:1002-1003`).

## Static pre-finding, registered as a PREDICTION so the run can falsify it

`BodyOrient_Heading(m) = atan2(m[10], m[8])` (`BodyOrientationIntegrate.cpp:334`), and
`m[8], m[9], m[10]` is the at/forward row (`:149`). So `(cos(heading), sin(heading))`
recovers `normalize(m[8], m[10])` **exactly**, and `atan2` is scale-invariant so a
non-unit basis does not change that. The record's `+0x9d4`/`+0x9dc` are written from the
**same** yaw in the **same** function.

> **PREDICTION: C2 is refuted by construction** — the bridge and the record should agree to
> float precision (order **1e-7 rad ≈ 6e-6 deg**), not to 0.9 deg. **If the measurement
> disagrees, the measurement wins and the prediction is reported as wrong.**

## Method — port side only, no build

`re/tools/sa_headwatch.py` (new), on `re/tools/sa_boostwatch.py`'s pattern: spawn
`mashed_re.exe` as `sa_capture.py` does and poll with `ReadProcessMemory`. **No injection,
no Frida, no game-code change.**

Addresses resolved **from the linker map, not hardcoded** (`sa_boostwatch.py`'s hardcoded
`G_RECORDS = 0x00c09b88` is already stale against today's map, which says `0x00c09b90` —
so this tool reads the map every run):

- `g_aib` → `TrackRenderer.obj`, map `0x00c09460`. Layout `TrackRenderer.cpp:63-70`:
  `pos[4][2]` +0, `vel[4][2]` +32, **`fwd[4][2]` +64**, `alive[4]` +96.
  So `g_aib.fwd[v]` = `g_aib + 64 + v*8`.
- `g_records` (Vehicle) → `VehiclePhysicsRun.obj`, map `0x00c09b90`;
  `unsigned char g_records[16 * kRec]` (`VehiclePhysicsRun.cpp:122`), stride `0xd04`.
  So the record forward row is `g_records + v*0xd04 + 0x9d4` (x) and `+ 0x9dc` (z).

## Gates, registered before the run

- **H-BASE** — three legs, all required, same shape as the leader witness that passed:
  1. **Independent-channel agreement.** `g_aib.pos[v]` must agree with the same run's
     `MASHED_AI_STEPDUMP` `own_x` / `own_z` for that slot, and `g_records[v]+0x9e4` must
     lie inside that run's `rec_9e4` range for that slot.
  2. **Liveness.** The sampled values must change across samples.
  3. **Non-degenerate.** At least one sampled field non-zero per slot.
  Any leg failing → **VOID**, not interpreted.
- **H-JITTER** — the sampler is a poll, so bridge and record are read microseconds apart,
  not atomically. Each sample therefore re-reads `g_aib.pos[v]` **after** the record read
  and reports `|pos_before − pos_after|`. **If the jitter bound is not at least 10x smaller
  than the H-C2 difference being claimed, the result is VOID** — a poll cannot manufacture
  a verdict (memory `next-sample-pairing-needs-a-frame-marker`).
- **H-C2 — the discriminator.** `d = max over samples of
  |atan2(fwd_z, fwd_x) − atan2(rec_z, rec_x)|` in degrees, per car.
  - **d < 0.01 deg** → **C2 REFUTED and C3 REFUTED for this pair** (no lossiness, no
    staleness between record and bridge). The residual is then in the basis → **C1**.
  - **d ≥ 0.1 deg** → **C2/C3 LIVE.** Report the distribution and let its *shape* say
    which: a near-constant offset indicates staleness (C3), scatter indicates lossiness
    (C2). **No guess** — if the shape is ambiguous it is reported as ambiguous.
  - **0.01 ≤ d < 0.1** → reported, **no verdict**.
- **H-C1 consequence.** If C2 and C3 are refuted, the heading share is a **physics
  output**. That is a **D2 WATCH** row — filed with evidence, and **no D2 code is changed
  in this session**.

**Registered decision rule:** whichever candidate the measurement selects, this step
**names the next move and stops**. Nothing is fixed here, so no outcome can be a fit.

## Out of scope

No fix of any candidate. No `.rsp` edit. No build. No C-level move. No band change.
Nothing touching `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6, the
substep/chunk loop, `ReassertContacts` or player-car speed on Training is changed; such a
finding is reported as a **"D2 REOPEN CANDIDATE"** row.

## Rules

Muted launch, `MASHED_TITLE` set, `MASHED_WIN_POS=primary-bl`, **never**
`MASHED_NAV_DEMO`. PID tracked, only mine killed. No Frida needed on this leg.
`original/MASHED.exe` untouched; no `unlock_*` patch.
