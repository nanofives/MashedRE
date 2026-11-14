# First divergence: o2 (original) vs og1 (original)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-ORACLE-GATE\re\frida\warp_normalize.tsv,C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-ORACLE-GATE\re\frida\replay_masks.tsv (148 blocks wholly masked); ignore: none
- blocks differing at every record: 11; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 10 ['0x616000', '0x772c00', '0x773800', '0x773c00', '0x7d4000', '0x7d7000', '0x7dc800', '0x7de000']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 3 of 600, first NEW block 0x614400 at record 3
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 3 counted blocks differ, 1 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [3, 3, 3, 4, 4, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt200.data.bin)
- run tick 1: OK (ckpt201.data.bin)

## Dwords differing at tick 1 in the 3 blocks: 22 (PTR 17, VALUE 5)

| va | ref t0 | ref t1 | run t0 | run t1 | new at t1 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x0068bc2c | 00000000 | 00000000 | 00000000 | 00000004 | True |  - | none (abs) | other page 0x0068b000 |
| 0x00771998 | 00000000 | 0001a4b2 | 00000000 | 0001a4af | True |  - | none (abs) | other page 0x00771000 |
| 0x0077199c | 00000000 | 0001a4b2 | 00000000 | 0001a4af | True |  - | none (abs) | other page 0x00771000 |
| 0x007719a0 | 00000000 | 0001a4b2 | 00000000 | 0001a4af | True |  - | none (abs) | other page 0x00771000 |
| 0x007f1030 | 00000000 | 1341c1c3 | 00000000 | 13419e9b | True |  - | none (abs) | frame timer block 0x007f0ff0..0x007f1034 |

## Writers (watch.jsonl)
- ref:?: 1 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- ref:0x604604: 1 hits, 1 in the frame tick 0 -> 1 (tick == 0); last writer pc 0x45d6a8 (0x45d6a8) at tick 0 value 0x4, bt 0x1aff74|0x45d6a8|0x4669fb|0x4028e0|0x8aa300|0xa3040d89|0xd89008a|0x8aa308
- ref:0x898c00: 530 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
