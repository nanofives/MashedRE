# T2 route table: the 618 functions with no C4 body

`t2_route_table.tsv` has one row per function that runs on the T2 path (phase `race-load` or `race-tick` of
`verify/census_20261116/census_T2-RACETICK-CENSUS.tsv`) and whose hooks.csv level is not C4. 618 rows, sorted by T2 calls.
Columns: `rva`, `class`, `route`, `calls` (T2 calls), `hooks_level` (C1/C2/C3, `none` = no hooks.csv row), `source`
(re/match source path, empty = none), `best_pct` (best match % of that source, empty = none), `name`, `subsystem`,
`binding`, `in_provisional` (1 = listed in re/match/PROVISIONAL.tsv), `insitu_verdict` (PROV-INSITU 2026-11-11, empty = not tested).

## Classes and routes

| Class | Definition (hooks.csv `subsystem`) | Functions | Share of T2 calls | Has a source | Route |
|---|---|---|---|---|---|
| game code | any subsystem that is not third-party-library (audio 69, render 43, vehicle 22, gameplay 17, util 17, boot 13, ai 7, track 6, frontend 6, hud 5, particle 3, camera 2, input 1, physics 1, sky 1) | 213 | 68.9% | 196 (70 at 80%+, 74 at 50-80, 52 under 50, 17 none) | replay inside the original on ref1, then provisional bind |
| library (Lua/qhull/CRT) | `third-party-library[...]` with lua, qhull, crt, libpng or zlib | 203 | 1.7% | 0 | library build from public source, outside the port count (L-67, L-70, L-97) |
| RenderWare core | `third-party-library[renderware]` | 92 | 20.0% | 54 (1 at 80%+, 15 at 50-80, 38 under 50, 38 none) | L-124 route (matched RW core on d3d9), not MATCH workers |
| RW-Physics | `third-party-library[RenderWare-Physics-3.7]` | 62 | 3.1% | 59 | replay inside the original, then `PORT-RWP-WIRE` |
| no hooks.csv row | empty subsystem, level `none` | 48 | 6.3% | 0 | discovery first (function + C1 row), then by class |

Counts are what `xtab.py` printed on the input below: 213 / 203 / 92 / 62 / 48, equal to BATCH_PLAN_runs53-56.md.
The "has a source" column counts rows with a `best_pct` (source count = 618 - 309 no-source = 309 in total).
Level mix of the 618: C2 442, C3 88, C1 40, none 48. In-situ replay verdicts exist only for game code: IDENTICAL-FOR-2724 70,
DIVERGED 18, CRASH 10, NOT-WIRED 1, HANG 1, untested 113. Note: `0x005cb2a0` (2.4 M calls) is class "game code" because its subsystem is `render`, not a library row.

## Input and regeneration

Input commit: `b24bba46c` (race/first-frame-parity, "orch: ledger snapshot run 52"). Inputs read: the census TSV above,
`re/analysis/match_ledger.tsv`, `re/match/PROVISIONAL.tsv`, `verify/provinsitu_20261111/PROV_VERDICTS.tsv`.
`orch/integration` does not yet hold the last file, so the table was generated with `--root` on the main checkout at that commit:

    py -3.12 re/census/t2_route_table.py --root C:/Users/maria/Desktop/Proyectos/Mashed

Check: `t2_route_table.tsv` and `t2_nonc4.tsv` (output of `census_join2.py`, byte-identical to log/orch/audit_20261121/t2_nonc4.tsv)
hold the same 618 rva set, with equal `calls` and `best_pct` on every row. `census_join2.py` and `xtab.py` are copies of the audit scripts
(`census_join2.py` has the main checkout path hard-coded as ROOT and writes `t2_nonc4.tsv` beside itself; `xtab.py` reads it from the cwd):

    py -3.12 re/census/census_join2.py ; cd re/census ; py -3.12 xtab.py

Logs of that run: `census_join2.log`, `xtab.out`.
