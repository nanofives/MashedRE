# First divergence: RO_DEF-fdref2 (exe) vs RO_RONEG-fdneg2 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (21 blocks wholly masked); ignore: none
- blocks differing at every record: 2; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 2 ['0x772c00', '0x7e9400']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 399 of 600, first NEW block 0x7f0c00 at record 399
- **first divergent record 399** (tick 400)
- at record 399 (tick 400): 1 counted blocks differ, 1 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

## Dumps
- ref tick 399: OK (ckpt06.data.bin)
- ref tick 400: OK (ckpt07.data.bin)
- run tick 399: OK (ckpt06.data.bin)
- run tick 400: OK (ckpt07.data.bin)

## Dwords differing at tick 400 in the 1 blocks: 1 (VALUE 1)

| va | ref t399 | ref t400 | run t399 | run t400 | new at t400 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x007f0ff0 | 00001324 | 00001356 | 00001324 | 00001357 | True | .data (bss) 00000000 | 0x004924f0 sub_004924f0 [boot] @0x00492679; 0x00493390 FUN_00493390 [util] @0x00493453 | frame timer block 0x007f0ff0..0x007f1034 |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| OTHER | .data (bss) | 1 | 0 | 0x7f0ff0 |

## Writers (watch.jsonl)
- ref:0x7f0ff0: 603 hits, 1 in the frame tick 399 -> 400 (rec == 399); last writer pc 0x18b09 (_PortTop_Body_00493390+0xc9) at tick 399 value 0x1356, bt 0x1a4865
- run:0x7f0ff0: 604 hits, 2 in the frame tick 399 -> 400 (rec == 399); last writer pc 0x1a7445 (_ReplayTrace_Tick+0x85) at tick 400 value 0x1357
