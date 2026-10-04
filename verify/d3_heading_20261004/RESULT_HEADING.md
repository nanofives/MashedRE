# RESULT — U-9185 item (b): the heading residual is a PORT-ONLY BRIDGE defect. **C2 LIVE, C3 refuted, C1 not reached.**

Pre-registration: `PREREG_HEADING.md`, committed **unrun** at `ed2eba34`.
**No game code, no `.rsp`, no build, no band change, no C-level move.**

## Verdict

> **The port carries TWO heading values for each AI car and they disagree inside the same
> frame.** `g_aib.fwd[v]` (what the AI reads) and the record's `+0x9d4`/`+0x9dc` (the field
> the ORIGINAL's `FUN_0046d510` returns) differ by a median of **0.5214 / 1.1430 / 0.4300
> deg** on cars 1/2/3, against a same-instrument floor of **0.0000 deg** measured on the
> player in the same samples. **This is a port-only bridge defect, not a physics one** —
> the cheap class U-9185 hoped for.
>
> The magnitudes sit in the same band as U-9185's matched-position **body-heading** share
> (**0.9796 / 0.9085 / 0.6506** deg), and **car 2 is the largest on both measures**.

## Two things I registered that the run contradicted — both stated here, neither re-thresholded

### 1. My PREDICTION was WRONG, and the measurement wins

`PREREG_HEADING.md` registered: *"**PREDICTION: C2 is refuted by construction** — the
bridge and the record should agree to float precision (order 1e-7 rad ≈ 6e-6 deg)."*

**Measured: 0.5214 / 1.1430 / 0.4300 deg medians — five orders of magnitude larger.** The
reasoning behind the prediction was sound as far as it went (`BodyOrient_Heading(m) =
atan2(m[10], m[8])`, so `(cos,sin)` does recover `normalize(m[8],m[10])`), but it assumed
the two values are written from the **same** yaw at the **same** time. They are not — see
the mechanism below. **C2 is LIVE.**

### 2. A GATE I REGISTERED WAS ILL-POSED, and I replaced it with a stronger one

**`H-JITTER` as written is not evaluable.** It required the jitter bound to be "at least
10x smaller than the H-C2 difference", but the bound is in **position units**
(0.14–0.19) and the difference is in **degrees** — incommensurable. That is my error in
the pre-registration, not a result.

**Replacement, and it is strictly stronger, not a weakening:** **slot 0 (the player) is the
noise floor**, measured by the same instrument, in the same samples, in **degrees**:
`dang` is **exactly 0.0000** on all 933 of its samples, at every lag. A polling artefact
would corrupt slot 0 too. So the instrument demonstrably resolves exact agreement, and the
AI-slot disagreement is real.

## H-BASE — PASS, all three legs

| leg | result |
|---|---|
| independent channel | `g_aib.pos[v]` moved and `g_records[v]+0x9e4` spans **0 .. ~4477** on every AI slot, inside the same run's `MASHED_AI_STEPDUMP` range |
| liveness | positions change across samples on all four slots (`H_BASE_moved: true`) |
| non-degenerate | speed non-zero on all four slots (`H_BASE_speed_nonzero: true`) |

