# RESULT savelink write leg (2026-10-23): K0-K4 held

Branch `work/savelink`, PREREG `PREREG_savelink_write.md` (commit `7ecffe118`, before the build). Final exe `1ed749b6`. Not merged, default NOT flipped.
Evidence off git: `D:/MashedWT/savelink_data` (`write_leg_run2.txt`, `arms/`, `selftest_*.txt`, `arm_E.log`, `merged_by_exe.bin`). Private root
`D:/MashedWT/savelink_root`. Shared `original/gamesave.bin` sha256 `bd18788182b2343e` before and after; no `.preexe_bak` in the shared `original/`.

## Verdict

`MASHED_SAVE_ORIGINAL=1` (default OFF) makes the exe write `original/gamesave.bin`, patching the existing image in place. The ORIGINAL loads the
file it wrote on all 156 span cells. With the knob unset nothing measurable moves.

| prediction | result |
|---|---|
| K0 knob OFF: G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`, lint 8 known NEW=0 | held, on exe `481da067` and again on the final `1ed749b6` (the sim is code-layout sensitive, so both) |
| K1 self-test with the knob ON | 4 of 4: G-BYTES 151456/151456, G-LIVE 151456, G-ROUNDTRIP-LIVE 13/13, G-MERGE |
| K2 only {col 4, col 5 of rows 0..12, counter dword} change; `.preexe_bak` == shipped | 0 bytes changed outside the owned cells, 26 of 26 owned cells as written, backup equal to shipped |
| K3 the original's table == the merged file's span | True on 156 cells (`arm_E.log`) |
| K4 knob OFF leaves `original/gamesave.bin` byte-identical, no backup | True |

Scope of K0: G-CTRL and B were rerun, not (b) 9/9, (e), D2 or the 13 tracks. The change is confined to the save TUs and gated by an env var that
is unset on those paths; I did not re-measure those four.

## Findings

1. **The self-test's own G-SMOKE would have deleted the reference.** `GameSave_SelfTest` ends with `std::remove(path)`. With the knob ON and no
   `MASHED_SAVE_PATH`, `path` is `original/gamesave.bin`; the first knob-ON run lost the private root's file that way. `GameSave.cpp` now
   refuses when the path is `original/gamesave.bin`. This is the same pattern as the 2026-10-11 zeroing (`verify/gamesave_zero_20261011/REPRO.md`).
2. **A from-zero write would have wiped the original's file.** `BuildSpanFromProgress` + `BuildImage` zero the launch gates (cols 1, 3, 11), the savedata gate
   (`+0x4ec`) and the whole profile block. Under the knob `SaveProgress` patches in place instead; G-MERGE measures that nothing else changes.
3. **Unlock semantics under the knob.** The shipped save carries `2` in col 4 on every row, so with the knob ON `Campaign_LoadProgress` loads the
   standalone's own progression as all-unlocked. Not measured in a campaign flow here.
4. **Col 5 is not claimed.** The standalone writes its private trophy value into col 5; the original's meaning of that cell is `[UNCERTAIN]`. The
   original loads the cell (K3), but what it does with it was not tested.

## Not claimed

Profile block (`U-3560`), the savedata gate at `+0x4ec` (the exe never writes it under the knob), the in-game Save/Load buttons end to end, and any
C-level change: no `hooks.csv` row moves, since the four `GameSave` functions keep their .asi C4 and this adds a path to the exe copy.

## Decision for Mariano

Flip the default? NOT proposed: it would make every default run overwrite the diffing reference. If a flip is wanted, the open points are finding 3
and 4, and the backup file `original/gamesave.bin.preexe_bak` that the first write leaves next to the reference.
