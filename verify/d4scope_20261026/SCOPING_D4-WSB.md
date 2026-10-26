# SCOPING D4-WSB: B5e per-row C4 campaign

Source: ROADMAP.md:3000 (WS-B row, tagged `[UNCERTAIN: needs a hooks.csv filter of B5e rows below C4]`) and ROADMAP.md:2991.
Filter run on `hooks.csv` @ c2bf772f8 (5945 rows). Text-only, no game run.

## The filter
`subsystem == third-party-library[RenderWare-Physics-3.7]` (288 rows): 5 are C4 (`0x0057c210`, `0x0055ac00`, `0x0055b800`, `0x0055dff0`, `0x0055e200`), **283 are below C4**:

| Band (RVA) | C2/mapped | C2/new | C2/ported | C3/impl | C3/ported | total |
|---|---|---|---|---|---|---|
| BROAD 0x00558000-0x0055efff | 52 | 9 | 5 | 10 | 1 | 77 |
| BVH   0x0055f000-0x00565fff | 43 | 1 | 5 | 22 | 0 | 71 |
| SOLV  0x00566000-0x0056ffff | 32 | 0 | 12 | 30 | 0 | 74 |
| NARROW 0x00570000-0x0057bfff | 13 | 0 | 27 | 10 | 0 | 50 |
| TAIL  0x0057c000-0x0057cfff | 7 | 4 | 0 | 0 | 0 | 11 |

(C2: 210 = 147 mapped + 14 new + 49 ported. C3: 73.) The `B5e` text in `hooks.csv` notes matches 77 rows, all inside these bands. Only 4 rows sit in a `Collision/` source file as the `file` column (`RwpIntegrator.cpp`, `RwpSolverPartition13.cpp`, `RwpSolverIntegrate6.cpp`), so most rows are `mapped` (analysis plate only, no port) or ported with a plate path in `file`.

## Why this is not one step
- C3 -> C4 needs a canonical-scenario run with the hook installed (CLAUDE.md "Confidence promotion"). Hot-path functions: Frida one function at a time, short runs.
- C2/mapped rows are not ported. Under S-DoD criteria 2-3 (ROADMAP.md:146) a row the default build executes must be ported, a row it does not execute must be explicitly dispatched or justified. Which is which is unknown until an executed-set census runs on the default build (`re/frida/coverage_census.py` exists).
- D-11064 (`DEFERRED.md:1238`): `RwpSolverLeaves1.cpp` routes FUN_00564310's `_rand()` through `0x005c229b`, which does not exist in the standalone. Re-pickup condition is "B5e lane-end (K24 root swap / standalone truth)".

## Steps emitted (ledger rows)
1. `D4-WSB-CENSUS` (wave 2, frida-boot): executed-set census of the 283 rows on the default build. Everything else depends on it.
2. `D4-WSB-C4-BROAD`, `-BVH`, `-SOLV`, `-NARROW` (wave 3): C3 -> C4 on the executed C3 rows per band (10 / 22 / 30 / 10 candidates).
3. `D4-WSB-C3-PORTED` (wave 3): the 49 C2/ported rows to C3 via shadow A/B, executed rows first.
4. `D4-WSB-MAPPED-DISP` (wave 3, text-only after census): the 147 mapped + 14 new rows split into executed (port steps) and unexecuted (explicit dispatch, or DEFERRED row via re-classify).
5. `D4-WSB-RAND-D11064` (wave 3, build): standalone rebind of the CRT rand thunk, DEFERRED D-11064.

No row runs D2 arms, so none is `solo`. All game runs go through `mashed_lock.py run`.
