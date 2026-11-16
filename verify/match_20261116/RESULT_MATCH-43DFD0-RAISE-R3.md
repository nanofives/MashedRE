# RESULT MATCH-43DFD0-RAISE-R3 (merge of ARMS + LOOPS + REGS)

Branch `orch/MATCH-43DFD0-RAISE-R3`, base 810042e0e. Match only, no build, no gates, no body run (L-168).
Merge order: ARMS (24331ccdd), LOOPS (2feaa0ad7), REGS (79250fa13, includes f5209802c). No conflicts
(git merged `re/match/s08/0043dfd0.c` and `UNCERTAIN_REGISTER.md` without markers; register gained one line per branch).

Measured with `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes`.

## Scores

| Source | score | regs | ops | instructions | differing bytes | first differing byte |
|---|---|---|---|---|---|---|
| base 810042e0e | 49.8% | 67.0% | 88.6% | n/a | n/a | n/a |
| ARMS alone | 51.1% | | | | | |
| LOOPS alone | 49.95% | | | | | |
| REGS alone | 50.02% | | | | | |
| **A: all three merged (kept)** | **51.5%** | 67.7% | 89.5% | 2799/2817 | 6419 of 11066 B | +0x5f |
| B: A with f5209802c (fade-tail rewrite) reverted | 51.3% | 67.6% | 89.3% | 2810/2817 | 6433 of 11082 B | +0x5f |

A scores higher than B, so the fade-tail rewrite stays and A is the branch head. B was a scratch
`git revert -n f5209802c`, discarded with `git reset --hard`.

## Checks

- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` in `mashedmod/src/mashed_re/Match`
  (VS2022 vcvars32, syntax only): exit 0.
- Source header line 2 updated to the combined score (51.5%).

## Lead for round 4

- The 0/1 register roles flip only on the `FUN_00472640(0xff)` constant at 0x0043f9d7 in case 0xff4b0000. Diagnostic:
  changing it to 0xfe puts 0 in ebx and 1 in ebp, about 59.6%. That changes behaviour, so it was not kept. See
  `RESULT_MATCH-43DFD0-RAISE-R3-REGS.md`.
- LOOPS next: retry the split-store / index-form once the roles move.
- ARMS leftover: the `DAT_0067e9f8` reload in the cases at about source lines 1373 and 1447 (original 0x0043fc37 and 0x0043fa18).
