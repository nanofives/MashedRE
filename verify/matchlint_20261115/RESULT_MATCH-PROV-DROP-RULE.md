# RESULT MATCH-PROV-DROP-RULE (2026-11-15)

Verdict: GREEN. Branch `orch/MATCH-PROV-DROP-RULE` from `orch/integration` f4cd9b352. Row kind: tool/text (no build, no
gates). Opus 5.5. Proposed by MATCH-RWT-PORT-FOLLOWUPS (`verify/match_20261114/RESULT_MATCH-RWT-PORT-FOLLOWUPS.md`,
"Open proposals"), which found 92 stale rows by hand.

## Result

| item | value |
|---|---|
| rule text | `re/match/PROVISIONAL.tsv` header comment (line 1, appended) and `re/orchestrator/WORKER_RULES.md` "Unmatched bodies (L-126)" bullet |
| tool | `re/tools/match/prov_drop.py` (new; `c4_reclass.py` and `equiv.py` not edited) |
| tests | `re/tools/tests/test_prov_drop.py`: **5 passed** (tmp-dir fixtures, canned match results, compiler not run) |
| dry run on f4cd9b352 | 931 rows examined: **DROP 0**, KEEP 931 |
| table | `verify/matchlint_20261115/PROVDROP_dryrun_f4cd9b352.tsv` (one row per PROVISIONAL row: verdict, reason, toolchain, flags, match, relocheck, include file:line), 139,840 B, sha256 `85D45656ED718EFDE45D62A66FE6C13BE1AEC7A6DC0D7FCC720063C7AF515E49` |
| `--apply` | **not run** on the branch (PROVISIONAL.tsv data rows unchanged; only its comment line changed) |

DROP 0 is the expected state: MATCH-RWT-PORT-FOLLOWUPS (merged at 324435be2) already dropped the 92 rows that met the
test, and no match row since then has brought another provisional source to byte identity.

## The rule

A PROVISIONAL.tsv row describes a NOT-PROVEN source. It leaves the list, in the same commit that closes the match, when
all of these hold for the row's own `source` and `rva`:

1. `match.py --bytes`: 0 differing unmasked bytes, using the toolchain and flags recorded in the source itself
   (`// TOOLCHAIN` line, `ddk` default; `// FLAGS:` line, `/O2` default; the same lookup match.py does at
   `re/tools/match/match.py:90` and `:320`). Only `ddk` and `rw9044` count (not `vc19`).
2. `relocheck.py` reports `OK` for that RVA.
3. The exe includes the source: a TU listed in `mashedmod/exe_sources.rsp` (the exe source list, `mashedmod/build.bat:237-248`,
   "NEW EXE TUs GO IN exe_sources.rsp") has a non-commented `#include "..."` that resolves to the row's `re/match/` path,
   directly or through a quoted include of a file under `mashedmod/src`.

Kept (with the reason in the table): lifted or hand bodies with no re/match source (L-137 / L-141), a missing source,
`__declspec(naked)`, a non-match toolchain, an RVA also listed in `ADMITTED.tsv`, a source no exe TU includes, a match.py
error, no `// MATCH:` symbol at the RVA, differing bytes, relocheck not OK. Why it matters: `ddkobj.py load_provisional()`
(`re/tools/ddkobj/ddkobj.py:283`) keeps every listed RVA out of the object build, so a stale row keeps a byte-identical
DDK unit out of `mashed_re_ddk.exe` with no message.

Include resolution check (on f4cd9b352): every one of the 2036 `#include` lines under `mashedmod/src` that names a `.c`
file uses the relative `../re/match/` form, and all 2036 resolve to an existing file. So no wrapper reaches a re/match
source through an include root the tool does not follow. [UNCERTAIN] preprocessor conditions are not evaluated: an
include under `#if 0` would count as included. Test for it: a fixture with an `#if 0` include, plus a grep of
`mashedmod/src` for `#if` lines directly above a re/match include. Not run here.

## Rows it would drop

None at f4cd9b352.

## Rows it keeps (931)

