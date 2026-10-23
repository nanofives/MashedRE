# RESULT ghost-block round trip (2026-10-23): Q0-Q3 held, Q1 = the menu path restores the block

PREREG `PREREG_ghost_roundtrip.md` (commit `10f29dc12`, before any arm). Evidence off git: `D:/MashedWT/savelink_data/ghost_g1/`
(`block_before.bin`, `block_after.bin`, `rec_frames.bin`, `gamesave_captured.bin`, `block_menu_reload.bin`), logs `ghost_g1.log`,
`ghost_g2.log`, `ghost_g3.txt`. Private root restored to the shipped save afterwards; shared `original/gamesave.bin` sha256
`bd18788182b2343e` before and after. No exe code changed in this leg (exe `1ed749b6` from the write leg), so no gate rerun is owed.

## Verdict

A real TimeTrial ghost record, copied into the block by the game's own `FUN_00483ca0` and saved by the game's own writer, survives every hop:
the file carries it byte for byte, a fresh original restores it byte for byte through the menu path, and the standalone's in-place write
(`MASHED_SAVE_ORIGINAL=1`) changes none of its bytes.

| prediction | result |
|---|---|
| Q0 capture is real | held. TimeTrial (`DAT_0067e9fc` = 2): `DAT_0063bb14` live, `rec[0]` = 0x10b, `rec[1]` = 0x127c, `rec[2]` = 0x1e, `rec[0x5b]` = 120, `rec+0x174` = 114050. After the copy: block was all zero before; frames sit verbatim at `0x19c..0x127b`; everything from `0x127c` on is zero; the file's `0x4..` equals the block byte for byte |
| Q1 reload | **the block is restored.** A fresh original booted to the mode-select menu holds 150,076 of 150,076 bytes equal to the file (2,888 non-zero) |
| Q2 standalone | held. G-MERGE with the captured file as the base: 0 bytes changed outside the owned cells, 26 of 26 owned cells as written; the original then dumps the merged span on all 156 cells |
| Q3 pointer | the header carries `rec[5]` = `0x14c8ab18` (a heap address in the writing process) verbatim at block offset `0x14` |

## What the record looks like (block offsets, from `block_after.bin`)
`+0x00` 0x10b, `+0x04` 0x127c (= 0x19c + 120 x 0x24, the stride `FUN_00483ca0` walks), `+0x08` 0x1e (the size table's first id, float 20.0),
`+0x0c` 0x41a00000 (20.0f), `+0x10` 10, `+0x14` heap pointer, `+0x16c` (dword 0x5b) 120, `+0x174` 114050. 33 non-zero bytes in the 0x19c header.
The id and the float match the table entry; that `+0x0c` is the table float and `+0x10` the `10` argument of `FUN_00482900` is read from the
values only `[UNCERTAIN: no writer read]`.

## Limits
- Synthetic: the copy ran OUTSIDE `ReplaySave`'s gate (`DAT_005f29c8`, best-time compare, table float). It proves storage and reload, not that
  the game would keep this run. Natural arming (`FUN_00429310` calling `FUN_00411870(param_1 != 0)`) is not read.
- One record, slot 0, id 0x1e, a full 120-frame buffer. Records for other ids and a second record behind the first are not exercised.
- The heap pointer at `+0x14` reloads as a stale address into a different process. Whether `Replay::Load` (`0x00483d10`) dereferences it or
  rebases it is not tested here (a TimeTrial run on the reloaded save would show it).
- The header dump `rec_header.bin` came back empty (Frida returns one binary buffer per RPC reply); the header was checked through the six
  fields `snap` read, and through the block itself.
- The captured file's span differs from the shipped one: `SerializeToBuffer` snapshots the live table in the warped race.

## Tracker input (for re-classify, not applied)
U-3560: "profile/career block" -> the 13-slot ghost buffer (sizes from `DAT_005f29d0`), populated only by TimeTrial ghost saves; layout of the
record header partly read (above). The wrong `0x2443C` in `save_gamesave_d3/00404e80.md` and `structs/gamesave_layout.md:17` and the stale
U-1567 citations are still to fix.

## Amendment 1 result (2026-10-23): the stale pointer is discarded, the reloaded ghost is the time-attack record

Arm G4 (PREREG amendment 1, commit before the run). Captured save in the private root, `scenario_launch.py --mode 2 --track 0 --hold 40`.

- **G4a/G4b VOID (harness).** Without a preload the in-race block was all zero (`block_in_race.bin`: 0 non-zero bytes): the warp skips the
  frontend step that restores the save, so `Replay::Load` missed and `FUN_00411d90` fell back to a fresh object (`DAT_0063bb08 +0x174` = 0,
  `DAT_0063bb2c` = 0). Same cause as `RESULT_ACCEPT2.md`.
- **G4c** (`SAVELINK_PRELOAD=1`: the game's own `SaveLoad 0x00404e50` then `DeserializeFromBuffer 0x00404e80`, called at the menu before the
  launch; buffer magic `0xDEADBEEF`, 856 non-zero block dwords after the restore):

| prediction | result |
|---|---|
| R1 the record loads | held: `DAT_0063bb10` = `DAT_0063bb08` = 0x14d2e810; `[0]` 0x10b, `[2]` 0x1e, `[0x5b]` 120, `+0x174` 114050 (the captured run's time); `DAT_0063bb2c` = 1 |
| R2 stale pointer | **discarded**: `[5]` = 0x14da1da8 (not the stored 0x14c8ab18) and `[5] == [6]`; `[7]` is the walking cursor |
| R3 frames | 0 bytes differ outside node `+0x1c` over 120 x 0x24; all 120 `+0x1c` links differ (rebuilt in the new buffer) |
| R4 no crash | the race held 40 s, no exit or crash line in `ghost_g4c.log` |

So the stored heap pointer is harmless: `Replay::Load` overwrites it with the fresh allocation before any read through it. The loader also reads
`(n + 2) * 0x24` frame bytes, 2 nodes more than the writer copied (`FUN_00483ca0`: `n * 9` dwords): the extra 0x48 bytes come from whatever follows
the record in the block (zero here) `[UNCERTAIN: what the two extra nodes are used for]`.

The preload is the game's own pair called by hand, in order, at the menu; which frontend function calls them in normal play is not read.
