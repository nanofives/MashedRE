# SCRIBE_QUEUE fragment — batch_ah session 3 (author-only promote-c2)

## Queued

2026-06-01  ah_s3  bucket=bucket_audio_005a7b60_005ab620  rvas=005a7b60,005a7e70,005a7ea0,005a7ee0,005a7f70,005a8060,005a8390,005a8560,005a86a0,005a8750,005a8890,005a8960,005a89a0,005a89b0,005a89c0,005a89d0,005a8e70,005a8f30,005a9280,005a9ab0,005a9be0,005a9c50,005a9d00,005a9de0,005a9e40,005a9e60,005a9f10,005a9f60,005a9ff0,005aa030,005aa060,005aa1e0,005aa200,005aa330,005aa3d0,005aa7a0,005aa8a0,005aa900,005aa9c0,005aaa00,005aaac0,005aab00,005aab70,005aabe0,005aacf0,005aad40,005aadc0,005aaff0,005ab010,005ab070,005ab090,005ab150,005ab180,005ab190,005ab1c0,005ab2f0,005ab340,005ab370,005ab560,005ab620

## Session metadata

- **Bucket dir:** `re/analysis/bucket_audio_005a7b60_005ab620/` (60 plate `.md` files, one per RVA; untracked — central finalize commits).
- **Pool slot:** Mashed_pool8 (read-only, opened OK first try; no lock leak; program_close clean).
- **Confidence target:** C2 (author-only — no re-classify/hooks.csv edits performed; central finalize promotes C1→C2).
- **U-ID range used:** U-6600..U-6648 (49 filed; range allotment U-6600..U-6699).
- **S-ID range:** S-5200..S-5299 — **none filed** (no new stubs; existing stubs S-0109/S-0111/S-0984/S-0986/S-1364/S-3683/S-3684/S-3685 noted as cleared because their callees are now real/C2 functions).

## Subsystem confirmation (audio hypothesis held for all 60)

All 60 RVAs **confirmed `audio`** — no reclassification to shader-compiler / csl-pipeline / third-party-library / other. Sub-clusters observed:
- **RWS chunk parser / object model** (005a7b60 chunk-tree parser reading RWS IDs 0x80a/0x80c/0x802; 005a7e70/005a7ea0/005a7ee0 list visitor/dealloc/init around `DAT_007dca7c`).
- **Stream/voice manager + mixer worker thread** (005a8060 manager init + thread spawn; 005a8390/005a8560/005a86a0/005a8750/005a8890/005a8960/005a89d0 voice ops guarded by semaphore `DAT_007dcae0` around list `DAT_007dcad8`; 005a89a0/005a89b0/005a89c0 getters).
- **3D spatializer** (005a9280: distance attenuation + doppler + directional pan matrix).
- **Voice/channel alloc/free/reset tree** (005a9ab0/005a9be0/005a9c50/005a9d00/005a9de0/005a9e40/005a9e60/005a9f10/005a9f60/005a9ff0/005aa030/005aa060/005aa1e0; pools `DAT_007dcc9c`/`DAT_007dcbf4`/`DAT_007dcc18`).
- **Bus-array routing** (005aa200/005aa330/005aa3d0/005aa900/005aa9c0; entry stride 0x18, default cb `FUN_004d7ff0`).
- **Global-list teardown/walk + ref-count** (005aaa00/005aaac0/005aab00/005aab70/005aabe0/005aacf0/005aa8a0; list `DAT_007dccf0`, root getter 005aad40 = `DAT_007DCD20`).
- **Audio timer** (005aaff0/005ab010/005ab070 around `DAT_007dcdf0`/`DAT_007dcdf8`/`DAT_007dce00`).
- **Streaming-audio source async I/O** (005ab090/005ab150/005ab180/005ab190/005ab1c0/005ab2f0/005ab340/005ab370/005ab560/005ab620): request submit/seek/cancel scheduler under locks `DAT_007dd618`/`DAT_007dd620`, queue `DAT_007dd624`. **Cross-cut note:** these call into the file-stream manager (`FUN_00550670`/`FUN_00550910`/`FUN_005509b0`/`FUN_00550bd0` in the 0x0055xxxx band) and use 0x800 sector alignment — reached from the audio streaming-source path, so kept `audio`, but the manager itself is shared I/O infra (candidate separate subsystem if later split).

## Flags for the central finalize / master-Ghidra sweep

1. **005aa1e0 needs `function_create` at master** — listing-level LAB inline comparator, no Ghidra function envelope; `function_create` was rejected here (read-only slot). Transcribed from disassembly instead (plate has full asm). **Correction:** hooks.csv's existing note says the callback wraps `FUN_005ade10`; the **literal** CALL target is **`FUN_005adf30`** (verified from rel32 `0x00003d3f` @0x005aa1f1). Body = `return FUN_005adf30(*node, ctx) == 0 ? 1 : 0;` (NEG/SBB/INC inverted-boolean). See U-6625.
2. **Other LAB_ callbacks lacking envelopes** (data-referenced; create at master if desired): LAB_005a8460, LAB_005a8680, LAB_005a84d0 (005a8060); LAB_005a91b0 (005a8f30); LAB_005a8640 (005a8560); LAB_005aa180 (005a9ff0); LAB_005aa8d0 (005aa8a0); LAB_005aac00 (005aabe0); LAB_005ab100 (005ab090/2f0/340 completion cb). See U-6604/6607/6615/6624/6629/6634/6640.
3. **005ab180** is a 4-byte JMP thunk to **005ab620** (`thunk_FUN_005ab620`); `decomp_function` on 0x005ab180 resolves to 005ab620's body. Plate kept thunk-only; full body documented under 005ab620.

Scribe outcome: queued for sweep (see this fragment, row ah_s3). Do NOT git commit — files left untracked for central finalize.