| reason | rows | what |
|---|---|---|
| no-re-match-source | 695 | 679 `RwCore/RwCore_Lifted_NN.cpp` (lifted naked-asm scaffolding, L-137 / L-141), 16 `RwCore/RwCore_Hand.cpp` |
| not-byte-identical | 202 | every one is included by an exe TU (201 provisional, 1 refused). Best: 0x004d4a20 99.4% 4 B differ, 0x00433f40 99.3% 2 B (relocheck BAD), 0x00473c20 99.0% 2 B, 0x00556e90 99.0% 14 B, 0x004c0c20 98.9% 1 B, 0x004c0f00 98.8% 1 B. 175 rows are below 90% |
| not-included | 29 | see below |
| admitted | 4 | also in ADMITTED.tsv, none byte-identical (below) |
| match-error | 1 | 0x00536780 `re/match/rwl03/00536780.c`: match.py stops with "no Ghidra size for 0x00536780; refusing to guess the original's length". Included by `mashedmod/src/mashed_re/RwCore/RwCore_00536780.cpp:31` |

**not-included (29).** Only one is byte-identical:

| rva | source | toolchain / flags | match.py --bytes | relocheck | include |
|---|---|---|---|---|---|
| **0x00551a50** | re/match/rwl04/00551a50.c | rw9044, /O2 /Ob1 /G5 | 100.0%, 121 B, 0 differing bytes, BYTE-IDENTICAL | OK checked=2 unchecked=0 | none: no `#include` of `re/match/rwl04/00551a50.c` in any file under `mashedmod/src` |

This is the known MATCH-551A50-INCLUDE case. It drops on its own once a wrapper in `exe_sources.rsp` includes the source.

The other 28 are not byte-identical either (percent, differing bytes): 0x00408b00 97.0 / 93, 0x00424eb0 98.6 / 5,
0x00441d40 69.2 / 1063 (refused), 0x00442e00 65.1 / 133, 0x00443440 58.4 / 1520 (refused), 0x00443dc0 57.5 / 2206,
0x00445aa0 67.4 / 1823, 0x00448220 97.0 / 81, 0x00468d80 82.1 / 447 (refused), 0x00469aa0 60.0 / 352 (refused),
0x00469df0 63.1 / 3695 (refused), 0x0046e9e0 81.6 / 1114 (refused), 0x0046ef70 43.9 / 1560 (refused), 0x0046f6c0
50.7 / 3016 (refused), 0x004709a0 55.7 / 568 (refused), 0x00479100 70.9 / 264, 0x00481a30 69.7 / 231, 0x00481e00
85.9 / 121, 0x00487180 11.7 / 260 (refused), 0x00499400 90.1 / 269, 0x00551ad0 59.3 / 212, 0x00558cb0 78.4 / 92,
0x005a75b0 43.5 / 462, 0x005a7b60 60.9 / 594, 0x005a9f10 81.0 / 22, 0x005aba20 77.6 / 542, 0x005abfa0 67.1 / 479,
0x005ad0b0 44.3 / 493. These are the PROV-DIVERGED-18 / PROV-FIX-9 bodies: no file under `mashedmod/src` includes them
(for example a grep for `00448220` under `mashedmod/` finds comments only, `TrackRenderer.cpp:4614` and others).

**admitted (4), listed in both PROVISIONAL.tsv and ADMITTED.tsv:**

| rva | source | match.py --bytes | relocheck | include |
|---|---|---|---|---|
| 0x0045a3a0 | re/match/s09/0045a3a0.c | 81.0%, 117 B, 32 differ | BAD | `mashedmod/src/mashed_re/Match/PortTop_0045a6b0.cpp:39` |
| 0x00493640 | re/match/s15/00493640.c | 67.9%, 176 B, 59 differ | BAD | `mashedmod/src/mashed_re/RwCore/RwCore_00493640.cpp:28` |
| 0x004c4a50 | re/match/s19/004c4a50.c | 97.2%, 717 B, 103 differ | BAD | `mashedmod/src/mashed_re/RwCore/RwCore_004c4a50.cpp:12` |
| 0x00536e90 | re/match/rwl03/00536e90.c | 99.1%, 330 B, 13 differ | OK | `mashedmod/src/mashed_re/RwCore/RwCore_00536e90.cpp:10` |

