# RESULT HB-TRACKER-AT-MERGE

Verdict: GREEN (tool row, no build, no gates, no game run).

## What changed
- `re/tools/tracker_merge3.py` (new): keyed 3-way merge. `hooks` mode keys on the RVA (first field, 8 hex), `allow` mode
  on the first token of a non-`#` line (`re/tools/dual_copy_allowlist.txt`). Per key: one-sided change or delete stands,
  identical change keeps one, new rows from the other side are inserted after their predecessor, the same key edited
  differently on both sides = conflict (exit 1, keys on stdout, `<ours>` untouched). CRLF kept.
  `re/tools/tracker_merge3_selftest.py`: 11 fixtures, 11/11 PASS.
- `re/orchestrator/merge_train.json`: `conflict_keyed` {hooks.csv: hooks, dual_copy_allowlist.txt: allow};
  `re/promotions/*` added to `text_only_globs` (a promotions-only branch stays text-only; `.tsv` was not covered).
- `re/orchestrator/merge_train.ps1`:
  - `Merge-One`: keyed files go through `tracker_merge3.py` before the halt check (a same-key conflict aborts the merge
    and names the RVA in the set-aside reason, as before for other conflicts).
  - `Apply-Promotions` (called from `Try-Batch` after each row's merge): for every `re/promotions/<ID>.tsv` the merge
    adds, runs `c4_reclass.py batch --tsv ... --result <result column> --apply --strict` in the merged tree (the
    current head, so earlier rows of the batch are already in hooks.csv), commits hooks.csv + CHANGELOG and moves the TSV
    to `re/promotions/applied/`. A refused row or a missing `result` column sets the branch aside (conflict path).
  - 5 new self-test checks on a second fake repo with real merges.
- Docs: `WORKER_RULES.md` (promotion convention) and `roadmap-loop/SKILL.md` (c4_reclass paragraph).

## Evidence
`pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest`: `SELFTEST done: 0 failure(s)` (full log
`verify/hbtracker_20261112/selftest_full.log`, 48 KB). The 5 new checks:
1. two branches on different hooks.csv rows both merge (3 rows, no duplicate RVA)
2. the same RVA edited two ways = set aside naming `00401000`, merge aborted, tree clean
3. promotions TSV applied on the current head: row C4 with the other branch's notes edit kept, CHANGELOG line, TSV moved to applied/
4. refused promotion (write-set not covered) = branch set aside, hooks.csv left at C3
5. `tracker_merge3_selftest.py` 11/11

## Limits
- The match route needs the DDK toolchain (`tools/vc7ddk`) inside the merged tree's reach; only the `frida-replay`
  route was exercised on the fake repo. libid/match go through the same `c4_reclass.py` code path unchanged.
- Other trackers (STUBS.md, UNCERTAINTIES.md) keep the hand-merge rule: no run showed the same pattern, and they have no
  per-key line shape. Not changed.
- Rows already in flight with `--apply` on their branch keep working (the CHANGELOG union still applies; a hooks.csv
  same-RVA edit is still a conflict).
