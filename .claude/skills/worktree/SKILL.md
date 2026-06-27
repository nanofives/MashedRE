---
name: worktree
description: Manage git worktrees for parallel Mashed RE work — each subsystem sweep, hook landing, or experiment gets its own isolated checkout bound to its own Ghidra pool slot. Triggers on "worktree", "isolate", "branch off", "parallel work", "spin up a sandbox for X", or when the user starts a new sweep / experiment that shouldn't pollute main.
---

# worktree — per-effort isolation

The Mashed repo lives at `C:\Users\maria\Desktop\Proyectos\Mashed`. Long-running RE efforts (subsystem sweep, big risky hook, asset-format spike) deserve their own git worktree so that:
- Trackers (`hooks.csv`, `STUBS.md`, etc.) advance independently and merge deliberately at the end.
- A regression on one effort doesn't pause others.
- Each worktree binds to its own Ghidra pool slot, so two sessions never write to the same `Mashed_poolN`.

Worktrees live under `.worktrees/<name>/` (gitignored — only the directory exists locally; the `git worktree` registry tracks them).

## When to invoke

- User says "let's start the audio sweep" / "isolate the Phase-3 boot work" / "spin up a sandbox for the .piz repacker."
- User says "switch to / resume worktree X."
- User says "merge the audio worktree back into main."
- User says "list worktrees" / "what's running."
- User says "tear down worktree X."

## Architecture

```
C:\Users\maria\Desktop\Proyectos\Mashed\           # main worktree, branch=main
└── .worktrees\
    ├── audio-sweep\                                # branch=phase5/audio
    ├── boot-rework\                                # branch=phase3/boot
    └── piz-repacker\                               # branch=phase2/piz-repack
```

Each worktree has its own:
- Working copy of all source.
- `.claude/` (inherited; can be diverged for per-effort settings).
- **Bound Ghidra pool slot** (recorded in `.worktrees/<name>/.pool_slot`).
- **Bound Frida log dir** (`log/<worktree-name>/`).

> ⚠️ **NEVER junction/symlink `original/` (or any shared immutable dir) into a
> worktree, and NEVER `git worktree remove --force`.** A junction inside a worktree
> + `--force` removal deletes the junction's TARGET — this WIPED the real `original/`
> install once (re/diag/KNOWN_ISSUES.md `WORKTREE-SYMLINK-WIPE`, 2026-06-27). The
> standalone reads main's assets WITHOUT a junction: the exe self-locates the repo
> root by walking up from its own path, or set `MASHED_ROOT=<abs path to main repo>`
> when running a worktree build. Remove worktrees ONLY via
> `py -3.12 scripts/diag.py wt-remove <path>` (strips reparse points first, never --force).

Workspaces share:
- `original/` (immutable) — accessed by the exe's self-locate or `MASHED_ROOT` env.
  **Do NOT junction/symlink it into the worktree** (see warning above).
- `Mashed.gpr` master and `mashed_pool/` (single physical pool serves all worktrees; the binding is logical).
- The Ghidra binary at `TD5RE\ghidra_12.0.3_PUBLIC\`.

## Standard create flow

```bash
NAME=audio-sweep
BRANCH=phase5/audio
cd "C:/Users/maria/Desktop/Proyectos/Mashed"

# 1. Create worktree at .worktrees/<NAME>
git worktree add ".worktrees/$NAME" -b "$BRANCH"

# 2. Bind a pool slot
SLOT=$(bash scripts/ghidra_pool.sh acquire)
echo "$SLOT" > ".worktrees/$NAME/.pool_slot"

# 3. Bind a log dir
mkdir -p "log/$NAME"

# 4. Open new Claude session there:
#    The user runs `claude` from the new worktree path.
echo "Worktree ready at .worktrees/$NAME — open a new Claude session there."
echo "Bound pool slot: $SLOT"
```

## Standard merge flow

```bash
NAME=audio-sweep
cd "C:/Users/maria/Desktop/Proyectos/Mashed"

# 1. Inside the worktree, run /re-classify on every touched function to reconcile trackers.
# 2. Inside the worktree, ensure clean working tree.
# 3. From main:
git checkout main
git merge --no-ff "phase5/audio" -m "Merge audio sweep (S-DoD satisfied)"

# 4. Resolve tracker conflicts (hooks.csv, STUBS.md etc.) — see "Tracker merge rules" below.

# 5. Release pool slot bound to the worktree.
SLOT=$(cat ".worktrees/$NAME/.pool_slot")
bash scripts/ghidra_pool.sh release "${SLOT#Mashed_pool}"

# 6. Tear down the worktree — SAFE removal only (never --force; strips reparse points).
py -3.12 scripts/diag.py wt-remove ".worktrees/$NAME"
git branch -d "phase5/audio"
```

## Standard teardown flow (abandoning an experiment)

```bash
NAME=failed-spike
SLOT=$(cat ".worktrees/$NAME/.pool_slot" 2>/dev/null)
# SAFE removal — NEVER `git worktree remove --force` (it follows an `original`
# junction and wipes the real install; see WORKTREE-SYMLINK-WIPE). diag wt-remove
# strips any reparse points first, then removes without --force.
py -3.12 scripts/diag.py wt-remove ".worktrees/$NAME"
git branch -D "$(git config -f .worktrees/$NAME/.git/config worktree.branch 2>/dev/null || echo experimental/$NAME)"
[ -n "$SLOT" ] && bash scripts/ghidra_pool.sh release "${SLOT#Mashed_pool}"
```

## Tracker merge rules

`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md` are line-oriented and have monotonic IDs. Merging:

- **`hooks.csv`** — merge by RVA. If both branches updated the same RVA: take the **higher** confidence; if they diverge on `name`/`subsystem`, prefer the worktree that has the function in its scope (e.g., audio sweep wins for any audio-subsystem row). Conflict on diverging facts → halt; user resolves.
- **STUBS / UNCERTAINTIES / DEFERRED** — IDs are monotonic; on merge, **renumber** the worktree's new IDs to land after the highest existing ID on main. Update inline references (`// STUB S-NNNN`, `[UNCERTAIN U-NNNN]`) accordingly.

A small Python helper at `re/tools/merge_trackers.py` (to be added when first conflict arises) automates this. Until then: do it by hand and commit the renumber separately from the content merge.

## Naming conventions

- Branch: `phase<N>/<topic>` (e.g. `phase5/audio`, `phase3/boot-rework`).
- Worktree dir name: kebab-case topic (`audio-sweep`, `boot-rework`).
- Frida log dir: matches worktree name.

## Anti-patterns

- Two worktrees writing to the same Ghidra slot. Forbidden — `.pool_slot` files exist for this reason.
- Worktrees outside `.worktrees/`. Forbidden — keeps `git worktree list` clean.
- Deleting a worktree without releasing its pool slot. Forbidden — slot stays locked, blocks other sessions.
- Merging with open `[UNCERTAIN]` markers in source that don't have rows in `UNCERTAINTIES.md`. Forbidden — the `re-classify` skill exists for this.
- Pushing branches that contain `re/prior_art/` content (those are unlicensed third-party clones). Use `git push -- --no-thin` filters or scrub before publishing.
