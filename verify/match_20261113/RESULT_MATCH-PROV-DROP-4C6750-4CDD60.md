# RESULT MATCH-PROV-DROP-4C6750-4CDD60 (text/tracker row, no build, no gates)

Verdict: GREEN

## What changed
Removed the rows for 0x004c6750 (RwCore_004c6750) and 0x004cdd60 (RwCore_004cdd60) from both lists, CRLF kept:
- re/match/PROVISIONAL.tsv (was rows 78 and 113)
- verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv (was rows 60 and 96)
`git diff --stat`: 2 files, 4 deletions, 0 insertions. No other row touched.

## Why they are droppable
Commit 757ceb6a6 (MATCH-RWT-S2) made both sources byte-identical (rw9044), relocheck OK, with
re/promotions/MATCH-RWT-S2.tsv. The NOT-PROVEN 83.3% / 79.2% provisional status no longer describes them.
That commit is in orch/integration (reachable from the worktree base e47e10795).

## How the files relate
- re/match/PROVISIONAL.tsv is the list tools read: re/tools/ddkobj/ddkobj.py `load_provisional()` (line 283).
  Other mentions (port_closed_sets.py, provbodies/gen_wrapper.py, prov/prov_fixlink.py) are comments or docs.
- verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv is the RWC-CORE-RUN evidence copy. Only prose/ledger
  files name it (DECISIONS.md, INTEGRATION_LOG.md, ledger). No tool reads it.
- Reason is recorded, so both were dropped to stay consistent.

## Check
`load_provisional()` before: 1003 RVAs. After: 1001, and 0x004c6750 / 0x004cdd60 are no longer in it.
`ddkobj.py check` not run: it needs mashedmod/build/ddkobj/ddkobj_build.json from a build, and this row has no build.
No lint reads these rows besides the loader above.

## Read-only note: other rows that pending branches now byte-match (not edited here)
Both lists carry these rows, so the merge of the branches can drop them:
- orch/MATCH-RWT-S1B: 0x004cb4c0 (PROVISIONAL.tsv row 93, RWC row 75)
- orch/MATCH-RWT-S3: 0x004cbbd0 (PROVISIONAL.tsv row 99, RWC row 82) and 0x004e5fc0 (PROVISIONAL.tsv row 300, RWC row 283)
Row numbers are from before this edit. I did not open those branches to verify their match, they come from the task text.
