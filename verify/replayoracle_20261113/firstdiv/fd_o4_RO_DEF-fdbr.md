# First divergence: o4 (original) vs RO_DEF-fdbr (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (21 blocks wholly masked); ignore: none
- blocks differing at every record: 534; explained by mask / rule / PTR / IMGPTR dwords at tick 17 (both runs dumped): 0 []
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 536 counted blocks differ, 473 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536]

## Dumps
- ref tick 0: missing
- ref tick 1: missing
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Writers (watch.jsonl)
- run:0x5ea004: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- run:0x67eca4: 1 hits, 1 in the frame tick 0 -> 1 (rec == 0); last writer pc 0x187a3 (_RaceArmReset+0x13) at tick 0 value 0x1, bt 0x46741
- run:0x7f1004: 1201 hits, 1 in the frame tick 0 -> 1 (rec == 0); last writer pc 0x2a9b79 (LargeRangeBytes+0x9) at tick 0 value 0x0, bt 0x46741
- ref:0x898c00: 531 hits, 0 in the frame tick 0 -> 1 (rec == 0); last writer none at or before that frame
- run:0x898c00: 1 hits, 1 in the frame tick 0 -> 1 (rec == 0); last writer pc 0x1b43f (_MenuEntryArrayInit+0xf) at tick 0 value 0x0, bt 0x46741

## Re-run plan (missing evidence)
- ref (original): detorig.py --boot ... --ckpt-ticks 0,1
- run (exe): MASHED_REPLAY_TRACE_CKPT=0,1
