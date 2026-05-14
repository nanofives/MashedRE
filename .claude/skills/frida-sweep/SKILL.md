---
name: frida-sweep
description: Run the end-of-batch merge sweep for a parallel-fanout C3 batch — merge N worktree branches onto main, resolve the predictable text-file conflicts, rebuild the canonical mashed_re_dev.asi, and run the integration Frida diff against ALL promoted hooks. Drains "Queued" rows in re/PROMOTION_QUEUE.md. Triggers on "run the frida sweep", "drain the promotion queue", "merge the c3 batch", "merge the c3 sessions", "run the c3 merge", "drain PROMOTION_QUEUE". Counterpart: ghidra-sweep (drains SCRIBE_QUEUE.md after Ghidra-discovery batches).
---

# frida-sweep — PROMOTION_QUEUE drain session

The Frida sweep is a single session that lands a parallel C3 batch onto main: merges every queued worktree branch, resolves the three predictable text-file conflicts (`build.bat`, `hooks_registry.py`, `CHANGELOG.md`), rebuilds the canonical `mashed_re_dev.asi`, and runs `run_diff_parallel.py` against **every** promoted hook in the merged build. That last step — the integration diff — is the gate the sweep exists for.

**Recommended model:** `claude-sonnet-4-6`. The conflict-resolution patterns are mechanical (union of additions, no semantic judgment). The integration diff is observed, not reasoned about. Opus is wasted here; use it only if a regression surfaces and bisection requires real diagnosis.

**Not interchangeable with `ghidra-sweep`** (which drains `re/SCRIBE_QUEUE.md` via Ghidra MCP). The Frida sweep never touches the master Ghidra project, never calls MCP, never writes plates or bookmarks. Distinct domain.

## When to invoke

- After a `promote-c3-batch` fanout completes (N≥3 sessions, each on its own worktree branch).
- After the user has run all session prompts and each session has appended its row to `re/PROMOTION_QUEUE.md`.
- When the user says "merge the c3 batch", "drain the promotion queue", "run the frida sweep", or any variant.

Do NOT invoke this skill when:
- Only `re/SCRIBE_QUEUE.md` has work — that's `ghidra-sweep`.
- A single-session promotion just finished — that session ran `re-classify` directly on main; no merge needed.
- Any session branch is still in-progress (commit not yet pushed / queue row not yet appended).

## Commit policy (always commit, never ask)

