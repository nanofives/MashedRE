# RESULT savelink round trip (2026-10-23): P0-P4 all held

Branch `work/savelink`, PREREG `PREREG_savelink_roundtrip.md` (commit `6de73a05d`, before any arm). Run output: `roundtrip_result.txt`.
Evidence off git: `D:/MashedWT/savelink_data` (per-arm probe logs, arm saves, `savefmt_cli.exe`). Private root
`D:/MashedWT/savelink_root/original`. Shared `original/gamesave.bin` sha256 `bd18788182b2343e` before and after. No exe or
.asi code changed, so no gate rerun is owed.

## Verdict

The ORIGINAL accepts a `gamesave.bin` built by the standalone's `BuildImage` on every span cell, and the standalone's
`ParseImage` accepts the file the original's own writer produces. The control that could fail did fail the right way.

| prediction | result |
|---|---|
| P0 control (shipped save) dump == shipped span | True |
| P1 positive control moved exactly r2c1 (0 -> 1) | True, diff cells `[(2, 1)]` |
| P2 subject S == P on all 156 cells | True, no differing cell |
| P3 original-written file: size 0x24FA0, parses, span == live table | True |
| P4 `ParseImage` of S, counter 7 | PARSE ok, COUNTER 7 |

## Scope of the claim

- Covers the championship span (13 rows x 12 cells at file `0x24A40`), the counter slot and the magic. Nothing about the
  profile block (file `0x4`..`0x24A3F`): S carries zeros there, as the standalone does, and U-3560 (profile layout) stays open.
- One cell was edited (r2c1). The arms compare the whole 156-cell table, but the edit pattern is a single cell.
- The dump is the live table at `0x007f0a40` at the mode-select menu (`nav_champ_probe.py --plan dumptable`), the path that
  loaded an edited save before (`verify/chalsel_save_edited.bmp`). It replaces `scenario_launch.py`, which never loads the
  save (`verify/d4_save_20261005/RESULT_ACCEPT2.md`).
- The shipped save already carries magic `0xDEADBEEF`, so the magic gate is not what separates a blank save from a written one.

## Harness findings

- The Bash/PowerShell sandbox blocks the original from booting: AV `0xC0000005` at `0x005baf48` (`mov dl,[eax+0x78]`, NULL
  first argument) on both the shared `original/` and the private copy, 5 of 5. Launched with the sandbox disabled, the same
  exe boots to `MASHED [D3D9] [Release]`. Every original-side run in this lane needs the sandbox off.
- A private root needs its own AppCompat layer (`~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP`) on its `MASHED.exe`
  path; `setup_mashed_compat.ps1` only sets it for `original/`. It also drops `DWM8And16BitMitigation` from that value; restored.

## Next leg (needs Mariano's decision, not started)

Write the original-format file from the exe (item 2 of the plan): knob default OFF + PREREG, flip only on decision.
Open question for that leg: target `original/gamesave.bin` itself, or keep the sidecar name `mashed_re_gamesave.bin` and only
guarantee the format.