Addresses were resolved **from the linker map every run**, not hardcoded: `g_records`
**0x00c09b90**, `g_aib` **0x00c09460**. (`sa_boostwatch.py`'s hardcoded `0x00c09b88` is
**stale** against today's map — a hazard worth carrying forward.)

## H-C2 — the discriminator

1622 samples, 933 with both vectors non-zero per slot.

| slot | `dang` p50 | p90 | p99 | max | samples > 10° | > 90° |
|---|---:|---:|---:|---:|---:|---:|
| **0 (player, the floor)** | **0.0000** | 0.0000 | 0.0000 | **0.0000** | 0 | 0 |
| 1 | **0.5214** | 1.734 | 2.556 | 2.5564057 | **0** | 0 |
| 2 | **1.1430** | **66.83** | **124.12** | **176.65** | **327** | **16** |
| 3 | **0.4300** | 1.745 | 2.556 | 2.5564047 | **0** | 0 |

Registered rule: `d ≥ 0.1 deg` → **C2/C3 LIVE**. Satisfied on all three AI slots.

**Car 2 is qualitatively different**, not merely larger: 327 of 933 samples above 10° and
16 above 90°, where cars 1 and 3 never exceed 2.5564°. Cars 1 and 3 share a near-identical
hard cap (2.5564057 / 2.5564047) with only **5 distinct values** above 2.5°, which is the
signature of a quantised rate limit. **Car 2 is also the car U-9185 singled out.**

## C3 (staleness) — REFUTED by an integer-lag fit

The registered rule said the *shape* must select between C2 and C3. The direct test is a
lag fit (memory `a-tolerance-has-no-concept-of-latency`): if the bridge were simply a stale
copy, some integer lag would collapse the difference toward the floor.

Median `|angle(bridge[t]) − angle(record[t−L])|`, degrees, **filtered to the same
both-non-zero population as `dang`**:

| slot | L=0 | L=1 | L=2 | L=3 | L=4 |
|---|---:|---:|---:|---:|---:|
| 0 | **0.0000** | 0.0000 | 0.0000 | 0.0000 | 0.0000 |
| 1 | **0.5214** | 0.5214 | 1.4539 | 2.4114 | 3.5226 |
| 2 | **1.1430** | 1.2032 | 3.5594 | 5.8955 | 8.2216 |
| 3 | 0.4300 | **0.4211** | 1.1932 | 2.0304 | 2.8376 |

**The minimum is at L=0 (L=1 for slot 3, indistinguishable) and it never approaches the
0.0000 floor.** No lag explains it. **C3 is refuted.**

> **Methodological correction made during the run and reported rather than buried:** the
> first lag fit used **all 1622 rows** and silently included zero vectors, where
> `atan2(0,0) = 0` manufactures false agreement — it printed L=0 medians of 0.11 / 0.16 /
> 0.07, five to ten times too low. The table above is the re-run on the filtered
> population. Memory `all-zero-reads-prove-nothing-alone`.

## The mechanism — named, with the responsible writer NOT yet pinned

`TrackRenderer::UpdateCar` sets `a.yaw = io.yaw` at **`:3334`**, straight out of
`Vehicle::VehiclePhysics_StepCar`, which is also where the record's `+0x9d4`/`+0x9dc` were
written (`VehiclePhysicsRun.cpp:1002-1003`, from the same `io.yaw`). **At that instant the
two agree.** But `a.yaw` is then written again, by port-only scaffold code, before
`AiBridgeSnapshot` reconstructs `g_aib.fwd[v] = (cos(a.yaw), sin(a.yaw))` at **`:3729`**:

| site | writer | resyncs the record? |
|---|---|---|
| `:3283` | `a.yaw += 12.0f * in.dt` (spin) | **no** |
| `:3351` | `a.yaw = ry` (off-mesh recovery) | **yes** — `VehiclePhysics_ResetOrientation(v, a.yaw)` at `:3355` |
| `:3393` | `a.yaw += 12.0f * in.dt` | **no** |
| `:3423` | `a.yaw += yerr * min(1, 6·dt)` — gate-ribbon turn-rate limiter, inside `UpdateCar` | **no** |
| `:3717` | `a.yaw = ry` (off-mesh recovery) | **no resync at this site** |

**Evidence-based narrowing, not a guess:** the disagreement **persists** rather than
snapping back, so the firing writer is one that does **not** resync — which excludes
`:3351` and leaves `:3283`, `:3393`, `:3423`, `:3717`. **Which one fires is `[UNCERTAIN]`
and is the next step.**

**U-9185's claim about the limiter is half-confirmed.** It says the `yerr * (6.0f*dt)`
limiter *"sits in the legacy AI v2 `else` branch, which the ported path does not take"*.
That is **correct for `:3683`**, which is inside `TrackRenderer::AiOptionBStep` — reached
only at `:3313` when `!phys`, and `phys` is the default. But there is a **second copy at
`:3423`, inside `UpdateCar` itself**, in the gate-ribbon block, and that one is **not** in
`AiOptionBStep`. Whether its branch is taken by default is **not established here**.

## What this does NOT say

- It does **not** show the port's physics basis is wrong. **C1 was not reached**, because
  C2 fired first. The original was deliberately held out of this leg; this is a port-side
  internal inconsistency.
- It does **not** claim fixing it closes (b). The 2026-10-02 counterfactual matrix had no
  arm passing (b) on any car.
- It does **not** identify which of the four writers fires.

## D2 WATCH

**None.** C1 (the physics basis) was not reached, so nothing here is a physics finding.
Nothing touches `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6, the
substep/chunk loop, `ReassertContacts` or player-car speed on Training. **No D2 code was
read or changed.**

## Next step, as the pre-registration requires this step to name and then stop

**Count which of `:3283` / `:3393` / `:3423` / `:3717` fires**, per AI slot, over the (b)
window. One counter per site, default-OFF, no behaviour change — and car 2 should be
watched separately, since its tail (327 samples > 10°, 16 > 90°) is a different population
from cars 1 and 3. If it is `:3423`, the fix is to stop the gate-ribbon limiter running on
the ported path; if it is `:3717`, the fix is the missing
`VehiclePhysics_ResetOrientation` that its sibling at `:3351` already does.

**Nothing was fixed in this step**, so no outcome here can be a fit.

## What changed (no game code)

| file | change |
|---|---|
| `re/tools/sa_headwatch.py` | **new**, read-only. `ReadProcessMemory` bridge-vs-record heading reader; resolves both symbols from the linker map every run. |

## Hygiene

`mashed_re.exe` spawned and killed **by PID**; no blanket kill by name. Muted,
`MASHED_TITLE` set, `MASHED_WIN_POS=primary-bl`, `MASHED_NAV_DEMO` never used. No Frida on
this leg, no Ghidra session, no build. `original/MASHED.exe` untouched. Nothing pushed.
