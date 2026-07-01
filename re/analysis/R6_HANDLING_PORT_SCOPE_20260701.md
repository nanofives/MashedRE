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

| RVA | hooks.csv | Role (from physics/AI notes) |
|-----|-----------|------------------------------|
| `0x00416250` | ai C2 | **AI control-byte writer** — writes the per-car input array (`[0]/[1]` steer, `[4]` accel, `[5]` brake) that A4 `FUN_00470670` consumes. The AI→physics bridge. |
| `0x00443dc0` | ai C2 | nav lookahead (Option-B faithful lookahead core) |
| `0x00443300` | ai C2 | nav lookahead helper (pairs with 0x00443dc0) |

## Recommended FIRST target: `FUN_00416250` (AI control-byte writer)

Cleanest first increment because its **output is bounded and trivially diffable** (the
8-byte control array on the AI car's record), it runs **every frame for every AI car on
ANY track** (no jump/rare-state needed — unlike the physics airborne branches), and it is
the direct AI→physics-chain seam (so verifying it locks the AI's input to the now-C4
physics chain). Alternatives `0x00443dc0`/`0x00443300` (nav lookahead) are the upstream
producers — port after the writer so the diff chain builds outward from the seam.

## Turnkey port procedure (one function)

1. **Scope in Ghidra** (`ghidra-pool` acquire a slot; `program_close` + release when
   done): decompile `FUN_00416250`, cite every const/offset/RVA (NO-GUESSING), record
   ABI (args/ECX/return), reads/writes. Confirm size is a single-session port.
2. **Author the hook** (`hook-author`): `mashedmod/src/mashed_re/Ai/<Class>.cpp`, naked
   or C verbatim body, `RH_ScopedInstall(…, 0x00416250)`, runtime-toggleable, inline
   RVA comments. Build both targets (`mashedmod\build.bat`) then **restore
   `original/dinput8.dll`** (build.bat moves it aside).
3. **Diff-original** (the AI runs on opponent slots 1..3 automatically): install the hook
   live via `scenario_launch.py --track 0 --cars 4 --hooks 0x00416250` (any track — AI
   drives everywhere; Arctic idx 0 is fine, no jump needed). Compare the written control
   bytes MINE-vs-ORIGINAL per frame (in-process A/B self-test in the hook, same pattern as
   `PhysicsChainHooks.cpp` A4/A5/A6a/A6b; or `re/frida/run_diff.py` with a registry
   entry). GREEN (bit-identical control bytes across a race) → C3.
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
