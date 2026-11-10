# RESULT HB-TRAIN-CONFLICT-SKIP: a merge conflict sets one branch aside, the rest is gated as one batch

Verdict: GREEN (self-test 89 PASS, 0 FAIL; no game launched). Stacked on orch/HB-TRAIN-DRAIN.

## Evidence for the change
Pass `D:/MashedWT/mtrain_data/20261110_042108/merge_train.log`: batch PORT-OT-RWG-SLOTS + PORT-OT-FRONTEND +
PORT-COLOURSEL-COMMIT. At 04:22:33 PORT-OT-FRONTEND conflicted on hooks.csv while the batch was merged, before any
build or gate. The train logged `batch RED (conflict needs a hand merge ...): re-merging one at a time` and then ran the
gates once per row (two full gate sets), and the pass ended at 05:00:10 (39 min).

## What changed (`re/orchestrator/merge_train.ps1`)
- `Try-Batch` marks a failed merge with `conflict = $true` (with `bad` = the row). The merge happens before archive
  lint, tracker lint, build and gates, so a conflict result never ran any of them.
- `Resolve-Batch`: while the result is a conflict, the conflicting row is bounced (`conflict = $true`, gates
  `not run: merge conflict, set aside before build and gates`) and the remaining rows are merged and gated again as ONE
  batch, as many times as conflicts come up. A throw in that re-batch stops the pass with the rows left queued (same
  rule as HB-TRAIN-WTADD-RETRY). Only a failure after the merge (build, lint, gate RED) enters the existing
  one-at-a-time path, and then only for the rows still in the batch. INFRA keeps its rule (no bounce for it, the rest
  stays queued); a conflict row found before it is still bounced.
- The bounced row goes to `red-merge` and is dequeued as before; the REPORT names it `set aside, conflict ...`.
- Pass result file: each bounce carries `conflict`, plus `conflict_only` (verdict RED, every bounce a conflict, no
  error, and either something merged or every row conflicted).
- Drain (HB-TRAIN-DRAIN): a `conflict_only` pass is not a stop. If rows merged it fast-forwards and runs the release
  step as for GREEN (the same hash checks apply); if nothing merged it goes to the next pass without moving anything.
  A gate-RED bounce still stops the drain. The `max_passes` stop moved after the loop so a skipped pass cannot hide it.
- Doc: `re/orchestrator/MERGE_TRAIN.md` "Merge conflicts".

## Self-test
Summary lines: `verify/hb_train_20261110/selftest_conflict_skip.txt`. Raw log (not committed):
`C:/Users/maria/AppData/Local/Temp/claude/2279e0b7f1f09563/2e04f018-b1e0-463e-8db5-d34153b1d580/scratchpad/st_cs.log`,
SHA-256 `9575a71beb8d95c2208dafef4c9d3c23c84569e27257036d366c3987806bd384`.

New checks:
- Real merges on the fake repo, real Try-Batch + Resolve-Batch: A merges, B conflicts with A on `notes/conf.txt`
  (outside conflict_union), C clean. B is bounced as a conflict with gates "not run"; A+C are merged a second time and
  gated once as one batch (A merged twice in the log, no "re-merging one at a time"); the landed tree holds A's
  conf.txt and C's c.txt only.
- Fake Try-Batch routing: B conflicts -> calls `A+B+C, A+C`; two conflicts in a row -> `A+B+C, B+C, B`; the rest
  gate-RED -> `A+B+C, A+C, A, A+C` (one-at-a-time on the rest only, C bounced as RED, B as conflict); a single
  conflicting row -> one call, bounced; the rest INFRA -> B bounced, the rest stays queued.
- Drain: conflict-only pass with A landed -> fast-forward + release, next pass runs; conflict-only pass with nothing
  merged -> no fast-forward, no release, next pass runs; a gate-RED bounce still stops.
All earlier checks (HB-TRAIN-DRAIN, HB-TRAIN-STACK, INFRA, WTADD-RETRY, AST shape) still pass.

## Gate time saved
On the 20261110_042108 batch this rule gates RWG-SLOTS + COLOURSEL-COMMIT once instead of twice: one full gate set
fewer. Not measured live.
