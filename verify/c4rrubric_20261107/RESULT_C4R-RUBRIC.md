# RESULT C4R-RUBRIC (roadmap run 35, 2026-11-07)

**Verdict: GREEN.** L-93 (Mariano, 2026-11-07, option B) is written into the rubric, the re-classify skill, CLAUDE.md,
WORKER_RULES.md and the roadmap-loop MATCH section. The re-classify skill has a batch mode, `re/tools/c4_reclass.py`,
self-tested (9/9) and dry-run on 15 real rows. No tracker was changed: `hooks.csv` and `re/analysis/CHANGELOG.md` are
untouched on this branch. Promotions are C4R-MATCHBATCH's job.

No game runs, no gates (text and tooling only, as briefed).

## 1. The new rule (re/CONFIDENCE.md)

C4 certifies the **reverse engineering**, not the port. Any one of three routes:

- **frida (unchanged):** clean `diff-original` on a canonical scenario with the hook installed, plus the Write-set leg.
- **match:** C4 when all hold, nothing else asked:
  1. byte-identical under `re/tools/match/match.py` (relocations masked; `--bytes` whole-symbol compare has no
     differing byte, and the instruction compare is 100% or Ghidra's body is shorter than the symbol) with XP DDK
     `cl` 13.00.9176 / `c2` 13.00.9178 and the source's recorded `// FLAGS:`;
  2. `relocheck.py` OK (unchecked names go in the evidence);
  3. not `__declspec(naked)`, and not a body that is only inline `__asm` in a C frame (SimGolf G26);
  4. no `STUB` in the body.
- **libid:** identical to one function of the library object or the library source built with its recorded flags
  (libmatch `identical` / `identical-unlisted`, one candidate; `identical-ambiguous` refused).

Removed from the C4 gate: old match rules 2 (exe uses the source as body) and 3 (gates unchanged), L-68 (exe caller),
L-84 / L-89 (caller rulings), L-73 (qhull wait), and the libid "port uses the source" clause. They are now in a new
section **"Port axis"**, which defines the `hooks.csv` `port` column (`none` / `body` / `called`, derived by
`scripts/backfill_port.py`, step C4R-PORTCOL) and states that these rulings decide that column. L-67 / L-70 (CRT,
libpng, zlib out of the P-DoD port count) are kept there, with the note that those rows may still carry C4 by libid.

History stays readable: "What C4 meant before 2026-11-07 (history)" says what the old gate asked and that L-93
separated the two questions. Other edits in the rubric: the C3 section's C4 line is now "frida route", the dual-copy
clause gets a scope note (frida route only), a demotion rule for a `match` / `libid` C4 whose cited evidence stops
passing, and the anti-pattern line lists the three routes.

## 2. Batch mode (re-classify skill, "Batch C4 (L-93)")

```
py -3.12 re/tools/c4_reclass.py make-tsv --results verify/matchsNN_<date>/results.tsv [...] --out <batch.tsv>
py -3.12 re/tools/c4_reclass.py batch --tsv <batch.tsv> [--report <verdicts.tsv>]                  # dry run
py -3.12 re/tools/c4_reclass.py batch --tsv <batch.tsv> --apply --result <RESULT.md> [--strict]    # write
```

- Batch TSV header `rva  route  evidence`: `match` -> the matched `.c`; `libid` -> the libmatch TSV.
- `match` rows are recompiled at promotion time (`match.compare(--bytes)` + `relocheck.check`), so a score from an
  older slice run is never trusted on its own. Sources compile in parallel (`--jobs`, default 4).
- Every row prints `[N/M] 0x<rva> <route> PASS|REFUSED|SKIP <detail>`; a refusal names the criterion. No `hooks.csv`
  row -> refused. Already C4 -> skipped.
- `--apply` rewrites only the promoted `hooks.csv` lines (confidence `C4`, notes
  `<date> C4 by route <route> (L-93): c4=<route>; <evidence>; <RESULT>`), leaves the rest byte for byte, and prepends
  one CHANGELOG line per row below `<!-- ENTRIES -->`. `--strict` writes nothing if any row is refused. Idempotent.
- Why line-based: the current `hooks.csv` does **not** round-trip through the csv module (row `0041d820` has an
  over-quoted notes cell), so a whole-file rewrite (the `libid_reclass.py` way) would refuse. The batch tool only
  re-serializes the lines it promotes. Note for whoever runs `libid_reclass.py` next: it will refuse on this file.
- `make-tsv` reads the S01, S03 and S02+ `results.tsv` layouts and emits every row recorded at 100%.

Self-test: `py -3.12 -m pytest re/tools/tests/test_c4_reclass.py` -> **9 passed** (dry run writes nothing, apply
touches only promoted lines and keeps an over-quoted row intact, `--apply` needs `--result`, `--strict`, byte /
relocheck / short-body / toolchain refusals, second apply is a no-op). `test_libid_reclass.py` still 7 passed.

## 3. Dry run on real rows (no tracker writes)

Batch: `dryrun_batch.tsv` = 9 rows of `verify/matchs06_20261101/results.tsv` (3 C2 at 100%, 3 C3 at 100%, the
`by=bytes` row 0x00426700, the 97.9% row 0x00426810, the already-C4 row 0x00426c10), 2 synthetic inline-asm sources
pointed at real C2 rows (`synthetic_asm_naked.c`, `synthetic_asm_g26.c`), and 4 libid rows from
`verify/libidentity_20261030/libidentity_functions.tsv` (Lua identical, qhull identical, CRT identical-ambiguous, Lua
differs). Runtime 2.3 s.

```
[1/15] 0x004260e0 match PASS byte-identical 525 B, relocheck OK (25/0)
[2/15] 0x004262f0 match PASS byte-identical 73 B, relocheck OK (9/0)
[3/15] 0x00426340 match PASS byte-identical 280 B, relocheck OK (18/2)
[4/15] 0x00426070 match PASS byte-identical 6 B, relocheck OK (1/0)
[5/15] 0x00426080 match PASS byte-identical 6 B, relocheck OK (1/0)
[6/15] 0x00426090 match SKIP already C4 (no write)
[7/15] 0x00426700 match PASS byte-identical 125 B, relocheck OK (3/0)
[8/15] 0x00426810 match REFUSED not byte-identical: 29 differing unmasked bytes of 672 (insn 97.9%)
[9/15] 0x00426c10 match SKIP already C4 (no write)
[10/15] 0x004264d0 match REFUSED naked function (__declspec(naked)): an inline-asm transcription matches by construction
[11/15] 0x00426640 match REFUSED body is 2 inline __asm block(s) in a C frame with no other C statement (SimGolf G26 rule: not match evidence)
[12/15] 0x004b6fc0 libid PASS identical _lua_gettop
[13/15] 0x0057e750 libid PASS identical _qh_checkflipped_all
[14/15] 0x004a3090 libid REFUSED libmatch status identical-ambiguous (needs identical, one candidate)
[15/15] 0x004b6fd0 libid REFUSED libmatch status differs (needs identical, one candidate)
batch: 8 pass, 5 refused, 2 skipped (already C4), 15 rows
```

Full output with the 8 CHANGELOG lines `--apply` would write: `dryrun_output.txt`; per-row verdicts:
`dryrun_verdicts.tsv`. 0x00426090 was C3 in the S06 results and is C4 in `hooks.csv` now, so it skipped.

`--apply` was run once against **scratch copies** of `hooks.csv` and `CHANGELOG.md` (module paths pointed at the
copies): 8 rows written, 8 lines differ, nothing else. The written lines: `apply_on_copy_hooks_diff.txt`. Example
(0x00426340): `... | 2026-11-07 C4 by route match (L-93): c4=match; match.py re/match/s06/00426340.c --bytes
BYTE-IDENTICAL 280 B, insn 100.0% (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, c); relocheck OK checked=18
unchecked=2 [@__security_check_cookie@4,___security_cookie]; src sha256 b0caf4e07578fb25; verify/matchs06_20261101/RESULT_MATCH-S06.md`.

## 4. Files changed

- `re/CONFIDENCE.md`: C4 section, C4 routes, new Port axis section, scope note, demotion rule, anti-pattern line.
- `.claude/skills/re-classify/SKILL.md`: gate table, "Evidence types `match` and `libid`" rewritten for L-93, new
  "Batch C4 (L-93)" subsection, CRT / libpng / zlib / add-rows text now says the waiver does not block libid C4.
- `re/tools/c4_reclass.py` (new), `re/tools/tests/test_c4_reclass.py` (new).
- `re/tools/libid_reclass.py`: docstring line only (waive-lib no longer says "never libid C4").
- `CLAUDE.md`: section "Confidence promotion (NO overclaiming)" only. "Roadmap, DoD, and trackers" not touched
  (C4R-ROADMAP owns it).
- `re/orchestrator/WORKER_RULES.md`: the "No C4 without a canonical-scenario run" line.
- `.claude/skills/roadmap-loop/SKILL.md`: MATCH section (L-93 rule, batch tool, match-only slices, body swaps are
  port-axis work).
- `ROADMAP.md`, `re/NEXT_SESSION.md`: not touched.

## 5. For the next steps

- **C4R-MATCHBATCH:** `make-tsv` over every `verify/match*/results.tsv`, dry run with `--report`, then `--apply
  --result <its RESULT>`. Expect refusals where a slice source was edited after its run or relocheck was never OK.
  Rows with no `hooks.csv` row are refused and need a C1 row first.
- **C4R-LIBID:** batch rows `rva libid verify/libidentity_20261030/libidentity_functions.tsv` (and the pngzlib TSVs).
  The 17 `identical-ambiguous` rows in that TSV stay below C4 under this gate.
- **C4R-PORTCOL:** the rubric already names `scripts/backfill_port.py` and the three values. If that step settles on
  different value names, update "Port axis" in `re/CONFIDENCE.md` with it.
