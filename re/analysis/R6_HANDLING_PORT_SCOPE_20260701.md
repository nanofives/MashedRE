# R6 handling/AI verbatim-port lane — scoped kickoff (2026-07-01)

Selected via `AskUserQuestion` 2026-07-01 as the next autonomous-loop lane after the
physics chain reached full C4. Goal: convert the standalone's behaviorally-faithful (but
diff-unverified, C2) AI/handling functions into diff-verified verbatim ports (C2→C3/C4),
one function per bounded, verifiable increment. Anchored MASHED.exe SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.

## Why these functions (demand-driven)

R6 (race loop) already ports the camera/elimination/points/car-selection verbatim
(DIVERGENCE_LEDGER_3D #4/#5/#6/#3) and runs real Option-B opponent AI
([[project_wsc_opponent_ai_optionb]]). But the AI's own core functions are still **C2** —
the standalone reimplements their behavior (AiController.cpp / AiTargeting.cpp /
AiStandalone.cpp) without a bit-identity diff against the original. Ledger #8 ("AI
driving … the original follows AI.BSP with its own controller [SCAFFOLD → RE]") is the
open item this lane closes with real diffs.

| RVA | hooks.csv | Size | Role (from physics/AI notes) |
|-----|-----------|------|------------------------------|
| `0x00443300` | ai C2 | **306 B** | **Catmull-Rom spline eval** — the AI racing-line geometry helper (Ghidra-verified below). Pure leaf. |
| `0x00416250` | ai C2 | ~2.0 KB | **AI control-byte writer** — writes the per-car input array (`[0]/[1]` steer, `[4]` accel, `[5]` brake) that A4 `FUN_00470670` consumes. The AI→physics bridge. |
| `0x00443dc0` | ai C2 | ~3.5 KB | nav lookahead core (consumes the spline eval; big) |

## Recommended FIRST target: `FUN_00443300` (Catmull-Rom spline eval)

Ghidra-verified 2026-07-01 (Mashed_pool0, read-only, MASHED.exe BDCAE0…). The **ideal
first verbatim port**: a 306-byte **pure-math leaf** — no callees, no PRNG, deterministic,
bounded diffable output (2 floats). Far cleaner than the 2 KB writer / 3.5 KB nav core;
it's the geometry primitive the whole AI lookahead is built on, so porting it first builds
the diff chain from the leaf outward (`0x00443dc0` consumes it; `0x00416250` is the far
output seam).

**Full spec (transcribe verbatim):**
```
void FUN_00443300(int splineBase, int segIdx, float t, float* outXZ)
```
- Control points: `splineBase + idx*8` (2 floats: X at +0, Z at +4); count at
  `*(int*)(splineBase + 0x200)`. Uses the 4 wrapped points P0..P3 = indices
  [segIdx-1, segIdx, segIdx+1, segIdx+2] mod count (the decomp's `pfVar1..pfVar4`).
- `t` clamped to `[DAT_005d757c=0.0, _DAT_005cc320=1.0]` → `fVar6`; `fVar7 = fVar6²`.
- Output = the STANDARD Catmull-Rom (tension 0.5), per component:
  `out = 0.5 * ( 2*P1 + (P2-P0)*t + (-P0 + 3*P1 - 3*P2 + P3)*t³ + (2*P0 - 5*P1 + 4*P2 - P3)*t² )`
  (X uses `+0` floats, Z uses `+4` floats; `outXZ[0]`=X, `outXZ[1]`=Z).
- **Exact constants (Ghidra memory_read, cite these):** `_DAT_005cc31c`=3.0
  (0x40400000), `_DAT_005cc320`=1.0 (0x3f800000), `_DAT_005cc32c`=0.5 (0x3f000000),
  `_DAT_005cc358`=5.0 (0x40a00000), `_DAT_005cc35c`=4.0 (0x40800000),
  `DAT_005d757c`=0.0 (0x00000000).
- **x87 note:** it's polynomial float32 math with no transcendentals — likely bit-exact
  on a straight C transcription, but if a self-test shows a 1-ULP residual, keep the
  intermediates in x87 float10 (the FUN_00443300 evaluation order above) per the physics
  lane's [[project_phys_chain_float10_methodology]] gotcha.

**Verify:** in-process A/B self-test in the hook (snapshot `outXZ`, run MINE vs original
via an Orig-trampoline, compare the 2 output floats), installed live via
`scenario_launch.py --track 0 --cars 4 --hooks 0x00443300` (AI cars call it every frame —
no jump/rare state needed). GREEN over a race → C3.

Then port `0x00443dc0` (nav core, consumes this) and `0x00416250` (control writer, the
output seam) — building the AI diff chain outward from the leaf.

## Turnkey port procedure (one function)

1. **Scope in Ghidra** (`ghidra-pool` acquire a slot; `program_close` + release when
   done): decompile `FUN_00416250`, cite every const/offset/RVA (NO-GUESSING), record
   ABI (args/ECX/return), reads/writes. Confirm size is a single-session port.
2. **Author the hook** (`hook-author`): `mashedmod/src/mashed_re/Ai/<Class>.cpp`, naked
   or C verbatim body, `RH_ScopedInstall(…, 0x00416250)`, runtime-toggleable, inline
   RVA comments. Build both targets (`mashedmod\build.bat`) then **restore
   `original/dinput8.dll`** (build.bat moves it aside).
3. **Diff-original** (the AI runs on opponent slots 1..3 automatically): install the hook
   live via `scenario_launch.py --track 0 --cars 4 --hooks 0x<rva>` (any track — AI drives
   everywhere; Arctic idx 0 is fine, no jump needed). Compare the hook's output MINE-vs-
   ORIGINAL per frame (in-process A/B self-test in the hook, same pattern as
   `PhysicsChainHooks.cpp` A4/A5/A6a/A6b; or `re/frida/run_diff.py` with a registry entry).
   For `FUN_00443300` the compared output is the 2 `outXZ` floats. GREEN across a race → C3.
4. **`re-classify`** C2→C3 (C4 needs the canonical-scenario installed-hook diff, which
   this already is if run with the JMP live in a real race). Update hooks.csv +
   CHANGELOG in one transaction.

## Verification vehicle (already available)

- `re/frida/scenario_launch.py` — warps into a race on any track, installs a named hook
  live (`--hooks 0x00416250`), enables the physics self-test env. The AI drives opponent
  slots with no input injection needed (they race on their own).
- In-race player input override (if the player car's writer must be exercised):
  `FUN_00497310` return override (nav_agent.js method) — the injector at
  [[project_feel_calibration_injector]] (descriptor `0x007f1038`).
- Kill only spawned PIDs; boot via subprocess+attach, never `frida.spawn` on MASHED.exe.

## Guardrails

NO-GUESSING (cite RVAs; `ghidra-pool` + release); MASHED multi-instance (kill only your
PIDs); `py -3.12 scripts/diag.py doctor` at session start; branch `ws-r6-handling-ports`,
verify-before-merge; restore `original/dinput8.dll` after every build.

## Note: CLAUDE.md roadmap section is STALE

CLAUDE.md still says "active work is R1 … R2 (menu completion)" with "101 suspect C4
rows" — but `ROADMAP.md` marks R1/R2/R3 **DONE** (2026-06-09/10) and R1's
`C4_REVALIDATION.md` hit its exit criterion (0 suspect tags). The live frontier is
R4/R5/R6. Worth updating CLAUDE.md's "Current phase" paragraph.
