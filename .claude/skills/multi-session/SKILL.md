---
name: multi-session
description: Coordinate multiple concurrent Claude Code sessions on the Mashed RE project — slot ownership, lock hygiene, tracker conflicts, and handoff protocols. Triggers when the user mentions "another session", "two Claudes", "in parallel", "the other terminal", "merge from other session", or any sign that more than one Claude is touching the repo at once.
---

# multi-session — concurrent Claude coordination

This project supports running **multiple Claude Code sessions in parallel**, typically one per worktree. The constraints they share:

- One physical `Mashed.rep` master (read-only by convention).
- One physical pool of `mashed_pool/Mashed_pool0..N.rep` clones (each lockable by exactly one session at a time).
- Four trackers (`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`) that all sessions update.

This skill is the etiquette layer.

## When to invoke

- Before starting a session that will touch Ghidra or the trackers.
- When `acquire` returns "all slots locked" — diagnose vs panic.
- When a tracker merge produces a conflict.
- When the user says "the other Claude is working on …" — load context.
- At session shutdown — release slot, flush updates.

## Session-startup protocol

Every session, before any decomp work, runs:

```bash
# 1. Identify yourself — what worktree, what branch?
pwd                                              # should be Mashed root or .worktrees/<NAME>
git rev-parse --abbrev-ref HEAD
[ -f .pool_slot ] && cat .pool_slot              # if inside a worktree, see bound slot

# 2. Take pool inventory.
bash scripts/ghidra_pool.sh status

# 3. Pull latest trackers (in case another session committed).
git fetch && git status -sb

# 4. Acquire slot if not already bound.
bash scripts/ghidra_pool.sh acquire
```

## Session-shutdown protocol

Always run:

```bash
# 1. If any function was touched, run /re-classify to reconcile trackers.
# 2. Commit tracker updates separately from code:
git add hooks.csv STUBS.md UNCERTAINTIES.md DEFERRED.md re/analysis/CHANGELOG.md
git commit -m "trackers: <summary>"

# 3. Close any open Ghidra programs (mcp__ghidra__program_close).
# 4. Release the pool slot.
SLOT=$(cat .pool_slot 2>/dev/null)
[ -n "$SLOT" ] && bash scripts/ghidra_pool.sh release "${SLOT#Mashed_pool}"
```

If a session is killed (network drop, crash, host reboot): the next session that hits the project should run `bash scripts/ghidra_pool.sh cleanup` to clear stale `.lock`/`.lock~` files and reclaim slots.

## Slot ownership rules

- **One slot, one session.** Never `mcp__ghidra__project_program_open_existing` against a slot you didn't acquire (or that's bound to another worktree's `.pool_slot`).
- **Always `read_only=true`.** Writes go to the master `Mashed.gpr` only, in a single supervised session.
- **Master is exclusive.** Only one session may hold the master in writable mode at a time. If you're about to do GUI editing, announce it: write `master.WIP-<sessionid>` as a flag file in repo root, commit a tracker line, and only then open the master.
- **Sync after master writes.** When a master-editing session finishes, run `bash scripts/ghidra_pool.sh sync` to push fresh `.rep` to all unlocked slots. Other sessions should `cleanup` + `acquire` again to pick up changes.

## Tracker conflict resolution

When two sessions edit the same tracker file:

1. **Pull first, edit second.** Always `git pull --rebase` (or merge) before staging tracker edits. The `re-classify` skill assumes it's writing to current state.
2. **Conflicts in `hooks.csv`** — merge by RVA. Higher confidence wins. Diverging facts (different name/subsystem) → halt; user picks.
3. **Conflicts in monotonic-ID files** — both sessions allocated the same `S-NNNN` / `U-NNNN` / `D-NNNN`. Renumber the later-merged session's IDs upward; update inline `// STUB S-NNNN` and `[UNCERTAIN U-NNNN]` references via `re/tools/merge_trackers.py` (or by hand if not yet implemented).

## Communication conventions

- **Never assume the other session sees what you saw.** Memory writes (`MEMORY.md`) are scoped to your account, not the project.
- **Cross-session facts go in the trackers or in `re/analysis/`.** Anything important enough to share is important enough to commit.
- **End-of-session summary** to the user should always include: what slot was held, which trackers got rows, which functions promoted/demoted. So a future session has a clean handoff.

## Common failure modes

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| `LockException` on `program_open` | another session holds the slot | check `.pool_slot` files in `.worktrees/*`, or `cleanup` for stale locks |
| `acquire` returns "all locked" with only one Claude active | stale `.lock` files from killed sessions | `bash scripts/ghidra_pool.sh cleanup` + retry |
| `hooks.csv` merge conflict every commit | two sessions promoting same RVA | pre-coordinate which sessions own which subsystems; document in `re/analysis/SESSION_OWNERSHIP.md` |
| Master `Mashed.gpr` won't open (already locked) | another session has GUI open | check for `master.WIP-*` flag file or running Ghidra GUI process |
| Stub IDs collide | two sessions filed `S-NNNN` simultaneously | renumber the later session's IDs; use `merge_trackers.py` |

## Boundary: what this skill does NOT do

- It does not start or stop other Claude sessions; the user does that.
- It does not arbitrate "who's right" on a tracker fact. It enforces process; user resolves substance.
- It does not replicate state across accounts. Memory is per-account; the trackers are per-repo.
