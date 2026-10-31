# RESULT MATCH-L68-SWAP (run 27, 2026-10-31)

Verdict: **GREEN.** Both swaps in, one body per RVA, match.py 100% and relocheck OK on this branch, build lint clean, default gates unchanged. Not C4 (exe-callers=0 for both, L-68 ruling); no re-classify.

L-68 (a): the two S02 matches whose old port bodies diverged from the original
(verify/matchs02_20261031/RESULT_MATCH-S02.md, "Not swapped") become their RVAs' only port bodies.
Both have no caller in mashed_re.exe, so per the L-68 ruling (Mariano 2026-10-31) neither is C4 and this row
does no re-classify. Record for both: **match evidence: identical, swapped, exe-callers=0; C4 when a caller
lands (L-68 ruling).**

## Swaps

| rva | port symbol | targets | install | x87 | old body (removed) | new body | match.py | relocheck |
|---|---|---|---|---|---|---|---|---|
| 0x004114c0 | VtableTeardown_114c0 | both | 0 (MASS-DISABLED 2026-05-24, stays off) | no | Boot/BootLowRvaCluster.cpp | Match/MatchS02_004114c0.cpp | 100.0% 7/7 ins | OK checked=2 unchecked=0 |
| 0x00411350 | ReplayTimeFormat | both | 1 (moved) | yes (`MatchS02_00411350` in x87_tus.txt) | Save/ReplayTimeFormat.cpp (TU retired, out of both .rsp) | Match/MatchS02_00411350.cpp | 100.0% 28/28 ins | OK checked=2 unchecked=1 (`__ftol`) |

Flags `/O2 /GS /Ob1`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), re-run on this branch.
`rva_body_scan`: 0x004114c0 one body (Match/MatchS02_004114c0.cpp), no install; 0x00411350 one body and one
install (Match/MatchS02_00411350.cpp). Build lint: `[rva-lint] ... allowlisted=16 NEW=0`, `[asm-lint] OK`.
No dual_copy_allowlist entry named either RVA, so none went stale.

What each swap fixes (the old bodies were port bugs, MATCH-S02):
- 0x004114c0: the old body called `[[[0x007d3ff8]]+0x10c]`, one dereference more than the original
  `[[0x007d3ff8]+0x10c]` (matched source: `(**(code **)(DAT_007d3ff8 + 0x10c))(DAT_008a94a8)`).
- 0x00411350: the old body computed `(double)param_1 * k`; the original (matched) stores the product in a float
  local `t = (float)param_1 * 0.00033333333f`. S02 measured a difference on 1,787,798 of 1,800,000 inputs.

Exe callers: none for either. `grep` of mashedmod/src finds no call of `VtableTeardown_114c0` or
`ReplayTimeFormat` by name; Save/ReplayGetTimeAtIdx.cpp calls 0x00411350 by address (the .asi path, where the
installed hook now runs the matched body). The ReplayTimeFormat.cpp doc block (no x87 input arg, unsigned
param_1, scale a hair below 1/3000) moved into the wrapper header.

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT
`D:/MashedWT/MATCH-L68-SWAP_root` (robocopy of `original/`), wrapped in `test-slot` +
`mashed_lock.py run --label MATCH-L68-SWAP`, exe copied out of the build dir first.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs merge_train.json ref |
|---|---|---|---|---|---|
| 1 | commit 4bb70c206 | `53b269a5ca7168cb` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF self-exited in 473 s, each B arm in 39-40 s, exit 0. (A first attempt at 17:2x died before any arm data:
`A4_OUT` did not exist and `run()` failed copying `mashed_re.log` into it. Re-run with the folder created.)
No gate hash moved, so no DECISION.

## -Full

The train's `-Full` legs (vsum / e_launch, D2 x3, 13 tracks) are not run here: they are the train's pass for a
source row (row note `SOURCE -Full`). This branch touches `mashedmod/src` outside `full_set_paths`, so the train
must be run with `-Full` explicitly.

## Raw data (D:/MashedWT/MATCH-L68-SWAP_data, not committed)

```
53b269a5ca7168cb0b8d4bc67c3893f31cdf20b5f24d6740761e45570b853e15  exe_g1/mashed_re.exe (build of 4bb70c206)
57a44285fb8261e18f54651ccb69e7e866484a289ed803c9cd1620dd2e316aac  build1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K{0,1,26}.step.csv
f6cbf7af532ac5cba0c1d4ebb65ea0ce7071e91c5278b810cdfbcb31dbfa464a  arms_g1/H_DEF.log
58fb188a668d62f07d651563cf3365e6ed4d5081efd49f2fe9fe9a04fb71562b  arms_g1.log
```
Sources: `re/match/s02/004114c0.c` 45fadcde73f6c02f, `re/match/s02/00411350.c` f490ab12bb253356 (= S02
results.tsv), `Match/MatchS02_004114c0.cpp` 11dea2fe1de70c38, `Match/MatchS02_00411350.cpp` e917e77247798080.

## Next

MATCH-S02-RC (or whichever row re-classifies S02): record both RVAs as matched + swapped, exe-callers=0, not C4.
