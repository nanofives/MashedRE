# SCRIBE_QUEUE — batch_ah session 6 (author-only promote-c2)

Queued for the ghidra-sweep / central finalize. Do NOT hand-merge into hooks.csv; the sweep drains
this fragment. No git commit was made by this session — bucket dir + this fragment + `.pool_slot_ah_s6`
are left untracked.

## Queued

2026-06-01  ah_s6  bucket=re/analysis/bucket_audio_005b2220_005b8570  confidence=C1->C2  rvas=0x005b2220,0x005b2820,0x005b29e0,0x005b2b00,0x005b2de0,0x005b2fd0,0x005b3020,0x005b30e0,0x005b3300,0x005b34f0,0x005b3540,0x005b3550,0x005b3580,0x005b35a0,0x005b35e0,0x005b3620,0x005b3670,0x005b36b0,0x005b36f0,0x005b3760,0x005b3a00,0x005b3b30,0x005b3b60,0x005b3b80,0x005b3e60,0x005b4060,0x005b4150,0x005b41f0,0x005b4850,0x005b51d0,0x005b53b0,0x005b5860,0x005b5e00,0x005b5f70,0x005b6260,0x005b66c0,0x005b6710,0x005b6760,0x005b6820,0x005b6910,0x005b6a40,0x005b6b00,0x005b6be0,0x005b7010,0x005b70e0,0x005b7110,0x005b73b0,0x005b73e0,0x005b7430,0x005b74c0,0x005b7880,0x005b7980,0x005b7b30,0x005b7e20,0x005b8040,0x005b8080,0x005b8090,0x005b8130,0x005b81d0,0x005b8570

## Promotion target

All 60 RVAs: C1 → **C2** (author-only mechanical plate; one `.md` per RVA in the bucket dir).
Drift-skips: NONE (all 60 were `audio,C1` in hooks.csv at session start; none already C2+; six live
under `audio_rws_loader_d3/`, one under `audio_sfx_dispatch_d3/`, the rest under `bucket_005a5020/`
as prior C1 plates — fresh C2 plates were authored from live decompilation this pass).

## subsystem_observed (confirmation / reclassifications)

