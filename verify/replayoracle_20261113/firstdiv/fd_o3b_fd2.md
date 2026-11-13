# First divergence: o3b (original) vs RO_DEF-fd2 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\warp_normalize.tsv,C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-FIRST-DIVERGENCE\re\frida\replay_masks.tsv (21 blocks wholly masked); ignore: none
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 537 counted blocks differ, 473 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537, 537]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: MISMATCH 2 blocks (ckpt201.data.bin), blocks whose dump differs from the record: ['0x614400', '0x771800']
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
- ref:?: 1 hits, 1 in the frame tick 0 -> 1; last writer pc 0x492502 (0x004924f0 sub_004924f0 [boot]) at tick 0 value 0x4a57e4, bt 0x75b6d640|0x492502
- ref:0x5ea004: 0 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- run:0x5ea004: 0 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- run:0x604604: 0 hits, 0 in the frame tick 0 -> 1; last writer none at or before that frame
- ref:0x7f1030: 1999 hits, 1 in the frame tick 0 -> 1; last writer pc 0x49511a (0x00495110 FUN_00495110 [util]) at tick 0 value 0x13419e9b, bt 0x49277e|0x49511a|0x75b65d49|0x76fdc88b|0x76fdc811
- run:0x7f1030: 604 hits, 3 in the frame tick 0 -> 1; last writer pc 0x187fa (_Store7f1030_Qpc3Mhz+0xa) at tick 0 value 0x133344c7
- run:0x898c00: 1 hits, 1 in the frame tick 0 -> 1; last writer pc 0x1b43f (_MenuEntryArrayInit+0xf) at tick 0 value 0x0, bt 0x457b1
