# RESULT HB-TRAIN-DRAIN: merge_train.ps1 -Drain

Verdict: GREEN (self-test 79 PASS, 0 FAIL; no game launched, no live queue touched).

## Why
2026-11-10: the train sat idle 00:10-03:01 with about 15 branches queued (it ran one pass and waited for the lead),
plus about 10 min between later passes (row evidence `D:/MashedWT/mtrain_data/20261110_042108/merge_train.log`).

## What changed (`re/orchestrator/merge_train.ps1`)
- `-Drain`: a driver loop. Each pass is a child `merge_train.ps1 -NoFlip -ResultJson <file>`, so every pass runs the
  newest script on orch/integration and keeps its own PID file, reclaim, priority hold (HB-MASHEDLOCK-EARLY-PRIORITY)
  and REPORT. The driver takes no lock, no slot, no worktree.
- After each pass the driver reads the result file. It goes on only on GREEN with something merged and either
  (a) gate hashes equal to the refs re-read from `merge_train.json` after the pass, with the refs the pass used still
  equal to them, or (b) a text-only pass (no build input touched, so no hash can move).
- Then the autopilot step: the main checkout must have race/first-frame-parity checked out; race/first-frame-parity
  and main must both be ancestors of orch/integration (checked before anything moves); `git -c merge.autoStash=false
  merge --ff-only orch/integration` in the main checkout, retried on index.lock (8 tries, 15 s apart, the lock is never
  deleted); `git update-ref refs/heads/main <new> <old>`; then a check that both heads equal orch/integration.
- Then `py -3.12 scripts/lan_release/release_due.py` from the main checkout: NOT_DUE goes on, DUE runs
  `scripts/lan_release/publish.sh` with Git's bash.exe and waits for it before the next pass (log
  `drain_<ts>/publish_pass<N>.log`), ERROR or a failed publish stops (no retry).
- Stops with one `REPORT merge-train-drain` (verdict = stop class): RED, INFRA, HASH (hash unlike the refs, or refs moved
  during the pass), FLIP, ERROR, or STOP at `max_passes` (20). Queue empty = GREEN. Never pushes.
- `-NoFlip` (set by the driver on every pass): after the batch pick and before the priority hold or any merge, a batch
  with a decided-flip row (id matches `(^|-)FLIP(-|$)`, or the ledger title matches `decided flip`) is refused:
  verdict FLIP, nothing merged, every row stays queued. A false positive only stops the drain.
- Pass result file (`-ResultJson`): verdict, merged / bounced ids, infra, error, gates text, gate hashes, text_only, refs
  used, base and integration tip. Gate-Default / Run-Gates / Try-Batch now carry `hashes` and `textOnly` for it.
- The batch pick moved into `Select-Batch` (same rule, self-tested) and the queue read into `Get-Queue`.
- Driver data: `D:/MashedWT/mtrain_data/drain_<ts>/` (driver log, `pass<N>.result.json`, publish logs, REPORT.txt).
  Config overrides: optional `drain` object in `merge_train.json` (parity_branch, main_branch, max_passes, flip_id_rx,
  flip_title_rx, lock_tries, lock_wait_sec); defaults live in the script, `merge_train.json` is unchanged.
- Docs: `.claude/skills/roadmap-loop/SKILL.md` (merge train section) and `re/orchestrator/MERGE_TRAIN.md`.

Test-slot / lock order: unchanged. The arms still take mashed_lock inside the pass (Run-Arms); the driver adds no slot.
If the lead wants a slot it wraps the driver (`test-slot pwsh ... -Drain`): the slot is outside, `PROYECTOS_TEST_SLOT=held`
passes to the children, mashed_lock stays inside.

## Self-test (`pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest`)
Summary lines: `verify/hb_train_20261110/selftest_drain.txt` (93 lines). Raw log (not committed):
`C:/Users/maria/AppData/Local/Temp/claude/2279e0b7f1f09563/2e04f018-b1e0-463e-8db5-d34153b1d580/scratchpad/st_drain.log`,
SHA-256 `a857d07530b492a2edc871d03673baae27dba93f2ae0403ff1ff6d3a3b5d243c`.

New checks (fake main checkout with race/first-frame-parity checked out plus main and integration branches, fake
queue, fake pass child, fake release_due / publish):
1. 3 GREEN passes (one text-only), DUE after pass 2: both heads follow integration after every pass, events
   `pass1,release1,pass2,release2,publish2,pass3,release3` (publish done before pass 3), stops on the empty queue.
2. GREEN then RED: stop after pass 2, no fast-forward or release after the RED pass.
3. INFRA: stop, nothing moved, no release.
4. GREEN with a G-CTRL hash unlike the refs = HASH; refs moved during the pass = HASH; no fast-forward.
5. FLIP pass = stop, nothing moved.
6. index.lock in the main checkout: one wait, retry, then both heads equal integration.
7. main diverged from integration: refused before race/first-frame-parity moves.
8. main checkout on another branch: refused.
9. release_due ERROR and publish.sh rc=1 both stop before the next pass (no retry).
10. empty queue: no pass, GREEN.
11. AST/text: the drain dispatch sits before the pass body, the flip refusal before Enter-PassPriority, and the driver
    functions reference no mashed_lock call.
Plus unit checks: Select-Batch keeps the old pick, Test-FlipRow on 6 ids/titles, Get-BatchFlips reads ledger titles, the
result file round-trips with LF only. Every pre-existing self-test check still passes. Live read-only checks: `-Plan`
on the real queue prints the same batch as before; `roadmap.ps1 get` parses through the new reader.

## Not tested live
A real drain run (it would merge, fast-forward and publish). The first live use is the lead's call.

## Next
HB-TRAIN-CONFLICT-SKIP stacks on this branch: a conflict-only bounce then no longer counts as RED for the drain.
