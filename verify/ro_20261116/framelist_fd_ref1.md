### RO_ROIMG-pr1
# First divergence: ref1 (original) vs RO_ROIMG-pr1 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (163 blocks wholly masked); ignore: 0x00614400-0x00614800,0x00771800-0x00771c00
- blocks differing at every record: 122; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 5 ['0x5f3400', '0x5f9800', '0x603400', '0x632800', '0x90d800']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 118 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 118 blocks: 6049 (IMGPTR 104, MASK:audio 166, MASK:crt 10, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:os 15, MASK:pizfs 243, MASK:renderware 146, PTR 72, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 6, VALUE 5275)

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
| 0x006160d8 | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2bfc | other page 0x00616000 |
| 0x006160dc | 004ad1e0 | 004a5a0c | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c01 | other page 0x00616000 |
| 0x006160e0 | 004ad1e0 | 004a5a71 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c0b | other page 0x00616000 |
| 0x006160e4 | 004ad1e0 | 004a59d0 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c15 | other page 0x00616000 |
| 0x006160e8 | 004ad1e0 | 004a5a57 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c1f | other page 0x00616000 |
| 0x006160ec | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c29 | other page 0x00616000 |
| 0x00616114 | 00000000 | 00000f02 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616118 | 00000000 | 0485afe8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x0061611c | 00000002 | 0000000a | 00000002 | 00000002 | True | .data 00000002 | none (abs) | other page 0x00616000 |
| 0x00616128 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616160 | 00000000 | 00000003 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616168 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616408 | 00000000 | 00773a08 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
### RO_ROLOAD-pr1
# First divergence: ref1 (original) vs RO_ROLOAD-pr1 (exe)

- records compared: 300, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (163 blocks wholly masked); ignore: 0x00614400-0x00614800,0x00771800-0x00771c00
- blocks differing at every record: 122; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 5 ['0x5f3400', '0x5f9800', '0x603400', '0x632800', '0x90d800']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 300, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 118 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 118 blocks: 6043 (IMGPTR 104, MASK:audio 166, MASK:crt 10, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:os 15, MASK:pizfs 243, MASK:renderware 146, PTR 72, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 6, VALUE 5269)

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
| 0x006160d8 | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2bfc | other page 0x00616000 |
| 0x006160dc | 004ad1e0 | 004a5a0c | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c01 | other page 0x00616000 |
| 0x006160e0 | 004ad1e0 | 004a5a71 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c0b | other page 0x00616000 |
| 0x006160e4 | 004ad1e0 | 004a59d0 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c15 | other page 0x00616000 |
| 0x006160e8 | 004ad1e0 | 004a5a57 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c1f | other page 0x00616000 |
| 0x006160ec | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c29 | other page 0x00616000 |
| 0x00616114 | 00000000 | 00000f02 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616118 | 00000000 | 0485afe8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x0061611c | 00000002 | 0000000a | 00000002 | 00000002 | True | .data 00000002 | none (abs) | other page 0x00616000 |
| 0x00616128 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616160 | 00000000 | 00000003 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616168 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616408 | 00000000 | 00773a08 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
### RO_ROLOADC-pr1
# First divergence: ref1 (original) vs RO_ROLOADC-pr1 (exe)

- records compared: 283, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (163 blocks wholly masked); ignore: 0x00614400-0x00614800,0x00771800-0x00771c00
- blocks differing at every record: 122; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 5 ['0x5f3400', '0x5f9800', '0x603400', '0x632800', '0x90d800']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 283, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 118 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 118 blocks: 6043 (IMGPTR 104, MASK:audio 166, MASK:crt 10, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:os 15, MASK:pizfs 243, MASK:renderware 146, PTR 72, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 6, VALUE 5269)

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
| 0x006160d8 | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2bfc | other page 0x00616000 |
| 0x006160dc | 004ad1e0 | 004a5a0c | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c01 | other page 0x00616000 |
| 0x006160e0 | 004ad1e0 | 004a5a71 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c0b | other page 0x00616000 |
| 0x006160e4 | 004ad1e0 | 004a59d0 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c15 | other page 0x00616000 |
| 0x006160e8 | 004ad1e0 | 004a5a57 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c1f | other page 0x00616000 |
| 0x006160ec | 004ad1e0 | 004a5d92 | 004ad1e0 | 004ad1e0 | True | .data 004ad1e0 | 0x004a2bf7 FUN_004a2bf7 [boot] @0x004a2c29 | other page 0x00616000 |
| 0x00616114 | 00000000 | 00000f02 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616118 | 00000000 | 0485afe8 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x0061611c | 00000002 | 0000000a | 00000002 | 00000002 | True | .data 00000002 | none (abs) | other page 0x00616000 |
| 0x00616128 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616160 | 00000000 | 00000003 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616168 | 00000000 | 00001000 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
| 0x00616408 | 00000000 | 00773a08 | 00000000 | 00000000 | True | .data 00000000 | none (abs) | other page 0x00616000 |
