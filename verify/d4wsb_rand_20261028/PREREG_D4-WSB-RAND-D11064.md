# PREREG D4-WSB-RAND-D11064: standalone CRT rand rebind for FUN_00564310 (D-11064)

Committed alone, before any arm runs. Base: `orch/integration` @ `d92bb3a5e`.
Section constraint this run: the original MASHED.exe does not boot (no audio endpoint, AV at
`0x005baf48`), so every leg here is standalone or static. Nothing here can move a C-level.

## 1. Static facts the change rests on (MASHED.exe.unpatched, capstone, 2026-10-28)

- CRT `rand` = `0x005c229b..0x005c22bc`: `call 0x004a8828` (`_getptd`), `imul ecx,ecx,0x343fd`
  (`0x005c22a3`), `add ecx,0x269ec3` (`0x005c22a9`), store back to ptd`+0x14` (`0x005c22af`),
  return `(state >> 16) & 0x7fff` (`0x005c22b4`, `0x005c22b7`).
- CRT `srand` = `0x005c228e..0x005c229a`: `call 0x004a8828`, `[eax+0x14] = arg` (`0x005c2297`).
- Initial state is 1 per thread block: `_getptd` stores `1` to `[esi+0x14]` at `0x004a8870` after
  allocating the block (`0x004a884b`). The same store is at `0x004a8820` and `0x004a8a52`.
- `E8 rel32` callers of `rand` in `.text`: 2. `0x004c03b4` (in `FUN_004c0330`, hooks.csv
  `third-party-library[lua-4.0]`, no direct callers, so a Lua C function) and `0x00564317` (in
  `FUN_00564310`). Callers of `srand`: 1, `0x004c0502` (Lua band). So in the original the octant
  pick shares one RNG state with the Lua library only.
- Census (dep D4-WSB-CENSUS, `verify/wsbcensus_20261026/wsb_census.tsv:128`): `0x00564310` is EXEC on
  the **original**, phases `race_load,race_run`, 3/3.
- Standalone reach (static): the port body is reached only through `FUN_00564c80` <- `FUN_0055ab30`
  (RwpSolverGlue5.cpp:102) <- the island root `FUN_0047e9c0` <- `RwpSceneStepWrapper` (hook only,
  RwpIntegrator.cpp:488) and `VehicleCouplingBridge` (calls `0x0047ea40` by RVA, header says inert on
  the exe). No `StandaloneThunks_Install` targets `0x0047e*`, `0x0047f*`, `0x0055*` or `0x0056*`. On
  the exe `0x005c229b` is inside `g_b17_low_arena_pad` (exe_main.cpp:254, zero `.bss`, not
  executable), so a default-build call would fault. Prediction P2 below tests this.

## 2. Change (one TU, Collision/RwpSolverLeaves1.cpp, no new TU, no rsp edit)

- Knob `MASHED_OWN_RAND_564310` (exe only: code under `#ifdef MASHED_STANDALONE`). Unset or `0`:
  unchanged, the call still goes through `s_rand_orig = 0x005c229b`.
- Set: the call goes to a port-owned LCG with the original's constants and initial state 1.
- Seed-state policy: **one port-owned state, initial 1, not the UCRT `std::rand()`**. Reason: in the
  original the only other consumer of that state is the Lua library (`0x004c03b4` / `0x004c0502`).
  On the standalone, UCRT `std::rand()` is shared with `Ai/AiStandalone.cpp` (a documented
  approximation the original does not have), so binding to it would interleave calls the original
  never makes. A future Lua `math.random` / `randomseed` port must bind the same state. The original
  keeps the state per thread. The port keeps one process-wide state. `[UNCERTAIN]` that every
  original call of `0x00564310` runs on one thread: the census records first hits, not thread ids.
- Diagnostic `MASHED_OWN_RAND_LOG=<path>` (knob on only): one `selftest` line at startup with the
  first 8 outputs of a scratch state seeded 1, then one line per call for the first 64 calls.
- `.asi` path: the knob code is compiled out. The A/B path still calls `0x005c229b`, so RNG state stays
  shared with MASHED.exe.

## 3. Arms and predictions

| id | arm | prediction |
|---|---|---|
| P0 | `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` | both targets build, exit 0 |
| P1 | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (default) | equal to `merge_train.json` refs: G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664` |
| P2 | `run_arms.py H_ORAND B_ORAND_K0 B_ORAND_K1 B_ORAND_K26` (knob on + log) | hashes equal to P1. Each log has the `selftest` line and **0** call lines |
| P3 | offline: Python LCG from the cited constants, seed 1 | `41 18467 6334 26500 19169 15724 11478 29358`, equal to every P2 `selftest` line |

Reading:
- P1 met = default unchanged (gate). P1 missed = RED, revert.
- P2 met = the knob is correct and inert today: the standalone does not execute the island, so no
  default run can show a difference. P2 with call lines > 0 = the standalone reaches the island, and
  the default build would then be calling non-executable memory. That is a finding, reported as is.
- P3 is the only check of the LCG values. Bit identity of the octant pick vs the original needs an
  original-side run (BLOCKED this run, no C3/C4 claim).

Flip (default ON) is not part of this step. If it is recommended, it goes in the RESULT as a decision.
Raw arm data stays in `D:/MashedWT/D4-WSB-RAND-D11064_data`. Private root `D:\MashedWT\D4-WSB-RAND-D11064_root`.
