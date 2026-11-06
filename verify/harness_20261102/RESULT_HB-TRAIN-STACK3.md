# RESULT HB-TRAIN-STACK3: three harness branches stacked on one head

Verdict: **GREEN** (static). `merge_train.ps1 -SelfTest` on the stacked head: **55 PASS, 0 failures**.
No game boot (static task, display asleep).

## Branch

`orch/HB-TRAIN-STACK3`, cut from `orch/integration` @ `78be531e1`. Merge commits, in order:

| # | merged branch | source sha | merge commit | conflicts |
|---|---|---|---|---|
| 1 | `orch/HB-TRAIN-INFRA-PRECHECK` | `01fba6d56` | `4e974d91c` | none |
| 2 | `orch/HB-SELFTEST-D2DEFH` | `a9c3cc726` | `6cd8a056b` | `re/orchestrator/merge_train.ps1`, 3 hunks |
| 3 | `orch/HB-TRAIN-HOOKSCSV-TEXTONLY` | `f57d1bfeb` | `b380f9ede` | `re/orchestrator/merge_train.ps1`, 4 hunks |

All merges ran with `git -c merge.autoStash=false merge --no-ff`.

## Conflicts and resolutions

### Merge 2 (SELFTEST-D2DEFH into INFRA)

All 3 hunks sit in the HB-D2RERUN self-test block (fake arm runner + 3 checks). Both sides made the
same change: replace the hard-coded `D2_DEF_1` / `D2_DEF_2` with `merge_train.json` `full.d2_arms[0]` / `[1]`.
INFRA named them `$d2First` / `$script:d2Short`, SELFTEST named them `$A1` / `$A2`.

Resolution: took the INFRA side in all 3 hunks (one copy of the parametrisation, `$script:d2Short` /
`$d2First`). SELFTEST-D2DEFH's behaviour (arm names read from `d2_arms`, so the checks pass on
`D2_DEFH_n`) is kept in full. One extra line edit: the block's header comment still said
"D2_DEF_2 comes back short", changed to "the second d2_arms arm comes back short". No other file
conflicted. `verify/harness_20261102/RESULT_HB-SELFTEST-D2DEFH.md` came in unchanged.

### Merge 3 (HOOKSCSV-TEXTONLY into the stack)

| hunk | location | ours (INFRA stack) | theirs (HOOKSCSV) | resolution |
|---|---|---|---|---|
| 1 | after `Gate-Full` | new `Run-Gates` + `Resolve-Batch` | new `Tracker-Lint` | both kept: `Run-Gates`, `Resolve-Batch`, then `Tracker-Lint` (closing brace added between) |
| 2 | self-test, before HB-D2RERUN | HB-D2RERUN header with `$script:d2Short` / `$d2First` | tracker self-test block (6 Try-Batch cases + 2 glob controls), then HB-D2RERUN header with `$script:A1` / `$script:A2` | tracker block kept, then the ours D2 header (third copy of the D2 parametrisation dropped, resolved once) |
| 3 | fake `Run-Arms` line count switch | `$script:d2Short` | `$script:A2` | ours |
| 4 | 3 D2 checks + INFRA self-test block | INFRA checks and the whole INFRA-PRECHECK self-test | the 3 D2 checks with `$script:A1/A2` | ours (keeps the INFRA self-test) |

Auto-merged without conflict: the `Tracker-Lint` call inside `Try-Batch` (after archive lint, before the
text-only return, so it runs on every batch and the text-only return still comes before `Run-Gates`),
`merge_train.json` (`text_only_globs` gains `hooks.csv`), `re/orchestrator/tracker_lint.py`, and the
HOOKSCSV evidence files.

Ordering check: the tracker self-test calls the real `Try-Batch` and runs before the INFRA self-test
redefines `Run-Arms`, `Test-RenderProbe`, `Ensure-Root` and its fake `Try-Batch`, so each block exercises
the code it was written against. The `$base` the tracker block reassigns is script-scope only in the
self-test, and the INFRA block's `$base` is a function local inside its fake `Run-Arms`.

## Behaviour kept from each branch (all exercised by the self-test below)

- INFRA-PRECHECK: `Get-ArmInfra` (7 unit checks), render precheck BLOCKED = INFRA before any arm, INFRA
  bounces nothing and leaves rows queued, `Resolve-Batch` one-at-a-time stop on INFRA, AST check of the
  run body, `merge_train.json` unchanged by the self-test. Plus its non-ps1 files (`scripts/diag.py`,
  `MERGE_TRAIN.md`, `roadmap-loop/SKILL.md`), no conflicts.
- HOOKSCSV-TEXTONLY: `hooks.csv` in `text_only_globs`, `tracker_lint.py` on every batch, 6 tracker
  cases (rc-ok text-only, NCOL, CONF, DUP, PARSE, UDUP) and 2 glob controls.
- SELFTEST-D2DEFH: D2 rerun checks read `full.d2_arms`, run against `D2_DEFH_2`.

## merge_train.json gate refs

Unchanged against `orch/integration` `78be531e1`: `gctrl.ref` `8284be46/ba407a16/31fe043e`,
`b.ref` `1d5bf4f9/1d5bf4f9/1d5bf4f9`, `full.d2_arms` `D2_DEFH_1..3`. The only diff in the file is the
HOOKSCSV `text_only_globs` line.

## Self-test

Command (from the worktree, inside the test slot):
`test-slot bash -c 'pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest'`

Full output: `verify/harness_20261102/selftest_HB-TRAIN-STACK3.txt`
(sha256 `460ad59eccfc02d4899c7b5226a29efda7effaaf6d5cc8cec689575e3f08e01f`).

Count: 55 PASS, 0 FAIL. Source branches: INFRA-PRECHECK 47 PASS, HOOKSCSV-TEXTONLY 31 PASS
(the 23 checks of base `78be531e1`, 20 PASS + the 3 stale D2 FAILs in
`verify/hbhookscsv_20261102/selftest_base_78be531e1.txt`, plus 8 tracker checks). INFRA's 47 already
include those 23, so 47 + 8 = 55: every check of all three branches is present and passes.

Tail:
```
[mtrain 11:04:59]   SELFTEST PASS: short-once: D2_DEFH_2 rerun once (2 runs), others not rerun, 0 arms short
[mtrain 11:05:00]   SELFTEST PASS: short-always: reruns bounded at 2, arm still reported short (runs=3)
[mtrain 11:05:00]   SELFTEST PASS: full-length logs: no rerun
...
[mtrain 11:05:02]   SELFTEST PASS: normal path: batch GREEN = both merged (unchanged)
[mtrain 11:05:02]   SELFTEST PASS: AST: run body = Try-Batch -> Resolve-Batch, red-merge only for $bounced, verdict INFRA when $infra
[mtrain 11:05:02]   SELFTEST PASS: merge_train.json unchanged by the selftest (sha256 30E470FB2F98)
[mtrain 11:05:02] SELFTEST done: 0 failure(s)
SELFTEST PASS (0 failure(s)) log C:\Users\maria\AppData\Local\Temp\mtrain_selftest\20261102_110441\merge_train.log
```

## Gates

Not run: static harness change, display asleep (every boot fails with 0x8876086A). The three
source rows merge through this head, reviewed as one SOURCE `-Full` row.
