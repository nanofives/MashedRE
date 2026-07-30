# Promotion-method efficiency scorecard — 2026-07-29

Surface-level pilot of every promotion/verification method raised in the
bottleneck investigation, each run once and measured on the same yardstick.
Goal: find and rank the throughput levers, not to promote in bulk yet.

Boot budget spent this session: **4 game boots** (1 STATE batch + 3 Stalker,
2 of them concurrent) against the ~15-boot/session d3d9-wedge ceiling.

## Results table

| # | Method | Pilot run | Setup cost | Yield / cost | Boots | Addressable pool | Verdict |
|---|--------|-----------|-----------|--------------|-------|------------------|---------|
| 1 | **STATE batch (many hooks / 1 boot)** | 5 hooks (+1 repeat control) in ONE boot | 0 (in production) | **4 GREEN / 1 boot**, 0.3s in-race window used, "1380 hooks/min of window" | 1 | ~947 STATE rows | **Ship it.** The lane works on real candidates; boots are no longer the cap. |
| 2 | **Offline emulation — Unicorn differ** | 12 fns, 8 655 vectors | high (per-kind onboarding) | 12/12 PASS in **2m23s, ZERO boots**, parallelizable, account2-viable | 0 | math/leaf fns fitting a kind (~dozens now; more per new kind) | **Highest ceiling.** No boot, no nav flake, no GPU wedge. Growth = generalize kinds. |
| 2b| **Offline replay (compiled TU)** | 12 fns, both modes | med | 24/24 PASS in **0.3s** | 0 | same as Unicorn | Fastest iteration loop in the project; pairs with #2. |
| 3 | **Stalker dynamic write-surface** | 3 mutators (0x495110, 0x5b8080, 0x49f210) | med (new tool, this session) | resolved EVERY statically-opaque indirect; full write set per call in ~5–7s | 1/target | the **1 126 "indirect-unreachable" mutators** static can't touch (77%) | **Breakthrough.** Directly attacks the largest blocked pool. See detail below. |
| 4 | **Live-argument capture** | mechanism in-runner (CAPTURE_JS); not separately booted | 0 (built 07-29) | turns pointer-arg false-fails into real verdicts; same boot as #1 | 0 extra | pointer-arg STATE hooks (a "large share" of the 947) | **Enable by default** in the STATE batch; near-zero marginal cost. |
| 5 | **arg_type shape-inference (account2)** | 30 render rows inferred | 0 (worker, $2.23, off-quota) | 5 rows → existing handlers; 17 → new-handler shapes; 8 no-plate | 0 | 288 STATE + 149 MANUAL render rows | **Keep delegating.** Free triage; but see the sober finding below. |
| 6 | **Multi-instance parallel boots** | 2 concurrent MASHED, independent Stalker captures | 0 | **no interference** — B captured clean in 5s while A ran concurrently; A failed only on independent nav-flake (NOT-FIRED, wedged depth=2) | 2 (parallel) | multiplies any boot lane | **Viable but flake-limited.** Instances don't contend; parallel speedup is bounded by the ~50% nav success rate, not by co-running. |

## Method 3 detail — the Stalker result is the headline

`scripts/write_surface.py` (static) blocks a mutator the instant its call tree
contains a register/vtable dispatch: **1 126 of 1 446 mutators (77%)** classify
"indirect — NOT reachable" (commit b77f6b42), capping the snapshot/restore A/B
lane at ~142 targets. The new `re/frida/stalker_write_surface.py` follows ONE
natural in-game call with Frida Stalker and records the effective address of
every writing instruction through the *concrete* runtime dispatch.

Measured, three targets static had flagged with opaque indirects:

- **0x00495110** — static: 1 ABSOLUTE write + 1 opaque indirect at 0x004950c8.
  Dynamic: the indirect **resolved to 0x76ad85f0 (a system DLL call, no game
  write)**; the ONLY image write is 0x7f1030 — matching the static absolute.
  Write surface fully known → snapshot/restore viable. The "opaque dispatch"
  was a false blocker.
- **0x0049f210** — dynamic: 2 heap writes; indirect 0x49f21f resolved to image
  fn 0x49de80. Surface knowable, pointer-relative (needs the arg to restore).
- **0x005b8080** — NOT-FIRED: nav wedged at depth=2 (the ~50% flake), so the
  audio target never executed on this path. No capture; retry on a fresh boot.

