# RESULT PORTCELL-BACKFILL-217 (applied via the documented scripts)

Base: orch/integration b34672ee2. Reference file: D:/MashedWT/HUDINIT-A-rebase_data/hooks.csv.full-backfill (stale for some rows, see below).

## Map used (read-only, built by sibling DDKOBJ-42AD90 from the same commit, no mashedmod source change)
- Path: C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/DDKOBJ-42AD90/mashedmod/build/mashed_re.map
- mtime: 2026-11-12 14:47:40 (-03:00), size 3936113
- sha256: fb12f98418947863896d54dc1e59f0778c211977b3bcc3c10e88673f769ab629

## Commands (run in the worktree)
1. `py -3.12 scripts/backfill_port.py --check --build-dir <map dir>` -> FATAL: exe_file stale on 257 rows (script's own precondition).
2. `py -3.12 scripts/backfill_exe_file.py` (257 rows given an exe_file, 24 old values changed)
3. `py -3.12 scripts/backfill_port.py --build-dir <map dir>` -> rows changed: 462
4. Re-check: `backfill_port.py --check --build-dir ...` rows changed 0; `backfill_exe_file.py --check` 0 changes.
Backups (git-ignored): log/hooks.csv.pre-exe_file-backfill.bak, log/hooks.csv.pre-port-backfill.bak.

## Diff (hooks.csv only, 6431 rows before and after): 719 cells = 462 port + 257 exe_file
Port transitions: ''->none 217 (the 217 drift rows), ''->called 2, none->body 206, none->called 25, body->called 11, called->body 1.
- The 245 non-blank-to-none port changes and the 257 exe_file cells are NOT in the 217 task: the head's exe_file was stale, and backfill_port refuses to run until it is fixed; the port cells follow from the new exe_file values. Lead can drop them only by not taking the exe_file step (then port cannot be derived).
- The 4 "stale-wrapper" rows (00427ca0, 00556cc0, 00556d70, 00557110) are `called`, not regressed.
- One row goes called->body: 0043dfd0 (exe_file now ProvS08_0043dfd0.cpp|Race/RaceModes.cpp, two bodies, none reached from WinMain per the map). Derived by the script, not hand-edited.
- vs the HUDINIT-A file: 247 port cells still differ (the file is older than the tree for these); the script's output wins.
- 2 rows present only in the current tree (00550c50, 00550da0) were derived by the script like any other.
