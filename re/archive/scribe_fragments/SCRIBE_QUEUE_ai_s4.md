# SCRIBE_QUEUE — batch_ai session 4 (author-only promote-c2)

Queued for the ghidra-sweep / central finalize. Do NOT hand-merge into hooks.csv; the sweep drains
this fragment. No git commit was made by this session — bucket dir + this fragment are left untracked.

## Queued

2026-06-02  ai_s4  bucket=re/analysis/bucket_audio_005c98d0_005cb0ef  rvas=0x005c98d0,0x005c9d10,0x005ca2d0,0x005ca2f0,0x005ca2f8,0x005ca300,0x005ca320,0x005ca340,0x005ca360,0x005ca368,0x005ca380,0x005ca3a0,0x005ca3a8,0x005ca3b3,0x005ca3d0,0x005ca3d8,0x005ca3e3,0x005ca3ee,0x005ca3f9,0x005ca404,0x005ca412,0x005ca430,0x005ca438,0x005ca450,0x005ca470,0x005ca478,0x005ca490,0x005ca498,0x005ca4a0,0x005ca4a8,0x005ca4c0,0x005ca4c8,0x005ca4e0,0x005ca500,0x005ca520,0x005ca540,0x005ca560,0x005ca580,0x005ca5a0,0x005ca5c0,0x005ca5c8,0x005ca5e0,0x005ca600,0x005ca620,0x005ca640,0x005ca680,0x005cb000,0x005cb07f,0x005cb0ef

## Promotion target

All 49 RVAs: C1 → **C2** (author-only mechanical plate; one `.md` per RVA in the bucket dir).
Drift-skips: NONE (all 49 were `audio,C1,mapped` in hooks.csv at session start; none already C2+).
No `function_create` was needed: every RVA already has a function boundary (FUN_/named/Unwind@).

## subsystem_observed (confirmation / RECLASSIFICATIONS)

**This is a MIXED / vendored bucket — 47 of 49 reclassify OUT of `audio`.** The hooks.csv comment band
already flagged it: `MULTI[audio-dll-mixer, rw-v3d-vector, msvc-seh-funclets, crt-fiddb]`. Confirmed:

- **audio (HELD, 2):**
  - 0x005c98d0 FUN_005c98d0 — voice-mixer accumulate stage (dry L/R + reverb-bus, mono/stereo, ramp interp). Pure leaf.
  - 0x005c9d10 FUN_005c9d10 — voice resampler / sample-fetch (32.32 fixed-point pitch, linear interp; calls CRT 64-bit helpers).
  - Both are driven by FUN_005c7ee0; they are the per-voice DSP core of the RenderWare audio mixer.

- **third-party-library[msvc-crt] (RECLASS audio→3rd-party, 1):**
  - 0x005ca2d0 __aullshr — MSVC CRT 64-bit unsigned right-shift intrinsic (Ghidra FidDB single-match "Library: Visual Studio").

- **third-party-library[msvc-seh-funclets] (RECLASS audio→3rd-party, 43):**
  - 0x005ca2f0,0x005ca2f8,0x005ca300,0x005ca320,0x005ca340,0x005ca360,0x005ca368,0x005ca380,0x005ca3a0,
    0x005ca3a8,0x005ca3b3,0x005ca3d0,0x005ca3d8,0x005ca3e3,0x005ca3ee,0x005ca3f9,0x005ca404,0x005ca412,
    0x005ca430,0x005ca438,0x005ca450,0x005ca470,0x005ca478,0x005ca490,0x005ca498,0x005ca4a0,0x005ca4a8,
    0x005ca4c0,0x005ca4c8,0x005ca4e0,0x005ca500,0x005ca520,0x005ca540,0x005ca560,0x005ca580,0x005ca5a0,
    0x005ca5c0,0x005ca5c8,0x005ca5e0,0x005ca600,0x005ca620,0x005ca640,0x005ca680
  - All are Ghidra `Unwind@` funclets: compiler-emitted C++ exception-unwind glue. Two forms:
    (a) tail-JMP form — load object ptr into ECX (`LEA ECX,[EBP±off]` or `MOV ECX,[EBP-0x10]` + optional
        `ADD ECX,<subobject-off>`), then `JMP <dtor-body>`; (b) helper-call form (0x005ca300, 0x005ca438,
        0x005ca5e0) — `PUSH frame; CALL 0x004a3bda; POP ECX; RET` (MSVC local-unwind/dtor-dispatch helper).
  - They are referenced only from the C++ EH `FuncInfo`/scope-table data in `.rdata` (`MOV EAX,0x5e7ba8`..
    `0x5e7f88` handler funnels through `0x004a3cb3`), driving unwind of the audio mixer functions above.
    Per policy (vendored-library family) these are shallow-plated, not deep-plated.

- **third-party-library[renderware] (RECLASS audio→3rd-party, 3):**
  - 0x005cb000 RwV3dTransformPoints — array point transform by 4x3 RwMatrix (rotate + translate column).
  - 0x005cb07f RwV3dTransformVectors — array direction-vector transform (rotate only, no translate column).
  - 0x005cb0ef RwV3dTransformPoint — single-point form of RwV3dTransformPoints.
  - RenderWare 3.x core V3d math (already canonically named in the project); shallow-plated by policy.

Net for the audio C1 drain: only 0x005c98d0 + 0x005c9d10 count as genuine audio C2; the other 47 should
be retagged to their `third-party-library[...]` subsystem by central finalize (they were never audio).

## Uncertainties filed (range U-7300..U-7399)

- U-7300  0x005c9d10 — exact rounding of the x87 `float10` whole-sample estimate at the `__aullrem` site
  (Ghidra `ROUND` intrinsic). Mechanical only; per-sample interpolation math is fully captured. Non-blocking.

No other uncertainties: the funclets, __aullshr, and RwV3dTransform* are fully transcribed and vendored.

## Stubs filed (range S-5900..S-5999)

NONE (no static placeholders). Every callee is a defined function or a CRT helper:
- Mixer/resampler callees: __allmul (0x004a4220), __aullrem (0x004aa060), __aullshr (0x005ca2d0).
- Funclet tail-call destructor/cleanup bodies (callees-not-yet-reversed, but all defined functions; noted
  as [STUB] in each plate for traceability, not as STUBS.md rows): 0x004a1790, 0x0049c6d0, 0x004a01b0,
  0x004a1180, 0x0049c6a0, 0x0049f300, 0x0049eeb0, 0x004a2ae0, 0x0049f090, 0x004a26f0, and the
  local-unwind helper 0x004a3bda.
