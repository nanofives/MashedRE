# RESULT MATCH-PCFUNCS-REGEN (2026-11-07)

**Verdict: GREEN, with one deliberate deviation: the 14 stale rows are kept.** `re/console/pc_funcs.csv` is
regenerated from the current clone (`Mashed_pool0`, read-only, released) with the file's own generator
`re/tools/console/ghidra_scripts/DumpFunctions.java` (`tools/dump.py`, no `create` step, master not opened). Header and
CRLF line endings unchanged. Gates: not run (data only, no `mashedmod/` change).

## Diff (diff_summary.tsv), old 5852 rows -> 6354 rows
| kind | rows |
|---|---|
| added (in Ghidra, not in old table) | 502 |
| size changed | 1: 0x004fbe1b was 90 (end 0x004fbe79), now 6 (end 0x004fbe20); Ghidra split it and added 0x004fbe21 (20 B), which is one of the 502 |
| name only (FUN_ -> real name, same extent) | 48 |
| in old table, absent from the Ghidra clone | 14 (kept, see below) |

The 14 stale entries: 0x00448940, 0x00451680, 0x00451690, 0x00451760, 0x00451a50, 0x00451d80, 0x00452130, 0x00452170,
0x004527f0, 0x00452ad0, 0x00452af0, 0x00452b20, 0x0047ad30, 0x004917a0. Each lies in a gap after the end of a Ghidra function
(e.g. 0x00448940 after FUN_00448880 ending 0x00448937), so Ghidra currently has no function there.

**Why they are kept:** a pure dump drops them, and the before/after score then shows exactly these 14 sources
(re/match/s08, s09, s13, s15; 12 insn 100 %, 2 bytes 100 %) going from 100 % to `refused` (`score_diff_all.tsv`, 14 rows
`Y`, 0 other changes over 2056 sources). Dropping them would regress 14 matched rows for a Ghidra gap, so
`pc_funcs.csv` = full dump + those 14 old rows verbatim (`retained_stale_rows.csv`). With them kept, the 14 sources
score the same as before (`score_diff_stale14_final.tsv`: 0 changes). Decision for the lead: either accept the union, or
have master get functions defined at these 14 entries and re-dump (the proper fix, master write via ghidra-sweep).

## Before/after collect score (tools/score_all.py, collect.py rule: insn acc, or bytes when no bad bytes and bytes_n >= size)
- `score_diff_all.tsv`: every `re/match/**/*.c` on orch/integration (2056 files), old table vs plain dump: 2399 scored
  rows unchanged except the 14 above. With the 14 retained: **no row changes score or verdict.**
- `score_diff_m2extra.tsv`: sources that exist only on `orch/C4R-BATCH-M2` (s17, s18, s20-s24, extracted to scratch):
  631 unchanged, 8 bytes unchanged, **23 rows go refused -> scored** because they get a size:
  - 100.0 %: 0x004af31a (19), the 13 requested rows, the 5 S24 nosize rows, and 0x005512d0 (90).
  - below 100: 0x00550c50 80.0 (298), 0x00550da0 96.4 (369), 0x00550f20 90.6 (296), 0x00554940 12.5 (2187).
  (No row dropped. Sizes below are Ghidra body sizes; the 4 sub-100 rows are scores, not regressions.)

## The 13 rows refused only for a missing size
| rva | old size | regen size | score now |
|---|---|---|---|
| 0x004d8430 | none | 59 | 100.0 insn |
| 0x004d8470 | none | 16 | 100.0 insn |
| 0x004d8530 | none | 25 | 100.0 insn |
| 0x004d8550 | none | 10 | 100.0 insn |
| 0x004d86d0 | none | 42 | 100.0 insn |
| 0x004d8700 | none | 42 | 100.0 insn |
| 0x004d8730 | none | 26 | 100.0 insn |
| 0x004d8750 | none | 26 | 100.0 insn |
| 0x004d8b70 | none | 90 | 100.0 insn |
| 0x004d8fa0 | none | 96 | 100.0 insn |
| 0x00551180 | none | 13 | 100.0 insn |
| 0x005514e0 | none | 40 | 100.0 insn |
| 0x00555910 | none | 466 | 100.0 insn |

S24 nosize rows: 0x00550c30 28, 0x00550d80 22, 0x00551050 62, 0x00551290 23, 0x005512d0 90, all 100.0 insn.
0x00553e80: already sized (100), unchanged, 100.0 (source re/match/s24/00554200.c).

## Ghidra size vs compiled-symbol end (S24 nosize_ends.tsv, `orch/C4R-BATCH-M2:verify/matchs24_20261107/nosize_ends.tsv`)
All 8 rows agree: sym_len = Ghidra size for 0x00550c30 28, 0x00550d80 22, 0x00551050 62, 0x00551180 13, 0x00551290 23,
0x005512d0 90, 0x005514e0 40, 0x00555910 466. No disagreement to report. Each Ghidra body ends 1 B before the recorded
`end` (end is exclusive), and the pad_to addresses are the next function entries. The 10 `0x004d8xxx` rows have no
nosize_ends file on any branch; their sizes are checked only by the 100 % insn score at that size.

## Files
`diff_summary.tsv`, `retained_stale_rows.csv`, `score_diff_all.tsv`, `score_diff_m2extra.tsv`,
`score_diff_stale14_final.tsv`, `tools/{dump.py,score_all.py,score_stale.sh}`.
Raw (not committed): D:/MashedWT/MATCH-PCFUNCS-REGEN_data/dump_pc_funcs.csv sha256
70630b484652365c1eedc081f837e92f698314e70049ce44a2b2c87885e1c541 (identical to the S19-FU dump, same clone),
pc_funcs_old.csv (old table), score_all.log, score_extra.log, m2/ (extract of orch/C4R-BATCH-M2 re/match).
No tracker edits. Other consumers (`xbuild_match.py`, `odis.py`) read the new table; not re-run here.
