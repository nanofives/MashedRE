---

# WS-B1 collision-architecture options brief — gate D1 (2026-07-06)

The ROADMAP WS-B / RE_MASTER_PLAN §5 stop-and-ask gate: how should the standalone source the collision + rigid-body behavior that faithful vehicle physics (WS-A) needs? The fork as stated in the plan: **Option A** = vendor real qhull-2002.1 and reconstruct the RW-Physics call surface around it; **Option B** = port the RW-Physics contact subset verbatim (true to F-DoD). This brief lays out both with cited evidence. **No recommendation — the decision is the owner's.** Format mirrors `RENDERER_GATE_BRIEF.md`.

Anchored to MASHED.exe SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Every RVA cited traces to an existing analysis note (verified read-only this session); C-levels are the current `hooks.csv` values.

> **Status note the owner must see first:** a narrower version of this gate was already **RATIFIED Option B on 2026-06-16** (`COLLISION_GATE_BRIEF.md`) *for the contact subset*, and B2/B3/B4 have since been ported (C2). What re-opens D1 is **WS-A8 evidence from 2026-07-01** (`WS_A8_REALPHYS_2026-07-01.md`): the deferred qhull/RW-Physics-body question is now the live blocker, so D1 is really "do we now need **system 2**?" — not "how do we get contacts?" (that is done).

## CURRENT-EVIDENCE (what is known, cited)

**MASHED runs two distinct collision/physics systems** (`COLLISION_GATE_BRIEF.md` headline, live-Ghidra-verified 2026-06-16):

1. **First-party contact-solver system** (0x0046–0x0047 band). Hand-rolled SAT / half-plane tests on `COLLI*.BSP` triangle soup + fixed 4-vertex vehicle hulls. Feeds the WS-A force integrator `FUN_0046ddb0`. Callees are **RW math leaves only — zero qhull, zero 0x55-band.**
2. **RenderWare-Physics 3.7 rigid-body world** (0x55 band + qhull island 0x57c5b0+). A separate physics world `DAT_006ce274` with 4 collision bodies `DAT_006c9a78[0..3]`.

**qhull is load-time only, not per-frame** (`COLLISION_GATE_BRIEF.md` §"single use-site", each RVA verified):
- The only consumer of the `"qhull s Pp"` option string (`0x0062406c`) is `FUN_0057ca30`, which has **exactly one** external caller `FUN_00481e00`.
- The convex-hull build `FUN_0047d3c0` runs **only** when `DAT_0086caa0 == 0x7b`, a flag set by physics-world init `FUN_0047f840` and cleared after a single build (`0x0047eba7`). qhull executes at scene/physics-world **setup**, not per frame.
- The per-tick RW-Physics step `FUN_0047eb30` advances bodies through `FUN_0055dff0/0055ac00/0055deb0` and reads body transforms via `FUN_0057c210` — **never re-enters qhull**.

**The per-frame contact path is qhull-free** and its geometry producer is first-party: `FUN_00538c80` (RW broadphase walk of `COLLI*.BSP`) + callback `LAB_00468b80` fills the terrain batch `DAT_0088e60c` (`WSB2_B3_CONTACT_PORT_MAP.md`). Captured original baseline (`log/wsb_contact_baseline.json`, race game-mode 6): broadphase yields 2–7 tris/query, all 4 wheels register contact, qhull never ran during the race window.

**hooks.csv census of the qhull/RW-Physics band** (~0x0057c–0x005a5, this session; band edges approximate ±a few rows):

| band ~0x0057c5b0..0x005a5820 | count |
|---|---|
| total rows | 246 |
| C1 | 127 |
| C2 | 119 |
| C3 / C4 | **0** |
| tagged `third-party-library[qhull/RW-Physics]` | **246 (100%)** |

Every row in the band is library-tagged, C1/C2, **unported** — consistent with the "~2,116 third-party rows out of scope for porting by policy" (RE_MASTER_PLAN §1).

**Contact/collision functions OUTSIDE the band the race slice actually calls** (all first-party, ported at C2, one C3):

| RVA | name | C | system |
|---|---|---|---|
| `00470c70` | 16-car dispatcher | C2 | shared |
| `004709a0` | VehicleCollisionBroadPhase (substep entry) | C2 | 1 |
| `0046f6c0` | VehicleWheelContactSolver (orchestrator) | C2 | 1 |
| `00538c80` | RW broadphase walk / `LAB_00468b80` producer | C2 | 1 |
| `00468d80` | VehicleTerrainContactSolver | C2 | 1 (B2) |
| `004694e0` | VehicleObjectContactSolver | C2 | 1 (B2) |
| `0046cc40` | WheelTerrainContactClassifier | C2 | 1 (B2) |
| `0046c5f0` | TriangleFaceNormal (leaf) | C2 | 1 (B2) |
| `00469aa0` | VehicleContactScanUpdate | C2 | 1 (B2) |
| `00468b40` | VehicleContactHistoryLookup | **C3** | 1 (B2) |
| `00469df0` | VehicleVehicleCollisionImpulse (car↔car) | C2 | 1 (B3) |
| `0046ddb0` | WheelForceIntegrate (WS-A consumer) | C4* | 1→A |

