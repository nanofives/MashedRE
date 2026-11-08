# RESULT C4R-MATCHBATCH (roadmap run 35, 2026-11-07)

**Verdict: GREEN.** 1,620 rows promoted to C4 by route match (L-93) through `re/tools/c4_reclass.py batch --apply`.
Every promoted row was recompiled at promotion time with XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 and its recorded
flags (`match.compare --bytes`) and `relocheck` reported OK. 175 rows of the batch were already C4 (skipped), 3 were
refused. The consolidated ledger `re/analysis/match_ledger.tsv` covers 2,129 RVAs. `tracker_lint` against
`orch/integration`: NEW=0 (pre-existing 3, same as before the apply). `backfill_port.py`: 0 rows changed.

Branch base: `orch/C4R-BASE` @ 1fb65e900 (orch/integration + C4R-RUBRIC + C4R-PORTCOL). Text-only step: no build, no
game run, gates not run.

## 1. What ran

| step | command | output |
|---|---|---|
| ledger | `py -3.12 verify/c4rmatchbatch_20261107/build_ledger.py --jobs 8` (53 s) | `pairs.tsv`, `re/analysis/match_ledger.tsv`, `batch.tsv`, `build_ledger.log` |
| make-tsv (reference) | `c4_reclass.py make-tsv --results verify/match*/results.tsv` | `batch_recorded100.tsv` |
| dry run | `c4_reclass.py batch --tsv batch.tsv --report verdicts.tsv --jobs 8` | `dryrun.log`, `verdicts.tsv` |
| apply | `c4_reclass.py batch --tsv batch.tsv --apply --result <this file> --jobs 8 --date 2026-11-07` | `apply.log`, `hooks.csv`, `re/analysis/CHANGELOG.md` |
| port column | `scripts/backfill_port.py --allow-stale --build-dir ../C4R-PORTCOL/mashedmod/build` | `backfill_port.log` |
| lint | `re/orchestrator/tracker_lint.py --repo . --base orch/integration` (before and after) | `lint_before.txt`, `lint_after.txt` |
| counts | `ladder_counts.py hooks.csv` before / after | `ladder_before.json`, `ladder_after.json`, `ladder_tables.md` |

**Why a ledger builder in front of make-tsv.** `make-tsv` over the 19 slice `results.tsv` files (pilot, s01-s16, s19)
gives 1,826 rows for 1,782 distinct RVAs, and it cannot be fed to `batch` as is: 10 pilot rows carry the evidence
`g2s_raw` (a folder, `verify/matchpilot_20261030/g2s_raw/<rva>.c`), 33 pilot rows carry a bare file name (lives in
`re/match/pilot/`), 44 RVAs appear twice (a later slice re-ran them, and `batch` refuses a duplicate RVA), and the
FU / FIX slices (`gffu`, `s08fu`, `t01g`, the `matchfix*` and `*fu*` folders) wrote no `results.tsv` at all. So
`build_ledger.py` reads every `// MATCH: 0x<rva>` annotation in `re/match/**/*.c` (2,056 sources, all annotated) and
`verify/matchpilot_20261030/g2s_raw/*.c` (50 annotated): 2,106 sources, 2,451 annotations, 2,450 (rva, source)
pairs, 2,129 RVAs. It recompiles each source once with `c4_reclass.eval_source` and gates every pair with
`c4_reclass.gate_match` (the same code `batch` runs, but independent of `hooks.csv`, so rows already C4 are gated
too). Per RVA the ledger keeps the best source (a PASS first, then the source a slice recorded at 100%, then the
highest instruction score). `batch.tsv` = every RVA whose best source passes now or that a slice recorded at 100%:
1,798 rows. The `batch` run then recompiled everything again before writing.

## 2. The count gap (1,781 / 1,796 / this run)

- Slice `results.tsv` rows at 100%: 1,826 rows, **1,782 distinct RVAs** (44 RVAs recorded by two slices). The
  C4R-PORTCOL count of 1,781 is one under this. I did not find which row it dropped and did not force it.
