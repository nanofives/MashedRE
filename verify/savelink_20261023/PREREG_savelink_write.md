# PREREG savelink write leg (2026-10-23), committed before the build or any arm

Decision (Mariano, 2026-10-23): the exe writes `original/gamesave.bin` itself, behind a knob. Round trip established first
(`RESULT_savelink_roundtrip.md`, P0-P4 held on the span cells).

## Change
- `MASHED_SAVE_ORIGINAL=1` (default OFF, `Save/GameSave.cpp` exe arm): the save path becomes `original/gamesave.bin` (cwd is the repo
  root after exe_main's self-locate). `MASHED_SAVE_PATH` still wins. First write copies an existing file to
  `original/gamesave.bin.preexe_bak` when that backup is absent.
- Under the knob `SaveProgress` (`Race/GameFlow.cpp`) patches the existing image in place (col 4 unlock = 2/0 and col 5 trophy of the
  rows it tracks, the counter at `0x24EFC`) instead of `BuildImage` from zero, which would wipe the launch gates (cols 1, 3, 11), the
  savedata gate (`+0x4ec`) and the profile block of the original's file. No valid file -> the old rebuild.
- Knob OFF: the code path is the old one. The self-test's `pathA` now comes from `GameSave_FilePath()` (same value as before when OFF).

## Known consequence, stated before measuring
col 4 is "track unlock" to `ApplyProgressFromSpan` but the shipped save carries `2` in col 4 on every row, so with the knob ON and the
shipped file the standalone's own progression loads as all-unlocked. That is a behaviour change under the knob, not a defect of OFF.
Col 5 is the standalone-private trophy column (`kColTrophyPriv`); the original's meaning of that cell is `[UNCERTAIN]`, so what the
original does with trophy values written there is NOT claimed.

## Predictions (knob OFF = default)
- K0: with the knob unset the exe is behaviourally unchanged: G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`
  (the sim is code-layout sensitive, so this is measured, not assumed); `rva-lint` 8 known NEW=0; no file named `original/gamesave.bin*`
  is created or modified by a default run.
## Predictions (knob ON, private root as cwd, `MASHED_SAVE_SELFTEST=1`)
- K1: `GameFlowSaveSelfTest` still passes 3 of 3 with the knob ON (G-BYTES against `GameSave_FilePath()`, G-LIVE 151456, G-ROUNDTRIP-LIVE 13).
- K2: the self-test's `Campaign_SaveNow` leaves `original/gamesave.bin` (private root) at 0x24FA0 bytes, magic `0xDEADBEEF`, with every
  byte outside {col 4, col 5 of rows 0..kAreaCount-1, the counter dword} equal to the shipped file; `.preexe_bak` equals the shipped file.
- K3: the ORIGINAL (private root, `nav_champ_probe.py --plan dumptable`) dumps a table equal to that file's span on all 156 cells.
- K4: the knob OFF run in the same private root leaves `original/gamesave.bin` byte-identical (sha256) and creates no `.preexe_bak`.

## Decision rule
K0 fails -> the change is not knob-free: stop, report which hash moved, no flip question arises. K2 or K3 fails -> findings table, no code change
beyond fixing a defect in this leg. All hold -> report; flipping the default is Mariano's call and is NOT proposed here (it overwrites the
diffing reference by default).
