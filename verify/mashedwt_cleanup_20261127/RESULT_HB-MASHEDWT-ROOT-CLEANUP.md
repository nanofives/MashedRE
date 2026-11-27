# RESULT HB-MASHEDWT-ROOT-CLEANUP

**Decision B: MET for the row-close delete.** Measured on D: (free-bytes delta): 1.013 GB freed by closing step `u9778` (2 roots, 0.506 GB each). Fake-root test: 0.0094 GB delta == reported tree size. The junction/hardlink share design is NOT implemented (design only, below).

## Changes (branch orch/HB-MASHEDWT-ROOT-CLEANUP)
- `scripts/mashedwt_cleanup.py` (new): `cleanup_for_step`, `prep_root`, CLI `prep <src> <root>` / `clean <step> [--dry-run]`.
- `scripts/diag.py`: `_close_step_root(wt)` called after a successful `wt-remove` (single path and `--all-stale`). Row-close point: `merge_train.ps1:131-133` `Remove-Wt` runs `diag.py wt-remove`, and `Cleanup-Step` (line 200) calls it after a GREEN fast-forward. Step name = worktree folder name (a trailing `_wt` stripped), roots = `D:\MashedWT\<step>_root*`.
- `WORKER_RULES.md:52` and `MERGE_TRAIN.md:105`: root prep without `hook_dual_install.log`, close deletes `original\`.

## Guards (every one tested)
Removes only `<root>\original`; `_data` and the rest of the root stay. Refuses: any other root has a reparse point into the root (scan of depth-1 links of every root, resolved targets), fixed roots (kfdot, mtrain, PORT-BATCH-TOOL, savelink, D5-DEPS-LOCAL, DDKOBJ-SPIKE), root mtime < 7 days, root path in a live process command line (Win32_Process). A junctioned `original\` loses the link only. Reparse points inside `original\` are unlinked one by one (`cmd /c rmdir`) before `shutil.rmtree`. No `rmdir /s`, no `Remove-Item -Recurse`.

## Checks (log: `test_output.txt`, `py -3.12 scripts/mashedwt_cleanup_test.py`, fake base on D:)
All 12 PASS, RESULT PASS 0: prep_root rc<8; prep_root omits hook_dual_install.log; plain root original\ removed; root files and _data kept; reported bytes == tree size; inbound-junction root refused with target intact; linked original\ unlinked with target intact; junction inside original\ not followed (outside file survives); young root refused; live-cmdline root refused; fixed root refused; diag.py wiring (2 call sites).
Real row: `clean u9778` on D:\MashedWT (lead told first by message): `u9778_root` and `u9778_root13` written 2026-10-21, no inbound link, no live process, no worktree. free before 593,318,686,720 B, after 594,331,766,784 B = 1.013 GB. Both roots still hold `extracted`, logs. Survey before: 169 roots with `original\`, 44 junctions (e.g. into `PORT-TOP-04J_rootK\original`). No root written since 2026-11-20 was touched. Dry-run candidate list: 85 further old roots hold 4.68 GB (not deleted: not asked, leads decide).
Not run: `diag.py wt-remove` end to end on a real worktree (the worktree of this row is kept for the lead; the call path is covered by the wiring check only). [UNCERTAIN] row-close of a step whose root name differs from its worktree folder name: test = `diag.py wt-remove .worktrees/<step>` on a throwaway worktree with `<step>_root`.

## Design only: shared read-only original\ (follow-up row filed)
Share-mode check (memory file-share-modes-piz-exclusive-rws-shared): only `0x004b6738` opens with share mode 0 (`.piz`). So: a standalone-only root (no original-side game) can hold `original\` as one junction to a shared read-only copy (`D:\MashedWT\_shared_original`); an original-side root keeps private `.piz` and written files (`gamesave.bin`, `videocfg.bin`) and may hard-link `.rws`. Not implemented: needs the lock/hub path and a read-only ACL tested with a live arm. Cleanup already handles links (refuses targets that others point to).

## Unfinished
Junction/hardlink design; 85 old roots (4.68 GB) and 44 junction roots left; `hook_dual_install.log` is still written by the game into private roots at run time (only the copy omits it); no tool enforces `prep` usage, workers following older text still robocopy it. frida-<hash> folders in %LOCALAPPDATA%\Temp: 1 before, 1 after (this run created none).
