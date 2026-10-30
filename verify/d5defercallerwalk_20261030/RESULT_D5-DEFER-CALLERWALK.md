# RESULT D5-DEFER-CALLERWALK (text-only, 2026-10-30)

Verdict: GREEN as a walk. 2 rows closed (D-11076, D-11077). 5 groups stay open: D-11080, D-11081, D-11082, D-11084 have a caller in an executed function, D-11083 has unresolved callers [UNCERTAIN]. Gates: not run (text-only). No game run, no build, no test slot.

## Method
- Call graph: `log/ghidra_funcfacts.jsonl` (Ghidra dump, 6362 functions, per-function refs-to-entry with kind) joined with `log/xref_mashed.json` (capstone graph). Both read through `re/tools/xref.py` caches. Group RVAs read from the DEFERRED.md rows (D-11080 26, D-11081 19, D-11082 14, D-11083 9, D-11084 2 = 70 RVAs).
- Walk: upward closure from each group over UNCONDITIONAL/CONDITIONAL/COMPUTED CALL, JUMP and DATA refs, attributing each ref source address to its containing Ghidra function range. An ancestor counts as executed when `re/coverage/reach.tsv` has reached_race_load or reached_race_run = 1 (order: menu, race, race_load, race_run).
- Scratch scripts (not committed): scratchpad `walk.py`, `up.py`.
- Limit: a static caller in an executed function proves a call site exists, not that the branch ran. The census says the target was NOT-EXEC in 3/3 runs, so the call sites sit on branches the census did not take. That is why these groups stay open (condition 1 of the row, a wider census, is still the way to promote them).
- `xref.py --string` on 0x005e5924, 0x005e5db8, 0x005e54c4 returned None for all three (searches for the slot address; it cannot see base+index reads of a table).

## Table
| D-id | Functions | Callers found (RVA-cited) | Verdict | Invocation |
|---|---|---|---|---|
| D-11076 | pickup visuals (not a NOT-EXEC group) | n/a, merged-item condition: D5-ICONCUBE-SPIN c8cfcdc11 + D5-PREGO-CUBE-FLIP 1b70ca47a (cube default ON), D5-ICONCUBE-GLOW 2f7a1bfdb (glow behind MASHED_RFX_EMITTERS, default OFF) | CLOSED | `py -3.12 re/tools/deferred_close.py close --date 2026-10-30 --reason "..." --evidence "verify/iconcube_20261028/RESULT_D5-ICONCUBE-GLOW.md; verify/d5deferredsweep_20261030/RESULT_D5-DEFERRED-SWEEP.md" D-11076` |
| D-11077 | part 0x12 morph, part 0x14 wobble | n/a, merged-item condition: D5-PARTMORPH 9db830bea, D5-PARTMORPH-PRODUCER 18ddec8b5 (knobs default OFF, flip = ledger D5-PARTMORPH-FLIPPREP) | CLOSED | `py -3.12 re/tools/deferred_close.py close --date 2026-10-30 --reason "..." --evidence "verify/partmorph_20261028/RESULT_D5-PARTMORPH.md; verify/d5deferredsweep_20261030/RESULT_D5-DEFERRED-SWEEP.md" D-11077` |
| D-11080 | 26 (0x00558c80..0x0055e440) | 0x55ac50 called from 0x47cea0 at 0x47ced5 (0x47cea0 race=1, run=1); 0x55b930 called from 0x57c550 at 0x57c55f (race_load=1); 0x55dd60 called from 0x47f480 at 0x47f4a5, which chains to 0x480340/0x426e10/0x40d020 (race_load=1) via 0x47f940 at 0x480575; 0x55e440 called from 0x47f940 at 0x47f998. Executed ancestor chain reaches 0x4929d0 (menu=1, race=1). | STAYS OPEN | none |
| D-11081 | 19 (0x0055fdd0..0x00565d50) | 0x561ea0 called from 0x47f6d0 at 0x47f76b (race=1, run=1); 0x562460 called from 0x47eb30 at 0x47eb8f (race, load, run all 1); 0x562560 called from 0x551cf0 at 0x551d19 (race_load=1); 0x564190 called from 0x57c550 at 0x57c568 (race_load=1). | STAYS OPEN | none |
| D-11082 | 14 (0x00566050..0x0056a250) | 0x569690 and 0x56a250 are called from 0x5697f0 (0x569d56, 0x569bd1, 0x569e1a), and 0x5697f0's address is taken by executed 0x55f800 at 0x55fdbf (DATA ref; 0x55f800 race_load=1), chain up through 0x55f450, 0x55dca0, 0x55dc70, 0x47f4c0, 0x47f840, 0x480340. Other members: callers only from unresolved sources (see below). | STAYS OPEN | none |
| D-11083 | 9 (0x00573890..0x0057bf30) | No caller found in an executed function. Not provable as none: 10 call sites sit at addresses with no containing Ghidra function (0x57bd22, 0x57bd40, 0x57bd5e -> 0x57bf30; 0x57af9e -> 0x57ae30; 0x57422b -> 0x578f20; 0x574118 -> 0x578ec0; 0x566a15, 0x57419b -> 0x5742c0; 0x5c51b4, 0x5c5742 -> 0x563b00), and DATA refs at 0x5e5924 (-> 0x574230) and 0x5e5db8 (-> 0x57ae30) have no resolved referrer. [UNCERTAIN] missing evidence: the owning functions of those 10 sites (functions Ghidra missed, see memory ghidra-missed-functions-repaired) and the base of the 0x5e5924 / 0x5e5db8 tables with its readers. | STAYS OPEN [UNCERTAIN] | none |
| D-11084 | 2 (0x0057c0d0, 0x0057c370) | 0x57c370 called from 0x47f290 at 0x47f2ed (0x47f290 called from 0x4074a0 at 0x4074c9, race_load=1, which chains to 0x40d440/0x4929d0), also from 0x421c80 at 0x421cb5, 0x47f380 at 0x47f3cd, 0x47d330 at 0x47d35e, 0x4827c0 at 0x4827ee. 0x57c0d0: callers only from unresolved sources 0x57bbcc, 0x57bbe4, 0x57bbfc, 0x57bc86, 0x57bc9a, 0x57bcae (no containing function). | STAYS OPEN | none |

## Closes
`DEFERRED.md` rows D-11076 and D-11077 struck by `re/tools/deferred_close.py` with a CHANGELOG line each (`re/analysis/CHANGELOG.md`). No hand edits.

## Next
To close D-11083 the unresolved sites need their owning functions (decomp_pc.py --create on the 0x574100..0x57bd00 and 0x5c5100..0x5c5780 ranges) and the table readers. D-11080/81/82/84 need the wider census (row condition 1), since their callers are in executed functions.
