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

## Pre-flight asserts (every session, every time)

The dominant multi-session failure mode is **silent hallucination of session control** — a session reports facts about a function in "the open program" without having verified which program is open, whether the slot is current, or whether another session is mid-edit. Run the four file-level asserts before any decomp work, and the MCP-side asserts after opening:

```bash
# After acquire, before any program_open:
bash scripts/ghidra_assert.sh preflight "$SLOT"     # slot-match + staleness + scribe-check
```

```text
After project_program_open_existing succeeds:
  - call mcp__ghidra__program_list_open and assert the returned path contains "$SLOT"
  - if not, close immediately — you've opened the wrong program

Before citing any RVA in conversation, source comment, hooks.csv, or analysis note:
  - call mcp__ghidra__function_at <rva> OR mcp__ghidra__listing_code_unit_at <rva>
  - if result is null, HALT — do not cite, do not "round" to a nearby valid address

Before any write-tool call (rename, retype, struct edit, comment_set, transaction_begin, etc.):
  - assert you hold a master.WIP-<sessionid> flag in repo root (scribe-check)
  - if not, the call is forbidden — slots are read-only by convention; only the scribe writes
```

The full write-tool list is in `ghidra-pool/SKILL.md` under "Write-tool gating." When in doubt, treat a tool as a write tool.

## MCP-down halts the session

Every parallel session is bound to Ghidra MCP — that is the only tool that can answer questions about MASHED.exe. If MCP is unreachable or returns an error on `health_ping` / `ghidra_info`, **the session must stop** and report the failure. Falling back to prior-art repos, web search, or "I'll just describe what the function probably does" is never acceptable: it produces hallucinated RE notes that then poison the trackers for every other session that reads them.

The same applies mid-session: if a single MCP call fails (timeout, transport error, server crash), do not retry against documentation. Surface the failure to the user, halt the in-flight function, and either reconnect or release the slot. See `ghidra-pool/SKILL.md` § "MCP availability is required" for the full rule.

## Scribe protocol (master writes)

Master `Mashed.gpr` accepts only one writer at a time. Every analysis session attempts a scribe
immediately after its analysis commit. The rule is **serial and immediate — no polling, no retry
loop.**

### Lock check — do this before touching the master

```bash
bash scripts/ghidra_assert.sh scribe-check
```

- **No `master.WIP-*` flag present** → proceed.
- **Flag present** → **stop and surface to the user verbatim:**

  > "Master is locked by `<flag-file-name>`. Scribe step skipped.
  > When the lock is released, re-run the scribe step manually for this subset:
  > bucket=`<bucket>`, RVAs: `<comma-separated list>`."

  **Do not poll. Do not retry automatically.** The user reads the message and says "retry" when
  the lock is gone.

### Execution order (when lock is clear)

1. **Claim flag** — `echo "" > master.WIP-<sessionid>`, `git add`, `git commit`.
   Record in `re/analysis/CHANGELOG.md`: `YYYY-MM-DD <sessionid> scribe-claim bucket=<b> rvas=<n>`.
2. **Open master writable** — `mcp__ghidra__project_program_open_existing` with `read_only=false`
   against `Mashed.gpr` (project_location = `C:/Users/maria/Desktop/Proyectos/Mashed`). Never
   against a pool slot.
3. **Write** — per-RVA, in address order, one at a time. Allowed writes depend on confidence level;
   see `re/SESSION_RULES.md` § "What to write at C1" for the first-pass write list.
   - Confirm `function_at <rva>` non-null before each write.
   - If any MCP write call fails: halt, surface error verbatim. Partial writes already applied are
     safe to leave. Do not continue to the next RVA without user approval.
4. **Save** — `mcp__ghidra__program_save`.
5. **Close** — `mcp__ghidra__program_close`.
6. **Sync** — `bash scripts/ghidra_pool.sh sync` — refreshes all unlocked slots and stamps
   `.last_master_sync`.
7. **Release flag** — `git rm master.WIP-<sessionid>`, `git commit`.
   Record in `re/analysis/CHANGELOG.md`: `YYYY-MM-DD <sessionid> scribe-release writes=<n> errors=<n>`.

Other sessions, after a sync lands on their branch, must `cleanup` + re-`acquire` so their slot's
`.rep` mtime moves past `.last_master_sync`. The `staleness` assert catches them if they forget.

## Sync timestamp (`.last_master_sync`)

`scripts/ghidra_pool.sh sync` writes `.last_master_sync` at the start of its run, before any `cp`. The semantics:

- A slot whose `.rep` mtime is **newer** than `.last_master_sync` was refreshed by this sync (or a later one) — **current**.
- A slot whose `.rep` mtime is **older** than `.last_master_sync` was locked when sync ran — **stale**. The session holding it is operating on pre-master-write state and may produce incorrect names/types in its output.
- No `.last_master_sync` file → no master writes have ever happened on this branch — all slots are current by definition.

`ghidra_assert.sh staleness` checks this before you trust the open program.

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
| Scribe step surfaced lock message and stopped | another session holds `master.WIP-*` | wait for that session to release the flag; then tell Claude "retry scribe" with the bucket+RVA list it printed |
| Stub IDs collide | two sessions filed `S-NNNN` simultaneously | renumber the later session's IDs; use `merge_trackers.py` |

## Boundary: what this skill does NOT do

- It does not start or stop other Claude sessions; the user does that.
- It does not arbitrate "who's right" on a tracker fact. It enforces process; user resolves substance.
- It does not replicate state across accounts. Memory is per-account; the trackers are per-repo.
