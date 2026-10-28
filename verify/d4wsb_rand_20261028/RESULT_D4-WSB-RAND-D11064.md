# RESULT D4-WSB-RAND-D11064: standalone CRT rand rebind for FUN_00564310 (D-11064)

**Verdict: DECISION.** Every PREREG prediction (P0-P3) is met. The rebind ships default OFF as
`MASHED_OWN_RAND_564310`, and the default gates are unchanged. On the standalone the knob does nothing
today because the island is not executed there: 0 calls in 4 knob-on arms. Flip recommendation in section 5.

PREREG: `verify/d4wsb_rand_20261028/PREREG_D4-WSB-RAND-D11064.md`, committed alone as `adcaa217d` before
any arm. Base `orch/integration` @ `d92bb3a5e`.

## 1. Dependency: is the island executed?

- **Original: yes.** D4-WSB-CENSUS (`verify/wsbcensus_20261026/wsb_census.tsv:128`): `0x00564310` EXEC,
  phases `race_load,race_run`, 3/3. The census result says so too (RESULT_D4-WSB-CENSUS.md:64).
- **Standalone: no.** Static (PREREG section 1): the only route into the port body goes through the
  island root `FUN_0047e9c0`. On the exe nothing reaches that root. Measured: 0 `call` lines in all 4
  knob-on logs (section 3, P2). The census covers the original only, so the rebind changes code that
  the original runs and the default standalone build does not.

## 2. Change

`mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp` (one TU, no new TU, no `exe_sources.rsp` /
`asi_sources.rsp` edit, so link order does not change):

- `#ifdef MASHED_STANDALONE`: `MASHED_OWN_RAND_564310` set routes the `_rand()` call in `FUN_00564310`
  to `OwnRand()`, a port-owned copy of the original CRT rand. It uses the same LCG (`*0x343fd`
  `0x005c22a3`, `+0x269ec3` `0x005c22a9`, `>>16 & 0x7fff` `0x005c22b4`/`0x005c22b7`) with initial state 1
  (`_getptd` stores 1 to ptd`+0x14` at `0x004a8870`). Unset or `0` keeps the call through
  `s_rand_orig = 0x005c229b`.
- Seed-state policy: one port-owned state, initial 1, **not** UCRT `std::rand()`. The only other
  consumer of the original state is the Lua library (`rand` caller `0x004c03b4` in `FUN_004c0330`,
  `srand` caller `0x004c0502`). The exe's `std::rand()` is also used by `Ai/AiStandalone.cpp`, an
  approximation the original does not have, so sharing it would mix in calls the original never
  makes. A future Lua `math.random` / `randomseed` port must bind `s_own_rand_state`.
- `MASHED_OWN_RAND_LOG=<path>` (knob on only): a `selftest` line at startup, then the first 64 calls.
- `.asi`: all knob code is compiled out. The string `MASHED_OWN_RAND_564310` is absent from
  `mashed_re_dev.asi` and present in `mashed_re.exe`. The A/B path still calls `0x005c229b`.
- `verify/savelink_20261023/run_arms.py`: new `ORAND` stack (knob + log). No other change.

## 3. Arms (standalone only, private root `D:\MashedWT\D4-WSB-RAND-D11064_root`, test slot + `mashed_lock.py run --label D4-WSB-RAND-D11064`)

Build P0: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, `=== Build OK ===`, rva-lint and
asm-lint OK. exe `e9835c0f`, asi `266098fb` (not deployed). The lock waited for a merge-train solo
window, then ran the 8 arms back to back. All 8 `exit=0 self_exited=True`.

