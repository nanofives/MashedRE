# C4R-DSHOW-NEWROWS: new BaseClasses rows from libid (2026-11-08)

**Verdict: GREEN (no-op by rule).** 0 rows added, 0 promoted, 96 addresses refused. hooks.csv unchanged.
Text and tracker only: no mashed_re build, no game run.

Input: `verify/c4rdshowbase_20261107/rel_ob1.tsv` (rel_ob1 build, `/Gz /MT /GX /O2 /Ob1`), 96 distinct
`identical-unlisted` addresses. `newrows.py` (this folder) applies the C4R-DXLIB-FU rule: `re/console/pc_funcs.csv`
must have a function starting at the address with body size equal to the symbol size; one library candidate; no
existing row. Output of the check: `candidates.tsv`.

## Refused (96, `candidates.tsv`)

| reason | count |
|---|---:|
| single candidate, no pc_funcs boundary at the address (the 83 candidate rows) | 83 |
| ambiguous (2 to 7 library functions) and no pc_funcs boundary | 11 |
| hooks.csv row present (0x004943d0, 0x00494c60) and ambiguous (23 scalar deleting destructors) | 2 |

Every one of the 83 single-candidate addresses lies in a gap between pc_funcs entries, e.g. 0x0049c790 sits between
`0x0049c770..0x0049c789` and `0x0049c800`. pc_funcs.csv has no function there, so the boundary and body size
cannot be checked and a row would carry an unverified extent. Adding them needs pc_funcs boundaries first
(a regen of pc_funcs.csv from a Ghidra clone with these functions created, MATCH-PCFUNCS-REGEN precedent), then
re-run `py -3.12 verify/c4rdshownewrows_20261108/newrows.py --apply` and the libid batch. The script is idempotent.

## Counts

c4_reclass batch: not run (empty batch). Ladder before = after: `{'other': 32, 'C4': 3605, 'C2': 2183, 'C3': 217, 'C1': 10}`
(`ladder_before.json`, `ladder_after.json`). tracker_lint before = after: `pre-existing=3 NEW=0`.