Same posture as `ghidra-sweep`: this skill owns the working tree for its duration; every commit step is executed without asking. The protocol is *defined* by its commit sequence. If a commit fails for a real reason (hook error, merge conflict that isn't one of the three known patterns, build break), surface the error verbatim and halt — do not interpret it as a confirmation prompt.

## Pre-conditions (check before starting)

1. **No active fanout sessions.** Inspect `.worktrees/c3-batch-*/`. For each worktree referenced in a Queued row, confirm there's no in-progress `.pool_slot` / no Claude session still running against it. If unsure, halt and ask the user.
2. **`git pull --rebase`** on main to ensure all session branches and queue rows are visible.
3. **Anchor check.** `sha256sum original/MASHED.exe` must equal `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Mismatch → halt.
4. **Baseline build clean on main.** Run `cmd /c mashedmod\build.bat > log/frida_sweep_baseline.txt 2>&1`. Main must build cleanly before any merge — otherwise we can't distinguish merge breakage from pre-existing breakage.
5. **Frida pool empty.** `scripts/frida_pool.sh cleanup` to clear stale locks and zombie MASHED.exe processes from the per-session diffs.

## Session ID

```bash
SESSION_SHORT_ID="frida-sweep-$(date -u +%Y%m%d-%H%M)"
# Example: frida-sweep-20260512-2230
```

## Sweep execution

```
1. git pull --rebase
2. cmd /c mashedmod\build.bat > log/frida_sweep_baseline.txt 2>&1   # baseline must succeed
3. scripts/frida_pool.sh cleanup
4. echo "$SESSION_SHORT_ID" > "frida-sweep.WIP-$SESSION_SHORT_ID"
   git add "frida-sweep.WIP-$SESSION_SHORT_ID"
   git commit -m "frida-sweep: claim $SESSION_SHORT_ID"
5. Append to re/analysis/CHANGELOG.md:
   "<YYYY-MM-DD>  $SESSION_SHORT_ID  frida-sweep-claim  branches=<N queued>"

6. For each Queued row in re/PROMOTION_QUEUE.md (in order):
     a. Parse: BRANCH, RVAS, EVIDENCE paths.
     b. git merge --no-ff "$BRANCH" -m "frida-sweep: merge $BRANCH"
        → If conflict-free: continue.
        → If conflict in one of the three known files: apply mechanical resolution
          (see § "Conflict resolution rules" below), `git add`, `git commit`.
        → If conflict in any OTHER file: halt. Surface the conflict to the user.
          Do NOT auto-resolve unknown conflict patterns.
     c. Move the queue row from "## Queued" to "## Merged",
        appending: drained-by=$SESSION_SHORT_ID
7. After all branches merged:
     a. cmd /c mashedmod\build.bat > log/frida_sweep_build.txt 2>&1
        → If build fails: halt. Surface the error verbatim. Do NOT commit
          the broken state to main (it's already committed via the merges —
          revert with `git reset --hard <pre-sweep-sha>` after surfacing).
     b. Collect ALL promoted hook names from the merged hooks_registry.py
        (use the rvas from the queue rows to map to hook names):
          HOOKS=$(awk -F',' 'NR>1 && $4=="C3" {print $1}' hooks.csv | ...)
        Or build the list directly from the queue rows by mapping RVAs to
        their registry keys.
     c. scripts/frida_pool.sh cleanup
     d. py -3.12 re/frida/run_diff_parallel.py $HOOKS > log/frida_sweep_integration_diff.txt 2>&1
        → Expect every hook GREEN.
        → If any RED: halt. Identify the regressing hook(s). Bisect by
          reverting the last-merged branch and re-running the diff — the
          branch whose revert makes the diff GREEN is the regressor.
          Surface the regressing branch + RVA to the user. Do NOT auto-revert
          main; that's the user's call.

8. Append to re/analysis/CHANGELOG.md:
   "<YYYY-MM-DD>  $SESSION_SHORT_ID  frida-sweep-release  branches=<N merged>  integration-diff=GREEN  hooks=<K>"

9. git rm "frida-sweep.WIP-$SESSION_SHORT_ID"
   git add -A
   git commit -m "frida-sweep: release $SESSION_SHORT_ID — <K> hooks integrated, diff GREEN"
```

## Conflict resolution rules (mechanical)

Three files conflict predictably because every session touches the same region. Resolve each pattern as follows — these are the ONLY conflict patterns this skill auto-resolves. Anything else halts the sweep.

### `mashedmod/build.bat`

Each session appends one `"%SRC%\<Subsystem>\<file>.cpp" ^` line before `/link /DLL`. Conflict pattern:

```
<<<<<<< HEAD
    "%SRC%\GameState\StateAccessors.cpp" ^
    /link /DLL
=======
    "%SRC%\Frontend\MenuButtons.cpp" ^
    /link /DLL
>>>>>>> c3/batch-a-s2
```

**Resolution:** union of additions, both before `/link /DLL`:

```
    "%SRC%\GameState\StateAccessors.cpp" ^
    "%SRC%\Frontend\MenuButtons.cpp" ^
    /link /DLL
```

Order doesn't matter; the linker doesn't care.

### `re/frida/hooks_registry.py`

Each session adds dict entries before the closing `}`. Conflict pattern:

```
<<<<<<< HEAD
    'get_dat_0067ecb4': {
        ...
    },
}
=======
    'menu_button_state': {
        ...
    },
}
>>>>>>> c3/batch-a-s2
```

**Resolution:** keep both dict entries, one closing `}`:

```
    'get_dat_0067ecb4': {
        ...
    },

    'menu_button_state': {
        ...
    },
}
```

If two sessions happened to choose the same key name, halt — that's a real conflict the user needs to rename.

### `re/analysis/CHANGELOG.md`

Each session prepends lines near the top (the format is newest-first). Conflict pattern:

```
<<<<<<< HEAD
2026-05-12  c3-batch-a-s1  ... — 5 hooks promoted
2026-05-12  sweep-...      scribe-claim ...
=======
2026-05-12  c3-batch-a-s2  ... — 5 hooks promoted
2026-05-12  sweep-...      scribe-claim ...
>>>>>>> c3/batch-a-s2
```

**Resolution:** keep BOTH sessions' lines, sort by timestamp (newest first; ties broken by session id ascending):

```
2026-05-12  c3-batch-a-s1  ... — 5 hooks promoted
2026-05-12  c3-batch-a-s2  ... — 5 hooks promoted
2026-05-12  sweep-...      scribe-claim ...
```

## Integration diff — the load-bearing step

The whole point of this sweep is the diff that runs **after** all branches are merged and the canonical `.asi` is rebuilt. Per-session diffs proved each hook works in isolation; the integration diff proves they don't break each other when linked into the same DLL.

Common cross-hook regressions the integration diff catches:
- Two sessions chose the same `extern "C"` symbol → linker symbol clash (build fails).
- Two sessions targeted near-adjacent RVAs with 5-byte inline JMP patches that overlap → original behavior overwritten for one of them.
- Two sessions modified a struct definition inconsistently → field offsets drift, one hook reads stale data.
- Header `#include` ordering surfaces only when both `.cpp` files are in the same TU.
- One session's reimpl writes a side effect that another session's hook reads from later.

