# First divergence: o2 (original) vs RO_DEF-fd2 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\warp_normalize.tsv,C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\replay_masks.tsv (21 blocks wholly masked); ignore: none
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- reporting tick 18 (record 17) as asked
- at record 17 (tick 18): 1 counted blocks differ, 475 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537]

## Dumps
- ref tick 17: OK (ckpt202.data.bin)
- ref tick 18: MISMATCH 1 blocks (ckpt203.data.bin), blocks whose dump differs from the record: ['0x771800']
- run tick 17: OK (ckpt02.data.bin)
- run tick 18: OK (ckpt03.data.bin)

## Dwords differing at tick 18 in the 1 blocks: 20 (VALUE 20)

| va | ref t17 | ref t18 | run t17 | run t18 | new at t18 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x00898c00 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898c34 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| 0x00898c68 | 00000000 | 409fffff | 00000000 | 00000000 | True | .data (bss) 00000000 | none (abs) | other page 0x00898000 |
| ... 17 more VALUE dwords (--max-dwords) | | | | | | | | |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| BOOT-WRITE | .data (bss) | 20 | 20 | 0x898c00 |

## Writers (watch.jsonl)
- ref:?: 1 hits, 0 in the frame tick 17 -> 18; last writer pc 0x42d3ef (0x0042d3e0 MenuEntryArrayInit [frontend]) at tick 16 value 0x0, bt 0x43d3ea|0x42d3ef|0x75b65d49|0x76fdc88b|0x76fdc811
- run:0x5ea004: 0 hits, 0 in the frame tick 17 -> 18; last writer none at or before that frame
- ref:0x604604: 1 hits, 0 in the frame tick 17 -> 18; last writer pc 0x45d6a8 (0x0045d460 FUN_0045d460 [audio]) at tick 0 value 0x4, bt 0x1aff74|0x45d6a8|0x4669fb|0x4028e0|0x8aa300|0xa3040d89|0xd89008a|0x8aa308
- run:0x604604: 0 hits, 0 in the frame tick 17 -> 18; last writer none at or before that frame
- run:0x7f1030: 604 hits, 1 in the frame tick 17 -> 18; last writer pc 0x18b0f (_PortTop_Body_00493390+0xcf) at tick 17 value 0x134071d4, bt 0x1a0255
- ref:0x898c00: 530 hits, 1 in the frame tick 17 -> 18; last writer pc 0x432763 (0x004325c0 FUN_004325c0 [frontend]) at tick 17 value 0x409fffff, bt 0x1|0x432763
- run:0x898c00: 1 hits, 0 in the frame tick 17 -> 18; last writer pc 0x1b43f (_MenuEntryArrayInit+0xf) at tick 0 value 0x0, bt 0x457b1
