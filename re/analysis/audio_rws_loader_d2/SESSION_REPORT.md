---
session: audio_rws_loader_d2
session_id: audio_rws_loader_d2-20260503-0600
date: 2026-05-03
slot_used: Mashed_pool4
slot_requested: Mashed_pool5
reason_for_slot_change: Mashed_pool5 had a Ghidra-internal lock from a concurrent session; fell back to Mashed_pool4 (unlocked, no lock~ present).
anchor: MASHED.exe size=2846720 SHA-256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E OK
---

## Work loop summary

Processed all 21 DEFERRED rows from D-0340..D-0360 (depth-2 callees from audio_rws_loader-cont1 session).
No cap (cap_count=0). Full set completed.

## Functions analyzed

| D-ID | RVA | Short description |
|---|---|---|
| D-0340 | 0x004d7ff0 | identity error-code pass-through (5 bytes) |
| D-0341 | 0x004d8480 | last-RWS-error record setter; guard: empty slot; globals DAT_007d6c5c/DAT_007d3ff8 |
| D-0342 | 0x00550950 | RWS stream read dispatch; vtable+0x30; fread-style |
| D-0343 | 0x00550af0 | RWS stream EOF/status check; vtable+0x44; indirect-jump unrecovered |
| D-0344 | 0x005509b0 | RWS stream seek; whence map 0→1/1→2/2→3; buffer-state invalidation |
| D-0345 | 0x005a7a40 | pool-object list searcher; FUN_005ade60 at obj+0x0c; returns obj or NULL |
| D-0346 | 0x005ade90 | doubly-linked list drainer → FUN_005ae920(DAT_009146c0) per node |
| D-0347 | 0x005a7ea0 | audio object destructor; +0x1b flags route to pool or heap |
| D-0348 | 0x005ae0c0 | 16-byte format descriptor copy/endian-swap via FUN_005aeca0 |
| D-0349 | 0x005ae010 | sub-struct A (audio_obj+0x24) init; FUN_005ae080 zero-init then set field |
| D-0350 | 0x005adfe0 | sub-struct B (audio_obj+0x34) init; FUN_005ae050 zero-init then set field |
| D-0351 | 0x005ac740 | wave_node sub-struct partial cleanup; conditional free at +0x10; zero +0x10/+0x14 |
| D-0352 | 0x005a7e70 | visitor over circular audio-object list DAT_007dca7c; calls FUN_005ade10 per node |
| D-0353 | 0x005ae030 | combined zero-init for sub-structs A+B via FUN_005ae080+FUN_005ae050 |
| D-0354 | 0x005abcb0 | wave_node destructor; +0x54 flags route to sub-obj pool or fallback DAT_007dd634 |
| D-0355 | 0x005ac210 | wave_node factory; parses RWS 0x803 chunk; allocs via FUN_005aea00(size,0x30806) |
| D-0356 | 0x005adf30 | 16-byte format memcmp; returns -1/0/+1; leaf |
| D-0357 | 0x005aec30 | PCM endian-swap in-place; bswap16 or bswap32; leaf |
| D-0358 | 0x005abd30 | PCM chunk feeder; vtable+0x14 feed_pcm; double-buffered streaming via FUN_005abf80 |
| D-0359 | 0x005abf80 | audio drain/pump vtable+0x1c dispatch; indirect-jump unrecovered |
| D-0360 | 0x005ae920 | pool-allocator free; bitmap-tracked fixed-size blocks; optional compaction |

## Tracker changes

- **hooks.csv**: +21 rows (all C1/MAPPED; subsystem=audio; file=re/analysis/audio_rws_loader_d2/*)
- **STUBS.md**: -21 rows (S-0101..S-0121 removed); +18 rows (S-0980..S-0997 for depth-3 callees)
- **DEFERRED.md**: +18 rows (D-2860..D-2877; depth-3 callees; bucket audio_rws_loader_d2-cont1)
- **UNCERTAINTIES.md**: +12 rows (U-0987..U-0998)

## Depth-3 DEFERRED (D-2860..D-2877)

18 new functions queued. Key ones:
- D-2860 005ade60: list searcher in pool objects
- D-2861 005aeca0: format-field pack/byte-swap helper
- D-2862 005ae080: sub-struct A zero-init
- D-2863 005ae050: sub-struct B zero-init
- D-2864 005aa060: DirectSound buffer creator
- D-2867 005aba20: wave_node field initialiser (large, 0x182 bytes)
- D-2871 005aca80: get RWS chunk data size
- D-2872 005acaa0: parse RWS chunk header (12-byte id+size+version)
- D-2874 005ac5f0: wave-state validator
- D-2877 005b3b80: fill streaming/loop buffer (large, 0x2d6 bytes)

## Key structural findings

1. **RWS stream vtable layout confirmed**: read=+0x30, seek=+0x38, eof=+0x44 relative to vtable ptr at stream+0x38.
2. **Audio object allocation flags at +0x1b**: bit0=pool-alloc (→DAT_007dca84), bit2=no-free; absence → heap-free via FUN_004522d0.
3. **Wave_node allocation flags at +0x54**: same pattern but uses sub-object pool at +0x0c+0x3c or fallback DAT_007dd634.
4. **Pool allocator FUN_005ae920**: bitmap-tracked fixed-size block allocator; optional compaction (param_1[6] bit1).
5. **FUN_005ac210**: central wave_node factory; parses RWS 0x803 chunk structure (nested chunk headers), handles endian-swap, allocates via FUN_005aea00(size, 0x30806).
6. **FUN_005aec30**: in-place PCM byte-swap (bswap16/bswap32) — confirms big-endian source (RWS on PS2/Xbox → PC little-endian).
7. **Two vtable-style unrecovered jumps** (FUN_00550af0 at +0x44, FUN_005abf80 at +0x1c): both marked U-0989/U-0998; return values likely int but suppressed by decompiler.

## Queue

Next pickup: `audio_rws_loader_d2-cont1` — decompile D-2860..D-2877 (18 functions).
Priority: D-2867 (005aba20, large wave_node init), D-2872 (005acaa0, RWS chunk header parser), D-2877 (005b3b80, PCM streaming feeder).
