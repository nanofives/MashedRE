# RESULT MATCH-RWL02 (2026-11-08)

Slice 0x004e2030..0x004ebc30, 133 rows `third-party-library[renderware]` C2 (RW core). Match-only: no trackers, build or gates.

**Verdict: GREEN (partial, honest counts).** 74 of 133 rows are 100% under `re/tools/match/match.py` and pass
`relocheck.py` OK. 59 rows are below 100%.

| step | rows |
|---|---|
| raw converter (g2s) 100%, relocheck OK | 36 |
| hand-matched to 100% by 3 sonnet helpers (h1 13, h2 13, h3 12; disjoint lists) | 38 |
| total 100% | 74 |
| relocheck OK | 74 (`relocheck.tsv`; 0x004e3470 has `unchecked=3`: `_RpAtomicRegisterPlugin`, `_RpClumpRegisterPlugin` names, no BAD line) |
| below 100% | 59 |
| C++ compile (VS2022 /TP, MATCH_PORT) | 130 of 133 ok. FAIL: 0x004e5850, 0x004e99b0, 0x004ea9a0 (all below 100%, int/pointer conversions not fixed) |
| `__asm` in any source | 0 |

Every 100% source passes the C++ check. 26 raw-converter 100% files needed C++ casts only (`typedef int code(...)` under
`__cplusplus`, explicit pointer casts); match and relocheck were re-run after the edits and the collect below re-scored all.

## Flags evidence
Raw converter run on all 133 rows under `/O2 /Ob1 /G5` (`g2s_g5`) and `/O2 /GS /Ob1` (`g2s_game`), best per row, ties to /G5
(`tools/g2s_best.py`). 131 final sources carry `// FLAGS: /O2 /Ob1 /G5`. Exceptions, both below 100%: 0x004e2650 on
`/O2 /GS /Ob1`, 0x004e5820 on `/O1 /Ob1 /G5`. No 100% row needs other flags. 0x004eba40 raw scored higher under the game
flags (66.7 vs 30.8) but its final 100% source is /G5.

## Below 100% (class: a = compiler-version codegen, b = source shape / out of budget, c = tool limit)
| rva | size | best% | regs% | flags | class |
|---|---|---|---|---|---|
| 0x004e2090 | 899 | 29.7 | 48.3 | /O2 /Ob1 /G5 | b |
| 0x004e2470 | 132 | 93.6 | 95.7 | /O2 /Ob1 /G5 | b |
| 0x004e2530 | 273 | 22.5 | 58.6 | /O2 /Ob1 /G5 | b |
| 0x004e2650 | 730 | 23.8 | 26.2 | /O2 /GS /Ob1 | b |
| 0x004e2930 | 560 | 34.3 | 53.9 | /O2 /Ob1 /G5 | b |
| 0x004e2bf0 | 884 | 38.2 | 68.1 | /O2 /Ob1 /G5 | b |
| 0x004e3160 | 598 | 9.9 | 50.7 | /O2 /Ob1 /G5 | b |
| 0x004e4130 | 89 | 67.6 | 83.8 | /O2 /Ob1 /G5 | b |
| 0x004e4380 | 38 | 72.2 | 72.2 | /O2 /Ob1 /G5 | b |
| 0x004e4450 | 143 | 98.1 | 98.1 | /O2 /Ob1 /G5 | a |
| 0x004e4c30 | 338 | 71.3 | 88.9 | /O2 /Ob1 /G5 | b |
| 0x004e4f70 | 165 | 81.8 | 81.8 | /O2 /Ob1 /G5 | b |
| 0x004e5020 | 364 | 70.5 | 73.5 | /O2 /Ob1 /G5 | b |
| 0x004e51e0 | 156 | 90.7 | 92.6 | /O2 /Ob1 /G5 | b |
| 0x004e5300 | 710 | 27.9 | 49.6 | /O2 /Ob1 /G5 | b |
| 0x004e55d0 | 129 | 88.2 | 94.1 | /O2 /Ob1 /G5 | b |
| 0x004e5700 | 288 | 89.8 | 92.9 | /O2 /Ob1 /G5 | b |
| 0x004e5820 | 21 | 57.1 | 57.1 | /O1 /Ob1 /G5 | b |
| 0x004e5850 | 923 | 57.4 | 72.5 | /O2 /Ob1 /G5 | b |
| 0x004e5bf0 | 57 | 70.6 | 76.5 | /O2 /Ob1 /G5 | b |
| 0x004e5c30 | 62 | 76.7 | 83.3 | /O2 /Ob1 /G5 | b |
| 0x004e5d30 | 237 | 91.2 | 91.2 | /O2 /Ob1 /G5 | a |
| 0x004e5ef0 | 60 | 88.0 | 88.0 | /O2 /Ob1 /G5 | a |
| 0x004e5fc0 | 318 | 77.3 | 77.3 | /O2 /Ob1 /G5 | a |
| 0x004e6100 | 327 | 31.7 | 60.3 | /O2 /Ob1 /G5 | b |
| 0x004e62f0 | 372 | 53.4 | 75.9 | /O2 /Ob1 /G5 | b |
| 0x004e6470 | 122 | 90.7 | 90.7 | /O2 /Ob1 /G5 | a |
| 0x004e6650 | 43 | 70.0 | 75.0 | /O2 /Ob1 /G5 | a |
| 0x004e6710 | 67 | 54.8 | 54.8 | /O2 /Ob1 /G5 | b |
| 0x004e6760 | 67 | 54.8 | 54.8 | /O2 /Ob1 /G5 | b |
| 0x004e67b0 | 201 | 97.1 | 97.1 | /O2 /Ob1 /G5 | b |
| 0x004e6920 | 127 | 90.9 | 90.9 | /O2 /Ob1 /G5 | a |
| 0x004e6ab0 | 592 | 46.1 | 67.5 | /O2 /Ob1 /G5 | b |
| 0x004e6e00 | 377 | 67.9 | 77.6 | /O2 /Ob1 /G5 | b |
| 0x004e6f80 | 37 | 37.5 | 87.5 | /O2 /Ob1 /G5 | b |
| 0x004e7060 | 952 | 39.5 | 53.9 | /O2 /Ob1 /G5 | b |
| 0x004e7420 | 2207 | 37.9 | 29.2 | /O2 /Ob1 /G5 | c |
| 0x004e7cc0 | 124 | 90.9 | 90.9 | /O2 /Ob1 /G5 | a |
| 0x004e7e50 | 60 | 88.0 | 88.0 | /O2 /Ob1 /G5 | b |
| 0x004e8120 | 724 | 50.2 | 43.4 | /O2 /Ob1 /G5 | b |
| 0x004e8440 | 242 | 70.8 | 70.8 | /O2 /Ob1 /G5 | a |
| 0x004e8640 | 269 | 77.9 | 77.9 | /O2 /Ob1 /G5 | b |
| 0x004e8750 | 443 | 91.1 | 92.4 | /O2 /Ob1 /G5 | b |
| 0x004e8940 | 82 | 92.3 | 92.3 | /O2 /Ob1 /G5 | b |
| 0x004e89a0 | 62 | 76.7 | 83.3 | /O2 /Ob1 /G5 | b |
| 0x004e8a10 | 595 | 58.6 | 66.7 | /O2 /Ob1 /G5 | b |
| 0x004e8c70 | 543 | 28.9 | 54.4 | /O2 /Ob1 /G5 | b |
| 0x004e8ea0 | 165 | 76.5 | 86.3 | /O2 /Ob1 /G5 | b |
| 0x004e9080 | 687 | 56.9 | 72.7 | /O2 /Ob1 /G5 | a/b |
| 0x004e9330 | 1303 | 51.5 | 56.2 | /O2 /Ob1 /G5 | b |
| 0x004e98d0 | 60 | 88.0 | 88.0 | /O2 /Ob1 /G5 | a |
| 0x004e9910 | 60 | 88.0 | 88.0 | /O2 /Ob1 /G5 | a |
| 0x004e99b0 | 1156 | 45.5 | 50.4 | /O2 /Ob1 /G5 | b |
| 0x004e9e40 | 980 | 34.4 | 58.1 | /O2 /Ob1 /G5 | b |
| 0x004ea220 | 663 | 46.5 | 55.3 | /O2 /Ob1 /G5 | b |
| 0x004ea9a0 | 2430 | 30.1 | 44.3 | /O2 /Ob1 /G5 | b |
| 0x004eb9e0 | 85 | 64.7 | 82.4 | /O2 /Ob1 /G5 | b |
| 0x004ebb00 | 296 | 83.3 | 86.1 | /O2 /Ob1 /G5 | b |
| 0x004ebc30 | 196 | 89.3 | 91.7 | /O2 /Ob1 /G5 | b |