Implication: a large fraction of the 1 126 "unreachable" mutators are blocked
only by static analysis's inability to *enter* an indirect call whose runtime
destination is either a leaf system call (no game write) or a resolvable image
function. Stalker turns "unknown surface" into an observed, unionable surface.
The right pipeline is **static scaffold + dynamic fill**, then guard the union
at restore. Next step: batch this over the 85 exercised mutators (one short
capture each, several per boot) and re-classify how many become reachable.

## Method 5 detail — a sober correction

The worker inferred arg_types for 30 of ~291 render STATE rows. Only **5 fit an
existing handler**; **17 need a NEW diff_template.js handler** (particle
ring-buffers, RW stream readers, vtable dispatch on live globals) and **8 have
no plate**. So arg_type inference is real triage but it does NOT by itself
unlock hundreds of flat-lane promotions — most render STATE rows are *procedures
needing live state*, echoing the 07-29 finding that safe and exercised are
anti-correlated. Its value is routing (which lane, which handler to build),
delivered off-quota. Keep it as a cheap classifier, not a promotion engine.

## Ranked levers (what to remove first)

1. **Roll the STATE batch into production** (Method 1) — proven on real rows;
   this is the immediate throughput win, ~947 rows addressable.
2. **Batch Stalker over the exercised mutators** (Method 3) — the only method
   that grows the *addressable* pool (attacks the 77% static can't reach).
3. **Enable live-arg capture by default** in the STATE batch (Method 4) — kills
   the pointer-arg false-fail class at ~zero marginal cost.
4. **Generalize veccap kinds** (Method 2) — every new kind onboards a family of
   math leaves into a zero-boot, parallel, account2-viable lane.
5. **Keep arg_type inference on account2** (Method 5) — free routing/triage.
6. **Parallelism** (Method 6) — real but secondary: instances don't contend,
   so it multiplies throughput up to the nav success rate. Fix nav flakiness
   first (it caps every boot lane, serial or parallel) and parallelism compounds
   the gain. Cheapest attack on the flake: harden `drive_to_results`' depth=2→3
   transition (both A and the STATE-lane voids died there).

## Follow-up: hardening the depth 2->3 transition (the suspected flake)

Measured the transition in isolation with `re/frida/nav_reach_probe.py` (boots
stock, drives title->2->3, reports reached/presses/secs, kills its own pid):

| mode | isolation (3 boots) | presses | contention (2 concurrent) |
|------|--------------------|---------|---------------------------|
| current  | **3/3 reached depth 3** | 5 | 1/2 (other threw on simultaneous spawn) |
| hardened | **3/3 reached depth 3** | 4 | (not re-run — see finding) |

**Finding — the transition is NOT the ~50% flake.** In isolation it is
deterministic (6/6 across both modes). The failures attributed to it were
under multi-instance CONTENTION: the parallel Stalker A run wedged at depth=2
while B ran, and a 2-concurrent probe pair had one instance throw a Frida
`spawn`/attach exception (a spawn RACE, not a nav failure) while the other
navigated normally. So the real parallelism blocker is **simultaneous
`frida.spawn`**, not this menu step.

**Still shipped** the closed-loop `Nav.advance_past_load_modal` (statenav.py)
and wired it into every drive site (`run_diff_scenario.py`, `statenav.py`,
`stalker_write_surface.py`, `verify_scoring_hooks.py`): it removes the
fixed-sleep race, has 2.5x the retry patience (12x2.5s vs 6x2.0s) for a laggy
transition, and cost one fewer confirm press with no regression (3/3). It is a
robustness improvement, **not** a proven flake fix — I could not reproduce a
depth-2->3 isolation failure to fix.

**Real next lever for parallelism: stagger spawns.** Two `frida.spawn` calls
within the same instant collide. A multi-instance driver must serialise the
spawn+attach (a few seconds apart) even though the games then run concurrently.
That, not the menu transition, is what caps Method 6.

## Follow-up: staggered multi-instance driver — built, and it exposes a ceiling

`re/frida/multi_state_driver.py` partitions a hook set into K chunks, spawns K
games **staggered** (default 12s, past each spawn+attach), runs an unmodified
`run_diff_scenario_batch.py` per game, aggregates verdicts, and kills by pid.

Validated K=2 on 4 hooks (2 per game):
- **Spawn collision GONE** — both games spawned, navigated, and produced
  verdicts; zero `frida.spawn` errors. So staggering IS the fix for the
  contention failure seen earlier.
- Aggregate **3/4 GREEN**, matching the single-boot results. Hygiene clean.
- **But speedup was only 1.12x** (80s wall vs ~90s serial for 2 boots).

**Why the speedup is marginal — and it's structural, not a bug:**
1. Each instance's cost is dominated by the **~40s boot+nav** to establish live
   state, not by the sub-second diffing. Two games booting at once **contend for
   the GPU** during exactly that heavy phase, so neither runs at full speed —
   the overlap window shrinks to almost nothing.
2. The STATE batch **already amortizes** the one expensive boot across up to ~28
   hooks. A second game buys a second live-state window but pays a second full
   boot+nav tax, and the two taxes overlap poorly.

**Verdict on parallelism:** the driver is correct and production-ready for
hygiene/aggregation, but multi-instance is a **minor** throughput lever for the
STATE lane specifically — the boot+nav cost and its GPU contention cap the gain
near ~1x for realistic chunk sizes. It only pays when per-game WORK is long and
GPU-light relative to boot (e.g. a batch Stalker sweep where each capture needs
its own boot anyway, or CPU-bound offline lanes — which don't boot at all and
are the better parallel target). The real throughput order is unchanged:
**STATE batch (Method 1) > Stalker reachability (Method 3) > offline emulation
(Method 2)**; parallel boots sit below them.

## Follow-up: batched Stalker reachability sweep — the yield, and a refinement

`re/frida/stalker_write_surface_batch.py` arms ALL targets before resume with a
single global follow-lock (one Stalker.follow active at a time), then captures
each mutator's first natural call across the whole boot->race->dwell window.
The first design (arm one at a time, 2.5s each, in-race only) captured **0/12**
— it missed every `exercised_prerace` mutator whose firing was already past.
Arm-all-before-resume fixed it and did NOT stall navigation.

Measured, one boot (~40s), 12 smallest exercised mutators:

| outcome | count | meaning |
|---------|-------|---------|
| **captured** | **10/12** | write surface + all indirects resolved |
| not-fired | 2 | 0x005b8080, 0x00491780 — didn't fire this run's window |
| **REACHABLE-now** (writes are ALL image globals) | **1** (0x00495110) | directly snapshot/restore-able by absolute address |
| heap-relative | 9 | writes include heap EAs (per-object state) |

**Yield: ~10 captured / boot; ~83% capture rate.** Full 85-mutator sweep ≈ **9
boots** (fewer if the per-boot batch grows — 12 armed interceptors did not stall
nav, so 20-30/boot is worth trying).

**The refinement (important, and it tempers the reachability claim):** static
analysis called these "indirect-unreachable," and Stalker DID resolve every
indirect (into system-DLL leaves that make no game write, or image functions).
But the dynamic capture shows most exercised mutators **write HEAP, not
globals** — e.g. 0x0047b860 touched 1642 distinct heap EAs, 0x004039c0 1255.
Only 1 of 10 writes exclusively to fixed globals.

So "reachable" splits in two:
- **Global-write mutators** (~10% here): trivially snapshot/restore by absolute
  address. Small set, cheap wins.
- **Heap-write mutators** (~90% here): still restorable, but only via a
  **per-run EA snapshot** — you must capture the exact heap addresses each run
  (which this tool produces) and restore those, exactly what the window-snapshot
  A/B orchestrator lane already does. NOT hardcodable, higher effort per target.

Net: batched Stalker cheaply (~10/boot) turns "static-unreachable" into "surface
KNOWN," which is real progress, but it does not make 77% of mutators *cheaply*
promotable — the heap-writers still need the per-run-snapshot orchestrator, one
bespoke driver per family. The genuinely cheap wins are the ~10% pure-global
subset; the sweep's main value is *classifying* which mutators fall in which
bucket so effort is spent where it pays.

Artifacts: `re/analysis/plans/exercised_mutators.txt` (all 85, size-sorted),
`re/analysis/plans/stalker_ws_batch_demo12.json` (the demo capture).

## Boots ledger

- STATE batch (5+1 hooks): pid 16648 — GREEN 4/6, REUSABLE, sentinel stable.
- Stalker 0x00495110: pid 30884 — fired, 7s.
- Stalker 0x005b8080: pid 40256 — NOT-FIRED (nav wedged depth=2), concurrent with B.
- Stalker 0x0049f210: pid 40716 — fired, 5s, concurrent with A → **no interference**.
- Nav probes (menu-only): 6 isolation + 2 contention boots.
- STATE confirmation (hardened drive): pid 35720 — 3/4 GREEN, populated race.
- Multi-instance K=2: game pids 35804 + 30368, staggered 12s — 3/4 GREEN, 1.12x.
- Batch Stalker (arm-one, in-race): 0/12 — design miss (missed prerace mutators).
- Batch Stalker (arm-all-before-resume): pid 40212 — 10/12 captured, 1 reachable-now.
