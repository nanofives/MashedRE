# RESULT C4R-MATCHBATCH-4 (2026-11-08)

**Verdict: GREEN.** 5 functions created in the master Ghidra project, `re/console/pc_funcs.csv` +5 (nothing else
changed), then 65 rows promoted to C4 by route match (L-93) through `c4_reclass.py batch --apply` (XP DDK cl
13.00.9176 / c2 13.00.9178, the file's `// FLAGS:`, byte compare plus relocheck OK). 0 refused, 2,914 already C4.
Text, tracker and master Ghidra only: no build, no game run, no `mashedmod/` change, gates not run.

Branch `orch/C4R-MATCHBATCH-4` from `orch/integration` @ 6b1f4f059 (MATCH-RWR01..06 merged).

## 1. Master Ghidra: 5 functions created

Input: the 5 MATCH-RWR06 rows that had no Ghidra function and no pc_funcs.csv entry
(`verify/matchrwr06_20261108/RESULT_MATCH-RWR06.md`, "Blocker for the C4R batch"). All 5 already had a hooks.csv row
(C1, `third-party-library[renderware]`), so no row was added.

Byte check first (`bytecheck.txt`): `verify/matchrwr06_20261108/tools/ns.py <source> <va> <extent>` (match.py with the
extent set explicitly), each source in `re/match/rwl03/`:

| va | extent | match.py | bytes_bad |
|---|---|---|---|
| 0x0052df20 | 19 | 100.0% (6/6 ins) | [] |
| 0x00537b20 | 84 | 100.0% (40/40 ins) | [] |
| 0x00537d20 | 71 | 100.0% (31/31 ins) | [] |
| 0x00537d70 | 106 | 100.0% (46/46 ins) | [] |
| 0x00537e60 | 105 | 100.0% (34/34 ins) | [] |

Write, with the C4R-DXLIB-GHIDRAFN guards (`re/tools/ghidra_scripts/CreateLibFunctions.java`, unchanged: refuse on
non-executable memory, any function overlapping `[va, va+size)`, defined data in it, `va` inside an existing
instruction; after `createFunction` the flow body must equal the extent or stop early on alignment fill):

- No `java.exe` running, no `Mashed.lock*` beside `Mashed.gpr`, before and after.
- Backup `Mashed.rep.bak-20261108-pre-rwr06fn` + `Mashed.gpr.bak-20261108-pre-rwr06fn` (81 MB, gitignored by
  `/Mashed.rep.bak-*/`).
- Rehearsal: `analyzeHeadless <Mashed> Mashed -process MASHED.exe -readOnly -noanalysis`, postScripts
  `DumpFunctions.java` (before), `CreateLibFunctions.java create_list.tsv rehearsal_readonly.tsv apply`,
  `DumpFunctions.java` (after). `ok=5 skipped=0 rejected=0`, all 5 `CREATED_EXACT` (flow body = extent),
  "Discarding changes". Both scripts were run from one scratch script dir (`analyzeHeadless.bat` rejects a
  `;`-joined `-scriptPath`).
- Control: master's dump before the write has sha256
  `f032eb0298353a397de3d78cbcb2d6393773092054c18453b7529f4595402043`, the after-write hash C4R-DXLIB-GHIDRAFN
  recorded. Master had no other change since that write.
- Write: same command without `-readOnly`, list `apply_list.tsv` (the 5 rehearsal-created addresses):
  `ok=5 skipped=0 rejected=0`, all `CREATED_EXACT`, `Save succeeded` (`apply_master.tsv`).
- After: read-only dump 6,470 -> 6,475 functions, sha256
  `efa1a0e8b0bbc80401ef923d8474f292cbb5cfdfbe6141d04314558093fc3290`, identical to the rehearsal dump.

No label inside an existing function was involved, nothing was split. Functions keep the default `FUN_<va>` name.
**Pool clones were not synced** (the brief forbids pool-slot writes): `mashed_pool/Mashed_poolN` lack these 5 (and the
129 of C4R-DXLIB-GHIDRAFN) until a session allowed to write pool slots runs `scripts/ghidra_pool.sh sync`.

## 2. pc_funcs.csv (`tools/regen_pcfuncs.py`, `regen_output.txt`, `pcfuncs_added.csv`)

C4R-DXLIB-GHIDRAFN recipe: the read-only dump of master after the write + the 14 retained stale rows of
`verify/matchpcfuncsregen_20261107/retained_stale_rows.csv`. The script is that RESULT's copy with only its output path
changed. `old 6483 -> new 6488: added 5 removed 0 changed 0 (stale kept 14)`. Added:

```
0x0052df20,0x0052df32,19,FUN_0052df20
0x00537b20,0x00537b73,84,FUN_00537b20
0x00537d20,0x00537d66,71,FUN_00537d20
0x00537d70,0x00537dd9,106,FUN_00537d70
0x00537e60,0x00537ec8,105,FUN_00537e60
```

Committed as b0d754121.

## 3. Batch (recipe of `verify/c4rmatchbatch4_20261108/RESULT_C4R-MATCHBATCH-3.md`)

`build_ledger.py` (copied unchanged from `verify/c4rmatchbatch4_20261108/`) rebuilt `re/analysis/match_ledger.tsv`,
`pairs.tsv` and `batch.tsv`: 4,085 RVAs, **2,979 pass** (was 2,914 after C4R-MATCHBATCH-3 plus later merges), batch.tsv
2,979 rows, 0 recorded-100 RVAs without an annotated source. `verify/matchpilot_20261030/g2s_raw/00499740.c` still
"compile failed (ddk)", as before (not a batch row).

Dry run `c4_reclass.py batch --tsv batch.tsv --report verdicts.tsv --jobs 8`: **65 pass, 0 refused, 2,914 skipped
(already C4)** (`dryrun.log`, `verdicts.tsv`). Apply with `--apply --result <this file> --jobs 8 --date 2026-11-08`:
same 65 PASS, `wrote hooks.csv (65 rows -> C4) and 65 CHANGELOG lines` (`apply.log`, `verdicts_apply.tsv`).
hooks.csv written line-based by c4_reclass (`git diff --numstat`: hooks.csv 65 / 65, CHANGELOG.md +65 / -0).

### Promoted: 65 (`promoted.tsv`, slice = the MATCH-RWR results.tsv that records the RVA at 100%)

| slice | rows |
|---|---|
| MATCH-RWR01 | 11 |
| MATCH-RWR02 | 11 |
| MATCH-RWR03 | 13 |
| MATCH-RWR04 | 18 |
| MATCH-RWR05 | 5 |
| MATCH-RWR06 | 7 (the 5 created above + 0x004eb9e0, 0x005333a0) |

From C2 51, C3 9, C1 5 (the 5 RWR06 rows). All 65 are `third-party-library[renderware]` rows (library scope).

**`volatile float *` sources (RWR03).** 0x0054d4d0 (`re/match/rwl04/0054d4d0.c`, 1785 B) and 0x0054dbd0
(`re/match/rwl04/0054dbd0.c`, 2379 B) match through a `volatile float *v = param_1;` read alias (0054dbd0 also types
DAT_005cc31c as float). Both are plain C: 0 `__asm` in either file, so neither is naked asm nor a lone `__asm` block in a
C frame. The L-93 rubric does not exclude a volatile qualifier, so both were promoted (C2 -> C4) as PASS rows like the
rest. Named here so a later rubric change can find them.

### Refused: 0

Rows in the ledger that do not pass (4,085 - 2,979 = 1,106) are not in batch.tsv and were not gated as refusals; that is
the open MATCH residue (M3), unchanged by this row.

## 4. Port column, lint, ladder

- `scripts/backfill_port.py --allow-stale --build-dir ../D4-FLIP-ST5-LEG2/mashedmod/build`: `rows changed: 0`,
  "hooks.csv already current" (`backfill_port.log`). It warns on 4 records of another width (185 / 00404830,
  1206 / 004464c0, 3127 / 0041f360, 3137 / 00420870) and leaves them untouched; this branch does not change any of them.
- `tracker_lint.py --repo . --base orch/integration`: before `hooks.csv 6144 rows ... pre-existing=3 NEW=0`, after
  `hooks.csv 6144 rows ... pre-existing=3 NEW=0` (`lint_before.txt`, `lint_after.txt`).
- `ladder_counts.py` (`ladder_before.json`, `ladder_after.json`):

| scope | C1 | C2 | C3 | C4 |
|---|---|---|---|---|
| overall | 10 -> 5 | 1868 -> 1817 | 167 -> 158 | 4099 -> **4164** |
| game | 0 -> 0 | 718 -> 718 | 103 -> 103 | 2547 -> 2547 |
| library | 10 -> 5 | 1150 -> 1099 | 64 -> 55 | 1552 -> **1617** |

(`other` 32 unchanged; 6,176 ladder rows.)

## Files

`bytecheck.txt`, `create_list.tsv`, `rehearsal_readonly.tsv`, `apply_list.tsv`, `apply_master.tsv` (Ghidra),
`tools/regen_pcfuncs.py`, `regen_output.txt`, `pcfuncs_added.csv` (pc_funcs), `build_ledger.py`, `build_ledger.log`,
`pairs.tsv`, `batch.tsv`, `dryrun.log`, `verdicts.tsv`, `apply.log`, `verdicts_apply.tsv`, `promoted.tsv` (batch),
`backfill_port.log`, `lint_before.txt`, `lint_after.txt`, `ladder_counts.py`, `ladder_before.json`,
`ladder_after.json`. Also edited: hooks.csv, re/analysis/CHANGELOG.md (+65), re/analysis/match_ledger.tsv,
re/console/pc_funcs.csv. Not committed: master dumps before/after (sha256 above), master backup
`Mashed.rep.bak-20261108-pre-rwr06fn`.