Per-row reasons are in `notes/h1.md`, `notes/h2.md`, `notes/h3.md` (helper reports). Main residuals: callee-saved push
shrink-wrap (0x004e6920, 0x004e7cc0, 0x004e6470), failure returns not cross-jumped in the original (0x004e5ef0,
0x004e9910, 0x004e98d0, 0x004e8440, 0x004e5d30, 0x004e7e50), x87 operand order (0x004e5fc0, 0x004e8640, 0x004e6100).
Class b rows of 560 B and up (0x004e2930, 0x004e3160, 0x004e5300, 0x004e2bf0, 0x004e7060, 0x004e9330, 0x004e8c70,
0x004e6ab0, 0x004ea220, 0x004e8120, 0x004e2090, 0x004e9e40, 0x004e7420, 0x004e2650, 0x004e5850, 0x004e99b0, 0x004ea9a0)
are structure-only raw-converter forms, not hand-matched (time cap). 0x004e7420: the Ghidra artifact
`&stack0x0 != 0x2c` was replaced by `local_2c != 0`, UNVERIFIED, not usable as a port body.
Large-row files have loosened callee prototypes (`void *`, `...` on FUN_004d7ff0) so they compile as C++.

## Other
- No-size rows: none (all 133 have a Ghidra size). Static callees held in a file: none.
- Permuter caution: `match_permute.py` produced semantically wrong variants (0x004e8750 95.5% by rewriting an address
  sum, stray byte stores); helpers diffed and rejected them. Anything promoted must be below 100% only by inspection.
- No tool fixes under `re/tools/match/`. `decomp_pc.py` was run from the main checkout (the worktree has no
  `mashed_pool`); raw decomp in `D:/MashedWT/MATCH-RWL02_data/decomp_raw.json`.
- Follow-ups: finish the 17 large structure-only rows; the 3 failing C++ files; the push-order residuals are shared with
  S19/S21 (flags_brief.md).
- results.tsv sha256 924572b77e9084e26da2d7ad9a1481ffe8b67fbd6bbf54cc92eb10327526f644
