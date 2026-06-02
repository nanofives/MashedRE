# SCRIBE_QUEUE fragment — batch_ai session 2 (ai_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  ai_s2  bucket=re/analysis/bucket_audio_005bcbb0_005bf470  confidence=C1->C2  rvas=005bcbb0,005bcbe0,005bcc70,005bcca0,005bcd10,005bcd80,005bcda0,005bce80,005bd500,005bd610,005bd6f0,005bd9e0,005bdad0,005bdc70,005bde50,005bde70,005bdef0,005bdf40,005bdff0,005be080,005be0f0,005be160,005be1b0,005be260,005be450,005be540,005be5f0,005be630,005be720,005be8c0,005be930,005be940,005be990,005be9f0,005beaa0,005beae0,005beb50,005beba0,005bec10,005bedc0,005bef20,005bf000,005bf050,005bf0c0,005bf160,005bf230,005bf350,005bf370,005bf470

## Notes for the sweep

- **Count**: 49 RVAs, 49 plates authored in the bucket dir. None drift-skipped
  (all were `audio,C1` in hooks.csv at session start; none already C2+).
- **Subsystem confirmation**: all 49 CONFIRMED `audio` (no reclassifications).
  This is the **RenderWare Audio DirectShow filter pair** — `RwaDSSource` and
  `RwaDSRenderer` — verified by the embedded filter/pin name strings
  `u_RwaDSSource_00635838`, `u_RwaDSRenderer_0063581c`, `u_Output_006358d0`,
  `u_Input_00635a1c`. The code is app/middleware-authored over Microsoft
  DirectShow base-class shapes (CBaseFilter / CBasePin / IEnumMediaTypes /
  IMediaSample / IMemAllocator), so it is deep-plated as `audio`, NOT
  third-party-library. The prompt's "audio" hypothesis holds for the whole
  bucket.
- **Sub-families identified** (cross-referenced across plates):
  1. *SPSC semaphore ring buffer of COM ptrs* — ctor 005bcbe0, dtor 005bcc70,
     push 005bcca0, pop 005bcd10, drain 005bcd80, occupancy 005bcda0; layout
     `+0x00`write-idx `+0x04`read-idx `+0x08`cap `+0x0c`buf `+0x10`semaphore
     `+0x14`flags(bit0=owns-buf). 005bcbb0 = media-type format-block free.
  2. *RwaDSRenderer streaming core* — filter ctor 005bce80, per-tick pumps
     005bd6f0 / 005bdad0, secondary init 005bd9e0, pipe-flush 005bdc70, recursive
     pin-graph walk 005bd500, conditional global-COM release 005bd610. Uses
     DirectShow REFERENCE_TIME math (scale 10000000).
  3. *Media-buffer descriptor* (5-dword: base/pos/len/bound-obj/mode-tag; mode
     0=empty 1=IMediaSample 2=raw) — zero 005bde50, bind 005bde70, release
     005bdef0, copy/mover 005bdf40, set-discontinuity 005bdff0, test-discontinuity
     005be080.
  4. *COM node / IEnumMediaTypes enumerator* — node ctor 005be0f0, node teardown
     005be160, QI 005be1b0, enum ctor 005be8c0, cursor-reset 005be930, QI
     005be940, enum dtor 005be990, Next 005be9f0, Clone 005beaa0.
  5. *CBaseFilter base* — alloc 005beae0, mt-bookkeeping-zero 005beb50, multi-IID
     QI 005beba0, base dtor 005bec10, connected-pin setter 005bedc0, mt-copy
     005bef20, QueryId/CoTaskMem-string 005bf000, output-pin broadcast 005bf050
     (no-arg) / 005bf0c0 (6-arg).
  6. *RwaDSSource filter* — ctor 005be260 (0x378, internal worker thread), dtor
     005be450, GetDeliveryBuffer 005be540, enqueue+signal 005be5f0, peer-pin probe
     005be630, stack-COM service-register helper 005be720.
  7. *RwaDSRenderer filter* — ctor 005bf230 (0x340), dtor 005bf370, ring-pop gate
     005bf350, stack-COM service-register helper 005bf470, enumerator search
     005bf160.
- **No function_create performed**: every candidate was already a proper `FUN_`
  (no LAB_ candidate in the list). Several **LAB_ method handlers are referenced
  and flagged for promotion** by the sweep (filed as stubs): LAB_005bcef0 +
  LAB_005bd640 (installed at filter +0x28/+0x34 by 005bce80, S-5703); LAB_005be870
  / LAB_005be790 / LAB_005be4d0 (RwaDSSource pin + worker-thread handlers via
  005be260, S-5712; LAB_005be4d0 is the worker thread proc); LAB_005bdd00
  (callback passed to 005bd500 by 005bdc70, S-5705); plus PTR_LAB_006359f8 /
  PTR_LAB_006359b0 vtables (005bf230) and PTR_LAB_006358a4 (005be260).
- **New uncertainties filed** (range U-7100..U-7140, all NON-BLOCKING for C2 of
  these bit-identical leaves — data/struct/calling-convention semantics, not
  behavioral). Notable:
  - U-7114 — the 5-dword media-buffer descriptor layout + mode-tag enumeration
    (shared by 005bde50/de70/def0/df40/dff0/be080).
  - U-7120 / U-7138 — RwaDSSource (0x378) / RwaDSRenderer (0x340) object layouts.
  - U-7109 / U-7111 — pump field-block semantics (core +0x6dc..+0x7a8).
  - Register-stitched / decompiler-dropped-argument cases handled per the
    STOP-AND-ASK "[UNCERTAIN] + U-ID, continue" rule (NOT halted): U-7108
    (005bd500 callback/ctx args), U-7121 (005be540 GetDeliveryBuffer params),
    U-7124 (005be630 out-slot), U-7125 / U-7140 (005be720 / 005bf470 stack-COM
    helpers), U-7137 (005bf160 enumerator search), U-7139 (005bf350 dropped ring
    arg), U-7138 (005bf230 in_stack/unaff ctor args).
- **New stubs minted** (range S-5700..S-5722) for not-yet-reversed callees,
  chiefly the 0x5ae* custom pool/thread/sync band (FUN_005ae400/005ae650/
  005ae7e0/005ae800/005ae920/005aea00/005aeea0/005aef00/005aef30), FUN_004522d0
  (free), FUN_005adf30 (GUID compare), FUN_005a9e10 (base ctor), the RWS-audio
  core FUN_005b0ec0/005b0f10/005b1080/005b1180, the RwaDS helper band
  FUN_005bc920/005bf710/005bf7d0/005bfd10, and string helpers FUN_005c2f0a/
  005c2f58. Several callees are ALREADY classified and were NOT re-stubbed:
  FUN_005bf610 (C2), FUN_005bf690 (C2), FUN_005be190 (C3 AudioRwsSubZeroInit).
  Leave stub resolution to the central sweep.
- **size_bytes** computed as `body_end - body_start + 1` from each decomp
  (verified against the known 42-byte plate for 005bcbb0).
- Files left UNTRACKED (bucket dir + this fragment + .pool_slot_ai_s2). No git
  commit, no re-classify, no build, no Frida — per author-only mission. Program
  opened read/write in Mashed_pool1 and left for central finalize to close.
