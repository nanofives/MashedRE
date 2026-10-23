# PREREG ghost-block round trip (2026-10-23), committed before any arm

Decision (Mariano, 2026-10-23): semi-synthetic capture. Static basis: `re/analysis/save_ghost_block_20261023.md` (the "profile block" at file
`0x4..0x24A3F` is the 13-slot ghost buffer `*DAT_008a94a8`).

## Method (original only, private root `D:/MashedWT/savelink_root`, sandbox off)
- G1 capture: `scenario_launch.py --mode 2` (TimeTrial) warps the original into a race. At the end of the hold, a new `--ghost-capture <dir>`
  step reads `DAT_0063bb10`, `DAT_0063bb14`, `DAT_0067e9fc`, and from the recording `rec = *DAT_0063bb14`: `rec[0]`, `rec[1]`, `rec[2]`, `rec[5]`,
  `rec[0x5b]`, `rec+0x174`. It calls the game's own `FUN_00483ca0(rec, &DAT_008a94a8)` OUTSIDE its normal gate (this is the synthetic part),
  dumps the 0x24a3c-byte block, then `SerializeToBuffer 0x00404ee0` + `SaveWrite 0x00404f50`, which writes the private root's `gamesave.bin`.
- G2 reload: that file is the private root's save; a fresh original boots to the mode-select menu (`nav_champ_probe.py`, new plan `ghostdump`)
  and dumps `*DAT_008a94a8` (0x24a3c bytes) and the table `0x007f0a40`.
- G3 standalone: `write_leg.py`'s knob-ON self-test with the captured file as the pristine copy (G-MERGE), so the standalone's in-place write
  is checked over the populated block.

## Predictions
- Q0 (capture is real): in TimeTrial `DAT_0063bb14 != 0`, `rec[0] == 0x10b`, `rec[0x5b] > 0`, and after the copy the block's first
  `0x19c + rec[0x5b]*0x24` bytes are non-zero somewhere and equal `rec[0..0x66]` followed by `rec[0x5b]*9` dwords from `rec[5]`; the rest of the
  block stays zero. The written file's `0x4..` equals the dumped block byte for byte (SerializeToBuffer copies 0x928F dwords).
- Q1 (reload) [registered as UNCERTAIN, both outcomes are findings]: either the fresh original's block equals the file's `0x4..0x24A3F`
  (the menu path runs `DeserializeFromBuffer 0x00404e80` / copies it back), or it stays all zero (ghosts are loaded elsewhere, e.g. on entering a
  TimeTrial). A partly-equal block is neither and is a finding. The table must still equal the file's span (as in P0), or the arm is VOID.
- Q2 (standalone): G-MERGE reports 0 bytes changed outside the owned cells with the populated block present, so the exe's write keeps all
  150,076 block bytes.
- Q3 (pointer): `rec[5]` is a heap address; the stored header carries it verbatim at block offset `0x14`. Recorded, not judged.

## Decision rule
Q0 fails -> the capture recipe is wrong, stop. Q2 fails -> defect in the knob path, fix in this lane. Q1 is reported either way. Nothing here
changes a default; no gate rerun is owed unless exe code changes. Shared `original/` hash recorded before and after.