The tool keeps a row that is in both lists even when it becomes byte-identical: which list wins is a hand call.

**not-byte-identical (202), RVA and percent:** 0x004d4a20 99.4, 0x00433f40 99.3, 0x00473c20 99.0, 0x00556e90 99.0,
0x004c0c20 98.9, 0x004c0f00 98.8, 0x004c3040 97.1, 0x004c7a70 97.0, 0x0042f530 96.9, 0x004627f0 96.8, 0x004f8730 96.7,
0x0052fdb0 96.2, 0x00498510 95.3, 0x004db3e0 95.2, 0x0045d460 94.2, 0x00455400 94.0, 0x004f13d0 94.0, 0x004e2470 93.6,
0x004c3e20 92.6, 0x004fb210 92.2, 0x005adc10 92.2, 0x004f1b00 91.9, 0x00552890 91.2, 0x004c4dc0 91.2, 0x004da000 91.0,
0x004c4d20 90.9, 0x004c1a70 90.7, 0x0054e520 88.4, 0x0052eb40 88.0, 0x0052fe90 87.8, 0x00498810 86.7, 0x00450b10 86.6,
0x004c5010 84.7, 0x00428450 84.6, 0x004da4f0 84.5, 0x004cd500 83.3, 0x004647f0 83.3, 0x004d3e90 82.8, 0x004d9ee0 82.8,
0x0054eb90 82.6, 0x004c9fb0 81.3, 0x004da270 81.2, 0x004fb130 81.0, 0x005c47e0 80.8, 0x004cc050 80.6, 0x004d9360 79.8,
0x004cff20 79.7, 0x004cecb0 79.6, 0x004f0dc0 79.6, 0x004d5190 79.0, 0x004cbe80 78.6, 0x004fb2a0 78.5, 0x004daff0 77.8,
0x004c5e00 77.7, 0x004d07b0 77.0, 0x004f3d40 76.7, 0x0054f410 75.8, 0x005aab70 75.6, 0x004d9ba0 75.4, 0x004fb5a0 74.8,
0x005ae650 74.1, 0x004d9d00 73.5, 0x0053d820 72.5, 0x0052ea70 71.8, 0x004e8440 70.8, 0x004e5020 70.5, 0x004c39b0 69.9,
0x005a5e70 69.4, 0x0054b8c0 69.4, 0x0053fbb0 68.5, 0x004f3440 68.5, 0x005c4670 68.2, 0x004e02d0 67.8, 0x005530f0 67.8,
0x004e12b0 67.7, 0x004f1150 67.6, 0x0053fec0 67.3, 0x0053d0b0 67.1, 0x00473ee0 66.3, 0x004c8c70 66.2, 0x005a6f30 65.8,
0x0054cfa0 65.8, 0x004c4360 65.2, 0x004cf5a0 65.2, 0x004d0660 64.9, 0x00532a60 64.8, 0x0045cd30 62.8, 0x004c32b0 62.6,
0x004fa5f0 62.5, 0x004c30b0 61.7, 0x004d7de0 61.5, 0x004fb3e0 60.3, 0x00401690 60.2, 0x00556ca0 60.0, 0x004cc5e0 59.8,
0x004f1870 59.4, 0x005300d0 58.9, 0x004e8a10 58.6, 0x004dc8a0 58.3, 0x004e6ab0 58.3, 0x004e9080 56.9, 0x004de1b0 56.8,
0x0054c870 56.6, 0x004dfbc0 56.6, 0x004e1e70 56.5, 0x0053d6c0 56.3, 0x004d3510 55.8, 0x005412d0 55.6, 0x004d4910 55.4,
0x005533d0 55.0, 0x004f5030 54.9, 0x0042e5b0 54.3, 0x0054c4f0 54.1, 0x004e8120 53.8, 0x00549970 53.6, 0x004e62f0 53.4,
0x00543b30 52.9, 0x004df120 52.8, 0x004de800 52.5, 0x004e7420 52.3, 0x004e9330 52.2, 0x004d8300 51.6, 0x004d8810 50.7,
0x004f0f10 50.7, 0x005a6d60 50.0, 0x004e13f0 50.0, 0x004e7060 49.8, 0x004c2a30 49.6, 0x004cc230 48.0, 0x004d4f90 47.8,
0x004c4eb0 47.7, 0x00551840 46.5, 0x004e0920 46.3, 0x004ddcb0 46.2, 0x004cc6e0 45.9, 0x004cf160 45.6, 0x00474890 45.2,
0x004de390 45.0, 0x004c3e90 44.5, 0x005acda0 44.3, 0x0052daf0 44.0, 0x004d1020 42.1, 0x004c4220 40.7, 0x0043c5b0 40.5,
0x004f6870 39.9, 0x004ddd60 38.8, 0x004e2bf0 38.3, 0x004d4380 37.5, 0x004d2340 37.3, 0x0054d090 36.8, 0x0054fd60 36.5,
0x004dcdc0 36.2, 0x004d0d00 35.5, 0x004f71f0 34.7, 0x004c4680 34.5, 0x004e2930 34.3, 0x004f36c0 33.1, 0x004d3090 32.8,
0x004f57e0 32.7, 0x00530160 32.6, 0x004e8c70 32.2, 0x004e6100 31.7, 0x004e2090 30.7, 0x004d3b00 30.6, 0x004cdf20 28.3,
0x004e5300 27.9, 0x004f79f0 27.2, 0x004f7850 27.2, 0x004d8c40 27.0, 0x005aa0c0 26.2, 0x00546890 25.8, 0x004d9530 25.8,
0x004dc750 25.6, 0x0054ceb0 25.6, 0x004e2650 23.8, 0x004ddfb0 23.6, 0x004dee40 22.5, 0x004e2530 22.5, 0x005a6ea0 22.2,
0x0053d920 21.0, 0x005538a0 19.7, 0x0043dfd0 19.2, 0x005572c0 17.9, 0x004daa00 17.6, 0x004c1f80 17.4, 0x004f3150 16.5,
0x004c51a0 14.7, 0x004dc1f0 14.0, 0x004dabd0 13.4, 0x004dc160 12.8, 0x00554940 12.5, 0x0054a120 12.1, 0x004dc530 10.0,
0x004e3160 9.9, 0x005320b0 9.9, 0x004d9a60 9.2, 0x004dc4a0 8.8, 0x004dc300 8.5, 0x00532000 7.7, 0x004dc280 2.6,
0x004dc410 2.2, 0x004dc370 2.0. Bytes, relocheck and include file:line per row: the committed table.