- Of the 1,782, **3 do not pass the gate today** (section 4).
- **16 RVAs pass with no 100% row in any `results.tsv`**: FU / follow-up sources (`re/match/gffu/005c9d00.c`,
  `re/match/t01g/004111c0.c` for 0x0040fc00) and secondary `// MATCH:` functions in a multi-function source that the
  slice table listed under the file's first RVA (0x004025f0 in `s01/00401690.c`, 0x00404320 in `s01/00403fa0.c`,
  0x00404ee0 in `s01/00404e80.c`), plus s01 / s03 / s19 sources fixed after their slice run (0x004015a0, 0x004095a0,
  0x00412890, 0x00413f20, 0x00414120, 0x00417cf0, 0x0041a1e0, 0x004c07b0, 0x004c0830, 0x004c4430, 0x004c4470).
- 1,782 - 3 + 16 = **1,795 RVAs byte-identical + relocheck OK today**. The survey's 1,796 is one over this.
- 1,795 = 175 already C4 + **1,620 promoted**, the survey's "about 1,620 not yet C4" exactly.

## 3. Ladder before / after (`hooks.csv`, 6,001 csv records)

"other" = 32 records whose confidence cell reads blank through the csv module (the rows of irregular width that
`backfill_port.py` also reports, unchanged by this step). Game vs library = `subsystem` starts with
`third-party-library`.

| scope | C1 | C2 | C3 | C4 | other | rows |
|---|---|---|---|---|---|---|
| overall | 66 -> 57 | 4580 -> 3596 | 1069 -> 442 | 254 -> **1874** | 32 -> 32 | 6001 |
| game | 9 -> 0 | 2226 -> 1242 | 942 -> 315 | 196 -> **1816** | 32 -> 32 | 3405 |
| third-party library | 57 -> 57 | 2354 -> 2354 | 127 -> 127 | 58 -> 58 | 0 | 2596 |

Promoted by old level: C1 9, C2 984, C3 627. No library row was promoted (the MATCH slices only reconstructed game
functions; library C4 is C4R-LIBID).

Top 10 subsystems by rows promoted:

| subsystem | promoted | C1 | C2 | C3 | C4 | rows |
|---|---|---|---|---|---|---|
| render | 418 | 5 -> 0 | 640 -> 354 | 240 -> 113 | 17 -> 435 | 902 |
| gameplay | 274 | 4 -> 0 | 235 -> 63 | 105 -> 7 | 32 -> 306 | 376 |
| util | 184 | 0 | 267 -> 179 | 111 -> 15 | 29 -> 213 | 407 |
| frontend | 180 | 0 | 77 -> 24 | 140 -> 13 | 48 -> 228 | 265 |
| boot | 86 | 0 | 162 -> 90 | 23 -> 9 | 8 -> 94 | 193 |
| vehicle | 84 | 0 | 77 -> 33 | 50 -> 10 | 25 -> 109 | 152 |
| particle | 73 | 0 | 84 -> 26 | 16 -> 1 | 2 -> 75 | 102 |
| audio | 72 | 0 | 443 -> 386 | 140 -> 125 | 1 -> 73 | 584 |
| ai | 51 | 0 | 33 -> 18 | 40 -> 4 | 4 -> 55 | 77 |
| track | 51 | 0 | 57 -> 11 | 5 -> 0 | 1 -> 52 | 63 |

Rest: hud 50, input 27, smplfzx 12, physics 11, camera 10, powerups 10, unknown 8, save 7, sky 5, video 3,
world-objects 2, debug-overlay 2.

## 4. Refused rows

**In the batch (3, all recorded at 100% by their slice):**

| reason | n | rows |
|---|---|---|
| relocheck BAD | 1 | 0x00402750 `re/match/s01/00402750.c`: the string literals are written `"d:\toastart\common\font36.piz"` with single backslashes, so `\t` compiles to a TAB: ours `64 3a 09 6f ...` vs original @ 0x005cc41c `64 3a 5c 74 ...`. The byte compare masks the relocated string address, relocheck compares the pointee and catches it. Fix = escape the backslashes, then it re-enters MATCHBATCH-2. |
| whole symbol equal but shorter than the original body | 1 | 0x0040ac80 `re/match/s02/0040ac80.c`: 12 of Ghidra's 22 instructions (54.5%). S02 recorded it `by=bytes`. |
| not byte-identical | 1 | 0x004c0ad0 `re/match/s19/004c0ad0.c`: 1 differing unmasked byte of 49, insn 100.0%. S19 recorded the instruction score (`by=insn`), not the byte compare. Source sha256 unchanged since S19. |

