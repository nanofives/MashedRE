# SCRIBE_QUEUE — batch_ah session 4 (author-only promote-c2)

Queued for the ghidra-sweep / central finalize. Do NOT hand-merge into hooks.csv; the sweep drains
this fragment. No git commit was made by this session — bucket dir + this fragment are left untracked.

## Queued

2026-06-01  ah_s4  bucket=re/analysis/bucket_audio_005ab710_005af040  rvas=0x005ab710,0x005ab980,0x005abe30,0x005ac540,0x005ac650,0x005ac7e0,0x005ac880,0x005aca40,0x005acda0,0x005ace70,0x005ad080,0x005ad0b0,0x005ad2e0,0x005ad320,0x005ad420,0x005ad540,0x005ad570,0x005ad5f0,0x005ad6a0,0x005ad6f0,0x005ad730,0x005ad770,0x005ad8b0,0x005ad8d0,0x005ad910,0x005ad980,0x005ad9d0,0x005ada40,0x005adbe0,0x005adc10,0x005add60,0x005ade60,0x005aded0,0x005adef0,0x005adf60,0x005ae170,0x005ae250,0x005ae300,0x005ae330,0x005ae380,0x005ae3a0,0x005ae470,0x005ae4b0,0x005ae4c0,0x005ae550,0x005ae590,0x005ae7e0,0x005aea60,0x005aeaf0,0x005aeb30,0x005aeb80,0x005aeb90,0x005aed20,0x005aeda0,0x005aeed0,0x005aef30,0x005aefa0,0x005aefc0,0x005af010,0x005af040

## Promotion target

All 60 RVAs: C1 → **C2** (author-only mechanical plate; one `.md` per RVA in the bucket dir).
Drift-skips: NONE (all 60 were `audio,C1,mapped` in hooks.csv at session start; none already C2+).

## subsystem_observed (confirmation / reclassifications)

NO reclassifications. The audio hypothesis held for the entire 0x005ab710..0x005af040 band. The
cluster is the **RenderWare audio engine** internals:
- Async sector-streaming I/O + cache (0xab710, 0xab980, 0xaeed0 poll).
- Codec decode/transcode pump + per-codec frame/sample math (0xabe30, 0xac650, 0xacda0, 0xace70).
- Source-object lifecycle (0xac7e0 factory, 0xac880 dtor, 0xaca40 buffer-set, 0xac540 flag).
- Mixer bus/channel routing (0xad0b0 route-builder, 0xad2e0 release, 0xad320 compat, 0xad420 chan-arrays,
  0xad540/0xad570/0xad5f0/0xad6a0 bus-config lifecycle + lookup).
- Channel-meta records + dispatcher (0xad6f0/0xad730 type-tagged alloc/free, 0xad770/0xad8b0/0xad8d0/
  0xad910/0xad980/0xad9d0/0xada40/0xadbe0/0xadc10/0xadd60).
- Generic intrusive-list utilities used by the audio layer (0xade60, 0xaded0, 0xadef0, 0xadf60).
- Fixed-block arena allocator + in-block first-fit (0xae250/0xae300/0xae330/0xae380/0xae3a0/0xae470/
  0xae4c0/0xae550/0xae590/0xae4b0 free-thunk) and the memory-manager hook table (0xaea60/0xaeaf0).
- Named bus-config singleton (0xaeb30/0xaeb80).
- Audio DSP/util math: linear→dB (0xaeb90), 3-vector length LUT-sqrt (0xaed20), 64-bit fixed mul (0xaeda0).
- Worker-thread / voice-stream management (0xaef30, 0xaefa0, 0xaefc0, 0xaf010, 0xaf040).
- Case-folding bounded compare (0xae170).

One thunk: 0x005ae4b0 is `thunk_FUN_004522d0` (kept name; tail-dispatch through `DAT_007d3ff8+0x10c`,
now confirmed = the free hook installed by FUN_005aea60).

## Uncertainties filed (range U-6700..U-6799)

- U-6700  0xab710  — DAT_007dd6xx streaming-queue globals: audio-exclusive vs shared RW-stream I/O (boundary).
- U-6701  0xab980  — Ghidra-typed void but caller consumes EAX as copied-byte count (return-type gap).
- U-6702  0xac650 / 0xacda0 / 0xace70 — literal values of codec-tag descriptors DAT_005e6414..64b4 + float _DAT_005e64c4.
- U-6703  0xad0b0  — route-table channel index-map: downstream mixer consumer of route[1]/route[2] (boundary).
- U-6704  0xad6f0 / 0xad730 — 5-entry type→size table at DAT_006336f8 + FUN_005ae920 free arg recovery.
- U-6705  0xadc10 / 0xadd60 — 8-dword descriptor field semantics + const table at DAT_00633720 (15 entries).
- U-6706  0xadf60 / 0xae4b0 / 0xaea60 — DAT_007d3ff8 method-table: +0x10c=free RESOLVED; +0xe8 slot unresolved.
- U-6707  0xae170  — asymmetric case-fold predicate (param_2 uses '<B' vs param_1 '>@'); decompiler-vs-asm.
- U-6708  0xae470  — Ghidra-typed void but caller consumes EAX as block pointer (return-type gap).
- U-6709  0xaeb30  — channel-descriptor arrays PTR_DAT_00633900 (14) / PTR_DAT_00633a18 (15) + name DAT_005e65dc.
- U-6710  0xaeb90  — dB conversion float constants _DAT_005cc990 (threshold), _DAT_005cd0b8 (scale); units (millibel?).
- U-6711  0xaed20  — sqrt mantissa LUT DAT_00633b48 + skip value DAT_005d757c.
- U-6712  0xaef30  — thread proc LAB_005aef70 has no function boundary (deeper callee; DEFERRED / function_create candidate).

All uncertainties are data-semantic / boundary / return-type-recovery / unread-const-table — non-blocking
for C2 of these bit-transcribed leaves (behavioral semantics fully captured per plate).

## Stubs filed

NONE. Every callee is a defined function, a kernel32/CRT import (__beginthread, SetThreadPriority,
WaitForSingleObject, ReleaseSemaphore, InterlockedExchange, _malloc, _free), or a runtime-resolved
indirect/vtable dispatch (not a static placeholder).

## Notes for the sweep

- All 60 plates carry frontmatter (rva, name, size_bytes, confidence_target=C2, subsystem_observed,
  callees_depth1, callers_noted, opened_in_slot=Mashed_pool9, session_date) + Mechanical description +
  Constants + Uncertainties + Stubs.
- size_bytes computed from Ghidra body_end - body_start (cited inline in each plate's frontmatter).
- Opened slot Mashed_pool9 read-only via MCP; no master writes performed; program session closed at end.
