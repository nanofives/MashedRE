// Mashed RE — promote-round round 9 (L3 curated small leaves, continued).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00498be0  RenderBitDepthGet    — render; 5B getter
//   0x0040dc80  UtilFloat63b910Get   — util; 6B float getter (FLD m32 — byte-verified)
//   0x0040dc90  UtilSlotIndexCondGet — util; 23B conditional getter
//   0x00429860  RaceStateFlagGet     — ai; 5B getter
//   0x00429840  RaceStateLatchSet    — ai; 23B conditional latch setter
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
// All five run menu-attach with seeded/observed handlers.
//
// Analysis:
//   re/analysis/settings_config_d2_cont1/00498be0.md
//   re/analysis/localization_d2/0x0040dc80.md
//   re/analysis/localization_d2/0x0040dc90.md
//   re/analysis/bucket_ai_00415d00_00452ea0/0x00429860.md
//   re/analysis/bucket_ai_00415d00_00452ea0/0x00429840.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RenderBitDepthGet  --  0x00498be0   (subsystem: render)
//
// Original: FUN_00498be0 (6 bytes, 0x00498be0..0x00498be5)
// Bytes: A1 14 34 77 00 / C3   (mov eax,[0x00773414]; ret)
// Signature: undefined4 FUN_00498be0(void)
//
// Constants (cited from function body at 0x00498be0):
//   0x00773414 — global dword (render bit depth per the note)
//
// Caller: FUN_004921d0 DisplayInit (C4).
// ---------------------------------------------------------------------------

// 0x00498be0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderBitDepthGet(void) {
    // 0x00773414 cited at 0x00498be0 body.
    return *reinterpret_cast<const std::uint32_t*>(0x00773414u);
}

RH_ScopedInstall(RenderBitDepthGet, 0x00498be0);

// ---------------------------------------------------------------------------
// UtilFloat63b910Get  --  0x0040dc80   (subsystem: util)
//
// Original: FUN_0040dc80 (7 bytes, 0x0040dc80..0x0040dc86)
// Bytes: D9 05 10 B9 63 00 / C3
//   (FLD dword ptr [0x0063b910]; ret — D9 05 = TRUE float load, NOT FILD;
//    byte-verified per the standing round practice after the 0x00429300 case)
// Signature: float10 FUN_0040dc80(void)
//
// Constants (cited from function body at 0x0040dc80):
//   0x0063b910 — float source (label X-position source per the note)
//
// Caller: FUN_00429660 (hud, C2).
//
// Uncertainties (non-blocking):
//   U-2276: semantic role of the float (data-semantic).
// ---------------------------------------------------------------------------

// 0x0040dc80
extern "C" __declspec(dllexport) float __cdecl UtilFloat63b910Get(void) {
    // FLD [0x0063b910] cited at 0x0040dc80 (instruction bytes above).
    return *reinterpret_cast<const float*>(0x0063b910u);
}

RH_ScopedInstall(UtilFloat63b910Get, 0x0040dc80);

// ---------------------------------------------------------------------------
// UtilSlotIndexCondGet  --  0x0040dc90   (subsystem: util)
//
// Original: FUN_0040dc90 (24 bytes, 0x0040dc90..0x0040dca7)
// Bytes: A1 D0 0F 7F 00 / 83 F8 0A / 74 0B / 83 F8 05 / 74 06 /
//        A1 14 B9 63 00 / C3 / 33 C0 / C3
//   (mov eax,[0x007f0fd0]; cmp eax,10; je ret0; cmp eax,5; je ret0;
//    mov eax,[0x0063b914]; ret;  ret0: xor eax,eax; ret)
// Signature: undefined4 FUN_0040dc90(void)
//
// Constants (cited from function body at 0x0040dc90):
//   0x007f0fd0 — game-mode global (condition; values 5 and 10 short-circuit)
//   0x0063b914 — returned global dword when mode is not 5/10
//   10, 5      — excluded mode values (return 0)
//
// Caller: FUN_00429660 (hud, C2).
//
// Uncertainties (non-blocking):
//   U-2277 / U-2278: semantics of mode values and the slot index (data-semantic).
// ---------------------------------------------------------------------------

// 0x0040dc90
extern "C" __declspec(dllexport) std::uint32_t __cdecl UtilSlotIndexCondGet(void) {
    // 0x007f0fd0 condition + 5/10 exclusions + 0x0063b914 source cited at
    // 0x0040dc90 body.
    const std::uint32_t mode = *reinterpret_cast<const std::uint32_t*>(0x007f0fd0u);
    if (mode == 10u || mode == 5u) {
        return 0u;
    }
    return *reinterpret_cast<const std::uint32_t*>(0x0063b914u);
}

RH_ScopedInstall(UtilSlotIndexCondGet, 0x0040dc90);

// ---------------------------------------------------------------------------
// RaceStateFlagGet  --  0x00429860   (subsystem: ai)
//
// Original: FUN_00429860 (6 bytes, 0x00429860..0x00429865)
// Bytes: A1 BC 91 89 00 / C3   (mov eax,[0x008991bc]; ret)
// Signature: undefined4 FUN_00429860(void)
//
// Constants (cited from function body at 0x00429860):
//   0x008991bc — race-state flag (0xb = lap-complete per the notes)
//
// Caller: FUN_004103a0 TimeTrial::LapFinishProcessor (C2). Paired latch
// setter: 0x00429840.
//
// Uncertainties (non-blocking):
//   U-3587: state-value semantics (data-semantic).
// ---------------------------------------------------------------------------

// 0x00429860
extern "C" __declspec(dllexport) std::uint32_t __cdecl RaceStateFlagGet(void) {
    // 0x008991bc cited at 0x00429860 body.
    return *reinterpret_cast<const std::uint32_t*>(0x008991bcu);
}

RH_ScopedInstall(RaceStateFlagGet, 0x00429860);

// ---------------------------------------------------------------------------
// RaceStateLatchSet  --  0x00429840   (subsystem: ai)
//
// Original: FUN_00429840 (23 bytes, 0x00429840..0x00429856)
// Bytes: 8B 44 24 04 / 85 C0 / 74 0A / 8B 0D BC 91 89 00 / 85 C9 / 75 05 /
//        A3 BC 91 89 00 / C3
//   (mov eax,[esp+4]; test eax,eax; je store;
//    mov ecx,[0x008991bc]; test ecx,ecx; jnz skip;
//    store: mov [0x008991bc],eax; skip: ret)
// Signature: void FUN_00429840(undefined4 param_1)
//
// Latch semantics (cited from the branch structure above): the store happens
// iff param_1 == 0 OR the current value at 0x008991bc == 0.
//
// Constants (cited from function body at 0x00429840):
//   0x008991bc — race-state flag (same global as RaceStateFlagGet)
//
// Caller: FUN_004103a0 TimeTrial::LapFinishProcessor (C2).
//
// Uncertainties (non-blocking):
//   U-3587: state-value semantics (data-semantic).
// ---------------------------------------------------------------------------

// 0x00429840
extern "C" __declspec(dllexport) void __cdecl RaceStateLatchSet(std::uint32_t param_1) {
    // branch structure + 0x008991bc cited at 0x00429840 body.
    if (param_1 != 0u) {
        if (*reinterpret_cast<const std::uint32_t*>(0x008991bcu) != 0u) {
            return;
        }
    }
    *reinterpret_cast<std::uint32_t*>(0x008991bcu) = param_1;
}

RH_ScopedInstall(RaceStateLatchSet, 0x00429840);