The 695 lifted / hand rows are listed by RVA in the table (reason `no-re-match-source`).

## Commands

```
py -3.12 re/tools/match/prov_drop.py --jobs 8 --tsv verify/matchlint_20261115/PROVDROP_dryrun_f4cd9b352.tsv   # dry run
py -3.12 -m pytest re/tools/tests/test_prov_drop.py -v                                                        # 5 passed
py -3.12 re/tools/match/prov_drop.py --apply          # the lead's separate step; drops the DROP rows, keeps every other byte
```

The dry run ran on the branch, whose only PROVISIONAL.tsv change is the comment line, so its rows are f4cd9b352's.
Tool behaviour covered by the tests: every keep reason, include through a wrapper header, a commented-out include,
a TU that is not in `exe_sources.rsp`, `--only`, a match.py error, and `--apply` dropping exactly the DROP lines with
CRLF line endings and the comment line byte-for-byte kept (and a second `--apply` changing nothing).

## Not done here

- `--apply`: nothing to drop at f4cd9b352. Whoever closes a match on a provisional source runs it in the same commit.
- `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv` (the RWC copy the followups row also edited) is not read by the tool.
  If the lead wants it kept in step, that is a `--prov <path>` option, not added here.
- 0x00551a50: needs a wrapper (MATCH-551A50-INCLUDE). The 4 rows listed in both PROVISIONAL.tsv and ADMITTED.tsv need a
  hand decision on which list holds them.

## Gates

Not run: tool row (no build, the exe and the object build cannot see a new tool or a comment line).
