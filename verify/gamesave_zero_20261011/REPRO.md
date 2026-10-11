# original\gamesave.bin zeroed on 2026-10-11: writer and guard

## Verdict

The writer is the Frida A/B harness calling **SaveWrite 0x00404f50** at the warm main menu.
SaveWrite writes the 0x24fa0-byte buffer at 0x00803358 to `"gamesave.bin"` (string at
0x005cc8e0), opened relative to the game's cwd, which is `original\` for every harness spawn
(`run_diff.py:620`, `run_negctl.py:204`). At the warm menu that buffer is all zero: the boot
never loads the save into it (the ~180 menu-walk reads of `gamesave.bin` do not fill
0x00803358). One call leaves `gamesave.bin` as 151,456 zero bytes. Both sides do it: the
original body (path1 orig arm) and the port `Save/GameSave.cpp SaveWrite` (path1 reimpl arm,
path2 installed hook), because the port writes the same buffer through the same file layer
(`gFileWrite(kGameSaveFilename, kGameSaveBuf, kGameSaveSize)`).

`save_write` is in the registry with `arg_type: none` and `path1_tests [0, 1]`, so every
run_diff, run_verify_hook and run_negctl job on it rewrites the shared save. Today's negctl
runs were the first such runs since 2026-10-05, when the live save still matched
`gamesave.bin.refbak_20260830` (`verify/d4_save_20261005/PREREG_SAVE.md:23`):

- the first full negctl run (runner commit 885bc946 at 15:29:20; `save_write` is job
  `negctl_jobs_20261011.tsv:3969`); file mtime 15:30:45.
- the re-earn batch b01 negctl run (`re/analysis/negctl/reearn_20261011/runs/b01/negctl_20261011_175913.tsv:61-68`,
  8 save_write jobs between 17:57:49 and 17:59:13, the run's live/final TSV stamps), then the
  b01 path1-regeneration loop in the same session (9a9b527d, background task `b6lphpwz5`): a
  plain `run_diff.py <hook>` per b01 hook, with `save_write` third from last. The loop finished at
  18:01:06 and its last line is `sha256sum ../../original/gamesave.bin` = `8e982900f54c...`
  (all zero). That is the 18:00:57 mtime.

[UNCERTAIN] which of the jobs in the first full negctl run made the 15:30:45 write: that run's
per-job timestamps were not kept. The attribution rests on save_write being in its job list,
the 15:29:20 start, and the fact that no other hook tested here writes the save (below).

## Evidence (all on a scratch COPY of original\, never on original\)

1. Direct call. Booted the copy's MASHED.exe (`MASHED_RE_NO_AUTO_HOOK=1`), waited 20 s at the
   menu, read 0x00803358..+0x24fa0: `nz=0, magic=0x0`. Called `NativeFunction(0x00404f50)()`:
   CreateFileA/W(`"C:gamesave.bin"`, access 0x40000000 GENERIC_WRITE), and the file went from
   sha256 `bd18788182b2` (348 non-zero bytes) to `8e982900f54c` (0 non-zero bytes).
   `8e982900f54c` is also the sha256 prefix of the zeroed copy saved at 18:00
   (`gamesave.zeroed_1800.bin`).
2. Control, stock harness: `run_diff.py save_write` with the unguarded `diff_template.js`:
   GREEN 2/2, save `bd18788182b2 -> 8e982900f54c`.
3. Control, stock path2: `run_verify_hook.py save_write` with the unguarded template: save
   `bd18788182b2 -> 8e982900f54c` (its FAIL verdict, "rel32 mismatch / no valid JMP", is the
   same with and without the guard).
4. Guarded: the same `run_diff.py save_write`, GREEN 2/2, save stays `bd18788182b2`; the
   151,456-byte write lands in `%TEMP%\harness_gamesave_<pid>.bin`.
5. Guarded path2: save stays `bd18788182b2`.
6. Guarded negctl: `run_negctl.py --handlers none,scalars_to_scattered_globals --hooks save_write`
   gives the same verdicts as b01 (real/baseline GREEN, null/ret RED, field_none CTRL-GREEN,
   field_first/field_last RED with flips=2); save stays `bd18788182b2`.

7. Exclusion sweep, unguarded template on the copy, save reset before each hook: every hook of
   the two other negctl runs active at 18:00:57 (b03 to 18:01:32, b04 to 18:01:19) plus b01's
   `autosave_trigger` and `thunk_replay_save` (21 hooks, `run_diff.py <hook>`). None changed the
   save (all `bd18788182b2`).

## Fix

`re/frida/diff_template.js` and `re/frida/verify_hook_install_template.js` (loaded by
run_diff, run_diff_warm, run_diff_scenario, run_diff_scenario_batch, run_negctl and
run_verify_hook) now hook CreateFileA/W first thing and send any GENERIC_WRITE open of
`*gamesave.bin` to `%TEMP%\harness_gamesave_<pid>.bin`. Reads are untouched. The race lane's
`pin_gamesave` (3e6870de) still pins its reads to the reference copy.

Not covered: Frida scripts that do not load either template (e.g. `early_window_leaf_diff.py`,
`multi_state_driver.py`, one-off probes). None of them has a `save_write` vector today.
Side note, not a save issue: `config_save` 0x004989b0 rewrites `original\videocfg.bin` the same way
(same canonical 512 bytes, registry comment at its entry).
