# PREREG: savelink2 (b) slice 2, GameSaveVFS (VfsFileExists 0x00550b00, AutosaveTrigger 0x004099a0) as shared bodies

Written before any arm. Build: exe `5f8af4bb`, dev .asi `5c5f8aa9` (work/savelink2, uncommitted on 12e7b14a9). Lint 0 NEW.
Mariano 2026-10-23: 0x004cbe80 keeps C4 (recorded with this slice).

## Change

- Save/GameSaveVFS.cpp holds one body of each function and joins exe_sources.rsp. In the exe both are dead code: the VFS
  words are private zeros, the autosave guard is bound to GameFlow_SaveCounterPtr, and the flags are private.
- Fixes from the disassembly (file header):
  - VfsFileExists: the prefix keeps its ':' (0x00550b4b terminator at buf[i+1]); the scan resumes after a miss
    (0x00550b74); a NULL default object takes the error-callback branch (0x00550b79..0x00550b8e).
  - AutosaveTrigger: printf 0x004a2cbd on 0x005ccb34, where the old copy called wprintf on 0x005cc7dc.

## Arms (private root D:/MashedWT/savelink_root, fresh .asi deployed there, sandbox disabled)

- **V1 sweep.** `MASHED_LEAF_SELFTEST=1 MASHED_LEAF_SELFTEST_SWEEP=1`,
  `scenario_launch.py --hooks 0x00550b00,0x004099a0 --cars 4 --hold 30`.
- **V2 live.** As V1 without the sweep, `--hold 60`.
- **G gates.** Exe `5f8af4bb`, `run_arms.py H_DEF-1 B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **L lint.** Already 0 NEW on this tree.

## Predictions

- **P1 (V1).** `SWEEP fn=550b00 cases=154 noo=0 ndiff=0`: active = 3 lists x 2 defaults x 2 callbacks x 11 names (132),
  inactive = 2 callbacks x 11 names (22). Both `dispatched` and `errcb` are non-zero.
  `SWEEP fn=4099a0 cases=4 noo=0 ndiff=0`: the guard-0 case leaves the sentinels and the non-zero cases write 4/1/0.
- **P2 (V2).** Live VfsFileExists calls show noo 0 and ndiff 0. The colon class may have 0 calls, in which case the colon
  paths rest on the sweep only. AutosaveTrigger has no live call in a race (it runs after a championship track).
- **P3 (G).** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6 and B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

## Analytic control (not run)

The old body fails these cases by construction:
- "a:b" with list L, where its prefix "a" matches obj1 and the original's "a:" matches obj0.
- "zz:a:b", where it stops at the first colon.
- Every default-NULL case with the callback set, where it skips the callback.

## Decision rule

- All hold: commit. Levels are kept: 0x00550b00 and 0x004099a0 stay C4, a file-column-only re-classify, and the C4
  question is raised like slice 1's.
- P1 or P2 fails: report the first failing case and do not commit the code.
- P3 fails: stop for Mariano.