A merge-sweep that fails to run the integration diff has not finished. Commit the merge only after GREEN.

## What the sweep does NOT do

- Does not author hooks, edit reimpls, or change semantics. Sessions did that.
- Does not touch the master Ghidra project or `mashed_pool/` slots — that's `ghidra-sweep`.
- Does not call any Ghidra MCP function.
- Does not retroactively edit a per-session `re-classify` transaction. If a session promoted a function incorrectly, that's a separate concern resolved via demotion.
- Does not skip the integration diff under any circumstance. No GREEN = no commit to main.
- Does not auto-revert main on regression — surfaces the offender, lets the user decide.

## Relationship to other skills

- **ghidra-sweep** — separate domain; never overlap. If both queues have work, run `ghidra-sweep` first (it may unblock C3 candidates whose callers were promoted there).
- **promote-c3-batch** — generates the batch file this sweep eventually drains.
- **re-classify** — already ran per-session inside each worktree. The sweep does not re-classify.
- **frida_pool** (`scripts/frida_pool.sh`) — used during the integration diff. Cleanup before, cleanup after.
- **worktree** — sessions ran in `.worktrees/c3-batch-<id>-s<N>/`. After the sweep succeeds, the worktrees can be torn down (`git worktree remove`) at the user's discretion.

## End-of-session report

```
Frida sweep ID:        frida-sweep-YYYYMMDD-HHMM
Branches merged:       <N> (list)
Branches halted:       <N> (list with halt reason)
Conflict patterns:     build.bat=<count>, hooks_registry.py=<count>, CHANGELOG.md=<count>
Unknown conflicts:     <N> (halt cases; surface the file+branch)
Build outcome:         ok | <verbatim error>
Integration diff:      GREEN (<K> hooks, all bit-identical) | RED (<list of regressing hooks>)
Regressing branch:     <branch-id> | none
Hooks now at C3:       <K> (running total: <C3 count after this sweep>)
Worktrees to tear down:<list of safe-to-remove worktrees>
```
