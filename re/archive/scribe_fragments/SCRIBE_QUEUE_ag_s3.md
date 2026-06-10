# SCRIBE_QUEUE fragment — batch_ag Session 3 (util C1->C2 author-only)

One row for central finalize to cat into `re/SCRIBE_QUEUE.md` (Queued section).
Format follows `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern".

AUTHOR-ONLY: 60 per-RVA C2 plates written to the bucket dir below. NO hooks.csv /
re-classify / ghidra-sweep / build / Frida / commit performed (central finalize does that).
Per the session prompt's hard constraint, files are left UNTRACKED (no git commit).

## Queued

2026-06-01  batch-ag-s3  bucket=re/analysis/bucket_util_0042f7a0_004764e0  rvas=0042f7a0,00430820,00430b00,00431b30,00431b50,00431b60,00431d70,00432290,0043dfd0,00441700,00441760,004417e0,00441820,00441990,00441c80,00441d40,004425d0,00442600,004427c0,00442a20,00442a60,00442e00,004430a0,004430b0,00445aa0,004464c0,00446520,0044e0a0,00458bf0,00459560,0045bba0,0045bfe0,0045c350,0045c480,0045c810,0045d1e0,0045d330,0045d3a0,0045d3f0,0045d430,0045d7a0,0045db50,0045dbe0,0045df70,0045dfc0,004657b0,00466b50,0046b4f0,0046baa0,0046cb30,00472640,00472650,00472690,00472820,004728a0,00475c10,004762c0,00476430,00476440,004764e0  level=c1->c2-authored  pool=Mashed_pool8  pool-assigned=Mashed_pool8  pool-slot-file=.pool_slot_ag_s3  mcp_session=fd03f14db4e84696b6e9f3034600d504  count=60/60  note=All 60 were C1/util in hooks.csv at start (no drift-skips; verified via grep). Each plate = frontmatter (rva,name,ghidra_name,size_bytes,confidence_target=C2,callees_depth1,callers_noted,opened_in_slot=Mashed_pool8,session_date) + mechanical description + constants table + uncertainties + stubs. No LAB_ entries in this range -> no function_create needed. ONE plate (0x0043dfd0) is flagged HOLD-at-C1 (see below) — central re-classify should NOT auto-flip it to C2.

### function_create (LAB_ -> function boundary)
- NONE. All 60 RVAs already had function boundaries (`function_at` non-null for every one). No writes made to the pool8 clone.

### HOLD-at-C1 (do NOT flip C1->C2 in central pass)
- **0x0043dfd0** — large frame/tick state machine (Ghidra body 0x0043dfd0..0x00440aa9 = 2777 bytes). Its own inline C1 note self-rates the analysis **C0** because the decomp is 63,977 chars / ~30,839 tokens and exceeds the MCP read buffer. Plate documents the readable structure (entry sync, DAT_0067eca4 phase machine {1,2,3,4}, camera-angle clamps DAT_007f0ef0/DAT_007f0eec, command-stream walk with -0xf?0000 sentinels, uVar10=FUN_0042ac90() opcode dispatch on 0xff??0000 codes) but the dispatch tail is NOT fully read. U-6100 filed. Recommend a dedicated single-function chunked-decomp session (server-side dump like inject_shape_decomp.py) before promotion. **subsystem_observed = frontend** (menu/screen state machine), not util.

### subsystem_observed reclass candidates (hooks.csv has all as util)
Reported as observations only; no frontmatter hard-flips (kept util to minimize blast radius — central re-classify decides). The 0x00441xxx..0x00446xxx range is a coherent **camera/spectator** cluster:
- **render/camera** (strong, mechanical evidence = camera-matrix/FOV/spectator math): 0x00441700 (FOV setup), 0x00441760 (RW-frame apply), 0x004417e0 (eye-vec3 accessor), 0x00441820 (camera-path sample), 0x00441990 (camera aim), 0x00441c80 (pos lerp), 0x00441d40 (camera smoothing), 0x004425d0 (camera-entry array clear), 0x00442600 (per-track camera-path init), 0x004427c0 (spectator selection w/ hysteresis), 0x00442a20 (camera init+matrix), 0x00442a60 (spectator distance metric), 0x00442e00 (scripted/animated camera), 0x00445aa0 (type-0 path camera follow), 0x004464c0 (camera-entry dispatcher), 0x00446520 (master camera director, 7411B), 0x0045d7a0 (replay/attract camera frame build).
- **render** (particle/effects): 0x00475c10 (particle aging), 0x00476440 (timed-effect update+flush), 0x00476430 (thunk->FUN_00475ef0 emitter-array init), 0x004762c0 (random unit vector on sphere), 0x0044e0a0 (conditional effect spawn).
- **render/track** (per-frame ticks, large): 0x0045bba0 (entity/prop update pass), 0x00466b50 (replay/attract/track tick, 1076B), 0x0045c350 (spline/path node lookup).
- **candidate audio**: 0x004657b0 (per-channel sound/state pump; FUN_005a89*/FUN_005a8750 are audio-engine queries), 0x0045dbe0 (pushes a clamped value to FUN_004943f0, audio-related).
- **genuine util** (confirmed): 0x00472650 (RNG float-lerp; 66 callers), 0x00472690 (RNG int-range), 0x00472640/004430a0/004430b0/0042f7a0/00430820/00431d70 (scalar getters/setters), 0x00472820/004728a0 (vec3/matrix validity checks), 0x0046b4f0/0046baa0/0046cb30 (per-player table read/init/offset-get), 0x004764e0 (arg-swap forwarder), 0x00431b30/00431b50/00431b60 (scalar/trig leaves), 0x00432290 (predicate), 0x0045db50/0045df70/0045dfc0/0045c810 (clamped-step/counter helpers), 0x0045d3f0/0045d430 (one-shot enable/disable latch pair).

### tracker reconciliations to apply during sweep (reported per NO-GUESSING, decomp-literal)
- **0x00472650** and **0x00472690** already carry inline **[C2 2026-05-18]** scribe comments (RNG lerp / RNG int) while hooks.csv still lists them C1 — these flip C1->C2 cleanly with no further work.
- **0x00476430** is `thunk_FUN_00475ef0` with Ghidra `thunk: true` (4-byte JMP). Flag for thunk reclass/rename in the sweep; NOT renamed in the author pass per batch rule. The decompiled body shown is the JMP target FUN_00475ef0 (emitter-array init).
- **0x0045bba0** — Ghidra reports body span 0x00453f60..0x0045be81 while the entry point is 0x0045bba0 (body precedes entry => non-contiguous / over-merged address set). U-6101 filed. Behavior fully read and C2-ready; recommend a function-boundary review (function_body_set / split) in the master during the sweep. size_bytes left as a descriptive string in the plate rather than a guessed integer.
- Naming note: hooks.csv carries project names for several rows (GhostMode::Clear, Course::GetLeaderIndex, Camera::*, CameraPath::*, Spectator::*, Bezier::*, TimeDisplay::SetEntry, CameraEntry::DispatchAll, Player::GetOffset3D) that are NOT present in the pool8 slot (slot shows FUN_/sub_/thunk_). Plates record both (name = hooks.csv project name, ghidra_name = slot symbol). No renames made.

### U-IDs minted this session (range U-6100..U-6199)
- U-6100: 0x0043dfd0 full mechanical transcription INCOMPLETE (decomp 63,977 chars exceeds MCP read buffer; dispatch tail + exit paths unread). Blocks C2 promotion -> HOLD at C1.
- U-6101: 0x0045bba0 Ghidra body-extent ambiguity (body 0x00453f60..0x0045be81 precedes entry 0x0045bba0; non-contiguous). Behavior unaffected; blocks a clean size_bytes only.
(Only 2 U-IDs minted; all other plate uncertainties were data-semantic global/struct meaning, which is non-blocking for behaviorally-verified leaves per project lesson.)

### S-IDs minted this session (range S-4600..S-4699)
- S-4600..S-4636 (37 stubs) — each names the out-of-bucket depth-1 callees not reversed in this author pass (RW math FUN_004c*, PRNG FUN_00534870, player accessors FUN_0046*/FUN_0040e*, audio FUN_005a8*, etc.). See individual plates' `## Stubs` sections for the per-RVA mapping.