| id | arm | result | prediction |
|---|---|---|---|
| P1 | H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | refs `5943c5f1/364350ae/52052890`, met |
| P1 | B_DEF_K0 / K1 / K26 | `3a03670c` / `9fcdbd3a` / `53778664` | refs `3a03670c/9fcdbd3a/53778664`, met |
| P2 | H_ORAND step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | equal to P1, met |
| P2 | B_ORAND_K0 / K1 / K26 | `3a03670c` / `9fcdbd3a` / `53778664` | equal to P1, met |
| P2 | 4 x `*.rand.txt` | 1 line each (`selftest`), 0 `call` lines | selftest + 0 calls, met |
| P3 | Python LCG, seed 1 | `41 18467 6334 26500 19169 15724 11478 29358` | equal to every selftest line, met |

The RUNLOG shows `MASHED_OWN_RAND_564310=1` on all four ORAND arms and on none of the DEF arms.

## 4. What this does not show (BLOCKED / open)

- **Bit identity vs the original: BLOCKED.** The original MASHED.exe does not boot this run (no audio
  endpoint, AV at `0x005baf48`). An octant-pick comparison needs an original-side run. Two parts are
  measured: the LCG constants (static, cited) and the first 8 outputs (P3). Nothing moves a C-level.
  `0x00564310` stays C2. No tracker edit.
- **The knob changes no call on the standalone today** (0 calls). It starts to matter when the
  standalone wires the island root (the RWP coupling path). Under the `.asi` the knob is not compiled.
- `[UNCERTAIN]` threading: the original state is per thread (`_getptd`), the port's is process-wide.
  Missing evidence: the thread id of each original call of `0x00564310` (the census records first hits only).
- `[UNCERTAIN]` Lua sharing: whether track or powerup scripts call `random` / `randomseed` during a
  race, which would interleave with the octant pick in the original. Missing evidence: a scan of the
  shipped `.lua` sources, or an original-side call log of `0x004c03b4` / `0x004c0502`.
- DEFERRED D-11064 is left open. The rebind exists, but the default still calls `0x005c229b`. The
  re-classify close or update belongs with the flip decision.

## 5. Decision: flip `MASHED_OWN_RAND_564310` default ON?

Recommendation: **yes, flip** (as a decision row, not done here). Reason: on the exe the default
target `0x005c229b` is zero `.bss` inside `g_b17_low_arena_pad` (exe_main.cpp:254). The exe is NX-compatible
(PE `DllCharacteristics` = `0x8100`, bit `0x100` set, read from `e9835c0f`), so the first standalone call into the island would fault there. With the
knob on, that call gets the original's LCG instead. Cost measured here: none. All 6 gate hashes are the
same with the knob on (section 3). Options: (a) flip now, inert until the island is wired; (b) keep OFF
and flip when the island is wired on the standalone. Evidence: this file.

## Raw data (not committed), `D:/MashedWT/D4-WSB-RAND-D11064_data/gates/`

| file | sha256 |
|---|---|
| H_DEF.step.csv | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| H_DEF.gates.csv | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| H_DEF.ptrace.log | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| B_DEF_K0.step.csv | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| B_DEF_K1.step.csv | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| B_DEF_K26.step.csv | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| H_ORAND.step.csv / gates.csv / ptrace.log | identical to the H_DEF rows |
| B_ORAND_K0 / K1 / K26 .step.csv | identical to the B_DEF rows |
| H_ORAND / B_ORAND_K0 / K1 / K26 .rand.txt | `dc84a66c7c9a45e24f23d32d055b99af8f6c5a068dcd8c72f6275688719cdf21` (all four) |
| RUNLOG.txt | `5eff87c5f20e81bfc57cba0f2830eaf0c64aeb5d613b3826a446ed728063bc9f` |
| mashed_re.exe (worktree build) | `e9835c0f603f00e90acd3278a8fc95cde146c95b489e6d0bf97e5f61bfaeedcd` |
| mashed_re_dev.asi (worktree build) | `266098fbc2b2e9ba0d9d77938efda4bfaf6a067e4814608beed6c71ced560a92` |

Run console log: `D:/MashedWT/D4-WSB-RAND-D11064_data/gates_run1.log`.
