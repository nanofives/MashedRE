# RESULT C4RECLASS-SHA-REFRESH

Verdict: GREEN (tool + tests + train hook; the refresh TSV moved to orch/HOOKS-SHA-RESYNC-AUDIOTICK in run 48). Row kind: tool/text, no build, no gates.

## What changed
- `re/tools/c4_reclass.py`: new `refresh` subcommand (docstring documents it). TSV `re/promotions/<ID>-refresh.tsv`,
  tab-separated header `rva`, `src`, `result`. Per row the tool recompiles `src` (match.compare --bytes + relocheck, the
  same `gate_match` as the match route) and REFUSES unless byte-identical + insn 100% + relocheck OK with a recorded
  compiler. Also REFUSED: row not in hooks.csv, row not at C4, row not `c4=match`, `src` differs from the source named
  after `match.py` in the note, no `src sha256 <16 hex>` token, duplicate rva. SKIP: token already current (idempotent).
  Only the last `src sha256` token in the notes is replaced. One CHANGELOG line per refreshed row. Dry run by default,
  `--apply --strict` as the batch path, `--hooks/--changelog` for rehearsals.
- `re/orchestrator/merge_train.ps1` (`Apply-Promotions`): a new `re/promotions/<ID>-refresh.tsv` is applied with
  `refresh --apply --strict`, committed and moved to `re/promotions/applied/`, like `-tag.tsv`. Batch files exclude it.
- Docs: `.claude/skills/re-classify/SKILL.md`, `re/orchestrator/WORKER_RULES.md`.
- Tests: the repo's existing file is `re/tools/tests/test_c4_reclass.py` (there is no `re/tools/test_c4_reclass.py`;
  a second file with that name would collide on import), so 8 refresh test functions were appended there.
  Run: `test-slot py -3.12 -m pytest re/tools/tests/test_c4_reclass.py -v` -> **21 passed in 2.35 s** (the rest existing; the new
  ones: changes only the sha, idempotent, dry run writes nothing, refuses below 100% (bytes and insn), refuses relocheck
  not OK, refuses not C4, refuses other source / missing token, --strict writes nothing on one refusal).
  Log: `verify/c4reclass_20261115/pytest_c4_reclass.txt`. The train step itself (ps1) was not exercised end to end:
  no train self-test case was added for refresh (the existing train self-test covers `-tag.tsv`).

## Refresh TSV and dry run (run 47, superseded: see Run 48 update)
`re/promotions/HOOKS-SHA-RESYNC-AUDIOTICK-refresh.tsv`. Dry run only (`verify/c4reclass_20261115/dryrun_audiotick.txt`):
```
[1/7] 0x00462dd0 refresh SKIP src sha256 ea5082998cc2a73b already current (no write)
[2/7] 0x00462ec0 refresh SKIP src sha256 aff0cadf0330f224 already current (no write)
[3/7] 0x0045cc50 refresh SKIP src sha256 07f440b85d1a5fb7 already current (no write)
[4/7] 0x004645c0 refresh SKIP src sha256 bb2e111af766ce04 already current (no write)
[5/7] 0x0045d250 refresh SKIP src sha256 b636755ba58c30c7 already current (no write)
[6/7] 0x00411530 refresh SKIP src sha256 0f47729eea7fbbd5 already current (no write)
[7/7] 0x004115c0 refresh SKIP src sha256 30da0a03283117cd already current (no write)
refresh: 0 would-refresh, 0 refused, 7 skipped (already current), 7 rows
```
All 7 are current against this worktree (base 37d117ca5, working-tree bytes), so the TSV applies as a no-op here.

## Finding for L0 (run 47, settled in Run 48 update)
The hash is of the working-tree bytes. With `core.autocrlf=true` the CRLF checkout hashes to `ea5082998cc2a73b` for
`re/match/s10/00462dd0.c`, while the committed (LF) blob hashes to `58b4c3e5e9bfc2b2`. A hash taken from a blob or an
LF checkout (hub notebook, `git archive`) reads as stale against the notes. That may be the origin of
HOOKS-SHA-RESYNC-AUDIOTICK. Decide whether the stale audit was run on LF bytes before applying this TSV: if the audit
hashes LF bytes, refresh would write CRLF-based hashes and the audit stays red. The tool was not changed to normalise
line endings (that would change every existing note's meaning). I did not check the other 6 sources' blob hashes.

## Run 48 update (2026-11-15): tool only on this branch, CRLF vs LF settled
- The refresh TSV was removed from this branch in a new commit a9d662969 (no rebase, no amend). The branch now carries
  the tool, the train hook, docs, tests and this RESULT only.
- Tests re-run on a9d662969: `test-slot py -3.12 -m pytest re/tools/tests/test_c4_reclass.py -v` -> **21 passed in
  8.15 s** (log `verify/c4reclass_20261115/pytest_c4_reclass.txt`, overwritten by this run).
- The dry run section above was taken at base 37d117ca5, before the audio-tick source edits reached the branch base,
  so its "all 7 SKIP" says only that the tokens matched the pre-edit sources. It is superseded below.

### Which bytes the stored `src sha256` is
`re/tools/c4_reclass.py:352` (gate_match, the match-route C4 writer) and `:406` (extent route):
`hashlib.sha256(path.read_bytes()).hexdigest()[:16]`, the file as it sits in the working tree of the checkout that ran
the promotion. On these Windows checkouts `core.autocrlf=true` and no `.gitattributes` rule covers `re/match/**/*.c`,
so those bytes are CRLF. The refresh subcommand uses the same function, so it writes the same convention.

Recomputed, all 8 rows (the 7 named plus 0x004624c0 from the run 44 ledger note), both ways:
- at 37d117ca5 (this branch's base): every stored token equals the CRLF working-tree hash, none equals the LF blob hash
  (0x00462dd0: stored ea5082998cc2a73b, CRLF ea5082998cc2a73b, LF blob 58b4c3e5e9bfc2b2; same pattern on all 8).
- at orch/integration 790e30336: every stored token equals NEITHER (0x00462dd0: CRLF f60e8f24192c3ede, LF blob
  24b9b73951b52081). Cause: 7e5466a15 (PORT-TOP-AUDIO-HWINIT-4627F0) and 4c86fecfe (PORT-TOP-AUDIOTICK-STOPS) edited all
  8 sources (12 insertions, 8 deletions). So the hashes ARE stale, and for a content reason, not a line-ending one.
  Full table in `verify/audio_20261114/RESULT_HOOKS-SHA-RESYNC-AUDIOTICK.md` on orch/HOOKS-SHA-RESYNC-AUDIOTICK.

Line endings remain a trap for anyone hashing a blob, `git archive` or an LF checkout (the hub notebook): those read
every token as stale. The tool was not changed to normalise (that would change every existing token).
[UNCERTAIN] whether every existing promotion ran on an autocrlf=true checkout: shown only for these 8 rows by the
recomputation above, not for the rest of hooks.csv.

### Where the refresh went
Branch orch/HOOKS-SHA-RESYNC-AUDIOTICK @ 9939cddea (existing worktree `.worktrees/HOOKS-SHA-RESYNC-AUDIOTICK`):
`re/promotions/HOOKS-SHA-RESYNC-AUDIOTICK-refresh.tsv`, 8 rows. Dry run there with this branch's tool: 8 would-refresh,
0 refused, 0 skipped, each still byte-identical + relocheck OK. Rehearsal `--apply --strict` on scratch copies of
hooks.csv and CHANGELOG.md: 8 rows written. Not enqueued: it needs this tool merged first.