*A5 core is C4-grounded per CHANGELOG 2026-07-01 (airborne 1-ULP residual U-8991). **System 1 is essentially fully mapped-and-ported (C2, one C3, chain self-test PASS); it is NOT on the critical path for either option — it already exists.**

**The live problem WS-A8 surfaced** (`WS_A8_REALPHYS_2026-07-01.md`, `vehicle_coupling.md`): with `MASHED_REAL_PHYSICS=1` the crash is fixed but dynamics are unfaithful — forward speed is unbounded (saturates the 1500 safety clamp in ~1.5 s) and steering produces negligible heading change. Root cause: the original's stability comes from a **two-body closed loop** — the A3–A6 chain integrates a render position, a stiff spring (PD gain `_DAT_005ccd6c=20.0` @0x005ccd6c, 0.06 s lookahead) drags an **RW-Physics proxy body** to it, the vendor solver integrates the proxy (collision), and readback resets render = body. **The standalone's single-body reduction of this loop diverges / loses the damping + turn coupling.** That proxy body world *is* system 2, and its 4 bodies are the qhull-built collision bodies. **This is what makes D1 a live decision again.**

## OPTION-A — vendor real qhull-2002.1 + reconstruct the RW-Physics call surface

**Scope.** Compile Brad Barber's public-domain qhull-2002.1 (exact version anchored `s_2002_1…` @0x00625e04) and wrap the single `FUN_0057ca30` bridge; then reconstruct **system 2**: physics-world init (`FUN_0047f840`, `FUN_00480340`), the 4-body build chain (`FUN_0047d3c0`→`FUN_004826d0`→`FUN_00481e00` 120-ray cone-cast→qhull→mesh), the per-tick RW-Physics integrator (`FUN_0055dff0/0055ac00/0055deb0` + `FUN_0055c000`, all 0x55-band), the body accessor `FUN_0057c210`, and the coupling bridge `FUN_0047eb30`.

**Effort signals.** The 0x55-band + qhull island is **246 hooks.csv rows, 100% C1/C2, 0 ported** (~165 KB per `[[qhull-rwphysics-island]]`). The wrapper/scene layer (`0047d3c0/004826d0/00481e00/0047f840/00480340/0047eb30/0057c210`) is C2-mapped but unported. qhull itself is vendored not reimplemented, so its bytes are "free," but the **0x55-band integrator that steps the bodies is not** — "the bodies are useless without the 0x55 integrator that steps them" (`COLLISION_GATE_BRIEF.md`). Largest surface of any option.

**Fidelity vs F-DoD.** Highest behavioral fidelity for the *coupling loop* — it reproduces system 2 exactly, which WS-A8 shows is what the car motion depends on. BUT vendored qhull is a **library dependency, not a verbatim port** — it does not satisfy F-DoD for those 246 rows; they stay library-tagged (which is the standing policy for third-party anyway, so arguably no fidelity loss by DoD terms — third-party is out of scope for porting).

**Binary / licensing.** qhull-2002.1 is **public domain** (no copyleft; redistribution clean) — the least of the licensing concerns. Binary grows by the qhull object code + the RW-Physics reconstruction. Pulls a middleware call-surface into the shipping `mashed_re.exe`.

**Risks.** (a) Quickhull is notoriously hard to get bit-right (degenerate/coplanar handling) — vendoring the real thing *avoids* that risk, a point in A's favor. (b) The 0x55-band RW-Physics integrator is itself un-RE'd library code; reconstructing its call surface faithfully is open-ended. (c) You inherit a two-body sim you must keep numerically matched to the original.

## OPTION-B — port the RW-Physics contact subset verbatim (true to F-DoD)

**Scope.** The contact subset (B2 car↔world `FUN_00468d80/004694e0/0046cc40/0046c5f0/0046f6c0`, B3 car↔car `FUN_00469df0`, producer `FUN_00538c80`+`LAB_00468b80`) — **already ported** (`WSB2_B3_CONTACT_PORT_MAP.md`, C2, self-tests PASS). B's remaining scope is a **faithful single-body reduction** of the coupling loop using the recovered constants (PD gain 20.0, 0.06 s lookahead, heading-align law) rather than reconstructing the proxy body — i.e. finish WS-A8 calibration.

**Effort signals.** Contacts are done. The residual is the coupling calibration WS-A8 identified as "open-ended calibration/RE." No qhull, no 0x55-band port. Smallest new surface — but the hard part (reproducing 2-body stability with one body) is **unsolved** and `vehicle_coupling.md` proves the naive verbatim single-body form **diverges within ~1 s**.

