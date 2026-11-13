# RESULT HB-DRAIN-VERDICT (run 38, tools row)

Verdict: GREEN. `merge_train.ps1 -Drain` now reports `verdict: PARTIAL` (exit 2) when the queue drained but at least
one pass set rows aside on a merge conflict. Before this change the same drain reported `verdict: GREEN` (exit 0).

## Problem

A conflict-only RED pass (every bounce a merge conflict, `conflict_only = true` in the pass result) does not stop the
drain on purpose (HB-TRAIN-CONFLICT-SKIP): the rest of the batch is fast-forwarded and released. `Invoke-Drain` then
set the drain verdict from the stop reason alone (`if (-not $stop) { 'GREEN' }`), so the set-aside rows were dropped
from the verdict. Run 37, second drain: pass 1 RED (HUDINIT-A set aside on hooks.csv), drain verdict GREEN.

## Change (`re/orchestrator/merge_train.ps1`)

- `Invoke-Drain` collects the ids of every conflict-only pass's bounces in `$aside`.
- Verdict: a stop keeps its own verdict (RED / INFRA / HASH / FLIP / ERROR / STOP). With no stop, any set-aside row
  gives `PARTIAL`, else `GREEN`.
- The REPORT `next:` line ends with `| set aside on a merge conflict, needs a hand merge (frida-sweep recipe) and a
  re-queue: <ids>` whenever rows were set aside, also when a later pass stopped the drain.
- `-Drain` exit code: 0 GREEN, 2 PARTIAL, 1 any stop.
- The returned hashtable carries `aside`.
- Doc: script header and `.claude/skills/roadmap-loop/SKILL.md` (drain mode paragraph) name PARTIAL.

Behaviour of the drain itself (fast-forwards, release step, when it stops) is unchanged. Only the verdict, the
`next:` text and the exit code change.

## Verification

`pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest` (fake repo, fake pass results, never reads the live
queue), run from this worktree: **114 PASS, 0 FAIL**, rc 0. Log: `verify/hbdrain_20261112/selftest.log`.
The self-test cases that cover this row:

| Case | Passes | Expect | Result |
|---|---|---|---|
| drain 12 | conflict-only RED (Q2 set aside), GREEN | PARTIAL, `aside`=Q2, ff + release after both passes | PASS |
| drain 12 (nothing merged) | conflict-only RED with nothing merged, GREEN | PARTIAL, no ff after pass 1 | PASS |
| drain 12 (gate RED) | RED with a gate bounce | RED, stops | PASS |
| drain 12b (new) | conflict-only RED, INFRA | INFRA, Q2 still named on `next:` | PASS |
| drain 12c (new) | GREEN, GREEN | GREEN, no set-aside clause | PASS |
| drain 12d (new) | source check | `-Drain` exits 2 on PARTIAL | PASS |

Drain cases 1 to 10 (GREEN, RED, INFRA, HASH, FLIP, index.lock, diverged main, wrong branch, release errors, empty
queue) still pass unchanged. The header doc edit came after the self-test run and is comment-only. The PowerShell
parser reports 0 errors on the final file. No real merge into orch/integration was run, and no game run, build or
gate run was needed (tools row, no exe change).

## Auto-resolve of hooks.csv / allowlist conflicts: recommendation, not implemented

In run 37, every set-aside (INTEGRATION_LOG lines for 2026-11-11) was on `hooks.csv` (HUDINIT-A, MATCH-RT2D-FONT-2),
`re/tools/dual_copy_allowlist.txt` (RWC-BIND-4C5890) or a matched source `re/match/rwl05/00554390.c`
(MATCH-RW-COMPILER-9044). The train already line-unions the files in `merge_train.json` `conflict_union`
(`Merge-One`, `git merge-file --union`). I did not extend it, for these reasons:

1. **hooks.csv: a line union is unsafe.** Two branches that touch the same RVA give two rows for one RVA. The tracker
   lint would bounce that as NEW DUP, so the result is the same set-aside, now with a lint failure. Two branches that
   touch different rows next to each other union correctly, but the train cannot tell the cases apart without parsing
   the file. Recommended: a keyed 3-way merge (`re/tools/hookscsv_merge3.py`) that indexes base / ours / theirs by
   RVA. It takes a row changed on one side only, takes rows added on one side only, and on a row changed on both sides
   with different values reports a conflict, which the train then sets aside as it does today. Run it through
   `Merge-One` for `hooks.csv` before the halt check, and add tracker-lint (already in the pass) as the backstop. Size:
   small to medium, needs its own self-test with 3-way fixtures.
2. **`re/tools/dual_copy_allowlist.txt`: a union is low risk** (one entry per line, not checked here whether order matters to the scanner),
   but adding it to `conflict_union` changes the default-path merge config, and the allowlist feeds the dual-copy lint
   in the build. That is the lead's call: a one-line `merge_train.json` edit plus a self-test fixture.
3. **`re/match/**` sources: never auto-resolve.** A conflicting matched source has two candidate bodies. Only one
   can be byte-identical, so a hand merge with `match.py` stays required.

## Files

- `re/orchestrator/merge_train.ps1` (verdict, report, exit code, 4 new or changed self-test checks)
- `.claude/skills/roadmap-loop/SKILL.md` (drain mode paragraph)
- `verify/hbdrain_20261112/selftest.log` (47 KB, self-test output)
