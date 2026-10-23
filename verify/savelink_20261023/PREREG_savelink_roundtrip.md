# PREREG savelink round trip (2026-10-23), committed before any arm is run

Lane `work/savelink`, base `3316ca779`. Question: does the ORIGINAL accept a `gamesave.bin` written by the standalone's format code
(`Save/GameSaveFormat.h` `BuildImage`), and does the standalone's `ParseImage` accept one written by the original (`SaveWrite`
`0x00404f50` after `SerializeToBuffer` `0x00404ee0`)?

Why prior attempts were VOID (`verify/d4_save_20261005/RESULT_ACCEPT2.md`): `scenario_launch.py` warps into a race and never loads the
save. This lane uses `re/frida/nav_champ_probe.py --plan dumptable`, which boots the original to the mode-select menu and dumps the
live table `0x007f0a40` (13 rows x 12 dwords). That path read an edited save before (`verify/chalsel_save_edited.bmp`, only Hold the
Flag padlocked). It runs on a private copy: `D:/MashedWT/savelink_root/original` (`MASHED_ORIG_DIR`). The shared `original/` is not touched.

## Arms (each = one boot, one dump)
- C control: the shipped save (`BD18788182B2...`).
- P positive control, built to be able to FAIL: shipped save with row 2 col 1 (file `0x24aa4`) 0 -> 1 via `re/tools/gamesave_edit.py`.
- S subject: image written by `BuildImage` (the standalone's writer, through `re/tools/savefmt/savefmt_cli.exe`) from the shipped span
  with the same r2c1 = 1 edit, save counter 7, magic `0xDEADBEEF`. The profile region is zero (the standalone has no live profile).
- R reverse: on a booted original that loaded P, call `0x00404ee0` then `0x00404f50` by Frida `NativeFunction` (the REPRO.md route),
  then parse the written file with the standalone's `ParseImage`.

## Predictions
- P0 (control passes): C's dump equals the shipped file's span cells (row r, col j at file `0x24a40 + 0x30*r + 4*j`).
- P1 (control can fail): P's dump differs from C's in exactly one cell, r2c1 = 1. If P == C the harness still does not load saves: VOID.
- P2 (subject): S's dump equals P's dump on all 156 cells. Any cell that differs is a finding; I report which.
- P3 (reverse): R's file is 0x24FA0 bytes, magic `0xDEADBEEF`, `ParseImage` returns true, and its span bytes equal P's dump cells.
- P4: the standalone's `ParseImage` of S round-trips: the span I put in comes back byte-identical, counter 7.

## Decision rule
P1 fails -> VOID, nothing below counts. P1 holds and P2 holds -> the original accepts the standalone's format on the span columns
(says nothing about the profile block, `U-3560`). P2 fails -> the failing cells go to the findings table, no code change in this leg.
Nothing here changes the exe, so no gate rerun is owed. `original/` SHA before and after is recorded.