**Fidelity vs F-DoD.** The ported contact functions are genuine verbatim transcriptions (F-DoD-eligible once diff'd). BUT the coupling *reduction* is explicitly **not** the original's algorithm — it is a stability-preserving approximation with env-tuned scale constants (`MASHED_CHAINSCALE`, `MASHED_TOPSPEED`, `MASHED_ALIGNRATE`). That is a **permanent, documented divergence** from bit-faithful behavior — the opposite trade from A.

**Binary / licensing.** No new dependency; smallest binary; no third-party code. Cleanest by those measures.

**Risks.** (a) The single-body reduction may never be made to both damp top speed AND convert steer→lateral→heading faithfully (WS-A8 open). (b) If diff-original later proves the rendered motion genuinely needs the proxy body world, **Option A returns for that piece anyway** — B does not foreclose A, and may just defer it (this is exactly what the 2026-06-16 ratification said, and 2026-07-01 is that "later").

## WHAT-CHANGES-DOWNSTREAM

- **WS-A (handling):** A's payoff is A8 becoming a faithful diff (real 2-body loop). B keeps A8 as a calibrated reduction — A8 stays "root-caused but not bit-faithful."
- **WS-C (verbatim AI drive):** gated on WS-A8 regardless. Under A the AI diffs against real physics; under B it diffs against the reduction (the position oracle is only as faithful as the physics beneath it).
- **WS-D (projectiles):** missile/mine trajectories need collision contacts — satisfied by system 1 (already ported) under **both** options. Projectile-vs-body impacts that route through the RW-Physics world would need A.
- **WS-J (impact/skid FX):** driven by contact events from system 1 — available under **both**. No qhull dependency for impact FX.
- **Renderer / everything else:** unaffected — this gate is physics-local.

## OPEN-UNKNOWNS (what a 1-session spike could resolve)

> **#1 ANSWERED 2026-07-06 — YES, LOAD-BEARING.** The spike ran same-day
> (`D1_SPIKE_PROXY_BYPASS_2026-07-06.md`, log/d1_spike_*.json): bypassing
> `VehiclePhysicsWorldStep` 0x0047eb30 in the live original produces a reproducible
> terminal wedge (2/2 runs, ~8–9 s after bypass; 3-wheel grounded, full throttle,
> zero motion, no recovery) plus ~halved heading response — while 2/2 control runs
> never wedge. Failure mode is wedge/no-recovery, NOT the standalone-style runaway,
> so the standalone's divergence is a second, separate gap (the reduction itself).

1. **Is system 2 actually load-bearing for the *rendered* car motion, or only for internal stability?** `COLLISION_GATE_BRIEF` step 2 (the deferred empirical test) was never run to conclusion; WS-A8 strongly implies yes but did not isolate it. A single Frida spike comparing render-struct velocity with the proxy body forced live vs bypassed would settle A-vs-B decisively. **This is the highest-value spike and it is cheap.**
2. **Can a single-body PD reduction be tuned to match original telemetry within tolerance?** WS-A8 left this "open-ended"; one calibration session against captured original telemetry would bound B's feasibility.
3. **Full island caller closure** — `COLLISION_GATE_BRIEF` [UNCERTAIN]: only the `FUN_0057ca30` qhull entry's single caller was proven; the other ~165 KB of the island was not exhaustively caller-swept. Only matters if A is chosen.
4. **Does the 0x55-band integrator have hidden per-frame entanglement** with anything besides the 4 proxy bodies? Assumed isolated; not proven.

## DECISION-INPUTS-SUMMARY

| Axis | Option A (vendor qhull + reconstruct RW-Physics) | Option B (port contact subset + calibrate reduction) |
|---|---|---|
| New port surface | 0x55-band + island: **246 rows, 100% C1/C2, 0 ported** + C2 wrapper layer | contacts **already ported**; residual = coupling calibration only |
| On critical path? | Yes — full system-2 reconstruction | No new collision port; blocker is the (unsolved) reduction |
| qhull | vendored (public domain, clean redistribution), load-time only | not needed |
| Fidelity vs F-DoD | reproduces the real 2-body loop; qhull stays library-tagged | contacts verbatim; coupling is a **documented permanent divergence** |
| Binary / deps | largest; middleware call-surface in ship exe | smallest; no new dependency |
| Key risk | 0x55-band integrator is un-RE'd; open-ended reconstruction | single-body reduction may never be faithful; A may return anyway |
| WS-A8 outcome | A8 becomes a true diff | A8 stays a calibrated approximation |
| Reversibility | — | B does not foreclose A (defers it) |
| Prior ruling | — | **ratified 2026-06-16** for contacts; re-opened by WS-A8 2026-07-01 |
| Decisive cheap test | Open-Unknown #1 spike settles which is needed | same spike |

---

*Sources read this session (read-only): `ROADMAP.md`; `RE_MASTER_PLAN_2026-07.md` §§1,3–5,7; `COLLISION_GATE_BRIEF.md`; `WSB2_B3_CONTACT_PORT_MAP.md`; `WS_A8_REALPHYS_2026-07-01.md`; `vehicle_coupling.md`; `vehicle_physics_cluster.md`; `RENDERER_GATE_BRIEF.md`; `hooks.csv` band census. No files written (read-only worker) — account3 should land this as `re/analysis/COLLISION_GATE_BRIEF_D1_2026-07.md` or append to the existing brief.*