NO hard reclassifications — the audio hypothesis held for the entire 0x005b2220..0x005b8570 band.
This is the **RenderWare RWS-audio engine's stream + codec pipeline** (tagged `audio`, consistent
with batch ah_s4's treatment of the adjacent 0x005ab710..0x005af040 band):
- **Semaphore-gated ring-buffer streaming backend** (shared context [[U-6900]], embedded at the
  stream object's `+0x5c`): sample-copy/convert pump (0x2220), claim/release/recycle slot
  (0x2820/0x29e0), decode-refill loop (0x2b00), seek/wrap (0x30e0), position projectors
  (0x2fd0/0x3020), one-shot arm/kick (0x2de0), stop/flush+drain (0x3300).
- **Codec/factory registrars** into two singletons: DAT_007ddb04 (0x34f0 register / 0x3540 teardown)
  and DAT_007ddd2c (0x73e0 register / 0x7430 teardown).
- **Format-conversion graph router**: converter-table search (0x36f0), recursive DFS chain finder
  with 5-slot LRU cache (0x3760), dry-run size driver (0x3a00) + wrappers (0x3b30/0x3b60), and the
  buffered multi-stage executor (0x3b80).
- **PCM converters** (converter-node transform fns, reached via node[1] — callers=0): channel
  mixdown mono↔stereo (0x3e60), output-size formula (0x4060), offset projector (0x4150), linear
  resampler (0x41f0), bit-depth/sign converter (0x4850).
- **Three ADPCM codecs**: EA-XA (header 0x51d0, encoder 0x53b0, decoder 0x5860, 14-sample/8-byte,
  big-endian, 8 coeffs); a 28-sample/16-byte variant (dispatcher 0x5e00, inner encoder 0x5f70,
  decoder 0x6260, 5 coeffs); standard IMA (dispatch 0x6710, mono dec 0x6760, mono enc 0x6820,
  stereo dec 0x6910, per-nibble enc 0x6a40 / dec 0x6b00, step table DAT_00634498). Channel
  dispatch 0x66c0.
- **Stream/codec object lifecycle**: codec sink-object ctor (0x6be0) + its handlers (0x7010 vcall
  fwd, 0x70e0 descriptor init, 0x7110 output-slot reserve); full audio-stream object ctor (0x74c0)
  + destructor (0x7880), tick state-machine (0x7980), completion callback (0x7b30), stop helper
  (0x7e20).
- **Async-IO request ring** (header [[U-6924]], semaphore at +0xc): init (0x8040), close (0x8080),
  enqueue (0x8090), dequeue (0x8130), reset (0x81d0).
- **Voice-node ctor wrapper** (0x8570 → FUN_005aa560(&DAT_007dde70,...); D-7385).

**Flagged for the sweep (subsystem boundary — NOT reclassified, deferring to central finalize):**
- 0x005b3580 / 0x005b35a0 / 0x005b35e0 / 0x005b3620 / 0x005b3670 / 0x005b36b0 — generic intrusive
  doubly-linked-list primitives (init / push-back / remove / find-by-key / find-by-field /
  index-by-position). And 0x005b3550 — generic list unlink+destroy. These are container utilities,
  but their only callers are inside the audio module (0x005ad*/0x005aa* range). Candidate
  `util`/`container` reclass; left `audio` pending the sweep's call.
- 0x005b73b0 — find-file-extension string helper (fully generic); only audio-module callers.
  Candidate `util` reclass; left `audio`.

## Uncertainties filed (range U-6900..U-6925)

All are data-semantic / layout / const-not-read / return-type-recovery — **non-blocking** for C2 of
these bit-transcribed functions (behavioral semantics fully captured per plate).

- U-6900  shared streaming context struct (param_1, offsets 0xec..0x18c); embedded at stream obj +0x5c.
- U-6901  0x2220 element-width enum (alignment-mask buckets → copy granularity), no named format table.
- U-6902  0x2b00 return-code semantics {0,2,3,4}.
- U-6903  0x2de0 &LAB_005b2e70 continuation tail (inside fn body, not a separate routine).
- U-6904  DAT_007ddb04 codec/factory registry singleton #1 + tables PTR_DAT_00634290/00634330.
- U-6905  0x3550 node struct (links +0x20/+0x24, prev-slot +4).
- U-6906  intrusive-list header/node layout (count[0], sentinel[1], tail[2]; embedded link at obj+0x20).
- U-6907  PTR_PTR_00634550 converter graph (0x33 paired ptrs) + LRU cache DAT_007ddb30 (stride 0x4c, 5 slots).
- U-6908  converter node flag (node+2)&1 / node[8]&1 (channel-invariant marker).
- U-6909  converter transform-fn ABI (6 args; out-buf NULL → size query, set → transform).
- U-6910  audio format-descriptor struct (rate@+0, size@+8, bits@+0xc, channels@+0xd, flags@+0x18).
- U-6911  _DAT_005cc320 float unit/1.0 phase-wrap const (resampler); literal not read.
- U-6912  EA-XA ADPCM coefficient tables DAT_00634438 / DAT_0063443c.
- U-6913  codec float scale/bias const family (_DAT_005cc9d0/005ce9fc/005ce9f0/005e6ccc/cd0/cd4/cd8/cdc/005dae8c).
- U-6914  codec name/FourCC keys DAT_005e6414 / DAT_005e6424 (compared via FUN_005adf30); strings not read.
- U-6915  second-codec coeff tables (encode-float DAT_006343e8/006343ec; decode-int DAT_00634410/00634414).
- U-6916  second-codec float consts DAT_005d757c / _DAT_005cc9b8 + block format-tag map (tag 2 vs EA-XA).
- U-6917  IMA step table DAT_00634498 + index-adjust table DAT_00634478.
- U-6918  codec sink-object handler-slot layout (+0x28/+0x2c/+0x30/+0x34/+0x3c, config +0x40/+0x44).
- U-6919  CPU-feature/variant lookup table DAT_00617ff8 + feature IDs (FUN_005aee20(0x28)).
- U-6920  0x6a40 Ghidra-typed void vs caller-consumed EAX 4-bit code (calling-convention recovery gap).
- U-6921  DAT_007ddd2c codec/factory registry singleton #2 + tables PTR_DAT_00634760/006347c8.
- U-6922  codec-output object layout (0x7110: arrays +0x94/+0x98/+0x9c, descriptor table +0xa4, ring +0x8c, record table +0x78).
- U-6923  audio-stream object layout (play-state +0x48, buf-state +0x50, sub-obj +0x54/+0x58, ring +0x5c, +0x94).
- U-6924  async-IO ring header (buffer@+0, capacity ushort@+4, head/tail/full @+6/+8/+0xa, semaphore@+0xc, entry stride 0x14).
- U-6925  DAT_007dde70 global audio object/manager singleton (0x8570 prepends as this/context to FUN_005aa560).

## Stubs filed (range S-5500..S-5531) — one S-ID per distinct out-of-bucket unreversed callee

S-5500 FUN_005aeda0 (mod/div byte-budget); S-5501 FUN_005ab370 (acquire sub-obj); S-5502 FUN_005ab1c0
(submit sub-obj); S-5503 thunk_FUN_005ab620 (sub-obj release); S-5504 FUN_005ad570 (registry init);
S-5505 FUN_005ad540 (register key); S-5506 FUN_005ad420 (install tables); S-5507 FUN_005ad5f0
(deregister); S-5508 FUN_005ae030 (object dtor); S-5509 FUN_004522d0 (free, CRT band); S-5510
FUN_005adf30 (key comparator); S-5511 FUN_005ac9e0 (fmt comparator); S-5512 FUN_005ac980 (build fmt
key); S-5513 FUN_005ac5f0 (fmt-key match); S-5514 FUN_005aea50 (scratch alloc); S-5515 FUN_005ac740
(release fmt/header buf); S-5516 FUN_005aea00 (fixed-size alloc); S-5517 FUN_005c9d00 (stereo decode
path); S-5518 FUN_005a9e10 (base obj init); S-5519 FUN_005aee20 (feature-bit query); S-5520
FUN_004d7ff0 (installed handler +0x30); S-5521 FUN_005b0f40 (base handler/dtor chain); S-5522
FUN_005b0f10 (descriptor base init); S-5523 FUN_005b1080 (output-buf alloc); S-5524 FUN_005ad2e0
(release sub-obj +0x58); S-5525 thunk_FUN_005b1500 (destruct ctx +0x5c); S-5526 FUN_005b0ec0
(state-change notify); S-5527 FUN_005b2080 (ring pump/advance); S-5528 FUN_005b0f90 (downstream
notify); S-5529 FUN_005b11d0 (return node to pool); S-5530 FUN_005aeea0 (semaphore create); S-5531
FUN_005aa560 (delegated ctor/registrar).

In-bucket callees (NOT stubs — plated this session) include: FUN_005b8090/005b30e0/005b3020/005b8090,
005b2820/29e0/2b00/2fd0, 005b36f0/3760/3a00, 005b4150/4060, 005b5f70/6a40/6b00, 005b7880/7980/7010/
70e0, 005b3300/81d0, 005b29e0, 005b3a00 — cross-references are noted per-plate.

## Notes for the sweep

- All 60 plates carry frontmatter (rva, name, size_bytes, confidence_target=C2, subsystem_observed,
  callees_depth1, callers_noted, opened_in_slot=Mashed_pool14, session_date=2026-06-01) +
  `## Mechanical description` + `## Constants` + `## Uncertainties` + `## Stubs`.
- size_bytes computed from Ghidra `body_end - body_start` (cited inline per plate).
- Slot **Mashed_pool14** opened read-only via MCP (top-level `mashed_pool/Mashed_pool14`, no on-disk
  lock; the stale lock in the nested `mashed_pool/Mashed_pool14/` dir is a separate older project and
  was not touched). No `ghidra_pool acquire` was run (slot pre-assigned). No master writes; program
  session closed at end. Actual slot recorded in `.pool_slot_ah_s6`.
- No `function_create` was needed: all 60 RVAs were already defined FUN_ functions (none LAB_).
- No STOP-AND-ASK conditions triggered (no null function_at, no vendored-library family — the codec
  is the game's own RWS-audio implementation, not a separable vendored lib like qhull).
- Files left UNTRACKED (bucket dir + this fragment + `.pool_slot_ah_s6`). No git commit, no
  re-classify, no build, no Frida — per author-only mission.
