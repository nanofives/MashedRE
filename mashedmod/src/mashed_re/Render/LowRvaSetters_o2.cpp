// Mashed RE — Render low-RVA global-setter reimplementations (c3-batch-o session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00409680  LedArrayInit        — 19B; fills 1152 floats of -1.0f at DAT_0063a5f0
//   0x004053d0  TrackDataSlotSet    — 44B; 3-global setter (if param_1 != 0)
//   0x00423630  AiDataBufInit       — 52B; fills 2x8192 dwords of 0xffffffff + 3 zero clears
//   0x00425ab0  EntryHeaderClear    — 87B; clears 2 dwords per entry across 8 entries at stride 0x4c
//   0x004235b0  AiPizLoad          — 116B; opens ai.piz, reads record 0x13269902 to DAT_007f1a9c
//
// Analysis notes:
//   re/analysis/promote_c2_render_lowrva/00409680.md
//   re/analysis/promote_c2_render_lowrva/004053d0.md
//   re/analysis/promote_c2_render_lowrva/00423630.md
//   re/analysis/promote_c2_render_lowrva/00425ab0.md
//   re/analysis/promote_c2_render_lowrva/004235b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (AiPizLoad only; others are leaves)
// ---------------------------------------------------------------------------

// 0x00423480  FUN_00423480 (C2) — AI path filename generator
// Formats "AI%%d.AI" and appends to DAT_0064410c path buffer; returns DAT_00644110.
static auto* const s_FUN_00423480 =
    reinterpret_cast<void(__cdecl*)(void)>(0x00423480);

// 0x00495280  FUN_00495280 (C2) — piz open wrapper
// FUN_00402b70 builds path; FUN_004b6570 opens piz; logs OK/FAILED.
static auto* const s_FUN_00495280 =
    reinterpret_cast<void(__cdecl*)(const char*)>(0x00495280);

// 0x004cc230  FUN_004cc230 (C2) — RwStreamOpen equivalent
// Opens a stream handle given (type, mode, &out_buf).
static auto* const s_FUN_004cc230 =
    reinterpret_cast<int(__cdecl*)(int, int, void*)>(0x004cc230);

// 0x004cc5e0  FUN_004cc5e0 (C2) — RwStreamFindChunk
// Scans stream for chunk matching type; outputs size+version; returns 1=found 0=not.
static auto* const s_FUN_004cc5e0 =
    reinterpret_cast<int(__cdecl*)(int, std::uint32_t, void*, void*)>(0x004cc5e0);

// 0x004cbd30  FUN_004cbd30 (C2) — RwStreamRead
// Reads bytes from stream handle into buffer.
static auto* const s_FUN_004cbd30 =
    reinterpret_cast<void(__cdecl*)(int, void*, std::uint32_t)>(0x004cbd30);

// 0x004cc160  FUN_004cc160 (C2) — RwStreamClose equivalent
// Closes stream context (case 1=noop 2=file-close 3=mem-writeback 4=cb-close).
static auto* const s_FUN_004cc160 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x004cc160);

// 0x004952f0  FUN_004952f0 (C2) — piz close wrapper
// Logs then calls thunk_FUN_004b67a0 to close piz.
static auto* const s_FUN_004952f0 =
    reinterpret_cast<void(__cdecl*)(void)>(0x004952f0);

// ---------------------------------------------------------------------------
// LedArrayInit  --  0x00409680
//
// Original: FUN_00409680 (19 bytes, 0x00409680..0x00409693)
// Signature: void FUN_00409680(void)
// Returns: void
//
// Body (memset-like loop cited from 0x00409680):
//   puVar2 = &DAT_0063a5f0;
//   for (iVar1 = 0x480; iVar1 != 0; iVar1 += -1) {
//       *puVar2 = 0xbf800000;   // -1.0f IEEE-754
//       puVar2 += 1;
//   }
//
// Constants (cited from 0x00409680 body):
//   0x0063a5f0 — base of filled region
//   0x480 (1152) — dword write count
//   0xbf800000   — IEEE-754 single -1.0f fill value
//
// Uncertainties (non-blocking):
//   U-3210: sentinel array purpose (1152-float grid paired with led.piz loader).
//
// Callers: FUN_00426e10 (track load) — twice (unconditional before FUN_00409710
//   and conditional reset if FUN_00409710 returns 0).
//
// ref: re/analysis/promote_c2_render_lowrva/00409680.md
// ---------------------------------------------------------------------------

