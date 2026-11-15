# RESULT MATCH-TAG-LINT (text/tool row, no build, no gates)

Head: orch/integration d21abc196. New files: `re/tools/match/tag_lint.py`, `re/tools/tests/test_tag_lint.py`. No source, tracker or other tool edited.

## What it checks
Two readers of the tag differ:
- `re/tools/match/match.py:317` (and `match_permute.py:48`): unanchored `//\s*MATCH:\s*(?:MASHED\.exe\s+)?(0x..)\s+(\S+)`, finds a tag anywhere on a line.
- `re/tools/ddkobj/ddkobj.py:62` (`MATCH_RE`): `^\s*//\s*MATCH:...` with `re.M`, finds a tag only with whitespace before it.

`tag_lint.py [--root <dir>] [--quiet]` walks `re/match/**/*.c` and reports each tag that has non-whitespace before it on its line (match.py sees it, ddkobj does not). Prints counts and a per-file `file:line: va after <code>` list. Exit 1 when any is found, 0 otherwise.

## Run on the head (`verify/matchlint_20261115/tag_lint_head.txt`)
- Scanned 4273 `.c` files, 0 with no tag at all.
- Flagged: **1 file, 1 tag**, exit code 1:
  - `re/match/s28/005bf690.c:29`: `0x005bf690` after `__declspec(dllimport) LONG __stdcall InterlockedDecrement(LONG * p0);` (the file has exactly one `MATCH: 0x` line, so ddkobj's `MATCH_RE` finds no tag in it).
- Not flagged on the head: `re/match/rwl02/004e4c30.c` (0x004e4c30). Commit e018bf5bf (MATCH-RWT-PORT-FOLLOWUPS) already moved its tag from `int FUN_004e4dd0(int p0);// MATCH: 0x004e4c30 RpLightStreamRead` onto its own line. Re-checking the pre-fix text with `lint_text` in the self-test reproduces the flag shape (test `test_trailing_tag`).

## Self-test
`py -3.12 re/tools/tests/test_tag_lint.py`: 4 tests, OK (clean files incl. indented and `MASHED.exe` forms, trailing tag exit 1, untagged file ignored, line number and prefix reported).

## Follow-up rows (sources not fixed here)
- MATCH-TAG-FIX-005BF690: put the `// MATCH: 0x005bf690 FUN_005bf690` tag of `re/match/s28/005bf690.c:29` on its own line (comment-only change, then confirm byte-identical with match.py and relocheck.py). Whether ddkobj currently refuses or skips this unit because of it is [UNCERTAIN]: test = run ddkobj on the unit before and after.
- Optional: call `tag_lint.py` from the merge-train or pre-commit checks so a new unit with a trailing tag fails early (needs a decision on where it runs).
