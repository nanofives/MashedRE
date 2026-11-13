# First divergence: o2 (original) vs RO_DEF-fdvs (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\warp_normalize.tsv,C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\replay_masks.tsv (21 blocks wholly masked); ignore: none
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 537 counted blocks differ, 473 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 537 blocks: 81068 (IMGPTR 10, MASK:audio 187, MASK:audio-glue 1, MASK:crt 56, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:d3dx9 139, MASK:lua 4, MASK:os 17, MASK:qhull 5, MASK:renderware 199, PTR 317, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 5, VALUE 80116)

| va | ref t0 | ref t1 | run t0 | run t1 | new at t1 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x005ea004 | 004a57e4 | 004a57e4 | 00000000 | 00000000 | False | .data 004a57e4 | none (abs) | other page 0x005ea000 |
| 0x005ea008 | 005ca6a0 | 005ca6a0 | 00000000 | 00000000 | False | .data 005ca6a0 | none (abs) | other page 0x005ea000 |
| ... 80114 more VALUE dwords (--max-dwords) | | | | | | | | |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| BOOT-WRITE | .data | 224 | 224 | 0x604604 |
| BOOT-WRITE | .data (bss) | 31698 | 31698 | 0x63bd98 |
| BOOT-WRITE | _rwdseg | 2 | 2 | 0x915000 |
| FILEINIT-MISSING | .data | 47577 | 47576 | 0x5ea004 |
| FILEINIT-MISSING | .data (bss) | 25 | 0 | 0x7032ec |
| OTHER | .data | 465 | 465 | 0x6147dc |
| OTHER | .data (bss) | 125 | 0 | 0x67d974 |

## Writers (watch.jsonl)
- ref:?: 1 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- run:0x5ea004: 0 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- ref:0x604604: 1 hits, 1 in the frame tick 0 -> 1; last writer pc 0x45d6a8 (0x0045d460 FUN_0045d460 [audio]) at tick 0 value 0x4, bt 0x1aff74|0x45d6a8|0x4669fb|0x4028e0|0x8aa300|0xa3040d89|0xd89008a|0x8aa308
- run:0x604604: 0 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- run:0x7f1030: 604 hits, 3 in the frame tick 0 -> 1; last writer pc 0x17d464 (_Store7f1030_Qpc3Mhz+0x24) at tick 0 value 0x133344c7
- ref:0x898c00: 530 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- run:0x898c00: 1 hits, 1 in the frame tick 0 -> 1; last writer pc 0x18b288 (_MenuEntryArrayInit+0x28) at tick 0 value 0x0, bt 0x1e951
