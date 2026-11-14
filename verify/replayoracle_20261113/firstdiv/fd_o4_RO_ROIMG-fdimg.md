# First divergence: o4 (original) vs RO_ROIMG-fdimg (exe)

- records compared: 566, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (21 blocks wholly masked); ignore: none
- blocks differing at every record: 260; explained by mask / rule / PTR / IMGPTR dwords at tick 17 (both runs dumped): 2 ['0x5f9800', '0x603400']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 566, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 260 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260]

## Dumps
- ref tick 0: missing
- ref tick 1: missing
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Writers (watch.jsonl)
- run:0x5ea004: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- run:0x67eca4: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- run:0x7f1004: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- ref:0x898c00: 531 hits, 0 in the frame tick 0 -> 1 (rec == 0); last writer none at or before that frame
- run:0x898c00: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame

## Re-run plan (missing evidence)
- ref (original): detorig.py --boot ... --ckpt-ticks 0,1
- run (exe): MASHED_REPLAY_TRACE_CKPT=0,1
