# First divergence: ref1 (original) vs RO_ROLOADRW-pr1 (exe)

- records compared: 283, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (163 blocks wholly masked); ignore: 0x00614400-0x00614800,0x00771800-0x00771c00
- blocks differing at every record: 121; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 5 ['0x5f3400', '0x5f9800', '0x603400', '0x632800', '0x90d800']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 16 of 283, first NEW block 0x67e800 at record 16
- **first divergent record 0** (tick 1)
- at record 0 (tick 1): 117 counted blocks differ, 76 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117, 117]

## Dumps
- ref tick 0: no-record (tick 0) (ckpt200.data.bin)
- ref tick 1: OK (ckpt201.data.bin)
- run tick 0: no-record (tick 0) (ckpt00.data.bin)
- run tick 1: OK (ckpt01.data.bin)

## Dwords differing at tick 1 in the 117 blocks: 5642 (IMGPTR 105, MASK:audio 166, MASK:crt 10, MASK:crt-clock 2, MASK:crt-cookie 1, MASK:os 15, MASK:pizfs 243, MASK:renderware 146, PTR 81, RULE:FILEPTR 1, RULE:HWND 4, RULE:RING4 4, RULE:WINMSG 6, VALUE 4858)

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
