# First divergence: ref1 (original) vs RO_ROLOAD-pr1 (exe)

- records compared: 600, block 1024 B, 3245 blocks, 2304 raw words; tick counters aligned: True
- masks: re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv (163 blocks wholly masked); ignore: 0x00614400-0x00614800,0x00771800-0x00771c00
- blocks differing at every record: 122; explained by mask / rule / PTR / IMGPTR dwords at tick 1 (both runs dumped): 5 ['0x5f3400', '0x5f9800', '0x603400', '0x632800', '0x90d800']
- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N 17 of 600, first NEW block 0x898c00 at record 17
- **first divergent record 0** (tick 1)
- reporting tick 301 (record 300) as asked
- at record 300 (tick 301): 503 counted blocks differ, 96 raw words (non-PTR, unmasked)
- counted blocks differing at records 0..15: [118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118, 118]

## Dumps
- ref tick 300: missing
- ref tick 301: OK (ckpt205.data.bin)
- run tick 300: missing
- run tick 301: OK (ckpt05.data.bin)

## Writers (watch.jsonl)
- no watch.jsonl in either run

## Re-run plan (missing evidence)
- ref (original): detorig.py --boot ... --ckpt-ticks 300,301
- run (exe): MASHED_REPLAY_TRACE_CKPT=300,301
- ref (original): detorig.py --boot ... --watch 0x5f2400 --watch-from install
- run (exe): MASHED_REPLAY_TRACE_WATCH=0x5f2400
