# First divergence: o2 (original) vs RO_DEF-fdref2 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (21 blocks wholly masked); ignore: none
- blocks differing at every record: 534; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 0 []
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 536 counted blocks differ, 473 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 536 blocks: 81041 (IMGPTR 10, MASK:audio 187, MASK:audio-glue 1, MASK:crt 56, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:d3dx9 139, MASK:lua 4, MASK:os 17, MASK:qhull 5, MASK:renderware 199, PTR 325, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 5, VALUE 80081)

| va | ref t0 | ref t1 | run t0 | run t1 | new at t1 | file init | static writers | region |
|---|---|---|---|---|---|---|---|---|
| 0x005ea004 | 004a57e4 | 004a57e4 | 00000000 | 00000000 | False | .data 004a57e4 | none (abs) | other page 0x005ea000 |
| 0x005ea008 | 005ca6a0 | 005ca6a0 | 00000000 | 00000000 | False | .data 005ca6a0 | none (abs) | other page 0x005ea000 |
| 0x005ea00c | 005ca6c0 | 005ca6c0 | 00000000 | 00000000 | False | .data 005ca6c0 | none (abs) | other page 0x005ea000 |
| 0x005ea010 | 005ca6e0 | 005ca6e0 | 00000000 | 00000000 | False | .data 005ca6e0 | none (abs) | other page 0x005ea000 |
| 0x005ea014 | 005ca700 | 005ca700 | 00000000 | 00000000 | False | .data 005ca700 | none (abs) | other page 0x005ea000 |
| 0x005ea018 | 005ca720 | 005ca720 | 00000000 | 00000000 | False | .data 005ca720 | none (abs) | other page 0x005ea000 |
| 0x005ea01c | 005ca740 | 005ca740 | 00000000 | 00000000 | False | .data 005ca740 | none (abs) | other page 0x005ea000 |
| 0x005ea020 | 005ca760 | 005ca760 | 00000000 | 00000000 | False | .data 005ca760 | none (abs) | other page 0x005ea000 |
| 0x005ea024 | 005ca780 | 005ca780 | 00000000 | 00000000 | False | .data 005ca780 | none (abs) | other page 0x005ea000 |
| 0x005ea028 | 005ca7a0 | 005ca7a0 | 00000000 | 00000000 | False | .data 005ca7a0 | none (abs) | other page 0x005ea000 |
| 0x005ea02c | 005ca7c0 | 005ca7c0 | 00000000 | 00000000 | False | .data 005ca7c0 | none (abs) | other page 0x005ea000 |
| 0x005ea030 | 005ca7e0 | 005ca7e0 | 00000000 | 00000000 | False | .data 005ca7e0 | none (abs) | other page 0x005ea000 |
| 0x005ea034 | 005ca800 | 005ca800 | 00000000 | 00000000 | False | .data 005ca800 | none (abs) | other page 0x005ea000 |
| 0x005ea040 | 004a40fe | 004a40fe | 00000000 | 00000000 | False | .data 004a40fe | none (abs) | other page 0x005ea000 |
| 0x005ea044 | 004a5e5e | 004a5e5e | 00000000 | 00000000 | False | .data 004a5e5e | none (abs) | other page 0x005ea000 |
| 0x005ea048 | 004a60a6 | 004a60a6 | 00000000 | 00000000 | False | .data 004a60a6 | none (abs) | other page 0x005ea000 |
| 0x005ea04c | 004af2b6 | 004af2b6 | 00000000 | 00000000 | False | .data 004af2b6 | none (abs) | other page 0x005ea000 |
| 0x005ea050 | 005c2a0e | 005c2a0e | 00000000 | 00000000 | False | .data 005c2a0e | none (abs) | other page 0x005ea000 |
| 0x005ea054 | 004af31a | 004af31a | 00000000 | 00000000 | False | .data 004af31a | none (abs) | other page 0x005ea000 |
| 0x005ea060 | 004a5f07 | 004a5f07 | 00000000 | 00000000 | False | .data 004a5f07 | none (abs) | other page 0x005ea000 |
| 0x005ea06c | 004af32d | 004af32d | 00000000 | 00000000 | False | .data 004af32d | none (abs) | other page 0x005ea000 |
| 0x005ea080 | 6e6f7242 | 6e6f7242 | 00000000 | 00000000 | False | .data 6e6f7242 | none (abs) | other page 0x005ea000 |
| 0x005ea084 | 0000657a | 0000657a | 00000000 | 00000000 | False | .data 0000657a | none (abs) | other page 0x005ea000 |
| 0x005ea088 | 766c6953 | 766c6953 | 00000000 | 00000000 | False | .data 766c6953 | none (abs) | other page 0x005ea000 |
| 0x005ea08c | 00007265 | 00007265 | 00000000 | 00000000 | False | .data 00007265 | none (abs) | other page 0x005ea000 |
| 0x005ea090 | 646c6f47 | 646c6f47 | 00000000 | 00000000 | False | .data 646c6f47 | none (abs) | other page 0x005ea000 |
| 0x005ea098 | 00000001 | 00000001 | 00000000 | 00000000 | False | .data 00000001 | 0x00499400 VideoSettingsDispatcher [save] @0x00499679 | other page 0x005ea000 |
| 0x005ea09c | 00010000 | 00010000 | 00000000 | 00000000 | False | .data 00010000 | none (abs) | other page 0x005ea000 |
| 0x005ea0a0 | 00000002 | 00000002 | 00000000 | 00000000 | False | .data 00000002 | none (abs) | other page 0x005ea000 |
| 0x005ea0a4 | 00030002 | 00030002 | 00000000 | 00000000 | False | .data 00030002 | none (abs) | other page 0x005ea000 |
| 0x005ea0a8 | 3f800000 | 3f800000 | 00000000 | 00000000 | False | .data 3f800000 | 0x004039f0 FUN_004039f0 [util] @0x00403a32,0x00403a54; 0x004046a0 FUN_004046a0 [util] @0x004047fd | other page 0x005ea000 |
| 0x005ea0ac | 41f00000 | 41f00000 | 00000000 | 00000000 | False | .data 41f00000 | 0x004046a0 FUN_004046a0 [util] @0x00404807 | other page 0x005ea000 |
| 0x005ea0b0 | ffffffff | ffffffff | 00000000 | 00000000 | False | .data ffffffff | 0x004039f0 FUN_004039f0 [util] @0x00403ad7; 0x004046a0 FUN_004046a0 [util] @0x0040480c | other page 0x005ea000 |
| 0x005ea0b4 | ffffffff | ffffffff | 00000000 | 00000000 | False | .data ffffffff | 0x004046a0 FUN_004046a0 [util] @0x004046cd,0x004047f7 | other page 0x005ea000 |
| 0x005ea0b8 | 005cc8d4 | 005cc8d4 | 00000000 | 00000000 | False | .data 005cc8d4 | none (abs) | other page 0x005ea000 |
| 0x005ea0bc | 005cc8c8 | 005cc8c8 | 00000000 | 00000000 | False | .data 005cc8c8 | none (abs) | other page 0x005ea000 |
| 0x005ea0c0 | 005cc8bc | 005cc8bc | 00000000 | 00000000 | False | .data 005cc8bc | none (abs) | other page 0x005ea000 |
| 0x005ea0c4 | 005cc8b0 | 005cc8b0 | 00000000 | 00000000 | False | .data 005cc8b0 | none (abs) | other page 0x005ea000 |
| 0x005ea0c8 | 005cc8a4 | 005cc8a4 | 00000000 | 00000000 | False | .data 005cc8a4 | none (abs) | other page 0x005ea000 |
| 0x005ea0cc | 005cc894 | 005cc894 | 00000000 | 00000000 | False | .data 005cc894 | none (abs) | other page 0x005ea000 |
| 0x005ea0d0 | 005cc884 | 005cc884 | 00000000 | 00000000 | False | .data 005cc884 | none (abs) | other page 0x005ea000 |
| 0x005ea0d4 | 005cc878 | 005cc878 | 00000000 | 00000000 | False | .data 005cc878 | none (abs) | other page 0x005ea000 |
| 0x005ea0d8 | 005cc868 | 005cc868 | 00000000 | 00000000 | False | .data 005cc868 | none (abs) | other page 0x005ea000 |
| 0x005ea0dc | 005cc858 | 005cc858 | 00000000 | 00000000 | False | .data 005cc858 | none (abs) | other page 0x005ea000 |
| 0x005ea0e0 | 005cc848 | 005cc848 | 00000000 | 00000000 | False | .data 005cc848 | none (abs) | other page 0x005ea000 |
| 0x005ea0e4 | 005cc838 | 005cc838 | 00000000 | 00000000 | False | .data 005cc838 | none (abs) | other page 0x005ea000 |
| 0x005ea0e8 | 005cc82c | 005cc82c | 00000000 | 00000000 | False | .data 005cc82c | none (abs) | other page 0x005ea000 |
| 0x005ea0ec | 005cc81c | 005cc81c | 00000000 | 00000000 | False | .data 005cc81c | none (abs) | other page 0x005ea000 |
| 0x005ea0f0 | 005cc80c | 005cc80c | 00000000 | 00000000 | False | .data 005cc80c | none (abs) | other page 0x005ea000 |
| 0x005ea0f4 | 005cc800 | 005cc800 | 00000000 | 00000000 | False | .data 005cc800 | none (abs) | other page 0x005ea000 |
| 0x005ea0f8 | 005cc7f4 | 005cc7f4 | 00000000 | 00000000 | False | .data 005cc7f4 | none (abs) | other page 0x005ea000 |
| 0x005ea0fc | 005cc7e4 | 005cc7e4 | 00000000 | 00000000 | False | .data 005cc7e4 | none (abs) | other page 0x005ea000 |
| 0x005ea100 | 005cc7d4 | 005cc7d4 | 00000000 | 00000000 | False | .data 005cc7d4 | none (abs) | other page 0x005ea000 |
| 0x005ea104 | 005cc7c8 | 005cc7c8 | 00000000 | 00000000 | False | .data 005cc7c8 | none (abs) | other page 0x005ea000 |
| 0x005ea108 | 005cc7bc | 005cc7bc | 00000000 | 00000000 | False | .data 005cc7bc | none (abs) | other page 0x005ea000 |
| 0x005ea10c | 005cc7ac | 005cc7ac | 00000000 | 00000000 | False | .data 005cc7ac | none (abs) | other page 0x005ea000 |
| 0x005ea110 | 005cc79c | 005cc79c | 00000000 | 00000000 | False | .data 005cc79c | none (abs) | other page 0x005ea000 |
| 0x005ea114 | 005cc790 | 005cc790 | 00000000 | 00000000 | False | .data 005cc790 | none (abs) | other page 0x005ea000 |
| 0x005ea118 | 005cc784 | 005cc784 | 00000000 | 00000000 | False | .data 005cc784 | none (abs) | other page 0x005ea000 |
| 0x005ea11c | 005cc774 | 005cc774 | 00000000 | 00000000 | False | .data 005cc774 | none (abs) | other page 0x005ea000 |
| 0x005ea120 | 005cc764 | 005cc764 | 00000000 | 00000000 | False | .data 005cc764 | none (abs) | other page 0x005ea000 |
| 0x005ea124 | 005cc758 | 005cc758 | 00000000 | 00000000 | False | .data 005cc758 | none (abs) | other page 0x005ea000 |
| 0x005ea128 | ff0000ff | ff0000ff | 00000000 | 00000000 | False | .data ff0000ff | none (abs) | other page 0x005ea000 |
| 0x005ea12c | 80000040 | 80000040 | 00000000 | 00000000 | False | .data 80000040 | none (abs) | other page 0x005ea000 |
| ... 80017 more VALUE dwords (--max-dwords) | | | | | | | | |

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| BOOT-WRITE | .data | 224 | 224 | 0x604604 |
| BOOT-WRITE | .data (bss) | 31663 | 31663 | 0x63bd98 |
| BOOT-WRITE | _rwdseg | 2 | 2 | 0x915000 |
| FILEINIT-MISSING | .data | 47577 | 47576 | 0x5ea004 |
| FILEINIT-MISSING | .data (bss) | 25 | 0 | 0x7032ec |
| OTHER | .data | 465 | 465 | 0x6147dc |
| OTHER | .data (bss) | 125 | 0 | 0x67d974 |

## Writers (watch.jsonl)
- ref:?: 1 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
- ref:0x604604: 1 hits, 1 in the frame tick 0 -> 1 (tick == 0); last writer pc 0x45d6a8 (0x0045d460 FUN_0045d460 [audio]) at tick 0 value 0x4, bt 0x1aff74|0x45d6a8|0x4669fb|0x4028e0|0x8aa300|0xa3040d89|0xd89008a|0x8aa308
- run:0x7f0ff0: 603 hits, 1 in the frame tick 0 -> 1 (rec == 0); last writer pc 0x2a9b74 (LargeRangeBytes+0x4) at tick 0 value 0x0, bt 0x46741
- ref:0x898c00: 530 hits, 0 in the frame tick 0 -> 1 (tick == 0); last writer none at or before that frame
