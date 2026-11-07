# RESULT MATCH-S19-FU-PCFUNCS (2026-11-06)

**Verdict: GREEN.** `re/console/pc_funcs.csv` now has rows for 0x004c07b0, 0x004c0830, 0x004c4430 and 0x004c4470.
The rows come from the file's own generator. The MATCH tools now size and score all 4 at 100% by instruction, so
they no longer need the forced-size path that MATCH-S19 used.

Gates: not run: data/tooling only (no `mashedmod/` change, no game run).

## How pc_funcs.csv is produced

`re/tools/console/ghidra_scripts/DumpFunctions.java` writes `entry,body_end,body_size,name` for every Ghidra function
(`re/tools/console/xbuild_match.py:15`, `re/analysis/console_symbol_harvest/REPORT.md:75`). The committed table
dates from 2026-06-12 (`3feca87be`). That was before the 2026-09-11 master repair that created 501 missed functions.
Consumers: `re/tools/match/match.py` (`ghidra_size`, line 124), `re/tools/match/odis.py`,
`re/tools/console/xbuild_match.py`.

## What was run

`tools/addrows.py` runs one analyzeHeadless session on pool slot `Mashed_pool0` (`-readOnly -noanalysis`), released
after the run. Inside it, `DecompPC.java <rvas> metadata create` runs first, then `DumpFunctions.java`. Only the
requested RVAs' rows are merged into `pc_funcs.csv`, in address order, with the checkout's line endings. The master
project was not opened.

All 4 already exist in the clone (`exact_entry: true`, `created_transiently` absent). They came from the 2026-09-11
repair, so the transient `create` was not needed. The MATCH-S19 note "Ghidra has no function at these entries" was
true of the June table, not of the current project.

Added rows (`added_rows.csv`):

| entry | body_end | body_size | name |
|---|---|---|---|
| 0x004c07b0 | 0x004c082b | 124 | FUN_004c07b0 |
| 0x004c0830 | 0x004c086c | 61 | FUN_004c0830 |
| 0x004c4430 | 0x004c446c | 61 | FUN_004c4430 |
| 0x004c4470 | 0x004c452e | 191 | FUN_004c4470 |

These sizes match the compiled-symbol lengths MATCH-S19 recorded (124, 61, 61, 191).

## The MATCH tools now see them

`tools/score4.py` scores each `re/match/s19/<rva>.c` with `verify/matchs19_20261102/tools/collect.py`'s rule. It runs
once against `pc_funcs.csv` at `orch/integration` and once against this branch (`scores.tsv`):

| rva | base: ghidra_size / collect | new: ghidra_size | acc % | regs % | bytes_n | bytes_bad | collect by | best % |
|---|---|---|---|---|---|---|---|---|
| 0x004c07b0 | None / refused | 124 | 100.0 | 100.0 | 124 | 0 | insn | 100.0 |
| 0x004c0830 | None / refused | 61 | 100.0 | 100.0 | 61 | 0 | insn | 100.0 |
| 0x004c4430 | None / refused | 61 | 100.0 | 100.0 | 61 | 0 | insn | 100.0 |
| 0x004c4470 | None / refused | 191 | 100.0 | 100.0 | 191 | 0 | insn | 100.0 |

`match.py re/match/s19/<rva>.c --bytes` also reports `BYTE-IDENTICAL (relocations masked)` for each
(flags `/O2 /GS /Ob1 /G5`, toolchain ddk).

Side-effect check: across every row of the old table, `match.ghidra_size` returns the same capped size before and
after. The insertion re-caps no neighbour.

## Drift (not changed here, follow-up candidate)

The clone's full dump has 6340 functions. After this change the table has 5852. 502 functions in the dump are absent
from the table. 14 table entries are absent from the dump: 0x00448940, 0x00451680, 0x00451690, 0x00451760,
0x00451a50, 0x00451d80, 0x00452130, 0x00452170, 0x004527f0, 0x00452ad0, 0x00452af0, 0x00452b20, 0x0047ad30,
0x004917a0. A full regeneration would change `ghidra_size` capping and xbuild_match inputs for other functions, so it
needs its own step with a before/after score diff.

## Raw files (not committed)

| file | sha256 |
|---|---|
| D:/MashedWT/MATCH-S19-FU-PCFUNCS_data/dump_pc_funcs.csv | 70630b484652365c1eedc081f837e92f698314e70049ce44a2b2c87885e1c541 |
| D:/MashedWT/MATCH-S19-FU-PCFUNCS_data/create.json | d1b30f08d101692a5fa97cce48532f02fea62c66718e3335df5e68a78cdd9c9a |
| D:/MashedWT/MATCH-S19-FU-PCFUNCS_data/headless.log | 61b2999e26eab1aad87ce8b21f2d9a21b7e562311826cbc314f3acf8add943f3 |
| D:/MashedWT/MATCH-S19-FU-PCFUNCS_addrows.log | d713df1c901b55f02932c09e43a78e2b41cf0db98572d4fd6340d7dd609579fd |

## Process note

The first `addrows.py` attempt (23:16) started while a new merge-train PRIORITY-HOLD had just opened
(`merge-train:20261106_231610`). Ghidra aborted before opening any project because the pool path resolved inside the
worktree. Slot 0 was released and nothing ran. The successful run started at 23:41, after the hold and linger had
cleared.

No tracker edits (nothing changes a function's C-level).
