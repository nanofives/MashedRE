# Session prompts — wave 8 (make physics MOVE + verify). Paste one per session.

Generated 2026-06-17 after WS-PHYS-CRASH-FIX. Verify+runtime pivot continues (renderer =
Track::World spike, ratified). COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: **Frida/boot/
standalone-runtime = MAIN tree only**; **analyzed Ghidra clones** (pool6/11). NO-GUESSING;
commit as nanofives + trailer; guard `original/`. **NOTE: subagent type `fork` is gone — use
`general-purpose` with self-contained prompts.**

STATE: MASHED_REAL_PHYSICS no longer crashes (RW fast-sqrt LUT guarded → std::sqrt fallback
standalone; .asi keeps the bit-identical LUT path, C4 leaves intact). The chain RUNS and
COMPUTES force (A4 +0x1a8=16.93, A6a +0x9b14 accum) but **the car doesn't move (speed ~0)**.
Root factors (from WS-PHYS-CRASH-FIX, re/analysis/PHYS_SMOKE_2026-06-17.md):
- **+0x278 grounded-count is a FIELD-MAP BUG** — the drive force aliases/overwrites it.
- **+0x9e0 grounded-int never reaches 0x40800000 (=4.0)** the suspension-velocity blocks gate on.
- A3 mass init makes the linear-integration term tiny.
Terrain soup is live (worldTris=3047). Steer still kinematic ([U-A8-STEER]). 0 fns C4. AI bridge
never runtime-tested (now also LUT-safe).

DEP ORDER: WS-A-VERIFY-3 + WS-PHYS-MOTION (coupled — the diff finds the field bugs) → SMOKE-3 ;
WS-AI-VERIFY ; WS-A8-STEER ; tails.

---

## WS-A-VERIFY-3 + WS-PHYS-MOTION — telemetry diff that ALSO fixes motion (LEAD, MAIN TREE). Pool: dev .asi + Frida + pool11
> The scenario-attach canonical-race **telemetry lane** is now the key tool: it pinpoints exactly
> where our record diverges from the original — which IS the motion bug. Steps:
> 1. In the dev .asi: ABI shims for the register-based originals (A4 EAX=record, A5 EDI=record),
>    bind our 10 globals to the original live `_DAT`, RH_ScopedInstall LIVE, scenario-attach a
>    canonical race, **per-frame diff the 0xd04 record fields** original-vs-reimpl (vel +0x9b0,
>    fwd +0x9d4, ang-vel +0x9bc, **grounded-count +0x278**, **grounded-int +0x9e0**, speed +0x9e4,
>    torque +0x1a8/+0x26c), hot-path-sampled one fn at a time.
> 2. The diff will expose: the **+0x278 field-map error** (re-check vehicle.md against the original
>    — which int is grounded-count vs which holds drive force; fix the offset in our ports), the
>    **+0x9e0 grounded-state** machine (why the original reaches 4.0 and we don't — the per-wheel
>    contact-flag write), and the **A3 mass** init (does our +0x50/+0x54/+0x58 match?).
> 3. FIX the ports so the per-frame record matches the original; resolve [U-A6A-FLOAT10].
> 4. Promote each now-GREEN fn to C4 via re-classify (inline-JMP live; no synthetic-bypass overclaim).
> Update the audit. This single lane closes the C4 debt AND makes the car move.

## WS-PHYS-SMOKE-3 — confirm the car drives (MAIN TREE). Pool: standalone run
> After MOTION fixes: MASHED_REAL_PHYSICS=1 race; confirm speed ramps with throttle, brake works,
> the car suspends + stays on track, stable. Screenshot + pos/vel. ON-vs-OFF. Declares "physics
> drives" ✓.

## WS-AI-VERIFY — AI runtime (MAIN TREE). Pool: standalone run
> MASHED_REAL_AI=1 race: does it run (LUT now safe) + do opponents follow the AI%d.AI line? Resolve
> [U-C-STEER-MAG]/[U-C-RATE0/1]; fix ctrl→motion. Screenshot opponent paths vs spline.

## WS-A8-STEER — map steering into the chain. Pool: Mashed_pool11 (worktree OK)
> RE the input-descriptor steer field (&DAT_007f1038 + map*0x13) → wheel steer-angle (+0x3c) path
> (unmapped; FUN_00470670 reads accel/brake/gate only) and wire it so physics-steering is real.
> Build green; runtime-verify in a follow-up smoke.

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails
> D-VISUAL: powerup visuals on the spike. J2: collision FX from live contacts. E-POLISH: vehicle
> lighting (ledger #9) + HUD on the spike. After the physics race drives + is verified.

---
## Recommended wave-8
**8a (MAIN tree, lead):** WS-A-VERIFY-3 + WS-PHYS-MOTION (one coupled session — the telemetry diff
finds the +0x278/+0x9e0/mass divergences, fix them, C4 the leaves) → WS-PHYS-SMOKE-3.
**8b (parallel after motion):** WS-AI-VERIFY (main tree) ‖ WS-A8-STEER (Ghidra). Then tails.
GOAL: the standalone drives a real-physics race (speed ramps, suspends, on-track) with the chain's
C4 debt closing fn-by-fn — physics provably the game.