**Ledger-wide (334 of 2,129 RVAs have no passing source; not in the batch except the 3 above):**

| reason | n | detail |
|---|---|---|
| not byte-identical | 330 | best insn score >= 95%: 79, 80-95%: 118, 50-80%: 90, < 50%: 43 (the 3 above included) |
| shorter than the original body | 2 | 0x0040ac80 (54.5%) and 0x004111c0 (`re/match/t01g/004111c0.c`, 84.7%) |
| relocheck BAD | 1 | 0x00402750 |
| match/relocheck failed | 1 | 0x00494b50 from `re/match/s16/00494b65.c`: "no Ghidra size for 0x00494b50", and `hooks.csv` has no row for it |

By current level: C2 276, C3 45, C4 12, no row 1. No source in the match tree is naked or inline-asm-only (0 asm
refusals, 0 `__asm` bodies of any kind under `re/match/` and the pilot `g2s_raw`). Every source compiled with the DDK
toolchain. Flags: `/O2 /GS /Ob1` 2,050, `/O2 /GS /Ob1 /G5` 77, `/Od` 1, `/O2 /Ob1 /G5` 1.

## 5. Surprises

- **No C4 row lost its match.** All 175 skipped (already C4) rows pass the match gate today. The 12 C4 rows whose best
  source is below 100% were never recorded at 100% by any slice, so nothing regressed: 0x00432b30 (74.2%),
  0x0043c5b0 (40.5%), 0x00467650 (56.1%), 0x00468980 (90.1%), 0x0046b1c0 (97.7%), 0x0046b540 (59.2%),
  0x0046ddb0 (81.7%), 0x00470670 (98.0%), 0x00473ee0 (66.3%), 0x00482900 (85.7%), 0x004c3c60 (97.3%),
  0x004c5010 (84.7%). Not demoted (they keep their frida-route C4).
- 0x00402750 shows relocheck catching a wrong string constant that the masked byte compare scored 100%: any
  pre-relocheck 100% claim with string literals in it is not evidence on its own, which is what L-93 already asks.
- 9 C1 rows went straight to C4 (no C2/C3 plate). L-93 allows it (the match is the identity and the behaviour at once).

## 6. Tracker effects

- `hooks.csv`: 1,620 lines rewritten (confidence `C4`, notes `+ " | 2026-11-07 C4 by route match (L-93): c4=match;
  match.py <src> --bytes BYTE-IDENTICAL ... relocheck OK ...; <this file>"`), every other line byte for byte.
- `re/analysis/CHANGELOG.md`: 1,620 lines below `<!-- ENTRIES -->`.
- `port` column: `backfill_port.py` 0 rows changed (none=5125, body=587, called=253). Run with `--build-dir` on the
  C4R-PORTCOL worktree build and `--allow-stale`: this branch's `mashedmod` tree is the same git tree
  (5e598fea8cf0) as that build's (C4R-PORTCOL worktree clean), the staleness is the checkout mtime of this new worktree.
- `tracker_lint` vs `orch/integration`: before and after both `OK ... pre-existing=3 NEW=0`.
- STUBS / UNCERTAINTIES / DEFERRED: not touched.

## 7. Files

`re/analysis/match_ledger.tsv` (columns: rva, source, n_sources, best_pct, bytes_bad, bytes_n, relocheck,
reloc_unchecked, compiler, flags, asm, recorded100, hooks_conf (before this step), subsystem, verdict, detail).
`verify/c4rmatchbatch_20261107/`: `build_ledger.py`, `ladder_counts.py`, `pairs.tsv`, `batch.tsv`
(sha256 d800c2bdda488548...), `batch_recorded100.tsv`, `verdicts.tsv`, `dryrun.log`, `apply.log`,
`build_ledger.log`, `backfill_port.log`, `lint_before.txt`, `lint_after.txt`, `ladder_before.json`,
`ladder_after.json`, `ladder_tables.md`.

## 8. Next

C4R-MATCHBATCH-2 re-runs `build_ledger.py` + `batch` after the M3 slices. Cheap carry-overs for it: 0x00402750 (escape
the backslashes), 0x004c0ad0 (1 byte), and the 79 RVAs whose best source is at >= 95%.
