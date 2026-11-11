# RESULT HB-V5-SKILL (run 37, section T0-TOOLS)

Verdict: GREEN. These are process and text changes to three orchestrator files. No build input changed, so no
build and no gates ran (match-only / tracker / text row kind).

## What changed

1. **`.claude/skills/roadmap-loop/SKILL.md`** (the L0 conductor)
   - New section "Row kinds (ROADMAP v5, L-127)". It says what each kind runs.
     - A top-down row runs both builds (default VS2022 and `MASHED_DDKOBJ=1`), `ddkobj.py check`, the knob boot,
       its self-test, and the menu walk when menu code changed. It runs no G-CTRL, no B and no 12x4 captures in the
       worker. The train runs the default gates once per batch.
     - The section also covers race-path, default-path and text rows, plus the L-124 RenderWare route.
   - The L-126 admission rule: EQUIV and EQUIV-IF-CALLERS-IGNORE are admitted into `ADMITTED.tsv`. A NOT-PROVEN
     source runs only as a listed provisional body (`PROVISIONAL.tsv`). Neither status is C4 (L-128). The section
     lists what is still a decision row.
   - Standing classes P1 / P2 (L-127): no `roadmap.ps1 decision` for them. Instead, one line in `DECISIONS.md` reading
     "auto, class Pn" with the evidence path.
   - Frozen rows (note starts "FROZEN by ROADMAP v5") are skipped in wave planning. The lead template tells leads not
     to claim them, and the worker task now carries the row kind.
   - The test-slot count changes from 6 to 8, per the workspace rule of 2026-11-09, with the memory gate as the real
     limit.
   - Run tag: L0 runs `roadmap.ps1 run <N>` at start, `list -Run <N>` on stop, and the kickoff template carries both.
   - Notes that INTEGRATION_LOG.md now records RED passes.
   - The run-37 trial model/effort table is NOT in SKILL.md (Mariano decides after the run). The role table is
     unchanged.
2. **`re/orchestrator/roadmap.ps1`**: run tag on ledger rows.
   - `run [<n>]` prints or sets the ledger's current run, as a top-level `run` field. An event line records the set.
   - claim, set, enqueue, decision and add stamp the row: `run` is the last run that moved it, and `runs` is every
     run that moved it. events.jsonl lines carry `run`. `-Run <n>` overrides it on any write.
   - `list [-Run n] [-Stage s]` prints one line per row plus a count-by-stage summary line. `status` shows the
     current run.
   - Rows moved before the tag existed have no `runs`. No backfill was done: events.jsonl never held a run number.
   - Compatible both ways. The old script (orch/integration `778b8b91d`) loads and saves a tagged ledger and keeps
     `run` / `runs`: checked on a copy, see below.
3. **`re/orchestrator/merge_train.ps1`**: INTEGRATION_LOG.md records RED passes.
   - Before, a line was written only when something merged.
   - Now a mixed pass names its bounced rows on the merged line (`Bounced: <id> (<branch>): <why>`).
   - A pass that bounced rows and merged nothing calls `Add-RedLogLine`. It commits a single "RED, nothing merged"
     line, touching only that file, in the worktree holding orch/integration. That is the same place
     `roadmap.ps1 snapshot` commits, and the next pass already folds such ledger-only tips.
   - An INFRA pass that bounced nothing writes no line.
   - Fix found by the selftest: a single-line `if/else` array unrolled to a string, so the header became `#`.
     `New-LogLines` now wraps it in `@()`.

## The six rows stuck on a merge conflict since run 29: closed

- The rows: `D4-DC-INC-00426090`, `-00426bb0`, `-00426bb0-X`, `-00426cb0`, `-00426cc0`, `-00426cf0` and
  `-00426d00`. That is seven ids, all on branch `orch/D4-DC-INC-pathprog` (6322ca50d), which bounced RED on a merge
  conflict with batch3 in train 20261101_165639.
- `00426090` and `00426cb0` were already `done`. The five still `blocked`, and since 2026-11-11 also FROZEN, were
  set to `done` in run 37.
- Evidence: `git merge-base --is-ancestor orch/D4-DC-INC-pathprog orch/integration` is true, and
  `git log orch/integration..orch/D4-DC-INC-pathprog` is empty.
- The branch landed through `D4-DC-INC-PATHPROG-REMERGE` (d351e7dcb, train 20261106_015649 GREEN: G-CTRL
  8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3, D2 3/3, 13 tracks PASS).
- No re-merge was needed and no hand-path work was done. The branch is left as it is.

## Checks run

- `merge_train.ps1 -SelfTest` on this branch: 103 PASS, 1 FAIL.
  - The two new checks pass: "RED pass log: 2 commits touching only INTEGRATION_LOG.md, newest first, bounced rows
    named, 0 CR", and the run-body wiring check.
  - The one FAIL is `match-textonly (live tree): ... 1 of 4061 re/match files are` build inputs. It fails the same
    way on unchanged orch/integration `778b8b91d` (baseline: 101 PASS, 1 FAIL, same line), so it was there before
    this change.
- `roadmap.ps1` on a copy of the live ledger (`MASHED_ORCH_STATE` pointed at a scratch copy):
  - `run` gave 0. `run 37` gave `RUN 0 -> 37`.
  - `add`, `claim`, `set working`, `set review -Run 38` and `enqueue` gave `run 37`, `runs [37, 38]`, and every
    event line carries `run`.
  - `list -Run 37` and `list -Run 38 -Stage merge-queued` each gave 1 row. `list -Run 36` gave 0 rows. `status`
    prints `run 37`.
  - A one-run row keeps `runs` as a JSON array.
  - The old script's `set` on the tagged copy kept `run` / `runs`.
- Live ledger, with this branch's script: `run 37` was set, the five `set ... done` writes are above, and
  `list -Run 37` was run.
- LF/CRLF: all three files stay CRLF-only, as they were (0 mixed lines).

## Not done / follow-ups

- The live ledger now has `run = 37`, set through this branch's script. Until this branch merges, the old script
  keeps the field but does not stamp rows. Rows moved meanwhile get no `runs`.
- WORKER_RULES.md already had the row-kind table and the object-build step (ROADMAP-V5, e393f1da9), so it was not
  touched.
