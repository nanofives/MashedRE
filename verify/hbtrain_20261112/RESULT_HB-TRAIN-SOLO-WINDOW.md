# RESULT HB-TRAIN-SOLO-WINDOW

Tooling row, options (a) and (b) only. Gate content, refs and hashes untouched (L-138). Option (c) not done.

## What changed
Files: re/orchestrator/merge_train.ps1, merge_train.json, MERGE_TRAIN.md.
- (a) `merge_train.ps1 -Gate [-Override] [-QueueFile f] [-GateNow t]`: exit 0 = START, 3 = WAIT. START at start_gate.min_rows (3) queued rows or oldest `at` older than max_wait_min (60). enabled=false makes it always START. Empty queue never starts. No PID file, lock or worktree. Config under `start_gate` in merge_train.json. The L0 conductor must call it before a pass or -Drain (the script's own -Drain loop is unchanged).
- (b) `Enter-PassPriority` moved from the top of the pass body into Try-Batch, after merge, archive lint, tracker lint, build.bat and lint_rva_bodies, immediately before Run-Gates. It is idempotent (Resolve-Batch re-enters Try-Batch), Exit-PassPriority releases only a hold that was taken. `priority_scope: "pass"` restores the old behaviour. Text-only batches never take the hold.
- Existing self-test assertions that pinned the old position were rewritten, plus a new ordering check and 6 gate checks.

## Dry-run output (fake queue copies in the scratchpad, clock fixed at 2026-11-12T12:00Z)
~~~
q0 rc=3 GATE WAIT: queue empty (rows=0 oldest_wait_min=0 min_rows=3 max_wait_min=60)
q1 rc=3 GATE WAIT: 1 row(s) < 3 and oldest waited 10 min < 60 (rows=1 oldest_wait_min=10 min_rows=3 max_wait_min=60)
q2 rc=0 GATE START: oldest row waited 90 min >= 60 (rows=2 oldest_wait_min=90 min_rows=3 max_wait_min=60)
q3 rc=0 GATE START: 3 rows queued >= 3 (rows=3 oldest_wait_min=60 min_rows=3 max_wait_min=60)
~~~
(q1 = 1 row, 10 min; q2 = 2 rows, oldest 90 min; q3 = 3 rows.) With -Override on q1: `GATE START: override`.

## Self-test (`merge_train.ps1 -SelfTest`, fake repo, private lock dir): rc=0, 126 PASS, 0 FAIL
~~~
SELFTEST PASS: priority window: Try-Batch takes the hold once, after build.bat (3627) and lint_rva_bodies (4381), right before Run-Gates (4806 < 4830)
SELFTEST PASS: gate: 1 row waiting 10 min = WAIT (1 row(s) < 3 and oldest waited 10 min < 60)
SELFTEST PASS: gate: 3 rows = START (3 rows queued >= 3)
SELFTEST PASS: gate: 2 rows, oldest waited 61 min = START (oldest row waited 61 min >= 60)
SELFTEST PASS: gate: -Override starts a 1-row queue (override)
SELFTEST PASS: gate: empty queue never starts, even with -Override (queue empty)
SELFTEST PASS: gate: 59 min wait is still WAIT (1 row(s) < 3 and oldest waited 59 min < 60)
SELFTEST done: 0 failure(s)
~~~
PowerShell parse check: 0 errors.

## Not tested
- No real pass: no live Try-Batch with a build, so the hold-after-build ordering is proven by AST offsets and the existing fake Run-Gates tests, not by a live PRIORITY-HOLD timeline.
- The live queue and live mashed_lock were not touched (status was free during the work).
- L0 conductor wiring of -Gate (skill text) is not changed here.
- Side effect to know: build.bat of a pass can now overlap a solo window another session opened, since the hold starts later. Set priority_scope to pass if that costs gate flakes.
