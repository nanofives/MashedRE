# First divergence: o4 (original) vs RO_ROIMG-fdimg (exe)

- records compared: 566, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (21 blocks wholly masked); ignore: none
- blocks differing at every record: 260; explained by mask / rule / PTR / IMGPTR dwords at tick 17 (both runs dumped): 2 ['0x5f9800', '0x603400']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 566, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- reporting tick 18 (record 17) as asked
- at record 17 (tick 18): 1 counted blocks differ, 78 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260]

## Dumps
- ref tick 17: OK (ckpt200.data.bin)
- ref tick 18: OK (ckpt201.data.bin)
- run tick 17: OK (ckpt02.data.bin)
- run tick 18: OK (ckpt03.data.bin)

## Dwords differing at tick 18 in the 1 blocks: 20 (VALUE 20)

| va | ref t17 | ref t18 | run t17 | run t18 | new at t18 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x00898c00 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898c34 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898c68 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898c9c | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898cd0 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898d04 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898d38 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898d6c | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898da0 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898dd4 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898e08 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898e3c | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898e70 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898ea4 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898ed8 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898f0c | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898f40 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898f74 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898fa8 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898fdc | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| BOOT-WRITE | .data (bss) | 20 | 20 | 0x898c00 |

## Writers (watch.jsonl)
- run:0x5ea004: 0 hits, 0 in the frame tick 17 -> 18 (tick == 17); last writer none at or before that frame
- run:0x67eca4: 0 hits, 0 in the frame tick 17 -> 18 (tick == 17); last writer none at or before that frame
- run:0x7f1004: 0 hits, 0 in the frame tick 17 -> 18 (tick == 17); last writer none at or before that frame
- ref:0x898c00: 531 hits, 1 in the frame tick 17 -> 18 (rec == 17); last writer pc 0x432763 (0x004325c0 FUN_004325c0 [frontend]) at tick 17 value 0x409fffff, bt 0x1|0x432763
- run:0x898c00: 0 hits, 0 in the frame tick 17 -> 18 (tick == 17); last writer none at or before that frame
