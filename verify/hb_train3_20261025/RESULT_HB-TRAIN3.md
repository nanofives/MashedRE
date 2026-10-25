# RESULT HB-TRAIN3 (HB-D2RERUN, HB-EOL, HB-WTCLEAN)

Branch orch/HB-TRAIN3, files: re/orchestrator/merge_train.ps1, merge_train.json, roadmap.ps1.
The live merge train was not run and the real queue was not read. Proof is `merge_train.ps1 -SelfTest`,
a new switch that builds a fake git repo under %TEMP% (autocrlf=true like the real checkout), stubs the arm runner and exits.

    pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest    # exit 0, 9 checks + 1 contrast line

## What changed
- HB-D2RERUN: new `Run-D2Arms` (called from Gate-Full). Arms whose motion_diag has fewer than d2_min_lines (1600) lines are re-run
  solo, only the short ones, at most `d2_max_reruns` (new key in merge_train.json, default 2) per arm. Each rerun logs
  `D2 rerun: <arm> ... rerun n/2`. An arm still short after the bound keeps failing the gate and is named in the gate text.
- HB-EOL: conflict stages are dumped with `Git-ShowBytes` (raw stdout stream to file), so merge-file and the committed blob keep LF.
  INTEGRATION_LOG.md is written by `Write-Lf` (UTF-8, LF). Remaining Set-Content/Add-Content in merge_train.ps1 only write untracked run data
  (logs, REPORT.txt, vsum/score text). roadmap.ps1 (called by the train) wrote roadmap.json, merge_queue.json, events.jsonl and DECISIONS.md
  with CRLF, and `snapshot` copies those into tracked ledger/ files: now written LF through Write-Lf/Append-Lf.
  Checked on a temp MASHED_ORCH_STATE: roadmap.json, merge_queue.json, events.jsonl, DECISIONS.md have 0 CR bytes after set/enqueue/decision.
- HB-WTCLEAN: after the GREEN fast-forward, `Cleanup-Step` per distinct merged branch: `py -3.12 scripts/diag.py wt-remove <path>`
  (path found from `git worktree list`, never force), then `git branch --delete` run in the worktree that holds orch/integration
  (the main checkout is on another branch, so -d there would judge merge-ness against the wrong HEAD). Either refusing means log and keep. No -D, no --force.
  Gotcha found by the test: in the Git wrapper function `-d` binds to -Debug and `-A` to -a, so the code uses `--delete`.

## Self-test output
```
[mtrain 23:49:02] SELFTEST fake repo <fake>
[mtrain 23:49:15]   union-resolved: re/analysis/CHANGELOG.md
[mtrain 23:49:15]   SELFTEST PASS: both branches merged, second one by union-resolve
[mtrain 23:49:16]   SELFTEST PASS: committed CHANGELOG.md has 0 CR bytes (27 bytes, entries a+b: True)
[mtrain 23:49:17]   SELFTEST PASS: Write-Lf output (INTEGRATION_LOG.md path) has 0 CR bytes
[mtrain 23:49:17]   SELFTEST contrast: Set-Content writes 2 CR byte(s) for 2 lines on this host
[mtrain 23:49:42]   cleanup step-clean: worktree and branch removed
[mtrain 23:49:45]   cleanup step-dirty: worktree removal refused (<fake>/.worktrees/step-dirty), worktree and branch kept
[mtrain 23:49:46]   cleanup step-ahead: git branch --delete refused, branch kept: error: the branch 'step-ahead' is not fully merged
[mtrain 23:49:47]   SELFTEST PASS: clean merged step: worktree and branch removed
[mtrain 23:49:47]   SELFTEST PASS: dirty step: wt-remove refused, worktree and branch kept (no force)
[mtrain 23:49:47]   SELFTEST PASS: unmerged step: git branch --delete refused, branch kept (no -D)
[mtrain 23:49:48]   D2 rerun: D2_DEF_2 motion_diag under 1600 lines, rerun 1/2
[mtrain 23:49:48]   SELFTEST PASS: short-once: D2_DEF_2 rerun once (2 runs), others not rerun, 0 arms short
[mtrain 23:49:49]   D2 rerun: D2_DEF_2 motion_diag under 1600 lines, rerun 1/2
[mtrain 23:49:50]   D2 rerun: D2_DEF_2 motion_diag under 1600 lines, rerun 2/2
[mtrain 23:49:51]   D2 short after 2 rerun(s): D2_DEF_2
[mtrain 23:49:52]   SELFTEST PASS: short-always: reruns bounded at 2, arm still reported short (runs=3)
[mtrain 23:49:53]   SELFTEST PASS: full-length logs: no rerun
[mtrain 23:49:53] SELFTEST done: 0 failure(s)
SELFTEST PASS (0 failure(s)) log C:\Users\maria\AppData\Local\Temp\mtrain_selftest\20261024_234858\merge_train.log
```

## Not covered
Gate-Full end to end (needs builds, a game root and the lock). Cleanup-Step is proven against a fake repo, not the live orch-integration worktree.
HB-SOLOSTALE untouched.