// 0x00409680
extern "C" __declspec(dllexport) void __cdecl LedArrayInit(void)
{
    // Fill 0x480 (1152) consecutive 4-byte words with 0xbf800000 (-1.0f) at DAT_0063a5f0.
    // Count 0x480 and base 0x0063a5f0 cited at 0x00409680 body.
    std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(0x0063a5f0u);
    for (int iVar1 = 0x480; iVar1 != 0; iVar1 += -1) {
        *puVar2 = 0xbf800000u;  // -1.0f IEEE-754 [fill sentinel]
        puVar2 += 1;
    }
}

RH_ScopedInstall(LedArrayInit, 0x00409680);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// TrackDataSlotSet  --  0x004053d0
//
// Original: FUN_004053d0 (44 bytes, 0x004053d0..0x004053fc)
// Signature: void FUN_004053d0(int param_1, undefined4 param_2)
// Returns: void
//
// Body (cited from 0x004053d0):
//   if (param_1 != 0) {
//       DAT_00639d74 = 0;
//       DAT_00639d70 = param_1;
//       DAT_00639d78 = param_2;
//       return;
//   }
//   DAT_00639d70 = 0;
//   DAT_00639d78 = 0;
//   return;
//
// Note: asymmetric branch — DAT_00639d74 is cleared only when param_1 != 0.
//
// Constants (cited from 0x004053d0 body):
//   0x00639d70 — "primary" slot (set to param_1 or 0)
//   0x00639d74 — "flag" slot (cleared to 0 only when param_1 != 0)
//   0x00639d78 — "payload" slot (set to param_2 or 0)
//
// Uncertainties (non-blocking):
//   U-3209: semantics of the triple (DAT_00639d70/74/78); asymmetric else-clear.
//
// Caller: FUN_00426e10 at end of track load with (DAT_00657448, FUN_004671a0(0)).
//
// ref: re/analysis/promote_c2_render_lowrva/004053d0.md
// ---------------------------------------------------------------------------

// 0x004053d0
extern "C" __declspec(dllexport) void __cdecl TrackDataSlotSet(
    int param_1, std::uint32_t param_2)
{
    if (param_1 != 0) {
        // if-branch: clear flag, store primary+payload. [0x004053d0 body, if-branch]
        *reinterpret_cast<std::uint32_t*>(0x00639d74u) = 0u;         // DAT_00639d74 = 0
        *reinterpret_cast<std::uint32_t*>(0x00639d70u) = static_cast<std::uint32_t>(param_1); // DAT_00639d70 = param_1
        *reinterpret_cast<std::uint32_t*>(0x00639d78u) = param_2;    // DAT_00639d78 = param_2
        return;
    }
    // else-branch: clear primary+payload; DAT_00639d74 is NOT cleared. [0x004053d0 else]
    *reinterpret_cast<std::uint32_t*>(0x00639d70u) = 0u;  // DAT_00639d70 = 0
    *reinterpret_cast<std::uint32_t*>(0x00639d78u) = 0u;  // DAT_00639d78 = 0
}

RH_ScopedInstall(TrackDataSlotSet, 0x004053d0);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// AiDataBufInit  --  0x00423630
//
// Original: FUN_00423630 (52 bytes, 0x00423630..0x00423664)
// Signature: void FUN_00423630(void)
// Returns: void
//
// Body (cited from 0x00423630):
//   DAT_00801a9c = 0;
//   fill DAT_007f1a9c with 0x2000 dwords of 0xffffffff;
//   fill DAT_007f9a9c with 0x2000 dwords of 0xffffffff;
//   DAT_007f1a54 = 0;
//   DAT_007f1a64 = 0;
//
// Constants (cited from 0x00423630 body):
//   0x00801a9c — adjacent dword cleared to 0
//   0x007f1a9c — first AI data buffer base (32 KB = 8192 dwords)
//   0x007f9a9c — second AI buffer base (same 32 KB)
//   0x2000 (8192) — dword fill count per buffer
//   0xffffffff   — sentinel fill value
//   0x007f1a54 — header/state slot cleared (-0x48 from buffer base)
//   0x007f1a64 — header/state slot cleared (-0x38 from buffer base)
//
// Uncertainties (non-blocking):
//   U-3216: second 32 KB buffer at DAT_007f9a9c purpose (double-buffer for AI update).
//
// Paired with AiPizLoad (FUN_004235b0) which loads ai.piz into DAT_007f1a9c.
//
// ref: re/analysis/promote_c2_render_lowrva/00423630.md
// ---------------------------------------------------------------------------

