# First divergence: og1 (original) vs RO_ROIMG-og1i (exe)

- records compared: 566, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-ORACLE-GATE\re\frida\warp_normalize.tsv,C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-ORACLE-GATE\re\frida\replay_masks.tsv (148 blocks wholly masked); ignore: none
- blocks differing at every record: 140; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 2 ['0x5f9800', '0x603400']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 566, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 139 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 139 blocks: 6592 (IMGPTR 1, MASK:audio 165, MASK:crt 10, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:os 14, MASK:pizfs 239, MASK:renderware 146, PTR 312, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 6, VALUE 5687)

| va | ref t0 | ref t1 | run t0 | run t1 | new at t1 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x00604604 | 00000000 | 00000004 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604698 | 00000000 | 00000008 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x0060472c | 00000000 | 0000000c | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x006047c0 | 00000000 | 00000010 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604854 | 00000000 | 00000014 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x006048e8 | 00000000 | 00000018 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x0060497c | 00000000 | 0000001c | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604aa4 | 00000000 | 00000028 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604b38 | 00000000 | 0000002c | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604bcc | 00000000 | 00000030 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604c60 | 00000000 | 00000034 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604cf4 | 00000000 | 00000038 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x00604fd8 | 00000000 | 00000042 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00604000 |
| 0x0060506c | 00000000 | 00000046 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605100 | 00000000 | 0000004a | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605194 | 00000000 | 0000004e | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605228 | 00000000 | 00000052 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x006052bc | 00000000 | 00000056 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605350 | 00000000 | 0000005a | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x0060550c | 00000000 | 00000060 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x006055a0 | 00000000 | 00000064 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605634 | 00000000 | 00000068 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x006056c8 | 00000000 | 0000006c | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x006057f0 | 00000000 | 00000074 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605884 | 00000000 | 00000078 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605ad4 | 00000000 | 0000007f | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x00605c90 | 00000000 | 00000085 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00605000 |
| 0x006147dc | 00493b50 | 00493b40 | 00493b50 | 00493b50 | True | .data 00493b50 | none (abs) | other page 0x00614000 |
| 0x006160d8 | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2bfc | other page 0x00616000 |
| 0x006160dc | 004ad1e0 | 004a5a0c | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c01 | other page 0x00616000 |
| 0x006160e0 | 004ad1e0 | 004a5a71 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c0b | other page 0x00616000 |
| 0x006160e4 | 004ad1e0 | 004a59d0 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c15 | other page 0x00616000 |
| 0x006160e8 | 004ad1e0 | 004a5a57 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c1f | other page 0x00616000 |
| 0x006160ec | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c29 | other page 0x00616000 |
| 0x00616114 | 00000000 | 00000f02 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616118 | 00000000 | 0d78a9d0 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x0061611c | 00000002 | 0000000a | 00000002 | 00000002 | True | .data 00000002 | none (abs) | other page 0x00616000 |
| 0x00616128 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616160 | 00000000 | 00000003 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616168 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616408 | 00000000 | 00773a08 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616410 | 00000000 | 00773a20 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616420 | 00000000 | 00773a38 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616428 | 00000000 | 00773a50 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616438 | 00000000 | 00773a68 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616440 | 00000000 | 00773a80 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616448 | 00000000 | 00773a98 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616458 | 00000000 | 00773ab0 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616460 | 00000000 | 043165c8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616468 | 00000000 | 00773ac8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616470 | 00000000 | 00773ae0 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616478 | 00000000 | 00773af8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616488 | 00000000 | 00773b10 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616490 | 00000000 | 00773b28 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616498 | 00000000 | 00773b40 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x006164a0 | 00000000 | 04316648 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616b90 | 00000001 | 00000002 | 00000001 | 00000001 | True | .data 00000001 | none (abs) | other page 0x00616000 |
| 0x00616f84 | 00000000 | 00000001 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00617f78 | 000000a4 | 000000b4 | 000000a4 | 000000a4 | True | .data 000000a4 | none (abs) | other page 0x00617000 |
| 0x00617f88 | 00000000 | 04311574 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00617000 |
| 0x00617f8c | 00000000 | 043196d4 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00617000 |
| 0x00617fa8 | 00000000 | 04311088 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00617000 |
| 0x00617fac | 00000000 | 043115b0 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00617000 |
| 0x00617ff0 | 00000000 | 04310ae8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00617000 |
| ... 5623 more VALUE dwords (--max-dwords) | | | | | | | | |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| BOOT-WRITE | .data | 1140 | 224 | 0x604604 |
| BOOT-WRITE | .data (bss) | 4395 | 4395 | 0x63bd98 |
| BOOT-WRITE | _rwdseg | 2 | 2 | 0x915000 |
| FILEINIT-MISSING | .data (bss) | 25 | 0 | 0x7032ec |
| OTHER | .data (bss) | 125 | 0 | 0x67d974 |

## Writers (watch.jsonl)
- no watch.jsonl in either run
- run:0x604604: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- run:0x8ad9c0: 0 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame

## Re-run plan (missing evidence)
- ref (original): detorig.py --boot ... --watch 0x604604,0x604698 --watch-from install
- run (exe): MASHED_REPLAY_TRACE_WATCH=0x604604,0x604698
