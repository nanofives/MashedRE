// Mashed RE — promote-round round 6 (L2 cheap re-earns, demoted-needs-reimpl).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004c9f50  RwGlobal7d4134Set        — render; 9B setter
//   0x004b6610  BootGlobalPairSet        — boot; two-param two-global setter
//   0x004b6560  BootGlobalPairSetThunk   — boot; 4B thunk -> 0x004b6610
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
//
// NOT included (round-6 triage deferrals — see PROMOTION_LOOP_LEDGER):
//   0x004c9f60 — guard + live-vtable side-effect shape (needs bespoke approach)
//   0x004b6540 — thunk to FUN_004b6640 which ALLOCATES via vtable freelist and
//                stores fresh pointers into globals (nondeterministic across
//                A/B); possible fit: allocator_nonnull — design-first
//
// Analysis:
//   re/analysis/skeleton_prep_boot_winmain_b/004c9f50.md
//   re/analysis/promote_c2_txd_loader/004b6610.md
//   re/analysis/promote_c2_txd_loader/004b6560.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwGlobal7d4134Set  --  0x004c9f50   (subsystem: render)
//
// Original: FUN_004c9f50 (10 bytes, 0x004c9f50..0x004c9f59)
// Bytes: 8B 44 24 04 / A3 34 41 7D 00 / C3
//   (mov eax,[esp+4]; mov [0x007d4134],eax; ret)
// Signature: void FUN_004c9f50(undefined4 param_1)
//
// Constants (cited from function body at 0x004c9f50):
//   0x007d4134 — destination global dword
//
// Caller: FUN_00493710 (RW_INIT_FN, C2). No uncertainties (pure leaf).
// ---------------------------------------------------------------------------

// 0x004c9f50
extern "C" __declspec(dllexport) void __cdecl RwGlobal7d4134Set(std::uint32_t param_1) {
    // 0x007d4134 cited at 0x004c9f50 body.
    *reinterpret_cast<std::uint32_t*>(0x007d4134u) = param_1;
}

RH_ScopedInstall(RwGlobal7d4134Set, 0x004c9f50);

// ---------------------------------------------------------------------------
// BootGlobalPairSet  --  0x004b6610   (subsystem: boot)
//
// Original: FUN_004b6610 (20 bytes, 0x004b6610..0x004b6623)
// Bytes: 8B 44 24 04 / 8B 4C 24 08 / A3 5C 3E 7D 00 / 89 0D 60 3E 7D 00 / C3
//   (mov eax,[esp+4]; mov ecx,[esp+8];
//    mov [0x007d3e5c],eax; mov [0x007d3e60],ecx; ret)
// Signature: void FUN_004b6610(undefined4 param_1, undefined4 param_2)
//
// Constants (cited from function body at 0x004b6610):
//   0x007d3e5c — destination global dword for param_1
//   0x007d3e60 — destination global dword for param_2 (adjacent, +4)
//
// Caller: FUN_00402750 via thunk 0x004b6560, with (FUN_00429290 ptr, 0).
//
// Uncertainties (non-blocking):
//   U-3225: semantics of the two globals (data-semantic).
// ---------------------------------------------------------------------------

// 0x004b6610
extern "C" __declspec(dllexport) void __cdecl BootGlobalPairSet(std::uint32_t param_1,
                                                                std::uint32_t param_2) {
    // 0x007d3e5c / 0x007d3e60 cited at 0x004b6610 body.
    *reinterpret_cast<std::uint32_t*>(0x007d3e5cu) = param_1;
    *reinterpret_cast<std::uint32_t*>(0x007d3e60u) = param_2;
}

RH_ScopedInstall(BootGlobalPairSet, 0x004b6610);

// ---------------------------------------------------------------------------
// BootGlobalPairSetThunk  --  0x004b6560   (subsystem: boot)
//
// Original: thunk_FUN_004b6610 (5 bytes, 0x004b6560..0x004b6564)
// Bytes: E9 AB 00 00 00   (jmp rel32 -> 0x004b6560 + 5 + 0xAB = 0x004b6610)
// Signature: forwards both args unmodified to FUN_004b6610.
//
// Reimpl forwards to the ORIGINAL-image target VA (call vs tail-jmp is
// observationally identical for a void __cdecl). When the 0x004b6610 hook is
// also installed, the call chains through that inline JMP — same behavior.
//
// Constants (cited from thunk body at 0x004b6560):
//   0x004b6610 — jump target (rel32 0xAB)
//
// Caller: FUN_00402750, with (FUN_00429290 ptr, 0).
//
// Uncertainties (non-blocking):
//   U-3225: inherited from FUN_004b6610 (data-semantic).
// ---------------------------------------------------------------------------

// 0x004b6560
extern "C" __declspec(dllexport) void __cdecl BootGlobalPairSetThunk(std::uint32_t param_1,
                                                                     std::uint32_t param_2) {
    // 0x004b6610 target cited at 0x004b6560 thunk body (E9 rel32 = 0xAB).
    using Fn = void(__cdecl*)(std::uint32_t, std::uint32_t);
    reinterpret_cast<Fn>(0x004b6610u)(param_1, param_2);
}

RH_ScopedInstall(BootGlobalPairSetThunk, 0x004b6560);