// 0x00423630
extern "C" __declspec(dllexport) void __cdecl AiDataBufInit(void)
{
    // Step 1: clear adjacent dword. [0x00423630 body, first write]
    *reinterpret_cast<std::uint32_t*>(0x00801a9cu) = 0u;

    // Step 2: fill first AI buffer (DAT_007f1a9c) with 0x2000 dwords of 0xffffffff.
    // Count 0x2000 and base 0x007f1a9c cited at 0x00423630.
    std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(0x007f1a9cu);
    for (int iVar1 = 0x2000; iVar1 != 0; iVar1 += -1) {
        *puVar2 = 0xffffffffu;
        puVar2 = puVar2 + 1;
    }

    // Step 3: fill second AI buffer (DAT_007f9a9c) with 0x2000 dwords of 0xffffffff.
    // Base 0x007f9a9c cited at 0x00423630.
    puVar2 = reinterpret_cast<std::uint32_t*>(0x007f9a9cu);
    for (int iVar1 = 0x2000; iVar1 != 0; iVar1 += -1) {
        *puVar2 = 0xffffffffu;
        puVar2 = puVar2 + 1;
    }

    // Step 4: clear two header/state slots. [0x00423630 body, last writes]
    *reinterpret_cast<std::uint32_t*>(0x007f1a54u) = 0u;  // DAT_007f1a54
    *reinterpret_cast<std::uint32_t*>(0x007f1a64u) = 0u;  // DAT_007f1a64
}

RH_ScopedInstall(AiDataBufInit, 0x00423630);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// EntryHeaderClear  --  0x00425ab0
//
// Original: FUN_00425ab0 (87 bytes, 0x00425ab0..0x00425b07)
// Signature: void FUN_00425ab0(void)
// Returns: void
//
// Body (hand-unrolled; cited from 0x00425ab0):
//   Clear 2 dwords at entry-relative offsets +0x3C/+0x40 for each of 8 entries,
//   starting at DAT_008992a0, stride 0x4c (76) bytes per entry.
//
//   DAT_008992a0 = 0;  DAT_008992a4 = 0;   // entry 0
//   DAT_008992ec = 0;  DAT_008992f0 = 0;   // entry 1 (+0x4c)
//   _DAT_00899338 = 0; DAT_0089933c = 0;   // entry 2 (+0x98)
//   _DAT_00899384 = 0; DAT_00899388 = 0;   // entry 3 (+0xe4)
//   _DAT_008993d0 = 0; DAT_008993d4 = 0;   // entry 4 (+0x130)
//   _DAT_0089941c = 0; DAT_00899420 = 0;   // entry 5 (+0x17c)
//   _DAT_00899468 = 0; DAT_0089946c = 0;   // entry 6 (+0x1c8)
//   _DAT_008994b4 = 0; DAT_008994b8 = 0;   // entry 7 (+0x214)
//
// Constants (cited from 0x00425ab0):
//   0x008992a0 — first clear address (entry 0, field +0x3C within per-entry struct)
//   0x4c (76)  — stride between entries
//   8          — entry count
//
// Semantics: zeroes the "enabled" guard fields (entry+0x3C and entry+0x40) for all 8
//   entries. FUN_00425e40 checks piVar3[0x10] (entry+0x40) as the trigger guard.
//
// Uncertainties: see shared struct doc _shared_898992_struct.md (non-blocking).
//
// Called from FUN_00426e10 before FUN_004262f0.
//
// ref: re/analysis/promote_c2_render_lowrva/00425ab0.md
// ---------------------------------------------------------------------------

// 0x00425ab0
extern "C" __declspec(dllexport) void __cdecl EntryHeaderClear(void)
{
    // Clears 2 dwords (entry+0x3C and entry+0x40) for 8 entries, stride 0x4c.
    // Base 0x008992a0, stride 0x4c, count 8 — all cited from 0x00425ab0 body.
    static const unsigned BASE = 0x008992a0u;
    static const unsigned STRIDE = 0x4cu;
    static const int COUNT = 8;

    for (int i = 0; i < COUNT; i++) {
        unsigned entry_base = BASE + static_cast<unsigned>(i) * STRIDE;
        *reinterpret_cast<std::uint32_t*>(entry_base)      = 0u;  // entry+0x00 (field +0x3C from struct base)
        *reinterpret_cast<std::uint32_t*>(entry_base + 4u) = 0u;  // entry+0x04 (field +0x40 from struct base)
    }
}

RH_ScopedInstall(EntryHeaderClear, 0x00425ab0);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// AiPizLoad  --  0x004235b0
//
// Original: FUN_004235b0 (116 bytes, 0x004235b0..0x00423624)
// Signature: undefined4 FUN_004235b0(void)
// Returns: 0 (failure) or 1 (success)
//
// Body (cited from 0x004235b0):
//   uVar3 = 0;
//   FUN_00423480();                                              // build AI path
//   FUN_00495280("d:\\ToastArt\\Common\\ai.piz");               // open piz
//   iVar1 = FUN_004cc230(2, 1, &DAT_00644110);                  // open stream
//   if (iVar1 != 0) {
//       iVar2 = FUN_004cc5e0(iVar1, 0x13269902, &uStack_8, auStack_4); // find chunk
//       if (iVar2 != 0) {
//           FUN_004cbd30(iVar1, &DAT_007f1a9c, uStack_8);       // read data
//           FUN_004cc160(iVar1, 0);                             // close stream
//           uVar3 = 1;
//       }
//   }
//   FUN_004952f0();                                             // close piz
//   return uVar3;
//
// Constants (cited from 0x004235b0 body):
//   "d:\\ToastArt\\Common\\ai.piz" — at 0x005cd330 (per C1 plate)
//   (2, 1, &DAT_00644110) — FUN_004cc230 args (role-2, mode-1, out-buf)
//   0x13269902 — record tag (+1 from led.piz tag 0x13269901)
//   0 — close flag (second arg to FUN_004cc160)
//   &DAT_007f1a9c — output buffer (32 KB, sentinel-filled by AiDataBufInit)
//
// Callees: all C2+ (FUN_00423480, FUN_00495280, FUN_004cc230, FUN_004cc5e0,
//   FUN_004cbd30, FUN_004cc160, FUN_004952f0).
//
// Uncertainties (non-blocking):
//   U-3215: record tag 0x13269902 significance (one greater than led.piz tag).
//
// Structurally identical to FUN_00409710 (led.piz loader, same batch) modulo
//   file path, record tag, and output buffer.
//
// ref: re/analysis/promote_c2_render_lowrva/004235b0.md
// ---------------------------------------------------------------------------

// 0x004235b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl AiPizLoad(void)
{
    std::uint32_t uVar3 = 0u;

    // Step 1: build AI path. [0x004235b0]
    s_FUN_00423480();

    // Step 2: open piz by path. [0x004235b0]
    s_FUN_00495280("d:\\ToastArt\\Common\\ai.piz");

    // Step 3: open stream handle. [FUN_004cc230(2, 1, &DAT_00644110)]
    int iVar1 = s_FUN_004cc230(2, 1, reinterpret_cast<void*>(0x00644110u));
    if (iVar1 != 0) {
        // Step 4: find chunk with tag 0x13269902. [FUN_004cc5e0]
        std::uint32_t uStack_8 = 0u;
        std::uint32_t auStack_4[1] = {0u};
        int iVar2 = s_FUN_004cc5e0(iVar1, 0x13269902u, &uStack_8, auStack_4);
        if (iVar2 != 0) {
            // Step 5: read record data into DAT_007f1a9c buffer. [FUN_004cbd30]
            s_FUN_004cbd30(iVar1, reinterpret_cast<void*>(0x007f1a9cu), uStack_8);
            // Step 6: close stream. [FUN_004cc160(iVar1, 0)]
            s_FUN_004cc160(iVar1, 0);
            uVar3 = 1u;
        }
    }

    // Step 7: close piz. [FUN_004952f0]
    s_FUN_004952f0();

    return uVar3;
}

// MASS-DISABLED 2026-05-24 needs-canonical-piz-state: RH_ScopedInstall(AiPizLoad, 0x004235b0);
// Phase A1 audit 2026-05-24: function depends on canonical PIZ-VFS state
// (s_FUN_00423480 builds path, s_FUN_00495280 mounts piz). At diff-attach
// time both sides AV at NULL deref of mount state. crash_equal_ok is banned
// as GREEN. Validation happens at the canonical AI-init call site.
